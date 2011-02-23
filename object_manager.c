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
 * object_manager.c
 * Keeps track of all objects (aircraft only right now) and their properties,
 * draws them all on command.
 */

#include <inttypes.h>

#include <vis_telemetry.h>
#include <vis_types.h>

#include "simple_aircraft_vis.h"

#define MAX_NUM_OBJECTS 128

typedef struct {
  int allocated;
  int id;
  char name[80];
  object_type_t type;
  void * obj_ptr;
} vis_object_item_t;

vis_object_item_t list_of_objects[MAX_NUM_OBJECTS];

// returns the object's index if already in list, otherwise return -1
static int
index_from_object_id(int id)
{
  for (int k=0; k<MAX_NUM_OBJECTS; k++)
    if ((list_of_objects[k].allocated == 1) && (list_of_objects[k].id == id))
      return k;
  return -1;
}

// returns the first free index, returns -1 if no free indices
static int
get_first_free_index()
{
  for (int k=0; k<MAX_NUM_OBJECTS; k++)
    if (list_of_objects[k].allocated == 0)
      return k;
  return -1;
}


static void *
allocate_vis_object(const object_type_t type, const void * obj_params)
{
  switch (type){
  case simple_aircraft:
  {
    const simple_aircraft_params_t * params = (simple_aircraft_params_t *)obj_params;
    return (void*) alloc_simple_aircraft(params);

    break;
  }
  default:
  {
    printf("allocate_vis_object() received unrecognized type of object (%d)\n", type);
    exit(1);
  }
  }
}

static void
free_vis_object(const object_type_t type, void * obj_ptr)
{
  switch (type){
  case simple_aircraft:
  {
    free_simple_aircraft( (simple_aircraft_t*)obj_ptr);
    break;
  }
  default:
  {
    printf("free_vis_object() received unrecognized type of object (%d)\n", type);
    exit(1);
  }
  }
}


static void
object_manager_init_handler(const lcm_recv_buf_t *rbuf __attribute__((unused)),
                            const char *channel __attribute__((unused)),
                            const vis_object_manager_init_t *msg __attribute__((unused)),
                            void *user __attribute__((unused)))
{
  printf("received object_manager_init, "
         "name: %s, id: %d, type: %d\n", msg->name, msg->id, msg->type);

  int index = index_from_object_id(msg->id);

  // if not yet in list, then add to list
  if (index == -1){
    index = get_first_free_index();
    if (index == -1){
      printf("object manager list of objects full :(\n");
      exit(1);
    }
    
    printf("object allocating\n");

    vis_object_item_t * vis_obj = &(list_of_objects[index]);

    memcpy( vis_obj->name, msg->name, 80*sizeof(char) );
    vis_obj->id = msg->id;
    vis_obj->type = (object_type_t)(msg->type);

    vis_obj->obj_ptr = allocate_vis_object(vis_obj->type, msg->params);
    vis_obj->allocated = 1;


  } else { // if already in list, re-initialize
    printf("object re-allocating\n");

    vis_object_item_t * vis_obj = &(list_of_objects[index]);

    // first free
    free_vis_object(vis_obj->type, vis_obj->obj_ptr );

    // then allocate again
    memcpy( vis_obj->name, msg->name, 80*sizeof(char) );
    vis_obj->id = msg->id;
    vis_obj->type = (object_type_t)(msg->type);

    vis_obj->obj_ptr = allocate_vis_object(vis_obj->type, msg->params);

  }

}

static void
pose_handler(const lcm_recv_buf_t *rbuf __attribute__((unused)), 
             const char *channel __attribute__((unused)), 
             const vis_pose_t *msg, 
             void *user __attribute__((unused)))
{
  int index = index_from_object_id(msg->id);

  if (index == -1)
    printf("pose message recieved but object_manager_init not yet received for that ID :(\n");
  else {
    vis_object_item_t * vis_obj = &(list_of_objects[index]);
    switch (vis_obj->type){
    case simple_aircraft:
    {
      update_aircraft_pose( (xyz_t*)&(msg->r_n2b_n), 
                            (quat_t*)&(msg->q_n2b), 
                            (simple_aircraft_t*)(vis_obj->obj_ptr));
      break;
    }
    default:
    {
      printf("no pose update function defined for this type (%d)\n", vis_obj->type);
      exit(1);
    }
    }
  }
}



void
init_object_manager()
{
  // initialize list of vis objects to unallocated
  for (int k=0; k<MAX_NUM_OBJECTS; k++)
    list_of_objects[k].allocated = 0;

  // subscribe to the object_manager_object init channel
  vis_lcm_subscribe_chan(object_manager_init_t, &object_manager_init_handler, NULL, "object_manager_init");

  // subscribe to the pose channel
  vis_lcm_subscribe_chan(pose_t, &pose_handler, NULL, "pose");
}



void
draw_object_manager_objects()
{
  for (int k=0; k<MAX_NUM_OBJECTS; k++){
    if (list_of_objects[k].allocated){
      switch (list_of_objects[k].type){
      case simple_aircraft:
      {
        const simple_aircraft_t * ac = (simple_aircraft_t*)(list_of_objects[k].obj_ptr);
        draw_simple_aircraft( ac, 0 );
        draw_simple_aircraft_trails( ac );
        draw_simple_aircraft( ac, 1 );
        break;
      }
      default:
        printf("no draw function set up for this object type type (%d)\n", list_of_objects[k].type);
        exit(1);
      }
    }
  }
}
