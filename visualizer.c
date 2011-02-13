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
 * visualizer.c
 * 3D visualizer - draws based on telemetry from simulated or real vehicle
 *
 * Note: This code is based on many excellent openGL tutorials from the NeHe website
 *       (http://nehe.gamedev.net/)
 */

#include <GL/glut.h>
#include <GL/gl.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include <ap_types.h>

#include "aircraft_vis.h"
#include "camera_manager.h"
#include "draw_text.h"
#include "drawing_functions.h"
#include "imagery_manager.h"
#include "keyboard_mouse.h"
#include "talking_visualizer.h"
#include "simple_model_vis.h"
#include "spline_geometry_vis.h"
#include "spline_trajectory_vis.h"
#include "visualizer_telemetry.h"
#include "visualizer_types.h"
#include "tether_vis.h"
#include "wind_vis.h"


static void
run_realtime(double dt);

double session_high_score;
double current_score;

/* The number of our GLUT window */
int window;
camera_pose_t camera_pose;
mouse_rotation_t mouse_rotation;

int window_width  = 1200;
int window_height = 900;

int
get_window_width()
{
  return window_width;
}

int
get_window_height()
{
  return window_height;
}

/* The function called when our window is resized */
void
resize_window(int width, int height)
{
  if (height==0) // Prevent A Divide By Zero If The Window Is Too Small
    height=1;
  
  window_width  = width;
  window_height = height;

  glViewport(0, 0, window_width, window_height); // Reset The Current Viewport And Perspective Transformation

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(50.0f,(GLfloat)window_width/(GLfloat)window_height,0.1f,100000.0f);
	
  set_camera();
  glMatrixMode(GL_MODELVIEW);
	
  glLoadIdentity();
	
  glutPostRedisplay ();
}


static void
manage_high_score( const est2User_t * const e2u, const rc_t * const rc )
{
  static double time_autonomous_started;
  static uint8_t currently_autonomous = 0;
  
  uint8_t rc_autonomous_switches = (rc->mode == 2) && (rc->aux2 == 2);

  // detect the beginning of autonomous mode
  if ( (currently_autonomous == 0) && (rc_autonomous_switches == 1) ){
    currently_autonomous = 1;
    time_autonomous_started = e2u->timeDouble;
  }

  // detect the end of autonomous mode
  if ( rc_autonomous_switches == 0 ){
    currently_autonomous = 0;
    return;
  }

  // update current score
  current_score = e2u->timeDouble - time_autonomous_started;
  
  // update session score
  if (current_score > session_high_score)
    session_high_score = current_score;


  // new high score audio message  
  static uint8_t high_score_smashed = 0;
  if (current_score > ALL_TIME_HIGH_SCORE){
    if (high_score_smashed == 0){
      visualizer_say("NEW HIGH SCORE!");
      high_score_smashed = 1;
    }
  }
}

void
draw_scene()
{
  static int first_run = 1;
  static aircraft_t * ac;
  static tether_vis_t tether_vis;
  static spline_geometry_t spline_geometry;
  static rc_t rc;
  static est2User_t e2u;
  static sensors2Estimator_t s2e;
  static system_energy_t se;

  if (first_run == 1){
    ac = create_aircraft(11.0);
    first_run = 0;
    /* set up telemetry stream */
    initialize_visualizer_telemetry(&spline_geometry, &rc, &e2u, &s2e, ac, &tether_vis, &se);
    init_spline_trajectory_vis();
    init_wind_vis();
    init_simple_model_vis();
  }

  telemetry_periodic();
  manage_high_score( &e2u, &rc);

  // Render GL stuff
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
  glPushMatrix();

  // put the visualizer in NED
  glRotatef( 90.0f, 1.0f, 0.0f, 0.0f);

  draw_grid();
  draw_tether(&tether_vis);
  draw_axes_from_euler(0.0f,0.0f,0.0f,0.0f,0.0f,-1.0f,30.0f,8.0f); // origin
  draw_aircraft( ac, 0 );
  draw_aircraft( ac, 1 );
  draw_aircraft_trails( ac );
  draw_all_spline_trajectories();
  draw_spline_geometry( &spline_geometry, ac->wingspan );
  draw_wind_est( &e2u );
  draw_wind_vis( DT );
  draw_simple_model_vis();
  draw_camera_focus();

  // satellite imagery
  manage_imagery();
 
  draw_text( &rc, &e2u, &s2e, &se, window_width, window_height, current_score, session_high_score );

  glPopMatrix();

  visualizer_talk( &s2e, &e2u, &rc);

  run_realtime( DT );

  // swap the buffers to display, since double buffering is used.
  glutSwapBuffers();
}


void
init_gl()
{
  init_imagery();

  //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);         // This Will Clear The Background Color To Black
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);         // grey background
  //glClearColor(0.5216f, 0.839216f, 0.913725f, 1.0f);        // blue sky background
  glClearDepth(1.0);                            // Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LESS);                         // The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);                      // Enables Depth Testing
  glShadeModel(GL_SMOOTH);                      // Enables Smooth Color Shading
 
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
  glPointSize(3.0);
  glEnable(GL_POINT_SMOOTH);
  
  set_camera();
}


void
setup_glut(){
  int dummyc = 1;
  char * dummyv = "";
  glutInit(&dummyc, &dummyv);

  /* Select type of Display mode:   
     Double buffer 
     RGBA color
     Alpha components supported 
     Depth buffered for automatic clipping */  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  

  /* get a window_width x window_height window */
  glutInitWindowSize(window_width, window_height);  

  /* the window starts at the upper left corner of the screen */
  glutInitWindowPosition(0, 0);

  /* Open a window */  
  window = glutCreateWindow("vis-o-mex");

  /* Register the function to do all our OpenGL drawing. */
  glutDisplayFunc(&draw_scene);  

  /* Go fullscreen.  This is as soon as possible. */
  //  glutFullScreen();

  /* Even if there are no events, redraw our gl scene. */
  // note: should probably take the frame rate function and put it here, instead of in draw_scene
  glutIdleFunc(&draw_scene);

  /* Register the function called when our window is resized. */
  glutReshapeFunc(&resize_window);

  /* Register the function called when the keyboard/mouse is pressed. */
  glutKeyboardFunc(&key_pressed);
  glutMouseFunc(&mouse_clicked);
  glutMotionFunc(&mouse_moved);

}

int
main()
{
  /* Initialize GLUT state - glut will take any command line arguments that pertain to it or 
     X Window - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */  

  // initialize score
  current_score = 0;
  session_high_score = 0;

  setup_glut();

  init_gl( window_width, window_height);

  init_visualizer_speech();

  /* Start Event Processing Engine */  
  glutMainLoop();  

  return 0; 
}

static uint64_t
us_since_epoch()
{
 
  struct timeval tv;
 
  uint64_t micros = 0;
 
  gettimeofday(&tv, NULL);  
  micros =  ((uint64_t)tv.tv_sec) * 1000000 + tv.tv_usec;
 
  return micros;
}

static void
run_realtime(double dt)
{
  uint64_t dt_us = (uint64_t)(dt*1.0e6);
  static uint64_t us_old = 0;

  uint64_t us_now = us_since_epoch();
  uint64_t us_next = us_old + dt_us;

  if (us_next > us_now){
    us_old = us_next;
    usleep(us_next - us_now);
  } else {
    us_old = us_now;
  }
}
