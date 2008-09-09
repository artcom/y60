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
//
//   $RCSfile: DC1394.h,v $
//
//   $Author: thomas $
//
//   $Revision: 1.2 $
//
//=============================================================================

#ifndef _ac_DC1394
#define _ac_DC1394

#include <y60/CaptureDevice.h>
#include <asl/PlugInBase.h>
#include <asl/ThreadLock.h>

#include <libraw1394/raw1394.h>
#include <dc1394/dc1394.h>

#define MAX_CAMERAS 8

namespace y60 {
/*! @addtogroup Y60componentsDC1394 */
/*  @{ */
    const std::string MIME_TYPE_CAMERA = "video/camera";
    class DC1394 : public CaptureDevice, public asl::PlugInBase {
    public:
        DC1394(asl::DLHandle theDLHandle);
        ~DC1394();
        virtual asl::Ptr<CaptureDevice> instance() const;
        std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>(0));
        virtual void readFrame(dom::ResizeableRasterPtr theTargetRaster);
        virtual void load(const std::string & theFilename);
        DEFINE_EXCEPTION(Exception, asl::Exception);
        virtual void startCapture();
        virtual void stopCapture();
        virtual void pauseCapture();
    private:
        dc1394camera_t * _myDevices[MAX_CAMERAS];
        const dc1394camera_t & getDeviceHandle() const { return (*_myDevices)[getDevice()]; };
        dc1394camera_t & getDeviceHandle() { return (*_myDevices)[getDevice()]; };
        unsigned int _myDeviceCount;
        bool _hasRingBuffer;
        bool _isTransmitting;
        // raw1394handle_t _myFirewire;

        void scanBus();
        void initRingBuffer();
        void freeRingBuffer();
    };
/* @} */
}

#endif // _ac_VFWCapture_VFWCapture_h_
