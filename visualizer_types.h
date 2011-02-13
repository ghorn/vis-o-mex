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
 * visualizer_types.h
 * Structs used for visualization
 */

#ifndef __VISUALIZER_TYPES_H__
#define __VISUALIZER_TYPES_H__

#include "../autopilot/interface_structs.h"

#define NUM_AC_VIS_HISTORY_POINTS 30
#define DT (3.0/100.0)
#define ALL_TIME_HIGH_SCORE 445 // 7:25


typedef struct {
  uint8_t initialized;
  uint16_t index;

  // core
  float wingspan;
  float aspect_ratio;
  float alpha;
  
  // derived
  float chord;

  euler_t e_n2b;
  quat_t q_n2b;
  xyz_t pos[NUM_AC_VIS_HISTORY_POINTS];
  xyz_t l_wingtip[NUM_AC_VIS_HISTORY_POINTS];
  xyz_t r_wingtip[NUM_AC_VIS_HISTORY_POINTS];
  
} aircraft_t;


#endif // __VISUALIZER_TYPES_H__
