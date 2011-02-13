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
 * drawing_functions.h
 * The main drawing function
 */

#ifndef __DRAWING_FUNCTIONS_H__
#define __DRAWING_FUNCTIONS_H__


void
draw_grid(void);

void
draw_wind_est( est2User_t * e2u );


void
draw_axes_from_euler(float yaw, float pitch, float roll,
                     float x, float y, float z,
                     float size, float width);



#endif // __DRAWING_FUNCTIONS_H__
