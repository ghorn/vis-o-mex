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
 * spline_utils.h
 * Wrap gsl spline alloc/free, also some spline types
 */

#ifndef __SPLINE_UTILS_H__
#define __SPLINE_UTILS_H__

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include <inttypes.h>
#include <ap_types.h>

#define MAX_NUM_SPLINE_KNOTS 128

/********** acknowledgements *********/
#define WILCO 0
#define NEGATIVE 1

typedef struct {
  gsl_spline * spl;
  gsl_interp_accel * acc;
} spline_t;

typedef struct {
  spline_t x;
  spline_t y;
  spline_t z;
  spline_t thrust;
  spline_t roll;
  spline_t airspeed;
} splines_t;

typedef struct {

  uint8_t gamma_lock;
  double gamma;
  spline_knot_data_t knot_data;
  splines_t splines;
  
} spline_traj_t;

#ifdef __cplusplus
extern "C"{
#endif
  
void reticulate_splines_from_knots(splines_t * splines, spline_knot_data_t * knot_data, int periodic);
void free_splines(splines_t * splines);

#ifdef __cplusplus
}
#endif
  

#endif // __SPLINE_UTILS_H__
