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
 * simple_model_vis.c
 */

#include <GL/gl.h>
#include <ap_types.h>
#include <ap_telemetry.h>
#include "camera_manager.h"

#include "aircraft_vis.h"

simple_model_t simple_model;
aircraft_t * ac;

static void
simple_model_handler(const lcm_recv_buf_t *rbuf __attribute__((unused)),
                     const char *channel __attribute__((unused)),
                     const ap_simple_model_t *msg,
                     void *user __attribute__((unused)))
{
  memcpy( &simple_model, msg, sizeof(simple_model_t));
  update_aircraft_pose( &simple_model.r_n2b_n, &simple_model.q_n2b, ac);
}

void
init_simple_model_vis()
{
  ap_lcm_subscribe(simple_model_t, &simple_model_handler, NULL);
  ac = create_aircraft( 67 );
}

void
draw_simple_model_vis()
{
  if (!ac->initialized)
    return;

//  set_camera_focus( &simple_model.r_n2b_n );

  draw_aircraft(ac, 0);
  float scale = 20;

  glPushMatrix();

  glLineWidth(1.0f);

  glTranslatef( simple_model.r_n2b_n.x, simple_model.r_n2b_n.y, simple_model.r_n2b_n.z );

  // draw command frame
  glColor3f(1.0f,0.0f,0.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*simple_model.cx_n.x, scale*simple_model.cx_n.y, scale*simple_model.cx_n.z);
  glEnd();

  glColor3f(0.0f,1.0f,0.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*simple_model.cy_n.x, scale*simple_model.cy_n.y, scale*simple_model.cy_n.z);
  glEnd();

  glColor3f(0.0f,0.0f,1.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*simple_model.cz_n.x, scale*simple_model.cz_n.y, scale*simple_model.cz_n.z);
  glEnd();

  // draw no lift rotation body axes
  glColor3f(1.0f,0.3f,0.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*simple_model.b0x_n.x, scale*simple_model.b0x_n.y, scale*simple_model.b0x_n.z);
  glEnd();

  glColor3f(0.3f,1.0f,0.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*simple_model.b0y_n.x, scale*simple_model.b0y_n.y, scale*simple_model.b0y_n.z);
  glEnd();

  glColor3f(0.3f,0.0f,1.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*simple_model.b0z_n.x, scale*simple_model.b0z_n.y, scale*simple_model.b0z_n.z);
  glEnd();


  // draw lift vectors
  glColor3f(1.0f,1.0f,0.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*simple_model.r_lift0_n.x, scale*simple_model.r_lift0_n.y, scale*simple_model.r_lift0_n.z);
  glEnd();

  glColor3f(1.0f,0.0f,1.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( scale*simple_model.r_lift_n.x, scale*simple_model.r_lift_n.y, scale*simple_model.r_lift_n.z);
  glEnd();

  // draw apparent wind
  glColor3f(0.0f,1.0f,1.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( simple_model.we_n.x, simple_model.we_n.y, simple_model.we_n.z);
  glEnd();


  // draw aero force
  glColor3f(0.0f,1.0f,1.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( simple_model.Faer_n.x, simple_model.Faer_n.y, simple_model.Faer_n.z);
  glEnd();

  // draw total force
  glColor3f(0.5f,1.0f,1.0f);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f( simple_model.F_n.x, simple_model.F_n.y, simple_model.F_n.z);
  glEnd();


  glPopMatrix();
}
