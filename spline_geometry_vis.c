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
 * spline_geometry_vis.c
 * Draw spline geometry
 */

#include <GL/gl.h>	// Header File For The OpenGL32 Library

#include <ap_telemetry.h>
#include <ap_types.h>
#include <spatial_rotations.h>

#include "spline_utils.h"

spline_geometry_t spline_geometry;

ap_lcm_copy_handler(spline_geometry_t);

void
init_spline_geometry()
{
  ap_lcm_subscribe_chan_cp(spline_geometry_t, &spline_geometry, "ap_spline_geometry_t_geometry");
}

void
draw_spline_geometry(float wingspan)
{
  float scale = 2*wingspan;

  // draw spline axes
  glPushMatrix();

  glLineWidth(1.0f);

  glTranslatef( spline_geometry.r_n2s_n.x, spline_geometry.r_n2s_n.y, spline_geometry.r_n2s_n.z );
  
  glColor3f(1.0f,0.0f,0.0f);
  glBegin(GL_LINE_STRIP);	
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*spline_geometry.sx.x, scale*spline_geometry.sx.y, scale*spline_geometry.sx.z);
  glEnd();

  glColor3f(0.0f,1.0f,0.0f);
  glBegin(GL_LINE_STRIP);	
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*spline_geometry.sy.x, scale*spline_geometry.sy.y, scale*spline_geometry.sy.z);
  glEnd();

  glColor3f(0.0f,0.0f,1.0f);
  glBegin(GL_LINE_STRIP);	
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*spline_geometry.sz.x, scale*spline_geometry.sz.y, scale*spline_geometry.sz.z);
  glEnd();

  glPopMatrix();

  double R_n2c[9];
  dcm_of_quat_a2b( R_n2c, &(spline_geometry.q_n2c));

  // draw command axes
  glPushMatrix();

  glLineWidth(1.0f);

  glTranslatef( spline_geometry.r_n2c_n.x, spline_geometry.r_n2c_n.y, spline_geometry.r_n2c_n.z );
  
  glColor3f(1.0f,0.0f,0.0f);
  glBegin(GL_LINE_STRIP);	
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*R_n2c[0], scale*R_n2c[1], scale*R_n2c[2] );
  glEnd();

  glColor3f(0.0f,1.0f,0.0f);
  glBegin(GL_LINE_STRIP);	
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*R_n2c[3], scale*R_n2c[4], scale*R_n2c[5] );
  glEnd();

  glColor3f(0.0f,0.0f,1.0f);
  glBegin(GL_LINE_STRIP);	
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*R_n2c[6], scale*R_n2c[7], scale*R_n2c[8] );
  glEnd();

  glPopMatrix();

  glLineWidth(1.0f);

  // draw carrot
  glPointSize( 5 );
  glPushMatrix();
  glBegin(GL_POINTS);
  glColor4f( 0.9f, 0.7f, 0.3f, 1.0f);
  glVertex3f( spline_geometry.r_n2rabbit_n.x, spline_geometry.r_n2rabbit_n.y, spline_geometry.r_n2rabbit_n.z );
  glEnd();
  glPopMatrix();
}
