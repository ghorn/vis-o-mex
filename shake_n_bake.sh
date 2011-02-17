# 
#  This file is part of vis-o-mex.
# 
#  Copyright (C) 2010-2011 Greg Horn <ghorn@stanford.edu>
#  
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# 


#  shake_n_bake.sh
#  first get image from url $1 and put it in $2.temp
#              when that is done, mv it from $2.temp to $2

curl $1 > $2.temp && mv $2.temp $2
