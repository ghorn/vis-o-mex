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
 * spline_trajectory_vis.c
 * Manage spline trajectory visualization structs. Draw traj spline.
 */

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <inttypes.h>
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <string.h>
#include <math.h>

#include <ap_types.h>

#include "spline_trajectory_vis.h"
#include "spline_utils.h"
#include <ap_telemetry.h>


#define NUM_INTERPOLATED_POINTS 1000
#define NUM_ANIMATION_SEGMENTS 4

typedef struct {
  uint8_t draw;
  uint8_t spline_allocated;
  float x[NUM_INTERPOLATED_POINTS];
  float y[NUM_INTERPOLATED_POINTS];
  float z[NUM_INTERPOLATED_POINTS];
  spline_knot_data_t knot_data;
  splines_t splines;
} spline_vis_t;

float alpha_animation[NUM_INTERPOLATED_POINTS];
int alpha_head = 0;

spline_vis_t spline_vis_fm;
spline_vis_t spline_vis_ap_active;
spline_vis_t spline_vis_ap_queued;
spline_vis_t spline_vis_transition;

void
fill_spline_vis(spline_vis_t * sv, spline_knot_data_t * kd, int periodic)
{
  memcpy( &(sv->knot_data), kd, sizeof( spline_knot_data_t ) );
  if (sv->spline_allocated == 1)
    free_splines( &(sv->splines) );

  reticulate_splines_from_knots( &(sv->splines), &(sv->knot_data), periodic);
  for (int k=0; k<NUM_INTERPOLATED_POINTS; k++){
    sv->x[k] = gsl_spline_eval( sv->splines.x.spl, ((double)(k))/(NUM_INTERPOLATED_POINTS - 1.0), sv->splines.x.acc);
    sv->y[k] = gsl_spline_eval( sv->splines.y.spl, ((double)(k))/(NUM_INTERPOLATED_POINTS - 1.0), sv->splines.y.acc);
    sv->z[k] = gsl_spline_eval( sv->splines.z.spl, ((double)(k))/(NUM_INTERPOLATED_POINTS - 1.0), sv->splines.z.acc);
  }
  sv->spline_allocated = 1;
}


static void
command_ground_handler(const lcm_recv_buf_t *rbuf __attribute__((unused)), 
                       const char *channel __attribute__((unused)), 
                       const ap_command_t *msg, 
                       void *user __attribute__((unused)))
{
  switch ( msg->val ){
  case VISUALIZER_HIDE_TRANSITION:
  {
    spline_vis_transition.draw = 0;
    break;
  }
  case VISUALIZER_HIDE_ACTIVE:
  {
    spline_vis_ap_active.draw = 0;
    break;
  }
  case VISUALIZER_HIDE_QUEUED:
  {
    spline_vis_ap_queued.draw = 0;
    break;
  }
  case VISUALIZER_HIDE_FLIGHT_MANAGER:
  {
    spline_vis_fm.draw = 0;
    break;
  }
  case VISUALIZER_SHOW_TRANSITION:
  {
    spline_vis_transition.draw = 1;
    break;
  }
  case VISUALIZER_SHOW_ACTIVE:
  {
    spline_vis_ap_active.draw = 1;
    break;
  }
  case VISUALIZER_SHOW_QUEUED:
  {
    spline_vis_ap_queued.draw = 1;
    break;
  }
  case VISUALIZER_SHOW_FLIGHT_MANAGER:
  {
    spline_vis_fm.draw = 1;
    break;
  }
  case VISUALIZER_SWAP_ACTIVE_QUEUED:
  {
    spline_vis_t spline_vis_temp;
    memcpy( &spline_vis_temp, &spline_vis_ap_active, sizeof(spline_vis_t));
    memcpy( &spline_vis_ap_active, &spline_vis_ap_queued, sizeof(spline_vis_t));
    memcpy( &spline_vis_ap_queued, &spline_vis_temp, sizeof(spline_vis_t));
    break;
  }
  }
}


static void
spline_command_ground_handler(const lcm_recv_buf_t *rbuf __attribute__((unused)), 
                              const char *channel __attribute__((unused)), 
                              const ap_spline_command_t *msg, 
                              void *user __attribute__((unused)))
{
  switch (msg->command.val){
  case VISUALIZER_LOAD_FLIGHT_MANAGER:
  {
    spline_knot_data_t knot_data;
    memcpy( &knot_data, &(msg->knot_data), sizeof(spline_knot_data_t) );
    fill_spline_vis( &spline_vis_fm, &knot_data, 1 );
    break;
  }
  case VISUALIZER_LOAD_ACTIVE:
  {
    spline_knot_data_t knot_data;
    memcpy( &knot_data, &(msg->knot_data), sizeof(spline_knot_data_t) );
    fill_spline_vis( &spline_vis_ap_active, &knot_data, 1 );
    break;
  }
  case VISUALIZER_LOAD_QUEUED:
  {
    spline_knot_data_t knot_data;
    memcpy( &knot_data, &(msg->knot_data), sizeof(spline_knot_data_t) );
    fill_spline_vis( &spline_vis_ap_queued, &knot_data, 1 );
    break;
  }
  case VISUALIZER_LOAD_TRANSITION:
  {
    spline_knot_data_t knot_data;
    memcpy( &knot_data, &(msg->knot_data), sizeof(spline_knot_data_t) );
    fill_spline_vis( &spline_vis_transition, &knot_data, 0 );
    break;
  }
  }
}

void
init_spline_trajectory_vis()
{
  spline_vis_fm.draw = 0;
  spline_vis_ap_active.draw = 0;
  spline_vis_ap_queued.draw = 0;
  spline_vis_transition.draw = 0;
  spline_vis_fm.spline_allocated = 0;
  spline_vis_ap_active.spline_allocated = 0;
  spline_vis_ap_queued.spline_allocated = 0;
  spline_vis_transition.spline_allocated = 0;

  for (int k=0; k<NUM_INTERPOLATED_POINTS; k++){
    alpha_animation[k] = 0.5*(sin(NUM_ANIMATION_SEGMENTS* ((double)k)*2.0*M_PI/((double)NUM_INTERPOLATED_POINTS) ) + 1.0);
    if (alpha_animation[k] < 0.1)
      alpha_animation[k] = 0.2;
    else
      alpha_animation[k] = 1.0;
  }
  ap_lcm_subscribe_chan(command_t, &command_ground_handler, NULL, "ground_ap_command_t");
  ap_lcm_subscribe_chan(spline_command_t, &spline_command_ground_handler, NULL, "ground_ap_spline_command_t");
} 

void
draw_one_spline_trajectory( spline_vis_t * sv, float * RGBA)
{
  if ( sv->draw == 0 )
    return;
  // just draw the knots
  glPushMatrix();
  glPointSize(5.0);
  glBegin(GL_POINTS);
  float fade = 1.0;
  glColor4f(fade*RGBA[0],fade*RGBA[1],fade*RGBA[2],fade*RGBA[3]);
  for (int k = 0; k < sv->knot_data.num_knots; k++)
    glVertex3f( sv->knot_data.x[k], sv->knot_data.y[k], sv->knot_data.z[k]);
  glEnd();

  // now draw the interpolated spline
  glLineWidth(2.0f);
  glBegin(GL_LINE_STRIP);
  for ( int k=0;k<NUM_INTERPOLATED_POINTS;k++){
    int j = alpha_head + k;
    if (j >= NUM_INTERPOLATED_POINTS)
      j -= NUM_INTERPOLATED_POINTS;
    if (alpha_animation[j] > 0.5)
      glColor4f(RGBA[0],RGBA[1],RGBA[2],RGBA[3]);
    else
      glColor4f(1.0f-RGBA[0],1.0f-RGBA[1],1.0f-RGBA[2],RGBA[3]);
    glVertex3f( sv->x[k], sv->y[k], sv->z[k] );
  }
  glEnd();

  glPopMatrix();
}

void
draw_all_spline_trajectories()
{
  float fm_RGBA[4];
  float active_RGBA[4];
  float queued_RGBA[4];
  float transition_RGBA[4];

  fm_RGBA[0] =  0.7f;
  fm_RGBA[1] =  0.4f;
  fm_RGBA[2] =  0.5f;
  fm_RGBA[3] =  1.0f;

  transition_RGBA[0] =  0.8f;
  transition_RGBA[1] =  0.8f;
  transition_RGBA[2] =  0.2f;
  transition_RGBA[3] =  1.0f;
  
  active_RGBA[0] =  0.2f;
  active_RGBA[1] =  0.8f;
  active_RGBA[2] =  0.3f;
  active_RGBA[3] =  0.8f;
  
  queued_RGBA[0] =  0.3f;
  queued_RGBA[1] =  0.4f;
  queued_RGBA[2] =  1.0f;
  queued_RGBA[3] =  1.0f;
  
  alpha_head -= 3;
  if (alpha_head < 0)
    alpha_head += NUM_INTERPOLATED_POINTS;

  draw_one_spline_trajectory( &spline_vis_fm, fm_RGBA );
  draw_one_spline_trajectory( &spline_vis_ap_active, active_RGBA );
  draw_one_spline_trajectory( &spline_vis_ap_queued, queued_RGBA );
  draw_one_spline_trajectory( &spline_vis_transition, transition_RGBA );
}
