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
 * simple_aircraft_vis.h 
 * Manage aircraft visualizer structs. Draw aircraft.
 */

#ifndef __SIMPLE_AIRCRAFT_VIS_H__
#define __SIMPLE_AIRCRAFT_VIS_H__

#include <ap_types.h>

#define NUM_AC_VIS_HISTORY_POINTS 30

typedef struct {
  uint8_t initialized;
  uint16_t index;

  // core
  simple_aircraft_params_t params;
  
  // derived
  float chord;

  euler_t e_n2b;
  quat_t q_n2b;
  xyz_t pos[NUM_AC_VIS_HISTORY_POINTS];
  xyz_t l_wingtip[NUM_AC_VIS_HISTORY_POINTS];
  xyz_t r_wingtip[NUM_AC_VIS_HISTORY_POINTS];
  
} simple_aircraft_t;


simple_aircraft_t * alloc_simple_aircraft( const simple_aircraft_params_t * const params );
void free_simple_aircraft(simple_aircraft_t * ac);
void update_aircraft_pose(xyz_t * pos, quat_t * q_n2b, simple_aircraft_t * ac);
void draw_simple_aircraft( const simple_aircraft_t * const ac, int draw_at_origin);
void draw_simple_aircraft_trails( const simple_aircraft_t * const ac );

#endif // __SIMPLE_AIRCRAFT_VIS_H__
