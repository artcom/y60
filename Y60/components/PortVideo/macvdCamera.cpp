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
#include <asl/Logger.h>

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
		AC_ERROR << "no camera found, vdgNew: failed to allocate";
		return false;
	}
	
	if(err = vdgInit(pVdg))
	{
		AC_ERROR << "no camera found, vdgInit err="<<err;
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
		AC_ERROR << "camera setup cancelled, vdgRequestSettings err="<<err;
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
		AC_WARNING << "vdgGetDataRate err="<<err;
	} else {
       AC_INFO <<  "macvdCamera::initCamera: vdgGetDataRate: milliSecPerFrame="<<milliSecPerFrame
               <<",framerate="<<framerate<<", bytesPerSecond="<<bytesPerSecond;
    }

	fps = vdgGetFrameRate(pVdg);	
	AC_DEBUG << "fps=" << fps;

	if(err = vdgPreflightGrabbing(pVdg))
	{
		AC_ERROR << "vdgPreflightGrabbing err="<<err;
		return false;
	}
	
	vdImageDesc = (ImageDescriptionHandle)NewHandle(0);
	if (err = vdgGetImageDescription( pVdg, 
									  vdImageDesc))
	{
		AC_ERROR << "vdgGetImageDescription err="<<err;
		return false;
	}

	this->width = (*vdImageDesc)->width;
	this->height = (*vdImageDesc)->height;
    AC_DEBUG << "video grabber size = " << this->width << "x" << this->height;

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
		AC_ERROR << "createOffscreenGWorld err="<<err;
		return false;	
	}
	
	// Get buffer from GWorld
	pDstData = GetPixBaseAddr(GetGWorldPixMap(dstPort));
	dstDataSize = GetPixRowBytes(GetGWorldPixMap(dstPort)) * (dstPortBounds.bottom - dstPortBounds.top); 
	dstDisplayBounds = dstPortBounds;

	
	// Set the decompression destination to the offscreen GWorld
	if (err = vdgSetDestination(	pVdg, dstPort ))
	{
		AC_ERROR << "vdgSetDestination err="<<err;
		return false;
	}
    
    AC_DEBUG << "macvdCamera::initCamera() buffer w="<<this->width<<",h="<<this->height<<", bytes="<<bytes;
	buffer = new unsigned char[this->width*this->height*bytes];
	return true;
}

unsigned char* macvdCamera::getFrame()
{
    AC_DEBUG << "macvdCamera::getFrame()";
	OSErr   err;
	int		isUpdated = 0;

	if (!vdgIsGrabbing(pVdg)) {
        AC_DEBUG << "macvdCamera::getFrame() !vdgIsGrabbing";
        return NULL;
    }

	if (err = vdgIdle( pVdg, &isUpdated))
	{
		AC_ERROR << "could not grab frame, vdgIdle err="<<err;
		return NULL;
	}
	
	if (isUpdated)
	{
        AC_DEBUG << "macvdCamera::getFrame() isUpdated";
		unsigned char *src = (unsigned char*)pDstData;
		unsigned char *dest = buffer;
		
		switch (colour) {
			case true: {
                AC_DEBUG << "macvdCamera::getFrame() uyvy2rgb w="<<width<<",h="<<height<<", src="<<(void*)src<<",dest="<<(void*)dest;
				uyvy2rgb(width,height,src,dest);
				break;
			}
			case false: {
                AC_DEBUG << "macvdCamera::getFrame() uyvy2gray w="<<width<<",h="<<height<<", src="<<(void*)src<<",dest="<<(void*)dest;
				uyvy2gray(width,height,src,dest);
				break;
			}
		}
        AC_DEBUG << "macvdCamera::getFrame() returning buffer="<<(void*)buffer;
		return buffer;
	}
    AC_DEBUG << "macvdCamera::getFrame() !isUpdated, returning 0";
	return NULL;
}

bool macvdCamera::startCamera()
{

	OSErr err;
	if (err = vdgStartGrabbing(pVdg))
	{
		AC_ERROR << "could not start camera, vdgStartGrabbing err="<<err;
		return false;
	}

	return true;
}

bool macvdCamera::stopCamera()
{
	OSErr err;
	if (err = vdgStopGrabbing(pVdg))
	{
		AC_ERROR << "could not stop camera, vdgStopGrabbing err="<<err;
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
