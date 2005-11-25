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
//   $RCSfile: VFWCapture.h,v $
//
//   $Author: thomas $
//
//   $Revision: 1.3 $
//
//=============================================================================

#ifndef _ac_VFWCapture_VFWCapture_h_
#define _ac_VFWCapture_VFWCapture_h_

#include <y60/MovieDecoderBase.h>
#include <asl/PlugInBase.h>
#include <asl/ThreadLock.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/*! @addtogroup VFWCapture */
/* @{ */
namespace y60 {
    const std::string MIME_TYPE_CAMERA = "video/camera";
    class VFWCapture : public MovieDecoderBase, public asl::PlugInBase {
    public:
        VFWCapture(asl::DLHandle theDLHandle);
        ~VFWCapture();
        virtual asl::Ptr<MovieDecoderBase> instance() const;
        std::string canDecode(const std::string & theUrl, asl::ReadableStream * theStream = 0);
        virtual double readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster);
        virtual void load(const std::string & theFilename);
        /**
         * loads a movie from the stream given by theSource
         * @param theFilename file to identify theSource
         */
        void load(asl::Ptr<asl::ReadableStream> theSource, const std::string & theFilename) {
            throw asl::NotYetImplemented(JUST_FILE_LINE);
        }

        DEFINE_EXCEPTION(Exception, asl::Exception);
        virtual void startMovie(double theStartTime = 0.0);
        virtual void stopMovie();
        void setGrabbedFrameUYVY(unsigned char* theImageBuffer);
        void setGrabbedFrameRGB(unsigned char* theImageBuffer);
        void getDevices();
/*    
    protected:
        int getInt(std::string theUrl, std::string theKey, int theDefault);
        std::string getString(std::string theUrl, std::string theKey, std::string theDefault);
        */
    private:
        HWND             _myGrabber;
        unsigned char *  _myLastGrabbedFrame;
        bool             _myFrameLock;
        asl::ThreadLock  _myLock;
        int              _myFrameHeight;
        int              _myFrameWidth;
    };
}
/* @} */

#endif // _ac_VFWCapture_VFWCapture_h_
