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

#ifndef _AC_PORTVIDEO_H__
#define _AC_PORTVIDEO_H__

#include <y60/video/CaptureDevice.h>
#include <asl/base/PlugInBase.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/PosixThread.h>

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
