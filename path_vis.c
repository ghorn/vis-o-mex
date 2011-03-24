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
 * path_vis.c
 */

#include <GL/gl.h>
#include <math.h>

#include <emlc_telemetry.h>
#include <emlc_types.h>
#include <xyz.h>
#include <misc_math.h>

#include "path_vis.h"

#define NUM_DISK_VERTICES 100

float disk_x[NUM_DISK_VERTICES];
float disk_y[NUM_DISK_VERTICES];
float disk_z[NUM_DISK_VERTICES];

crosswind_telem_t crosswind_telem;
hover_telem_t hover_telem;

int got_first_vis = 0;

static void
crosswind_telem_handler(const lcm_recv_buf_t *rbuf __attribute__((unused)),
                 const char *channel __attribute__((unused)),
                 const emlc_crosswind_telem_t *msg,
                 void *user __attribute__((unused)))
{
  got_first_vis = 1;

  // only update if changed
  if (!memcmp( &crosswind_telem, msg, sizeof(crosswind_telem_t)))
    return;

  memcpy( &crosswind_telem, msg, sizeof(crosswind_telem_t));

  double phi = atan2(-crosswind_telem.r_n2x0_n.y, -crosswind_telem.r_n2x0_n.x);
  double theta = acos( -crosswind_telem.r_n2x0_n.z/xyz_norm( &crosswind_telem.r_n2x0_n ) );
  //  printf("phi: %f, theta: %f\n", phi*180.0/M_PI, theta*180.0/M_PI);

  double R11 = cos(theta)*cos(phi);
  double R12 = -sin(phi);
  double R13 = cos(phi)*sin(theta);
  double R21 = cos(theta)*sin(phi);
  double R22 = cos(phi);
  double R23 = sin(theta)*sin(phi);
  double R31 = -sin(theta);
  double R32 = 0;
  double R33 = cos(theta);

  double R = sqrt( xyz_norm_squared( &crosswind_telem.r_n2x0_n ) - SQR(crosswind_telem.r0) );
  for (int k=0; k<NUM_DISK_VERTICES; k++){
    double q = 2.0*M_PI*((double)k)/(NUM_DISK_VERTICES - 1.0);
    disk_x[k] = crosswind_telem.r0*(R11*sin(q) + R12*cos(q)) - R13*R;
    disk_y[k] = crosswind_telem.r0*(R21*sin(q) + R22*cos(q)) - R23*R;
    disk_z[k] = crosswind_telem.r0*(R31*sin(q) + R32*cos(q)) - R33*R;
  }
}

emlc_lcm_copy_handler(hover_telem_t);

void
init_path_vis()
{
  emlc_lcm_subscribe_chan(crosswind_telem_t, &crosswind_telem_handler, NULL, "emlc_crosswind_telem_t_crosswind_telem");
  emlc_lcm_subscribe_chan_cp(hover_telem_t, &hover_telem, "emlc_hover_telem_t_hover_telem");
}

static void
draw_crosswind(void)
{
  glPushMatrix();

  // draw x0
  glPointSize( 10 );
  glBegin(GL_POINTS);
  glColor4f( 0.0f, 1.0f, 0.0f, 0.4f );
  glVertex3f( crosswind_telem.r_n2x0_n.x, crosswind_telem.r_n2x0_n.y, crosswind_telem.r_n2x0_n.z );
  glEnd();

  // draw disk
  glLineWidth(2.0f);
  glBegin(GL_LINE_STRIP);
  glColor4f( 1.0f , 0.0f, 1.0f, 1.0f);
  for (int k=0; k<NUM_DISK_VERTICES; k++)
    glVertex3f( disk_x[k], disk_y[k], disk_z[k] );
  glEnd();

  glPopMatrix();
}

static void
draw_hover(void)
{
  glPushMatrix();

  // draw Xn_req
  glPointSize( 10 );
  glBegin(GL_POINTS);
  glColor4f( 0.0f, 1.0f, 0.0f, 0.4f );
  glVertex3f( hover_telem.Xn_req.x, hover_telem.Xn_req.y, hover_telem.Xn_req.z );
  glEnd();

  glPopMatrix();
}

void
draw_path_vis()
{
  if (!got_first_vis)
    return;

  draw_crosswind();
  draw_hover();
}
