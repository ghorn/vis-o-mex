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
  * imagery_drawer.c
  * draw satellite imagery which is loaded with imagery_manager.c
  */

#include <GL/gl.h>
#include <stdio.h>

#ifdef OSX
#include <imlib2.h>
#elif LINUX
#include <Imlib2.h>
#endif


#include "imagery_drawer.h"

float imagery_transparency = 1.0f;

void
set_imagery_transparency(float alpha)
{
  imagery_transparency = alpha;
}

void
draw_tile( image_tile_t * image_tile )
{
  if (image_tile->status != TILE_READY)
    return;

  glPushMatrix();

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, image_tile->texture);

  glColor4f(1.0f,1.0f,1.0f, imagery_transparency);

  glBegin(GL_QUADS);

  double x_mean = 0.5*(image_tile->x_max + image_tile->x_min);
 
  glTexCoord2f( 0.0f, 0.0f);   glVertex3f( image_tile->x_max, image_tile->y_min, 0.0f );
  glTexCoord2f( 0.0f, 0.5f);   glVertex3f(            x_mean, image_tile->y_min, 0.0f );
  glTexCoord2f( 1.0f, 0.5f);   glVertex3f(            x_mean, image_tile->y_max, 0.0f );
  glTexCoord2f( 1.0f, 0.0f);   glVertex3f( image_tile->x_max, image_tile->y_max, 0.0f );

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}

void
setup_texture( image_tile_t * image_tile )
{
  glBindTexture( GL_TEXTURE_2D, image_tile->texture );

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

  // convert imlib image to 
  imlib_context_set_image(image_tile->image);
  unsigned int *image_data = imlib_image_get_data();

  glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, image_data);
}
