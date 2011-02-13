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
 * camera_manager.c
 * Manage openGL camera rotations/translations.
 */

#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The OpenGL32 Library
#include <math.h>
#include <inttypes.h>

#include "camera_manager.h"
#include "visualizer.h"

camera_pose_t camera_pose = {
  .phi = -60*M_PI/180.0,
  .theta = 20.0*M_PI/180.0,
  .rho = 300.0,
  .focus_x = 0.0,
  .focus_y = -100.0,
  .focus_z = -20.0,
  .draw_focus = 0
};


void set_camera(){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				// Reset The Projection Matrix

  int window_width = get_window_width();
  int window_height = get_window_height();

  gluPerspective(45.0f,(GLfloat)window_width/(GLfloat)window_height,0.1f,100000.0f);  // Calculate The Aspect Ratio Of The Window

  float camera_x = -cos(camera_pose.phi)*cos(camera_pose.theta)*camera_pose.rho + camera_pose.focus_x;
  float camera_y = -sin(camera_pose.phi)*cos(camera_pose.theta)*camera_pose.rho + camera_pose.focus_y;
  float camera_z = -sin(camera_pose.theta)*camera_pose.rho + camera_pose.focus_z;

  gluLookAt( camera_x, -camera_z, camera_y, camera_pose.focus_x, -camera_pose.focus_z, camera_pose.focus_y, 0,1,0);

  glMatrixMode(GL_MODELVIEW);
}

void set_camera_focus(const xyz_t * const f){
  camera_pose.focus_x = f->x;
  camera_pose.focus_y = f->y;
  camera_pose.focus_z = f->z;
  set_camera();
}

camera_pose_t * get_camera_pose(){
  return &camera_pose;
}


void draw_camera_focus(){
  if (camera_pose.draw_focus == 0)
    return;

  glPushMatrix();

  // grey
  glColor4f( 1.0f, 1.0f, 1.0f, 0.3f);

  // draw a dot
  glPointSize(10.0);
  glBegin(GL_POINTS);
  glVertex3f( camera_pose.focus_x, camera_pose.focus_y, camera_pose.focus_z );
  glEnd();

  // draw a line from the ground to the dot
  glLineWidth(1.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f( camera_pose.focus_x, camera_pose.focus_y, camera_pose.focus_z );
  glVertex3f( camera_pose.focus_x, camera_pose.focus_y, 0.0f );
  glEnd();


  glPopMatrix();
}
