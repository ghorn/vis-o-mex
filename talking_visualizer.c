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
 * talking_visualizer.c
 * ???
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <ap_types.h>

FILE * speech_pipe;
uint8_t speech_pipe_found;

/*
void init_visualizer_speech()
{
  printf("checking for speech pipe\n");
  int speech_fd;
  if ( (speech_fd = open("speech_pipe", O_NONBLOCK | O_RDWR )) < 0 ){
    printf("speech pipe not found, visualizer will not blab\n");
    speech_pipe_found = 0;
    return;
  }
  printf("speech pipe found\n");

  if ( (speech_pipe = fdopen( speech_fd, "wr")) == NULL ){
    printf("error opening file pointer from file descriptor\n");
    speech_pipe_found = 0;
    return;
  }

  speech_pipe_found = 1;
  printf("speech initialized successfully\n");
}

static void visualizer_say(const char * message)
{
  if (speech_pipe_found == 0)
    return;

//  char system_say_message[200];
//  sprintf(system_say_message, "say '%s' &", message);
//  fprintf(speech_pipe,"%s",system_say_message);

  char system_message[200];
  sprintf(system_message,"echo '%s\n' > speech_pipe &",message);
  system( system_message);
}
*/

void
init_visualizer_speech(){}

void
visualizer_say(const char * message)
{
  char system_say_message[200];
  sprintf(system_say_message, "say '%s' &", message);
  system(system_say_message);
}


void
visualizer_talk( const sensors2Estimator_t * const s2e __attribute__((unused)),
                 const est2User_t * const e2u __attribute__((unused)),
                 const rc_t * const rc __attribute__((unused)))
{
#ifdef USE_SPEECH
  // gps status
  static uint8_t prev_soln_valid = 123;
  if (prev_soln_valid != s2e->gpsPhys.solution_valid){
    switch (s2e->gpsPhys.solution_valid) {
    case 0:
      visualizer_say("gps lost");
      break;
    case 1:
      visualizer_say("phase lock");
      break;
    case 2:
      visualizer_say("code lock");
      break;
    }
    prev_soln_valid = s2e->gpsPhys.solution_valid;
  }

  // rc mode status
  static uint8_t old_rc_mode = 123;
  uint8_t new_rc_mode = rc->mode*3 + rc->aux2;
  
  if (old_rc_mode != new_rc_mode){
    char rc_message[80];
    switch (rc->aux2){
    case 0:
      sprintf(rc_message, "mode %d delta", rc->mode+1);
      break;
    case 1:
      sprintf(rc_message, "mode %d echo", rc->mode+1);
      break;
    case 2:
      sprintf(rc_message, "mode %d foxtrot", rc->mode+1);
      break;
    }
    visualizer_say(rc_message);
    old_rc_mode = new_rc_mode;
  }
#endif
}
