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
 * visualizer_telemetry.c
 * Telemetry handler for the visualizer
 */

#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <inttypes.h>
#include <stdio.h>


#include <lcm_telemetry_auto.h>
#include <lcm_interface.h>
#include <ap_telemetry.h>
#include <vis_telemetry.h>
#include <vis_types.h>

#include "visualizer_telemetry.h"
#include "visualizer_types.h"
#include "spline_trajectory_vis.h"

#include "imagery_manager.h"

static void
est2UserHandler(const lcm_recv_buf_t *rbuf __attribute__((unused)), 
                const char *channel __attribute__((unused)), 
                const ap_est2User_t *msg, 
                void *e2u)
{
  memcpy( e2u, msg, sizeof(est2User_t) );

  static uint8_t alt_below_20 = 0;
  
  double alt = -msg->x.r_n2b_n.z;

  if ((alt < 5) && (alt_below_20 == 0)){
//    system("say 'altitude warning lol' &");
    alt_below_20 = 1;
  }
  if (alt > 20)
    alt_below_20 = 0;

}

static void
sensors2EstimatorHandler(const lcm_recv_buf_t *rbuf __attribute__((unused)), 
                         const char *channel __attribute__((unused)), 
                         const ap_sensors2Estimator_t *msg, 
                         void *user)
{
  
  sensors2Estimator_t *s2e = (sensors2Estimator_t *) user;
  memcpy(s2e, msg, sizeof(sensors2Estimator_t));

  update_imagery_origin(&(s2e->gpsPhys));

}

ap_lcm_copy_handler(rc_t);
vis_lcm_copy_handler(system_energy_t);
  
void 
initialize_visualizer_telemetry(rc_t *rc_ext,
                                est2User_t *e2u,
                                sensors2Estimator_t *s2e_ext,
                                system_energy_t * se)
{
//  ap_lcm_init(NULL);
  lcm_init("udpm://239.255.76.67:7667?ttl=1"); 
  ap_lcm_subscribe(est2User_t, &est2UserHandler, e2u);
  ap_lcm_subscribe(sensors2Estimator_t, &sensors2EstimatorHandler, s2e_ext);
  ap_lcm_subscribe_cp(rc_t, rc_ext);
  vis_lcm_subscribe_chan_cp(system_energy_t, se, "sim_system_energy_t");
}

void
telemetry_periodic(void)
{
  vis_telemetry_send();
  lcm_check(vis_lcm.lcm, vis_lcm.fd);
  lcm_check(ap_lcm.lcm, ap_lcm.fd);
  lcm_check(simple_model_sim_lcm.lcm, simple_model_sim_lcm.fd);
}
