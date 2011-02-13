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
 * imagery_prober.c
 * find the max zoom level which has imagery available
 */

#include <stdio.h>
#include <stdlib.h>

#include "imagery_prober.h"
#include "imagery_manager.h"

#define PROBING 0
#define DONE_PROBING 1

#define IMAGERY_UNAVAILABLE 0
#define IMAGERY_AVAILABLE 1

#define MAX_FILENAME_LENGTH 100

// do we know what the max zoom level is that has imagery available?
int probe_status;

int zoom_level;

image_tile_t probe_tile;

int test_for_blank_image( image_tile_t * image_tile);

void
reset_probe( int zoom_level_reset)
{
  zoom_level = zoom_level_reset;
  probe_status = PROBING;
  probe_tile.status = TILE_UNINITIALIZED;
}

int
zoom_level_known()
{
  return probe_status;
}

int
run_probe(double lat0, double lon0)
{
  uint32_t ix_probe,iy_probe;
  ix_iy_from_lat_lon_zoom( &ix_probe, &iy_probe, lat0, lon0, zoom_level);
  probe_tile.ix = ix_probe;
  probe_tile.iy = iy_probe;
  probe_tile.zoom_level = zoom_level;

  // return error if tile not yet available
  get_imagery( &probe_tile);
  if (probe_tile.status != TILE_READY)
    return -1;
 
  // if tile is ready, check for blank image
  // if imagery is unavailable, decrease max zoom level and try again
  if ( test_for_blank_image( &probe_tile) == IMAGERY_UNAVAILABLE){
    printf("no imagery available at zoom level %d, trying zoom level %d\n", zoom_level, zoom_level - 1);
    reset_probe( zoom_level - 1);
    return -1;
  }
  
  // if imagry is available, end the zoom_level_probe
  probe_status = DONE_PROBING;
  return zoom_level;
}


int
test_for_blank_image( image_tile_t * image_tile)
{
  char filename[MAX_FILENAME_LENGTH];
  filename_of_ix_iy_zoom(filename, image_tile->ix, image_tile->iy, image_tile->zoom_level);

  char system_call[512];
  sprintf( system_call, "diff image_cache/blank_image.png %s > /dev/null", filename);
  if (system( system_call ) == 0 ){
    return IMAGERY_UNAVAILABLE;
  }
  
  return IMAGERY_AVAILABLE;
}
