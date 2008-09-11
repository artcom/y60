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
//   $RCSfile: GStreamerCapturePlugin.h,v $
//
//   $Author: thomas $
//
//   $Revision: 1.2 $
//
//=============================================================================

#ifndef _GSTREAMER_CAPTURE_PLUGIN_H_
#define _GSTREAMER_CAPTURE_PLUGIN_H_

#include <y60/video/CaptureDevice.h>
#include <asl/base/PlugInBase.h>
#include <asl/base/ThreadLock.h>

#include <gst/gst.h>

namespace y60 {
    
    class GStreamerCapturePlugin : public CaptureDevice, public asl::PlugInBase {
    public:
        GStreamerCapturePlugin(asl::DLHandle theDLHandle);
        ~GStreamerCapturePlugin();
        virtual asl::Ptr<CaptureDevice> instance() const;
        std::string canDecode(const std::string & theUrl, 
                              asl::Ptr<asl::ReadableStreamHandle> theStream = 
                              asl::Ptr<asl::ReadableStreamHandle>(0));
        virtual void readFrame(dom::ResizeableRasterPtr theTargetRaster);
        virtual void load(const std::string & theFilename);
        DEFINE_EXCEPTION(Exception, asl::Exception);
        virtual void startCapture();
        virtual void stopCapture();
        virtual void pauseCapture();
    private: 
        static void cb_handoff( GstElement *fakesrc, GstBuffer *buffer, GstPad *pad,
                                gpointer user_data ) {
            GStreamerCapturePlugin* that = 
                reinterpret_cast<GStreamerCapturePlugin*>(user_data);
            that->handoff(fakesrc, buffer, pad);
        }
        
        void handoff( GstElement *fakesrc, GstBuffer *buffer, GstPad *pad);
        
        static gboolean cb_buscallback( GstBus *bus, GstMessage* message,
                                    gpointer user_data ) {
            GStreamerCapturePlugin* that = 
                reinterpret_cast<GStreamerCapturePlugin*>(user_data);
            return that->buscallback(bus, message);
        }
        
        gboolean buscallback( GstBus *bus, GstMessage* message );

        guint8 _myBuffer[1024*1024];
        std::size_t _myBufferSize;
        asl::ThreadLock  _myBufferLock;

    };
}

#endif // _GSTREAMER_CAPTURE_PLUGIN_H_
