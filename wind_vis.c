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
 * wind_vis.c
 * ???
 */

#include <GL/gl.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
#include <time.h> // for random seed
#include <ap_types.h>
#include <ap_telemetry.h>

#include "wind_vis.h"
#include <ap_types.h>

#define NUM_PARTICLES 1000

#define MIN_POS_X -400.0
#define MAX_POS_X  400.0
#define MIN_POS_Y -400.0
#define MAX_POS_Y  400.0
#define MIN_POS_Z -300.0
#define MAX_POS_Z    0.0

typedef struct {
  xyz_t pos;
  xyz_t vel;
} dust_particle_t;

dust_particle_t particles[NUM_PARTICLES];
xyz_t true_wind;
static gsl_rng * rng;
int got_first_wind_message = 0;

static void spawn_particle(dust_particle_t * particle)
{
  // setup new particle
  particle->pos.x = gsl_ran_flat(rng, MIN_POS_X, MAX_POS_X);
  particle->pos.y = gsl_ran_flat(rng, MIN_POS_Y, MAX_POS_Y);
  particle->pos.z = gsl_ran_flat(rng, MIN_POS_Z, MAX_POS_Z);
  particle->vel.x = true_wind.x;
  particle->vel.y = true_wind.y;
  particle->vel.z = true_wind.z;
}

static void
true_wind_handler(const lcm_recv_buf_t *rbuf __attribute__((unused)), 
                  const char *channel __attribute__((unused)), 
                  const ap_true_wind_t *msg, 
                  void *user __attribute__((unused)))
{
  memcpy( &true_wind, &(msg->vel_ned), sizeof(xyz_t) );

  // initialize particles upon first wind message
  if (got_first_wind_message == 0)
  {
    for (int k=0; k<NUM_PARTICLES; k++)
      spawn_particle( &(particles[k]) );
    got_first_wind_message = 1;
  }
}

static void wrap_one_dimension( double * pos, const double pos_min, const double pos_max )
{
  if (*pos < pos_min)
    *pos += pos_max - pos_min;
  else if (*pos > pos_max)
    *pos -= pos_max - pos_min;
}

void draw_wind_vis(double dt)
{
  // don't do anything until first wind message
  // arrives and particles are initialized
  if (got_first_wind_message == 0)
    return;

  glPushMatrix();
  glPointSize(1.0);
  glBegin(GL_POINTS);
  glColor4f(1.0f, 1.0f, 1.0f, 0.2f);

  for (int k=0; k<NUM_PARTICLES; k++)
  {
    // integrate particle dynamics
    xyz_t acceleration;
    acceleration.x = (true_wind.x - particles[k].vel.x)/dt;
    acceleration.y = (true_wind.y - particles[k].vel.y)/dt;
    acceleration.z = (true_wind.z - particles[k].vel.z)/dt;
    particles[k].vel.x = true_wind.x;
    particles[k].vel.y = true_wind.y;
    particles[k].vel.z = true_wind.z;
    particles[k].pos.x += dt*particles[k].vel.x + 0.5*dt*dt*acceleration.x;
    particles[k].pos.y += dt*particles[k].vel.y + 0.5*dt*dt*acceleration.y;
    particles[k].pos.z += dt*particles[k].vel.z + 0.5*dt*dt*acceleration.z;

    // wrap particle if out of bounds
    wrap_one_dimension( &(particles[k].pos.x), MIN_POS_X, MAX_POS_X );
    wrap_one_dimension( &(particles[k].pos.y), MIN_POS_Y, MAX_POS_Y );
    wrap_one_dimension( &(particles[k].pos.z), MIN_POS_Z, MAX_POS_Z );

    // draw particle
    glVertex3f( particles[k].pos.x, particles[k].pos.y, particles[k].pos.z );
  }

  glEnd();
  glPopMatrix();
}

void init_wind_vis()
{
  // setup random number generator
  const gsl_rng_type * T;	
  gsl_rng_env_setup();
  T = gsl_rng_default;
  rng = gsl_rng_alloc (T);
  gsl_rng_set(rng,time(NULL));

  // subscribe to true wind message
  ap_lcm_subscribe_chan(true_wind_t, &true_wind_handler, NULL, "ap_true_wind_t");
}
