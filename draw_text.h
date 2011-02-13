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
 * draw_text.h
 * Overlay 2D text on the screen
 */

#ifndef __DRAW_TEXT_H__
#define __DRAW_TEXT_H__

void draw_text(const rc_t * const rc,
               const est2User_t * const e2u,
               const sensors2Estimator_t * const s2e,
               const system_energy_t * const se,
               const int window_width,
               const int window_height,
               const double current_score,
               const double session_high_score);


#endif // __DRAW_TEXT_H__
