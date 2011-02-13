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
 * camera_manager.h
 * manage openGL camera rotations/translations
 */

#ifndef __CAMERA_MANAGER_H__
#define __CAMERA_MANAGER_H__

#include <inttypes.h>
#include <ap_types.h>

typedef struct {
  float phi;
  float theta;
  float rho;
  float focus_x;
  float focus_y;
  float focus_z;
  uint8_t draw_focus;
} camera_pose_t;


void set_camera(void);
void set_camera_focus(const xyz_t * const f);
camera_pose_t * get_camera_pose(void);
void draw_camera_focus(void);

#endif // __CAMERA_MANAGER_H__
