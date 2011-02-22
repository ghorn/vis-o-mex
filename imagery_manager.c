/*
 * This file is part of vis-o-mex.
 *
 * Copyright (C) 2010-2011 Greg Horn <ghorn@stanford.edu>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

 /*
  * imagery_manager.c
  * cache satellite imagery from the web for display in visualizer
  */

#include <stdio.h>
#include <Imlib2.h>
#include <inttypes.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>

#include "imagery_manager.h"
#include "imagery_drawer.h"
#include "imagery_prober.h"
#include <ap_types.h>

#define MAX_ZOOM_LEVEL 16 // must be <= 21
#define NUM_SQUARE_RINGS 3
#define NUM_IMAGE_TILES ( 2*(2*NUM_SQUARE_RINGS-1)*(2*NUM_SQUARE_RINGS-1) )
//  2*(2*rings-1)^2: { 1 -> 2, 2 -> 18, 3 -> 50, 4 -> 98}
#define MAX_STRING_LENGTH 3000
#define EARTH_RADIUS 6371000.0
#define MAX_FILENAME_LENGTH 100


// don't compile if trying to draw more than a hard limit of image tiles
// this is here to stay off the blacklist :)
#define MAX_NUM_IMAGE_TILES 50
#if (NUM_IMAGE_TILES > MAX_NUM_IMAGE_TILES)
#error too many layers of tiles requested in imagery_magager.c
#endif


int test_for_blank_image( const uint32_t ix, const uint32_t iy, const uint8_t zoom_level);
int imagery_in_memory(const uint32_t ix, const uint32_t iy, const uint8_t zoom_level);
int imagery_cached( image_tile_t * image_tile);
void load_tile_from_cache(image_tile_t * image_tile);
int imagery_downloading(const uint32_t ix, const uint32_t iy, const uint8_t zoom_level);
void start_tile_download(image_tile_t * image_tile);
void lat_lon_centered_from_ix_iy_zoom( double * lat, double * lon, const uint32_t ix, const uint32_t iy, const uint8_t zoom_level);

void set_tiles();
void ix_iy_from_lat_lon_zoom( uint32_t * ix, uint32_t * iy, const double lat, const double lon, const uint8_t zoom_level);
void set_north_east_bounds_from_ix_iy_zoom( image_tile_t * image_tile);

// the origin according to joby gnss
double lat0 = 0;
double lon0 = 0;
double alt0 = 0;

// the closest indeces to the origin
uint32_t ix0,iy0;

// array of image_tiles
image_tile_t image_tiles[NUM_IMAGE_TILES];

// don't start looking for imagery until we know what imagery to pull
int got_lla_origin = 0;

// highest zoom level where there is still imagery available
int zoom_level;

void
init_imagery()
{
  reset_probe( MAX_ZOOM_LEVEL);

  // clear all times and assign all texture numbers
  for (int k=0; k<NUM_IMAGE_TILES; k++){
    image_tiles[k].status = TILE_UNINITIALIZED;
    glGenTextures( 1, &(image_tiles[k].texture) );
  }
}

void
reset_imagery()
{
  reset_probe( MAX_ZOOM_LEVEL );

  // clear all tiles from memory
  for (int k=0; k<NUM_IMAGE_TILES; k++){
    // unallocate memory if allocated
    if (image_tiles[k].status == TILE_READY){
      imlib_context_set_image(image_tiles[k].image);
      imlib_free_image();
    }
    image_tiles[k].status = TILE_UNINITIALIZED;
  }
}

#define EPSILON 1e-8

// get the lat0/lon0/alt0 ned origin from gps
// if the origin changes, re-initialize imagery
void
update_imagery_origin(const gpsPhys_t * const gpsPhys)
{
  if (gpsPhys->solution_valid == 0)
    return;

  double dlat = fabs(lat0 - gpsPhys->lat0);
  double dlon = fabs(lon0 - gpsPhys->lon0);
  double dalt = fabs(alt0 - gpsPhys->alt0);
  double originnorm = sqrt(dlat*dlat + dlon*dlon + dalt*dalt);

  if (originnorm > EPSILON) {
    lat0 = gpsPhys->lat0;
    lon0 = gpsPhys->lon0;
    alt0 = gpsPhys->alt0;
    printf("gps origin lat0: %.9f, lon0: %.9f, alt0: %.2f\n", lat0, lon0, alt0);
    if (got_lla_origin == 1){
      printf("gps lla origin changed by %lf (2-norm{rad,rad,m}),"
             " re-setting gps lla origin and reallocating tiles\n", originnorm);
    }
    reset_imagery(MAX_ZOOM_LEVEL);
  }
  got_lla_origin = 1;
}

// main image handler: probe for best zoom level, then get and draw tiles
void
manage_imagery()
{
  // return if we don't know what imagery to get
  if (got_lla_origin == 0)
    return;

  // if zoom level unknown, keep probing
  if (zoom_level_known() == 0){
    int ret = run_probe(lat0, lon0);
    // return if zoom level still unknown
    if (ret == -1)
      return;
    // if zoom level figured out, set zoom level and init tiles
    zoom_level = ret;
    ix_iy_from_lat_lon_zoom( &ix0, &iy0, lat0, lon0, zoom_level );
    printf("Imagery available at zoom level: %d\n", zoom_level);
    set_tiles();
  }

  // if zoom level is know, get/load tiles if they're not already gotten
  for ( int k=0; k < NUM_IMAGE_TILES; k++){
    get_imagery( &(image_tiles[k]));
    draw_tile( &(image_tiles[k]) );
  }

  // finally, display a message when all tiles are fetched/loaded
  for ( int k=0; k < NUM_IMAGE_TILES; k++)
    if (image_tiles[k].status != TILE_READY)
      return;

  static int last_tile_loaded = 0;
  if ( last_tile_loaded == 0 ){
    last_tile_loaded = 1;
    printf("Imagery finished downloading and/or loading from cache.\n"
           "%d image tiles loaded at zoom level %d\n", NUM_IMAGE_TILES, zoom_level);
  }
}

// figure out which tiles to draw
void
set_tiles()
{
  uint32_t ix,iy;
  uint32_t ix_list[NUM_IMAGE_TILES];
  uint32_t iy_list[NUM_IMAGE_TILES];

  int num_tiles = 0;
  int dix_min = - 2*NUM_SQUARE_RINGS + 2;
  int dix_max =   2*NUM_SQUARE_RINGS - 1;
  int diy_min = - NUM_SQUARE_RINGS + 1;
  int diy_max =   NUM_SQUARE_RINGS - 1;

  for ( ix = ix0 + dix_min; ix <= ix0 + dix_max; ix++){
    for ( iy = iy0 + diy_min; iy <=iy0 + diy_max; iy++){
      ix_list[num_tiles] = ix;
      iy_list[num_tiles] = iy;
      num_tiles++;
    }
  }

  // now we know which tiles to draw, load this information into the tile array
  for (int k=0; k<NUM_IMAGE_TILES; k++)
  {
    image_tiles[k].status = TILE_UNINITIALIZED;
    image_tiles[k].ix = ix_list[k];
    image_tiles[k].iy = iy_list[k];
    image_tiles[k].zoom_level = zoom_level;
    set_north_east_bounds_from_ix_iy_zoom( &(image_tiles[k]) );
  }
  
}

void
get_imagery(image_tile_t * image_tile)
{
  // if tile is already in memory, do nothing
  if ( image_tile->status == TILE_READY)
    return;

  // if tile isn't in memory but it is cached, then load it and set status to TILE_READY
  if (imagery_cached(image_tile) == 1 ){
    load_tile_from_cache( image_tile );
    return;
  }

  // if image is neither in memory nor cached, but it is downloading, do nothing
  if ( image_tile->status == TILE_DOWNLOADING )
    return;

  // if tile is not in memory, cached, or downloading, then download it
  printf("start image downloading\n");
  start_tile_download( image_tile);
}

// check and see if the image is cached
// if so, change it's status to TILE_UNINITIALIZED (to handle downloading tiles) and return 1
int imagery_cached( image_tile_t * image_tile)
{  
  char filename[MAX_FILENAME_LENGTH];
  filename_of_ix_iy_zoom(filename, image_tile->ix, image_tile->iy, image_tile->zoom_level);
  FILE * imagefile;
  if ((imagefile = fopen(filename,"r")) == NULL){
    return 0;
  }
  fclose(imagefile);

  return 1;
}

void
load_tile_from_cache( image_tile_t * image_tile)
{
  char filename[MAX_FILENAME_LENGTH];
  filename_of_ix_iy_zoom(filename, image_tile->ix, image_tile->iy, image_tile->zoom_level);
  FILE * imagefile;
  if ((imagefile = fopen(filename,"r")) == NULL){
    printf("this file was supposed to be available but it's not\n");
    fclose(imagefile);
    exit(1);
  }
  fclose(imagefile);

  if ( ( image_tile->image = imlib_load_image(filename) ) != 0){
    image_tile->status = TILE_READY;
    setup_texture( image_tile );
  } else {
    printf("imlib_load_image fail\n");
    exit(1);
  }
}

void
filename_of_ix_iy_zoom(char * filename, const uint32_t ix, const uint32_t iy, const uint8_t zoom_level)
{
  sprintf(filename, "image_cache/satellite_imagery_zoom_%d_ix_%d_iy_%d.png", zoom_level, ix, iy);
}

void
start_tile_download( image_tile_t * image_tile)
{
  image_tile->status = TILE_DOWNLOADING;

  double lat,lon;
  lat_lon_centered_from_ix_iy_zoom( &lat, &lon, image_tile->ix, image_tile->iy, image_tile->zoom_level);

  char filename[MAX_FILENAME_LENGTH];
  filename_of_ix_iy_zoom( filename, image_tile->ix, image_tile->iy, image_tile->zoom_level );

  printf("downloading %s\n", filename);
  
  char system_call[1024];
  sprintf(system_call, "sh shake_n_bake.sh 'http://maps.google.com/maps/api/staticmap?center=%.6f,%.6f&zoom=%d&size=256x256&maptype=satellite&format=png32&sensor=false' %s &", lat, lon, zoom_level, filename);

  int ret = system(system_call);
  if (ret == 10)
    return;
  return;
}

void
ix_iy_from_lat_lon_zoom( uint32_t * ix, uint32_t * iy, const double lat, const double lon, const uint8_t zoom_level)
{
 *ix = (lat +  90.0) / 180.0 * (1 << zoom_level );
 *iy = (lon + 180.0) / 360.0 * (1 << zoom_level );

}

void
lat_lon_centered_from_ix_iy_zoom( double * lat, double * lon, const uint32_t ix, const uint32_t iy, const uint8_t zoom_level)
{
  int num_segments = (1 << zoom_level);
  double lat_degrees_per_segment = 180.0/num_segments;
  *lat = lat_degrees_per_segment*(ix + 0.5) - 90.0;
  double lon_degrees_per_segment = 360.0/num_segments;
  *lon = lon_degrees_per_segment*(iy + 0.5) - 180.0;
}

void
set_north_east_bounds_from_ix_iy_zoom( image_tile_t * image_tile)
{
  double lat_center,lon_center;
  lat_lon_centered_from_ix_iy_zoom( &lat_center, &lon_center, image_tile->ix, image_tile->iy, image_tile->zoom_level);
 
  int num_segments = (1 << image_tile->zoom_level);
  double lat_degrees_per_segment = 360.0/num_segments;
  double lon_degrees_per_segment = 360.0/num_segments;

  double lat_min = lat_center - 0.5*lat_degrees_per_segment;
  double lat_max = lat_center + 0.5*lat_degrees_per_segment;
  double lon_min = lon_center - 0.5*lon_degrees_per_segment;
  double lon_max = lon_center + 0.5*lon_degrees_per_segment;
  
  image_tile->x_min = (lat_min - lat0)*M_PI*EARTH_RADIUS/180.0*cos(lat0*M_PI/180.0);
  image_tile->x_max = (lat_max - lat0)*M_PI*EARTH_RADIUS/180.0*cos(lat0*M_PI/180.0);
  
  
  image_tile->y_min = (lon_min - lon0)*M_PI*EARTH_RADIUS/180.0*cos(lat0*M_PI/180.0);
  image_tile->y_max = (lon_max - lon0)*M_PI*EARTH_RADIUS/180.0*cos(lat0*M_PI/180.0);

  double x_mean = 0.5*(image_tile->x_max + image_tile->x_min);
  image_tile->x_max = x_mean + 0.5*(image_tile->y_max - image_tile->y_min);
  image_tile->x_min = x_mean - 0.5*(image_tile->y_max - image_tile->y_min);
}
