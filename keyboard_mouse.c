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
 * keyboard_mouse.c
 * openGL callbacks for keyboard and mouse input
 */

#include <GL/gl.h>
#include <GL/glut.h>
#include <unistd.h>     // needed to sleep
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "keyboard_mouse.h"
#include "camera_manager.h"
#include "visualizer.h"

/* ASCII code for the escape key. */
#define ESCAPE 27

mouse_rotation_t mouse_rotation = {
  .x0 = 0,
  .y0 = 0,
  .button = 0,
};

/* The function called whenever a key is pressed. */
void
key_pressed(unsigned char key, int x, int y)
{
  // supress unused variable warnings
  (void)x;
  (void)y;
  /* avoid thrashing this call */
  usleep(100);

  camera_pose_t * camera_pose = get_camera_pose();
  
  switch(key)
  {
  case 'w':
  {
    camera_pose->focus_x += 10.0f*cos(camera_pose->phi);
    camera_pose->focus_y += 10.0f*sin(camera_pose->phi);
    set_camera();
    break;
  }
  case 's':
  {
    camera_pose->focus_x -= 10.0f*cos(camera_pose->phi);
    camera_pose->focus_y -= 10.0f*sin(camera_pose->phi);
    set_camera();
    break;
  }
  case 'd':
  {
    camera_pose->focus_x -= 10.0f*sin(camera_pose->phi);
    camera_pose->focus_y += 10.0f*cos(camera_pose->phi);
    set_camera();
    break;
  }
  case 'a':
  {
    camera_pose->focus_x += 10.0f*sin(camera_pose->phi);
    camera_pose->focus_y -= 10.0f*cos(camera_pose->phi);
    set_camera();
    break;
  }
  case 'r':
  {
    camera_pose->theta += 10.0f*3.14159f/180.0f;
    if (camera_pose->theta > 80.0f*3.14159f/180.0f)
      camera_pose->theta = 80.0f*3.14159f/180.0f;

    set_camera();
    break;
  }
  case 'f':
  {
    camera_pose->theta -= 10.0f*3.14159f/180.0f;
    if (camera_pose->theta < -80.0f*3.14159f/180.0f)
      camera_pose->theta = -80.0f*3.14159f/180.0f;

    set_camera();
    break;
  }
  case 'q':
  {
    camera_pose->phi += 10.0f*3.14159f/180.0f;
    set_camera();
    break;
  }
  case 'e':
  {
    camera_pose->phi -= 10.0f*3.14159f/180.0f;
    set_camera();
    break;
  }
  case 'x':
  {
    camera_pose->rho -= 10.0f;
    if (camera_pose->rho < 10.0f)
      camera_pose->rho = 10.0f;
    if (camera_pose->rho > 4000.0f)
      camera_pose->rho = 4000.0f;
    set_camera();
    break;
  }
  case 'z':
  {
    camera_pose->rho += 10.0f;
    set_camera();
    break;
  }

  case ESCAPE:
  {
  /* If escape is pressed, kill everything. */
    // glutDestroyWindow(window);
    exit(0);
    printf("ignoring ESCAPE key glutDestroyWindow call\n");
    break;
  }
  default:
    break;
  }// switch(key)

}

void
mouse_clicked(int button, int state, int x, int y)
{
  /* avoid thrashing this call - imitating key_pressed, still not sure why this is here */
  usleep(100);
  mouse_rotation.button = button;
  mouse_rotation.x0 = x;
  mouse_rotation.y0 = y;

  camera_pose_t * camera_pose = get_camera_pose();

  switch (mouse_rotation.button)
  {
  case GLUT_MIDDLE_BUTTON: // middle button - toggle draw camera focus
  {
    if (state == 0)
      camera_pose->draw_focus = 1;
    else
      camera_pose->draw_focus = 0;
    break;
  }
  case 3: // wheel up (zoom in)
  {
    if (state == 0){
      camera_pose->rho -= 25.0f;
      if (camera_pose->rho < 1.0f)
        camera_pose->rho = 1.0f;
      set_camera();
    }
    break;
  }
  case 4: // wheel down (zoom out)
  {
    if (state == 0){
      camera_pose->rho += 25.0f;
      set_camera();
    }
    break;
  }
  case GLUT_RIGHT_BUTTON: // pan
  {
    if (state == 0)
      camera_pose->draw_focus = 1;
    else
      camera_pose->draw_focus = 0;
    break;
  }

  default:
  {
//    printf("mouse button: %d\n", mouse_rotation.button);
    break;
  }
  } // switch(mouse_rotation.button)
}

void
mouse_moved( int x, int y)
{
  usleep(100);

  camera_pose_t * camera_pose = get_camera_pose();
  int window_width = get_window_width();
  int window_height = get_window_height();

  switch (mouse_rotation.button)
  {
  case GLUT_LEFT_BUTTON: // rotate
  {
    camera_pose->phi   -= ((float)( mouse_rotation.x0 - x ))/((float)(window_width))*2.0*3.14;
    camera_pose->theta -= ((float)( mouse_rotation.y0 - y ))/((float)(window_height))*2.0*3.14;
    if (camera_pose->theta > 0.5*3.14)
      camera_pose->theta = 0.5*3.14;
    if (camera_pose->theta < -0.5*3.14f)
      camera_pose->theta = -0.5*3.14f;
    mouse_rotation.x0 = x;
    mouse_rotation.y0 = y;
    set_camera();
    break;
  }
  case GLUT_RIGHT_BUTTON: // pan
  {
    camera_pose->focus_x += 500.0f*cos(camera_pose->phi)*((float)(y - mouse_rotation.y0))/((float)(window_height));
    camera_pose->focus_y += 500.0f*sin(camera_pose->phi)*((float)(y - mouse_rotation.y0))/((float)(window_height));;

    camera_pose->focus_x += 500.0f*sin(camera_pose->phi)*((float)(x - mouse_rotation.x0))/((float)(window_width));
    camera_pose->focus_y -= 500.0f*cos(camera_pose->phi)*((float)(x - mouse_rotation.x0))/((float)(window_width));;

    mouse_rotation.x0 = x;
    mouse_rotation.y0 = y;

    set_camera();
    break;
  }
/*
  case GLUT_MIDDLE_BUTTON: // zoom
  {
    camera_pose->rho += 1000.0f*((float)(y - mouse_rotation.y0))/((float)(window_height));
    if (camera_pose->rho < 1.0f)
      camera_pose->rho = 1.0f;
    if (camera_pose->rho > 2000.0f)
      camera_pose->rho = 2000.0f;
    mouse_rotation.x0 = x;
    mouse_rotation.y0 = y;
    set_camera();
    break;
  }
*/
  case GLUT_MIDDLE_BUTTON: // move camera focus
  {
    camera_pose->focus_z += 500.0f*((float)(y - mouse_rotation.y0))/((float)(window_height));
    if (camera_pose->focus_z > 0.0f)
      camera_pose->focus_z = 0.0f;
    mouse_rotation.x0 = x;
    mouse_rotation.y0 = y;
    set_camera();
    break;
  }
  default:
    break;
  }// switch (mouse_rotation.button)
}
