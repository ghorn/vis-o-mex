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
 * visualizer_telemetry.h
 * Telemetry handler for the visualizer
 */

#ifndef __VISUALIZER_TELEMETRY_H__
#define __VISUALIZER_TELEMETRY_H__

#include "spline_trajectory_vis.h"
#include "spline_geometry_vis.h"
#include <ap_types.h>

/* Set up the telemetry link with pointers to the persistent structs
 * that need updating upon message arrival */
void
initialize_visualizer_telemetry(rc_t *rc_ext,
                                est2User_t *e2u_ext,
                                sensors2Estimator_t *s2e_ext,
                                system_energy_t * se);

void
telemetry_periodic(void);

#endif // __VISUALIZER_TELEMETRY_H__
