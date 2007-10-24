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

#include "macvdCamera.h"

macvdCamera::macvdCamera()
{
	cameraID = -1;
	
	buffer = NULL;
	cameraName = new char[256];
	
	dstPort = NULL;
	vdImageDesc = NULL;
	pVdg = NULL;
}

macvdCamera::~macvdCamera()
{
	if (buffer!=NULL) delete []buffer;
	delete []cameraName;
}

bool macvdCamera::findCamera() {

	OSErr err;
	
	if(!(pVdg = vdgNew()))
	{
		//printf("no camera found\n");
		//printf("vdgNew: failed to allocate\n");
		return false;
	}
	
	if(err = vdgInit(pVdg))
	{
		printf("no camera found\n");
		//printf("vdgInit err=%d\n", err);
		return false;
	}

	cameraID = 0;
	return true;
}

bool macvdCamera::initCamera(int width, int height, bool colour) {

	if (cameraID < 0) return false;

	this->width =width;
	this->height=height;
	this->colour=colour;
	this->bytes =(colour?3:1);

	OSErr err;
	if(err = vdgRequestSettings(pVdg))
	{
		printf("camera setup cancelled\n");
		//printf("vdgRequestSettings err=%d\n", err);
		return false;
	}

    long nameLength = 256;
	if (err = vdgGetDeviceNameAndFlags(pVdg, cameraName, &nameLength, NULL))
	{
		sprintf(cameraName,"unknown camera");
	}

	long milliSecPerFrame;
	Fixed framerate;
	long bytesPerSecond;
	if (err = vdgGetDataRate(pVdg, &milliSecPerFrame, &framerate, &bytesPerSecond))
	{
		fps = 30;
	} else fps = (int)(framerate/65536);

	//fps = vdgGetFrameRate(pVdg);	
	//printf("%d\n",fps);

	if(err = vdgPreflightGrabbing(pVdg))
	{
		//printf("vdgPreflightGrabbing err=%d\n", err);
		return false;
	}
	
	vdImageDesc = (ImageDescriptionHandle)NewHandle(0);
	if (err = vdgGetImageDescription( pVdg, 
									  vdImageDesc))
	{
		//printf("vdgGetImageDescription err=%d\n", err);
		return false;
	}

	this->width = (*vdImageDesc)->width;
	this->height = (*vdImageDesc)->height;
    //printf("%dx%d\n",this->width,this->height);

	dstPortBounds.left = 0;
	dstPortBounds.right = this->width;
	dstPortBounds.top = 0;
	dstPortBounds.bottom = this->height;

	if (err = createOffscreenGWorld(	&dstPort,
										//k8IndexedGrayPixelFormat,
										//kYVYU422PixelFormat,
										//k24RGBPixelFormat,
										//kYUV420CodecType,
										k422YpCbCr8CodecType,
										&dstPortBounds))
	{
		printf("createOffscreenGWorld err=%d\n", err);
		return false;	
	}
	
	// Get buffer from GWorld
	pDstData = GetPixBaseAddr(GetGWorldPixMap(dstPort));
	dstDataSize = GetPixRowBytes(GetGWorldPixMap(dstPort)) * (dstPortBounds.bottom - dstPortBounds.top); 
	dstDisplayBounds = dstPortBounds;

	
	// Set the decompression destination to the offscreen GWorld
	if (err = vdgSetDestination(	pVdg, dstPort ))
	{
		//printf("vdgSetDestination err=%d\n", err);
		return false;
	}

	buffer = new unsigned char[width*height*bytes];
	return true;
}

unsigned char* macvdCamera::getFrame()
{
	OSErr   err;
	int		isUpdated = 0;

	if (!vdgIsGrabbing(pVdg)) {
        return NULL;
    }

	if (err = vdgIdle( pVdg, &isUpdated))
	{
		printf("could not grab frame\n");
		return NULL;
	}
	
	if (isUpdated)
	{
		unsigned char *src = (unsigned char*)pDstData;
		unsigned char *dest = buffer;
		
		switch (colour) {
			case true: {
				uyvy2rgb(width,height,src,dest);
				break;
			}
			case false: {
				uyvy2gray(width,height,src,dest);
				break;
			}
		}
		return buffer;
	}
	
	return NULL;
}

bool macvdCamera::startCamera()
{

	OSErr err;
	if (err = vdgStartGrabbing(pVdg))
	{
		printf("could not start camera\n");
		return false;
	}

	return true;
}

bool macvdCamera::stopCamera()
{
	OSErr err;
	if (err = vdgStopGrabbing(pVdg))
	{
		printf("could not stop camera\n");
		return false;
	}

	return true;
}

bool macvdCamera::stillRunning() {

	return vdgIsGrabbing( pVdg);
}

bool macvdCamera::resetCamera()
{
  return (stopCamera() && startCamera());
}

bool macvdCamera::closeCamera()
{

	if (dstPort)
	{
		disposeOffscreenGWorld(dstPort);
		dstPort = NULL;
	}

	if (vdImageDesc)
	{
		DisposeHandle((Handle)vdImageDesc);
		vdImageDesc = NULL;
	}

	if (pVdg)
	{
		vdgUninit(pVdg);
		vdgDelete(pVdg);
		pVdg = NULL;
	}	

	return true;
}

void macvdCamera::showSettingsDialog() {
}
