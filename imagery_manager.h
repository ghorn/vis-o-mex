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
  * imagery_manager.h
  * cache satellite imagery from the web for display in visualizer
  */

#ifndef __IMAGERY_MANAGER_H__
#define __IMAGERY_MANAGER_H__

#include <Imlib2.h>
#include <inttypes.h>

#include <ap_types.h>

#define TILE_UNINITIALIZED 0
#define TILE_DOWNLOADING   1
#define TILE_READY         2

typedef struct {
  Imlib_Image image;
  unsigned texture;
  int status;

  uint32_t ix;
  uint32_t iy;
  uint8_t zoom_level;

  double x_min;
  double x_max;
  double y_min;
  double y_max;

} image_tile_t;

void init_imagery(void);
void manage_imagery(void);
void update_imagery_origin(const gpsPhys_t * const gpsPhys);
void get_imagery( image_tile_t * image_tile );
void filename_of_ix_iy_zoom(char * filename, const uint32_t ix, const uint32_t iy, const uint8_t zoom_level);
void ix_iy_from_lat_lon_zoom( uint32_t * ix, uint32_t * iy,
                              const double lat, const double lon,
                              const uint8_t zoom_level);

#endif //__IMAGERY_MANAGER_H__
