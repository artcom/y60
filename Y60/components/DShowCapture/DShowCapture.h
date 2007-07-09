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
//   $RCSfile: DShowCapture.h,v $
//
//   $Author: thomas $
//
//   $Revision: 1.2 $
//
//=============================================================================

#ifndef _ac_DShowCapture_DShowCapture_h_
#define _ac_DShowCapture_DShowCapture_h_

#include <y60/CaptureDevice.h>
#include <asl/PlugInBase.h>
#include <asl/ThreadLock.h>
#include "DXSampleGrabber.h"
#include "DShowGraph.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace y60 {
/*! @addtogroup Y60componentsDShowCapture */
/*  @{ */
    const std::string MIME_TYPE_CAMERA = "video/camera";
    class DShowCapture : public CaptureDevice, public asl::PlugInBase {
    public:
        DShowCapture(asl::DLHandle theDLHandle);
        ~DShowCapture();
        virtual asl::Ptr<CaptureDevice> instance() const;
        std::string canDecode(const std::string & theUrl, asl::ReadableStream * theStream = 0);
        virtual void readFrame(dom::ResizeableRasterPtr theTargetRaster);
        virtual void load(const std::string & theFilename);
        DEFINE_EXCEPTION(Exception, asl::Exception);
        virtual void startCapture();
        virtual void stopCapture();
        virtual void pauseCapture();
    private:
        DShowGraph *      _myGraph;
        bool              _myDeinterlaceFlag;
    };
/* @} */
}

#endif // _ac_VFWCapture_VFWCapture_h_
