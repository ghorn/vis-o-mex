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
 * aircraft_vis.c
 * Manage aircraft visualizer structs. Draw aircraft.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>	// Header File For The OpenGL32 Library

#include <ap_types.h>
#include <spatial_rotations.h>
#include "aircraft_vis.h"

#define RAD2DEG 57.29577951308232

// set history all to first point
aircraft_t *
create_aircraft(float wingspan)
{
  aircraft_t * ac;
  if ( (ac = malloc(sizeof(aircraft_t))) == NULL){
    printf("aircraft malloc fail\n");
    exit(1);
  }

  ac->initialized = 0;
  ac->aspect_ratio = 6;
  ac->wingspan = wingspan;
  ac->chord = ac->wingspan/ac->aspect_ratio;
  ac->alpha = 1.0;

  return ac;
}

void
free_aircraft(aircraft_t * ac)
{
  free(ac);
}

void
set_wingtip_positions( aircraft_t * ac )
{
  xyz_t l_wingtip = {0.0, -0.5*ac->wingspan, 0.0};
  xyz_t r_wingtip = {0.0,  0.5*ac->wingspan, 0.0};
  rot_vec_by_quat_b2a( &(ac->l_wingtip[ac->index]), &(ac->q_n2b), &l_wingtip);
  rot_vec_by_quat_b2a( &(ac->r_wingtip[ac->index]), &(ac->q_n2b), &r_wingtip);

  ac->l_wingtip[ac->index].x += ac->pos[ac->index].x;
  ac->l_wingtip[ac->index].y += ac->pos[ac->index].y;
  ac->l_wingtip[ac->index].z += ac->pos[ac->index].z;
  ac->r_wingtip[ac->index].x += ac->pos[ac->index].x;
  ac->r_wingtip[ac->index].y += ac->pos[ac->index].y;
  ac->r_wingtip[ac->index].z += ac->pos[ac->index].z;
}

void
update_aircraft_pose(xyz_t * pos, quat_t * q_n2b, aircraft_t * ac)
{
  // first time initialization
  if (ac->initialized == 0){
    for (int k=0; k<NUM_AC_VIS_HISTORY_POINTS; k++){
      ac->pos[k].x = pos->x;
      ac->pos[k].y = pos->y;
      ac->pos[k].z = pos->z;
      ac->l_wingtip[k].x = pos->x;
      ac->l_wingtip[k].y = pos->y;
      ac->l_wingtip[k].z = pos->z;
      ac->r_wingtip[k].x = pos->x;
      ac->r_wingtip[k].y = pos->y;
      ac->r_wingtip[k].z = pos->z;
      memcpy( &(ac->q_n2b), q_n2b, sizeof(quat_t) );
      euler321_of_quat( &(ac->e_n2b), q_n2b);
      ac->initialized = 1;
      ac->index = 0;
    }
  } else { // normal update
    ac->index++;
    if (ac->index >= NUM_AC_VIS_HISTORY_POINTS)
      ac->index = 0;
    ac->pos[ac->index].x = pos->x;
    ac->pos[ac->index].y = pos->y;
    ac->pos[ac->index].z = pos->z;
    memcpy( &(ac->q_n2b), q_n2b, sizeof(quat_t) );
    euler321_of_quat( &(ac->e_n2b), q_n2b);
    set_wingtip_positions( ac );
  }
}

void
draw_aircraft( aircraft_t * ac, int draw_at_origin)
{
  if (ac == NULL){
    printf("draw_aircraft(ac) called, but ac == NULL\n");
    printf("maybe you forgot to call ac = create_aircraft(double wingspan) ?\n");
    exit(1);
  }

  if (ac->initialized == 0)
    return;

  glPushMatrix();

  float wingspan, chord;

  if (draw_at_origin == 1){
    wingspan = ac->wingspan*1.5;
    glTranslatef( 0.0f, 0.0f, -wingspan);
    chord = wingspan/ac->aspect_ratio;
  } else {
    glTranslatef((float)(ac->pos[ac->index].x),(float)(ac->pos[ac->index].y),(float)(ac->pos[ac->index].z));
    wingspan = ac->wingspan;
    chord = ac->chord;
  }

  glRotatef( ac->e_n2b.yaw*RAD2DEG, 0.0f, 0.0f, 1.0f);
  glRotatef( ac->e_n2b.pitch*RAD2DEG, 0.0f, 1.0f, 0.0f);
  glRotatef( ac->e_n2b.roll*RAD2DEG, 1.0f, 0.0f, 0.0f);

  // wing with colored wingtips
  float frac_not_tip = 0.7f;
  glBegin(GL_QUADS); // most of the wing
  glColor4f(0.15f, 0.15f, 1.0f, ac->alpha);
  glVertex3f(  0.25*chord,  0.5*wingspan*frac_not_tip, 0.0f);
  glVertex3f(  0.25*chord, -0.5*wingspan*frac_not_tip, 0.0f);
  glVertex3f( -0.75*chord, -0.5*wingspan*frac_not_tip, 0.0f);
  glVertex3f( -0.75*chord,  0.5*wingspan*frac_not_tip, 0.0f);
  glEnd();
  glBegin(GL_QUADS); // yellow wing top leading edge
  glColor4f(0.85f, 0.85f, 0.0f, ac->alpha);
  glVertex3f(  0.250*chord,  0.5*wingspan*frac_not_tip, -0.006f*wingspan);
  glVertex3f(  0.250*chord, -0.5*wingspan*frac_not_tip, -0.006f*wingspan);
  glVertex3f( -0.083*chord, -0.5*wingspan*frac_not_tip, -0.006f*wingspan);
  glVertex3f( -0.083*chord,  0.5*wingspan*frac_not_tip, -0.006f*wingspan);
  glEnd();
  glBegin(GL_QUADS); // orange wing bottom right stripes
  glColor4f(1.0f, 0.5f, 0.0f, ac->alpha);
  glVertex3f(  0.25*chord,  0.5*wingspan*frac_not_tip*0.4, 0.006f*wingspan);
  glVertex3f(  0.25*chord,  0.5*wingspan*frac_not_tip*0.7, 0.006f*wingspan);
  glVertex3f( -0.75*chord,  0.5*wingspan*frac_not_tip*0.7, 0.006f*wingspan);
  glVertex3f( -0.75*chord,  0.5*wingspan*frac_not_tip*0.4, 0.006f*wingspan);
  glEnd();
  glBegin(GL_QUADS); // orange wing bottom left stripes
  glColor4f(1.0f, 0.5f, 0.0f, ac->alpha);
  glVertex3f(  0.25*chord, -0.5*wingspan*frac_not_tip*0.4, 0.006f*wingspan);
  glVertex3f(  0.25*chord, -0.5*wingspan*frac_not_tip*0.7, 0.006f*wingspan);
  glVertex3f( -0.75*chord, -0.5*wingspan*frac_not_tip*0.7, 0.006f*wingspan);
  glVertex3f( -0.75*chord, -0.5*wingspan*frac_not_tip*0.4, 0.006f*wingspan);
  glEnd();
  glBegin(GL_QUADS); // right wingtip
  glColor4f(0.0f, 1.0f, 0.0f, ac->alpha);
  glVertex3f(  0.25*chord,  0.5*wingspan, 0.0f);
  glVertex3f(  0.25*chord,  0.5*wingspan*frac_not_tip, 0.0f);
  glVertex3f( -0.75*chord,  0.5*wingspan*frac_not_tip, 0.0f);
  glVertex3f( -0.75*chord,  0.5*wingspan, 0.0f);
  glEnd();
  glBegin(GL_QUADS); // left wingtip
  glColor4f(1.0f, 0.0f, 0.0f, ac->alpha);
  glVertex3f(  0.25*chord,  -0.5*wingspan, 0.0f);
  glVertex3f(  0.25*chord,  -0.5*wingspan*frac_not_tip, 0.0f);
  glVertex3f( -0.75*chord,  -0.5*wingspan*frac_not_tip, 0.0f);
  glVertex3f( -0.75*chord,  -0.5*wingspan, 0.0f);
  glEnd();

  // fuselage
  float fuselage_scale = 0.5;
  glColor4f(0.15f, 0.15f, 1.0f, ac->alpha);
  glBegin(GL_QUADS);
  glVertex3f(  0.25*wingspan,  0.0f, -0.5*fuselage_scale*chord);
  glVertex3f(  0.25*wingspan,  0.0f,  0.5*fuselage_scale*chord);
  glVertex3f( -0.75*wingspan,  0.0f,  0.5*fuselage_scale*chord);
  glVertex3f( -0.75*wingspan,  0.0f, -0.5*fuselage_scale*chord);
  glEnd();
  glBegin(GL_QUADS);
  glVertex3f(  0.25*wingspan, -0.5*fuselage_scale*chord, 0.0f);
  glVertex3f(  0.25*wingspan,  0.5*fuselage_scale*chord, 0.0f);
  glVertex3f( -0.75*wingspan,  0.5*fuselage_scale*chord, 0.0f);
  glVertex3f( -0.75*wingspan, -0.5*fuselage_scale*chord, 0.0f);
  glEnd();

  float tail_wingspan_scale = 0.5;
  float tail_chord_scale = 0.8;
  // horizontal stabilizer
  glBegin(GL_QUADS);
  glColor4f(0.15f, 0.15f, 1.0f, ac->alpha);
  glVertex3f(  -0.75*wingspan + chord*tail_chord_scale,   0.5*tail_wingspan_scale*wingspan, 0.0f);
  glVertex3f(  -0.75*wingspan + chord*tail_chord_scale,  -0.5*tail_wingspan_scale*wingspan, 0.0f);
  glVertex3f(  -0.75*wingspan,  -0.5*tail_wingspan_scale*wingspan, 0.0f);
  glVertex3f(  -0.75*wingspan,   0.5*tail_wingspan_scale*wingspan, 0.0f);
  glEnd();

  // vertical stabilizer
  glBegin(GL_QUADS);
  glColor4f(0.0f, 0.5f, 1.0f, ac->alpha);
  glVertex3f(  -0.75*wingspan + chord*tail_chord_scale, 0.0f, -0.5*tail_wingspan_scale*wingspan);
  glVertex3f(  -0.75*wingspan + chord*tail_chord_scale, 0.0f, 0.0f);
  glVertex3f(  -0.75*wingspan, 0.0f, 0.0f);
  glVertex3f(  -0.75*wingspan, 0.0f, -0.5*tail_wingspan_scale*wingspan);
  glEnd();

  // draw line from aircraft to ground
  glLoadIdentity(); // make sure we're no longer rotated.
  glTranslatef(0.0f,0.0f,0.0f);
  glColor4f(0.0f,0.0f,1.0f,0.5f);
  glBegin(GL_LINE_STRIP);
  glVertex3f((float)(ac->pos[ac->index].x),(float)(-ac->pos[ac->index].z),(float)(ac->pos[ac->index].y));
  glVertex3f((float)(ac->pos[ac->index].x),                          0.0f,(float)(ac->pos[ac->index].y));
  glEnd();

  glPopMatrix();
}


void
draw_path_fade( xyz_t * xyz, int index, float R, float G, float B, float A, float width)
{
  glPushMatrix();

  uint16_t k;
  uint16_t j;

  glLineWidth(width);

  glBegin(GL_LINE_STRIP);	
  j = index;
  float nInv = 1.0/NUM_AC_VIS_HISTORY_POINTS;
  for (k=0;k<NUM_AC_VIS_HISTORY_POINTS;k++){
    j++;
    if (j==NUM_AC_VIS_HISTORY_POINTS) j=0;
    glColor4f(R,G,B, k*nInv*A);
    glVertex3f((float)(xyz[j].x),(float)(xyz[j].y),(float)(xyz[j].z));
  }
  glEnd();					// Done Drawing The path

  glPopMatrix();

  glLineWidth(1.0f);
}


void
draw_aircraft_trails( aircraft_t * ac )
{
  if (ac->initialized == 0)
    return;

  draw_path_fade( ac->pos, ac->index, 1.0f,0.0f,0.0f,1.0f, 0.1*ac->wingspan);
  draw_path_fade( ac->l_wingtip, ac->index, 0.952941f, 0.050980f, 0.964706f, 0.784314f, 0.1*ac->wingspan);
  draw_path_fade( ac->r_wingtip, ac->index, 0.952941f, 0.050980f, 0.964706f, 0.784314f, 0.1*ac->wingspan);

}
