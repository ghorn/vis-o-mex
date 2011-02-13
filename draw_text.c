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
 * draw_text.c
 * Overlay 2D text on the screen
 */

#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#include "visualizer_types.h"
#include "../autopilot/interface_structs.h"
#include "../mathlib/xyz.h"

static int
get_minutes(double time_sec)
{
  int time_sec_floored = floor(time_sec);
  return time_sec_floored/60;
}


static double
get_seconds(double time_sec)
{
  int time_sec_floored = floor(time_sec);
  int minutes = time_sec_floored/60;
  return time_sec - minutes*60.0;
}


static void
renderBitmapString(unsigned x, unsigned y, void *font, char *string)
{
  char *c;
  glRasterPos2i(x, y);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}


static void
resetPerspectiveProjection()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}


static void
set_orthographic_projection( const int window_width, const int window_height)
{
  // switch to projection mode
  glMatrixMode(GL_PROJECTION);
  // save previous matrix which contains the 
  //settings for the perspective projection
  glPushMatrix();
  // reset matrix
  glLoadIdentity();
  // set a 2D orthographic projection
  gluOrtho2D(0, window_width, 0, window_height);
  // invert the y axis, down is positive
  glScalef(1, -1, 1);
  // mover the origin from the bottom left corner
  // to the upper left corner
  glTranslatef(0, -window_height, 0);
  glMatrixMode(GL_MODELVIEW);
}


void
draw_text(const rc_t * const rc,
          const est2User_t * const e2u,
          const sensors2Estimator_t * const s2e,
          const system_energy_t * const se,
          const int window_width,
          const int window_height,
          const double current_score,
          const double session_high_score)
{
  set_orthographic_projection(window_width, window_height);
  glPushMatrix();
  glLoadIdentity();

  char message[160];
  
  // high score
  glColor4f(0.15f,0.15f,1.0f,1.0f);
  sprintf(message, "current score: %d:%04.1f, session high score: %d:%04.1f, all time high score: %d:%04.1f", get_minutes(current_score), get_seconds(current_score), get_minutes(session_high_score), get_seconds(session_high_score), get_minutes(ALL_TIME_HIGH_SCORE), get_seconds(ALL_TIME_HIGH_SCORE));
  renderBitmapString(5,30, GLUT_BITMAP_HELVETICA_18, message);

  // joystick stuff
  glColor4f(0.25f,0.25f,1.0f,1.0f);
  sprintf(message,"mode: %d, aux2: %d, gear: %d, enable: %d, throttle: %f, yaw: %f, pitch: %f, roll: %f",rc->mode,rc->aux2,rc->gear,rc->enable,rc->throttle,rc->yaw,rc->pitch,rc->roll);
  renderBitmapString(5,60, GLUT_BITMAP_HELVETICA_18, message);

  glColor4f(0.0f,1.0f,0.0f,1.0f);
  // wind west azimuth
  double west_azi = e2u->ae2u.wind_est.wind_east_azimuth*180.0/M_PI - 180.0;
  while (west_azi < -180.0)
    west_azi += 360.0;
  sprintf(message,"wind west azimuth (deg): %.2f", west_azi);
  renderBitmapString(5,90,GLUT_BITMAP_HELVETICA_18,message);

  sprintf(message,"wind_est speed(m/s): %.2f", e2u->ae2u.wind_est.wind_speed);
  renderBitmapString(5,120,GLUT_BITMAP_HELVETICA_18,message);


  sprintf(message,"time: %.3f", e2u->timeDouble);
  renderBitmapString(5,150,GLUT_BITMAP_HELVETICA_18,message);

  glColor4f(1.0f,0.0f,0.0f,1.0f);
  // rc switch status
  switch (rc->aux2){
  case 0:
    sprintf(message,"MODE: %d DELTA", rc->mode +1);
    break;
  case 1:
    sprintf(message,"MODE: %d ECHO", rc->mode +1);
    break;
  case 2:
    sprintf(message,"MODE: %d FOXTROT", rc->mode +1);
    break;
  }
  renderBitmapString(5,180,GLUT_BITMAP_HELVETICA_18,message);

  // gps status
  switch (s2e->gpsPhys.solution_valid) {
  case 0:
    sprintf(message,"GPS: LOST");
    break;
  case 1:
    sprintf(message,"GPS: PHASE LOCK");
    break;
  case 2:
    sprintf(message,"GPS: CODE LOCK");
    break;
  }
  renderBitmapString(5,210,GLUT_BITMAP_HELVETICA_18,message);

  glColor4f(0.0f,1.0f,0.0f,1.0f);
  sprintf(message,"alpha(deg): %f", e2u->ae2u.alpha*180.0/M_PI);
  renderBitmapString(5,240,GLUT_BITMAP_HELVETICA_18,message);
  
  sprintf(message,"beta(deg): %f", e2u->ae2u.beta*180.0/M_PI);
  renderBitmapString(5,270,GLUT_BITMAP_HELVETICA_18,message);
  
  sprintf(message,"airspeed(m/s): %f", e2u->ae2u.v_I);
  renderBitmapString(5,300,GLUT_BITMAP_HELVETICA_18,message);
  
  sprintf(message,"norm(r_n2b_n): %f", xyz_norm(&(e2u->x.r_n2b_n)));
  renderBitmapString(5,330,GLUT_BITMAP_HELVETICA_18,message);

  /***** power/energy ******/
  sprintf(message,"power harvested: %.2f kW", se->power_harvested*0.001);
  renderBitmapString(5,360,GLUT_BITMAP_HELVETICA_18,message);
  
  sprintf(message,"energy harvested:  %.2f kJ", se->energy_harvested*0.001);
  renderBitmapString(5,390,GLUT_BITMAP_HELVETICA_18, message);
  
  sprintf(message,"conserved quantity: %f", se->conserved_quantity);
  renderBitmapString(5,420,GLUT_BITMAP_HELVETICA_18, message);
 
  glPopMatrix();
  resetPerspectiveProjection();
}
