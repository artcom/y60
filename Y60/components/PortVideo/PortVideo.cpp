
#include "PortVideo.h"
#include <asl/Logger.h>
#include <asl/Time.h>
#include <iostream>

#include <map>
#include <vector>

extern "C"
EXPORT asl::PlugInBase * y60PortVideo_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::PortVideo(myDLHandle);
}

using namespace std;
using namespace asl;

namespace y60 {

    PortVideo::PortVideo(asl::DLHandle theDLHandle) : 
        PlugInBase(theDLHandle), PosixThread(),
        _myCamera(NULL), _mySourceDepth(0),
        _mySourceBuffer(NULL), _myRingBuffer(NULL),
        _myWidth(320), _myHeight(240),
        _myBytesPerSourcePixel(0), _myBytesPerDestPixel(0),
        _myIsRunning(false)
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
        _myBytesPerDestPixel = _mySourceDepth/8;
        _mySourceBuffer = new unsigned char[_myWidth*_myHeight*_myBytesPerSourcePixel];
        _myDestBuffer = new unsigned char[_myWidth*_myHeight*_myBytesPerDestPixel];
        _myCameraBuffer = NULL;
        
        AC_INFO << "Allocating ringbuffer";
        _myRingBuffer = new RingBuffer(_myWidth*_myHeight*_myBytesPerSourcePixel);
        AC_INFO << "buffer allocation done.";
    }
  
    void PortVideo::run() {
        unsigned char * myCameraBuffer = NULL;
        unsigned char * myCameraWriteBuffer = NULL;
                
	    
        while (_myCamera) {	
            if (_myIsRunning) {
                _myLock.lock();
                
                myCameraBuffer = _myCamera->getFrame();
                
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
        }
    }

    void PortVideo::readFrame(dom::ResizeableRasterPtr theTargetRaster) {
        // AC_INFO << "PortVideo: reading frame";
        
        unsigned char * myCameraReadBuffer = NULL;
        /*
		// loop until we get access to a frame
		myCameraReadBuffer = _myRingBuffer->getNextBufferToRead();
		while (myCameraReadBuffer == NULL) {
			msleep(1);
			myCameraReadBuffer = _myRingBuffer->getNextBufferToRead();
        }
		
		// try again if we can get a more recent frame
		do {
			memcpy(_mySourceBuffer, myCameraReadBuffer, _myRingBuffer->size());
			_myRingBuffer->readFinished();
			
			myCameraReadBuffer = _myRingBuffer->getNextBufferToRead();
		} while( myCameraReadBuffer != NULL ); 
       */ 
        // theTargetRaster->resize(320, 240);
    }

    void PortVideo::load(const std::string & theFilename) {
        if(setupCamera()) {
            allocateBuffers();
            fork();
        } else {
            AC_ERROR << "could not setup camera";
        }
    }

    std::string PortVideo::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
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
