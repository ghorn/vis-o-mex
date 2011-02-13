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
 * spline_utils.c
 * Wrap gsl spline alloc/free, also some spline types
 */

#include "spline_utils.h"

void reticulate_splines_from_knots(splines_t * splines, spline_knot_data_t * knot_data, int periodic)
{

  // allocate and fill the spline structs
  splines->x.acc        = gsl_interp_accel_alloc();
  splines->y.acc        = gsl_interp_accel_alloc();
  splines->z.acc        = gsl_interp_accel_alloc();
  splines->thrust.acc   = gsl_interp_accel_alloc();
  splines->roll.acc     = gsl_interp_accel_alloc();
  splines->airspeed.acc = gsl_interp_accel_alloc();

  if (periodic == 1){
    splines->x.spl        = gsl_spline_alloc(gsl_interp_cspline_periodic, knot_data->num_knots);
    splines->y.spl        = gsl_spline_alloc(gsl_interp_cspline_periodic, knot_data->num_knots);
    splines->z.spl        = gsl_spline_alloc(gsl_interp_cspline_periodic, knot_data->num_knots);
    splines->thrust.spl   = gsl_spline_alloc(gsl_interp_cspline_periodic, knot_data->num_knots);
    splines->roll.spl     = gsl_spline_alloc(gsl_interp_cspline_periodic, knot_data->num_knots);
    splines->airspeed.spl = gsl_spline_alloc(gsl_interp_cspline_periodic, knot_data->num_knots);
  } else {
    splines->x.spl        = gsl_spline_alloc(gsl_interp_cspline, knot_data->num_knots);
    splines->y.spl        = gsl_spline_alloc(gsl_interp_cspline, knot_data->num_knots);
    splines->z.spl        = gsl_spline_alloc(gsl_interp_cspline, knot_data->num_knots);
    splines->thrust.spl   = gsl_spline_alloc(gsl_interp_cspline, knot_data->num_knots);
    splines->roll.spl     = gsl_spline_alloc(gsl_interp_cspline, knot_data->num_knots);
    splines->airspeed.spl = gsl_spline_alloc(gsl_interp_cspline, knot_data->num_knots);
  }

  double zero_to_one[knot_data->num_knots];
  for (int j=0; j<knot_data->num_knots; j++)
    zero_to_one[j] = ((double)(j))/((double)(knot_data->num_knots) - 1.0);

  gsl_spline_init( splines->x.spl,        zero_to_one, knot_data->x,        knot_data->num_knots);
  gsl_spline_init( splines->y.spl,        zero_to_one, knot_data->y,        knot_data->num_knots);
  gsl_spline_init( splines->z.spl,        zero_to_one, knot_data->z,        knot_data->num_knots);
  gsl_spline_init( splines->thrust.spl,   zero_to_one, knot_data->thrust,   knot_data->num_knots);
  gsl_spline_init( splines->roll.spl,     zero_to_one, knot_data->roll,     knot_data->num_knots);
  gsl_spline_init( splines->airspeed.spl, zero_to_one, knot_data->airspeed, knot_data->num_knots);

}


void free_splines(splines_t * splines)
{
  gsl_interp_accel_free((splines->x.acc));
  gsl_interp_accel_free((splines->y.acc));
  gsl_interp_accel_free((splines->z.acc));
  gsl_interp_accel_free((splines->thrust.acc));
  gsl_interp_accel_free((splines->roll.acc));
  gsl_interp_accel_free((splines->airspeed.acc));

  gsl_spline_free( (splines->x.spl) );
  gsl_spline_free( (splines->y.spl) );
  gsl_spline_free( (splines->z.spl) );
  gsl_spline_free( (splines->thrust.spl) );
  gsl_spline_free( (splines->roll.spl) );
  gsl_spline_free( (splines->airspeed.spl) );
}
