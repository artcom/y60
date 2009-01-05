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

#include "cameraTool.h"
	
cameraEngine* cameraTool::findCamera() {

	cameraEngine* camera = NULL;

	#ifdef WIN32
	camera = new dslibCamera();
	#endif

	#ifdef __APPLE__
	camera = new macvdCamera();
	#endif		
		
	#ifdef LINUX
	camera = new linuxfwCamera();
	if( !camera->findCamera() ){ 
		delete camera;
		// check video4linux
		camera = new v4linuxCamera();
	}
	#endif
	
	if( !camera->findCamera() ) { 
		delete camera;
		camera = NULL;
	} 
	
	return camera;
}

