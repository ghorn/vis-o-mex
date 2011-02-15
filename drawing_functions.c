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
 * drawing_functions.c
 * Orphan functions which were cluttering up visualizer.c
 * but are too short to have their own files.
 */

#include <GL/gl.h>

#include <ap_types.h>

void
draw_grid()
{
  glPushMatrix();

  unsigned dx = 100; // meters per grid line

  double radius = 1500;

  glLineWidth(1.0f);
  // lines which run east/west  
  glColor4f(1.0f,1.0f,1.0f,1.0f);
  for (double north=-radius; north<radius+0.1; north+=dx){
    glBegin(GL_LINE_STRIP);
    glVertex3f(north, -radius, -0.5f);
    glVertex3f(north,  radius, -0.5f);
    glEnd();
  }

  // lines which run north/south
  for (double east=-radius; east<radius+0.1; east+=dx){
    glBegin(GL_LINE_STRIP);
    glVertex3f( -radius, east, -0.5f);
    glVertex3f(  radius, east, -0.5f);
    glEnd();
  }

  glPopMatrix();
}


void
draw_wind_est( est2User_t * e2u )
{
  glPushMatrix();
  float windsock_height = 20.0f;
  glLineWidth(5.0f);
  glColor4f( 0.0f, 1.0f, 1.0f, e2u->ae2u.wind_est.wind_est_trust );
  glBegin(GL_LINE_STRIP);
  glVertex3f( 0.0f,0.0f, -windsock_height );
  double scale = 3.0f;
  glVertex3f( scale*e2u->ae2u.wind_est.wind_ned.x,
              scale*e2u->ae2u.wind_est.wind_ned.y,
              -windsock_height + scale*e2u->ae2u.wind_est.wind_ned.z );
  glEnd();

  glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );
  glBegin(GL_LINE_STRIP);
  glVertex3f( 0.0f, 0.0f, -windsock_height );
  glVertex3f( 0.0f, -scale*15.0f, -windsock_height );
  glEnd();

  glPopMatrix();
}


void
draw_axes_from_euler(float yaw, float pitch, float roll,
                     float x, float y, float z,
                     float size, float width)
{
  glPushMatrix();

  glLineWidth(width);

  glTranslatef(x,y,z);
  glRotatef( yaw,   0.0f, 0.0f, 1.0f);
  glRotatef( pitch, 0.0f, 1.0f, 0.0f);
  glRotatef( roll,  1.0f, 0.0f, 0.0f);
  
  glColor3f(1.0f,0.0f,0.0f);
  glBegin(GL_LINE_STRIP);	
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(size,0.0f,0.0f);
  glEnd();

  glColor3f(0.0f,1.0f,0.0f);
  glBegin(GL_LINE_STRIP);	
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(0.0f,size,0.0f);
  glEnd();

  glColor3f(0.0f,0.0f,1.0f);
  glBegin(GL_LINE_STRIP);	
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(0.0f,0.0f,size);
  glEnd();

  glPopMatrix();
  glLineWidth(1.0f);
}

