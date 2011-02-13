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
 * keyboard_mouse.h
 * openGL callbacks for keyboard and mouse input
 */

#ifndef __KEYBOARD_MOUSE_H__
#define __KEYBOARD_MOUSE_H__

typedef struct {
  int button;
  int x0;
  int y0;
} mouse_rotation_t;

void key_pressed(unsigned char key, int x, int y);
void mouse_clicked(int button, int state, int x, int y);
void mouse_moved( int x, int y);

#endif // __KEYBOARD_MOUSE_H__
