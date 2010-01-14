/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "PortVideo.h"
#include <asl/base/Logger.h>
#include <asl/base/Time.h>
#include <iostream>

#include <map>
#include <vector>

extern "C"
EXPORT asl::PlugInBase * PortVideo_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::PortVideo(myDLHandle);
}

using namespace std;
using namespace asl;

namespace y60 {

    PortVideo::PortVideo(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle), PosixThread(),
        _myCamera(NULL), _mySourceDepth(24), _myDestDepth(24),
        //_mySourceBuffer(NULL),
         _myRingBuffer(NULL),
        _myWidth(320), _myHeight(240),
        _myBytesPerSourcePixel(_mySourceDepth/8), _myBytesPerDestPixel(_myDestDepth/8),
        _myIsRunning(false),
        _async(false)
    {
    }

    PortVideo::~PortVideo() {
        freeBuffers();

        _myCamera->closeCamera();
        delete _myCamera;
    }

    void PortVideo::freeBuffers() {
        delete _myRingBuffer;
    }

    bool PortVideo::setupCamera() {
        _myCamera = cameraTool::findCamera();
        if (_myCamera == NULL) {
            return false;
        }

        bool myColor = false;
        if (_mySourceDepth==24) {
            myColor = true;
        }

        bool mySuccess = _myCamera->initCamera(_myWidth, _myHeight, myColor);

        if(mySuccess) {
            _myWidth  = _myCamera->getWidth();
            _myHeight = _myCamera->getHeight();

            AC_INFO << "camera: " << _myCamera->getName();
            AC_INFO << "format: " << _myWidth << "x" << _myHeight << " fps: " << _myCamera->getFps();
            return true;
        } else {
            AC_ERROR << "unable to setup camera";
            _myCamera->closeCamera();
            delete _myCamera;
            return false;
        }
    }

    void PortVideo::allocateBuffers() {
        AC_INFO << "Allocating buffers";
        _myBytesPerSourcePixel = _mySourceDepth/8;
        _myBytesPerDestPixel = _myDestDepth/8;
        //_mySourceBuffer = new unsigned char[_myWidth*_myHeight*_myBytesPerSourcePixel];
        //_myDestBuffer = new unsigned char[_myWidth*_myHeight*_myBytesPerDestPixel];
        //_myCameraBuffer = NULL;

        AC_DEBUG << "Allocating ringbuffer w="<<_myWidth <<",h="<<_myHeight<<" bypp="<< _myBytesPerSourcePixel;
        _myRingBuffer = new RingBuffer(_myWidth*_myHeight*_myBytesPerSourcePixel);
        AC_DEBUG << "buffer allocation done.";
    }

    void PortVideo::run() {
        unsigned char * myCameraBuffer = NULL;
        unsigned char * myCameraWriteBuffer = NULL;
        AC_DEBUG << "PortVideo::run()";

        while (_myCamera) {
            if (_myIsRunning && _myRingBuffer) {
                _myLock.lock();
                AC_DEBUG << "PortVideo::run() cam->getFrame()";
                myCameraBuffer = _myCamera->getFrame();
                AC_DEBUG << "PortVideo::run() cam->getFrame() returned "<<(void*)myCameraBuffer;

                if (myCameraBuffer != NULL) {
                    myCameraWriteBuffer = _myRingBuffer->getNextBufferToWrite();
                    if (myCameraWriteBuffer != NULL) {
                        memcpy(myCameraWriteBuffer, myCameraBuffer, _myRingBuffer->size());
                        _myRingBuffer->writeFinished();
                    }
                }
                _myLock.unlock();
                msleep(10);
            }
            msleep(10);
        }
    }

    void PortVideo::readFrame(dom::ResizeableRasterPtr theTargetRaster) {
        AC_DEBUG << "PortVideo: reading frame";

        unsigned char * myCameraReadBuffer = NULL;

		// loop until we get access to a frame
		myCameraReadBuffer = _myRingBuffer->getNextBufferToRead();
        if (_async && !myCameraReadBuffer) {
            return;
        }
		while (myCameraReadBuffer == NULL) {
			msleep(1);
			myCameraReadBuffer = _myRingBuffer->getNextBufferToRead();
        }
        theTargetRaster->resize(_myWidth, _myHeight);
        if (theTargetRaster->pixels().size() != _myRingBuffer->size()) {
            AC_ERROR << "target raster and video-in buffer size mismatch, targetraster="<<theTargetRaster->pixels().size()
                     <<", in-buffer ="<<_myRingBuffer->size();
            throw BufferSizeMismatch(JUST_FILE_LINE);
        }

		// try again if we can get a more recent frame
		do {
			memcpy(theTargetRaster->pixels().begin(), myCameraReadBuffer, _myRingBuffer->size());
			_myRingBuffer->readFinished();
			myCameraReadBuffer = _myRingBuffer->getNextBufferToRead();
		} while( myCameraReadBuffer != NULL );
    }

    void PortVideo::load(const std::string & theFilename) {
        AC_DEBUG << "PortVideo::load " << theFilename;

        int myFrameWidth = 0;
        int myFrameHeight = 0;
        unsigned myFrameRate = (unsigned)getFrameRate();
        unsigned myBitsPerPixel = 24;
#if 0
        unsigned myInputPinNumber = 0;
        unsigned myDeviceId = getDevice();
        unsigned myWhitebalanceU;
        unsigned myWhitebalanceV;
        unsigned myShutter;
        unsigned myGain;
#endif
        if (myFrameWidth == 0) {
            myFrameWidth = 320;
        }
        if (myFrameHeight == 0) {
            myFrameHeight = 240;
        }

        setPixelFormat(RGB);

        int idx = theFilename.find("width=");
        if (idx != std::string::npos) {
            myFrameWidth = asl::as_int(theFilename.substr(idx+6));
        }
        idx = theFilename.find("height=");
        if (idx != std::string::npos) {
            myFrameHeight = asl::as_int(theFilename.substr(idx+7));
        }
        idx = theFilename.find("fps=");
        if (idx != std::string::npos) {
            myFrameRate = asl::as_int(theFilename.substr(idx+4));
        }
        idx = theFilename.find("bpp=");
        if (idx != std::string::npos) {
            myBitsPerPixel = asl::as_int(theFilename.substr(idx+4));
        }
#if 0
        idx = theFilename.find("input=");
        if (idx != std::string::npos) {
            myInputPinNumber = asl::as_int(theFilename.substr(idx+6));
        }
        idx = theFilename.find("device=");
        if (idx != std::string::npos && theFilename.substr(idx+7).length() > 0) {
            myDeviceId = asl::as_int(theFilename.substr(idx+7));
        }
        idx = theFilename.find("whitebalanceb=");
        if (idx != std::string::npos && theFilename.substr(idx+14).length() > 0) {
            myWhitebalanceU = asl::as_int(theFilename.substr(idx+14));
        }
        idx = theFilename.find("whitebalancer=");
        if (idx != std::string::npos && theFilename.substr(idx+14).length() > 0) {
            myWhitebalanceV = asl::as_int(theFilename.substr(idx+14));
        }
        idx = theFilename.find("shutter=");
        if (idx != std::string::npos && theFilename.substr(idx+8).length() > 0) {
            myShutter = asl::as_int(theFilename.substr(idx+8));
        }
        idx = theFilename.find("gain=");
        if (idx != std::string::npos && theFilename.substr(idx+5).length() > 0) {
            myGain = asl::as_int(theFilename.substr(idx+5));
        }
        idx = theFilename.find("deinterlace=");
        if (idx != std::string::npos && theFilename.substr(idx+12).length() > 0) {
            _myDeinterlaceFlag = asl::as_int(theFilename.substr(idx+12)) == 1 ? true:false;
        }
#endif
        if(setupCamera()) {
            allocateBuffers();
            fork();
        } else {
            AC_ERROR << "could not setup camera";
        }
    }

    std::string PortVideo::canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream) {
        AC_DEBUG << "PortVideo::canDecode: theUrl=" << theUrl << ", theStream = " << (void*)theStream.getNativePtr();
        if (theUrl.find("portvideo://") != std::string::npos) {
            return MIME_TYPE_CAMERA;
        } else {
            return "";
        }
    }

    asl::Ptr<CaptureDevice> PortVideo::instance() const {
        return asl::Ptr<CaptureDevice>(new PortVideo(getDLHandle()));
    }

    void PortVideo::stopCapture() {
        _myIsRunning = true;
		_myCamera->stopCamera();
        CaptureDevice::stopCapture();
    }

    void PortVideo::startCapture() {
        AC_INFO << "startCapture";
        _myCamera->startCamera();
        CaptureDevice::startCapture();
        _myIsRunning = true;
    }

    void PortVideo::pauseCapture() {
        _myIsRunning = false;
        CaptureDevice::pauseCapture();
    }
}
