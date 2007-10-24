/*  portVideo, a cross platform camera framework
    Copyright (C) 2005 Martin Kaltenbrunner <mkalten@iua.upf.es>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef CAMERATOOL_H
#define CAMERATOOL_H

#ifdef WIN32
#include <windows.h>
#include "../win32/dslibCamera.h"
#endif

#ifdef LINUX
#include <stdio.h>
#include <stdlib.h>
#include "../linux/linuxfwCamera.h"
#include "../linux/v4linuxCamera.h"
#endif

#ifdef __APPLE__
#include <stdio.h>
#include <stdlib.h>
#include "macvdCamera.h"
#include "macsgCamera.h"
#endif


class cameraTool
{
public:
	
	static cameraEngine* findCamera();
};

#endif
