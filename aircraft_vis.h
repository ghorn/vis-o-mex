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
 * aircraft_vis.h 
 * Manage aircraft visualizer structs. Draw aircraft.
 */

#ifndef __AIRCRAFT_VIS_H__
#define __AIRCRAFT_VIS_H__

#include <ap_types.h>
#include "visualizer_types.h"

aircraft_t * create_aircraft( float wingspan );
void free_aircraft(aircraft_t * ac);
void update_aircraft_pose(xyz_t * pos, quat_t * q_n2b, aircraft_t * ac);
void draw_aircraft( aircraft_t * ac, int draw_at_origin);
void draw_aircraft_trails( aircraft_t * ac );


#endif // __AIRCRAFT_VIS_H__
