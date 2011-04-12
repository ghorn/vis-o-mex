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
 * tether_vis.c
 * Manage tether visualization structs. Draw tether.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <GL/gl.h>	// Header File For The OpenGL32 Library

#include <vis_telemetry.h>

#include "tether_vis.h"
#include "imagery_drawer.h" // for set_imagery_transparency()

tether_vis_t tether_vis;

float
map_tension_to_red01( float tension_newtons )
{
  static float max_tension_lbs = 0.0f;
  static float min_tension_lbs = 0.0f;
  float tension_lbs = tension_newtons*0.224808943;
  if (tension_lbs > max_tension_lbs)
    max_tension_lbs = tension_lbs;
  if (tension_lbs < min_tension_lbs)
    min_tension_lbs = tension_lbs;

  float red = min_tension_lbs + tension_lbs/(max_tension_lbs-min_tension_lbs);
  if (red < 0.0f)
    red = 0.0f;
  else if (red > 1.0f)
    red = 1.0f;
  return red;
}

vis_lcm_copy_handler(tether_vis_t);

void
init_tether_vis()
{
  vis_lcm_subscribe_chan_cp(tether_vis_t, &tether_vis, "sim_tether_vis_t_tether_vis");
}


void
draw_tether()
{
  glPushMatrix();

  int num_masses_underground = 0;

  glBegin(GL_LINE_STRIP);
  float red;
  for (int k=0; k<tether_vis.num_masses; k++){
    red = map_tension_to_red01( tether_vis.tension[k] );
    glColor4f( red , 0.0f, 1.0f - red, 1.0f);
    glVertex3f( (tether_vis.x)[k], (tether_vis.y)[k], (tether_vis.z)[k]);
    if ( (tether_vis.z)[k] > 0 )
      num_masses_underground++;
  }
  glEnd();

  glPointSize( 4 );
  glBegin(GL_POINTS);
  glColor4f( 0.0f, 1.0f, 0.0f, 0.4f );
  for (int k=0; k<tether_vis.num_masses; k++){
    glVertex3f( (tether_vis.x)[k], (tether_vis.y)[k], (tether_vis.z)[k]);
  }
  glEnd();

  // alpha == 1 for no masses underground
  // alpha == min_alpha for all masses underground
  float min_alpha = 0.6f;
  set_imagery_transparency( (min_alpha - 1)/tether_vis.num_masses*num_masses_underground + 1);
  
  glPopMatrix();
}
