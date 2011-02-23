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
 * init_and_draw_simple_aircraft.c
 * Pretty self explanatory I hope.
 *
 * Notice that DT, as defined in the makefile, is 0.01 or 100Hz. This is
 * the rate at which your inner or periodic loop runs. The vis_example_pose_only
 * telemetry is actually sent at a different rate, as defined in telemetry.xml
 */


#include <math.h>
#include <unistd.h>

#include <lcm_interface.h>
#include <vis_example_pose_only_telemetry.h>
#include <vis_example_pose_only_types.h>
#include <vis_types.h>

#define T_END 10.0
#define ID 3

pose_t pose;
#include <telemetry/vis_example_pose_only_pose_t_pose.h>


static void
send_object_manager_init()
{
  // send initial command which triggers object_manager to set up the ac
  // first set up the struct
  object_manager_init_t omi;

  // type
  object_type_t type = simple_aircraft;
  omi.type = type;

  // parameters
  simple_aircraft_params_t params = {
    .wingspan = 10,
    .aspect_ratio = 6,
    .alpha = 1
  };
  memcpy( omi.params, &params, sizeof(simple_aircraft_params_t) );

  // id
  omi.id = ID;

  // name
  sprintf((char*)(omi.name), "example_ac");

  // send the struct
  vis_example_pose_only_lcm_send_chan(&omi, object_manager_init_t, "object_manager_init");
}


static void
set_interesting_pose(const double t)
{
  // make pose interesting
  double q = 2.0*M_PI*t/T_END;
  
  pose.r_n2b_n.x = 10*cos(q);
  pose.r_n2b_n.y = 10*sin(q);
  pose.r_n2b_n.z = -10*(10+cos(2*q));
  
  pose.q_n2b.q0 = cos(0.5*3.0*q);
  pose.q_n2b.q1 = sin(0.5*3.0*q);
  pose.q_n2b.q2 = 0.0;
  pose.q_n2b.q3 = 0.0;
}


int
main()
{
  // initialize lcm
  vis_example_pose_only_lcm_init("udpm://239.255.76.67:7667?ttl=1"); 

  // tell give the visualizer the drawing parameters for this ID
  send_object_manager_init();

  // now send pose for T_END seconds
  pose.id = ID;
  for (double t=0; t<T_END; t+=DT)
  {
    // sleep to make sure DT is about right
    usleep( (int)(DT*1.0e6) );
    
    // make the ac wobble around
    set_interesting_pose(t);

    // call the periodic telemetry fcn
    vis_example_pose_only_telemetry_send();
  }  
}
