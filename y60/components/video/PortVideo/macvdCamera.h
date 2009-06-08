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

#ifndef macvdCamera_H
#define macvdCamera_H

#include "cameraEngine.h"
#include "vdigGrab.h"
		
class macvdCamera : public cameraEngine
{
public:
	macvdCamera();
	~macvdCamera();
	
	bool findCamera();
	bool initCamera(int width, int height, bool colour);
	bool startCamera();
	unsigned char* getFrame();
	bool stopCamera();
	bool stillRunning();
	bool resetCamera();
	bool closeCamera();
	
	void showSettingsDialog();
	
private:
	VdigGrab  *pVdg;
	ImageDescriptionHandle vdImageDesc;
	CGrafPtr	dstPort;
	Rect		dstPortBounds;
	Rect		dstDisplayBounds;
	void*		pDstData;
	long		dstDataSize;

};

#endif
