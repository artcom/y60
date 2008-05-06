//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _AC_PORTVIDEO_H__
#define _AC_PORTVIDEO_H__

#include <y60/CaptureDevice.h>
#include <asl/PlugInBase.h>
#include <asl/ThreadLock.h>
#include <asl/PosixThread.h>

#include "RingBuffer.h"
#include "cameraTool.h"


namespace y60 {
/*! @addtogroup Y60componentsPortVideo */
/*  @{ */
DEFINE_EXCEPTION(BufferSizeMismatch, asl::Exception);

    const std::string MIME_TYPE_CAMERA = "video/camera";
    class PortVideo : public CaptureDevice, public asl::PlugInBase, public asl::PosixThread {
    public:
        PortVideo(asl::DLHandle theDLHandle);
        ~PortVideo();
        
        virtual asl::Ptr<CaptureDevice> instance() const;
        std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>(0));
        virtual void readFrame(dom::ResizeableRasterPtr theTargetRaster);
        virtual void load(const std::string & theFilename);
        DEFINE_EXCEPTION(Exception, asl::Exception);
        virtual void startCapture();
        virtual void stopCapture();
        virtual void pauseCapture();

    private:
        // const dc1394camera_t & getDeviceHandle() const { return (*_myDevices)[getDevice()]; };
        // dc1394camera_t & getDeviceHandle() { return (*_myDevices)[getDevice()]; };
        cameraEngine * _myCamera; 
        //unsigned char * _myCameraBuffer;
        bool _myIsRunning;

        RingBuffer * _myRingBuffer; 

        //unsigned char * _mySourceBuffer;
        //unsigned char * _myDestBuffer;

        unsigned int _myWidth, _myHeight, _mySourceDepth, _myDestDepth;
        unsigned int _myBytesPerDestPixel, _myBytesPerSourcePixel; 
        
        asl::ThreadLock _myLock;
        bool _async;
   
        void freeBuffers();
        bool setupCamera();
        void allocateBuffers();
        
        // thread function
        void run();
    };
/* @} */
}

#endif // _AC_PORTVIDEO_H__ 
