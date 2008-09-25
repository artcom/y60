#include "GStreamerCapturePlugin.h"
#include <asl/base/Auto.h>

extern "C"
EXPORT asl::PlugInBase * GStreamerCapturePlugin_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::GStreamerCapturePlugin(myDLHandle);
}

namespace y60 {

    const int MAX_PORTS = 4;
    
    GStreamerCapturePlugin::GStreamerCapturePlugin(asl::DLHandle theDLHandle) : PlugInBase(theDLHandle), _myBufferSize(0)
    {
        memset( _myBuffer, 0, sizeof( _myBuffer ));
    }

    GStreamerCapturePlugin::~GStreamerCapturePlugin() {
    }

    void GStreamerCapturePlugin::handoff( GstElement *fakesrc, GstBuffer *buffer, 
                                          GstPad *pad) {

        int buffer_size = GST_BUFFER_SIZE (buffer);
        guint8* buffer_data = GST_BUFFER_DATA (buffer);

        assert( buffer_size < sizeof( _myBuffer ) );
        asl::AutoLocker<asl::ThreadLock> myAutoLocker(_myBufferLock);
        std::copy( buffer_data, buffer_data + buffer_size, _myBuffer ); 
        _myBufferSize = buffer_size;
    }

    gboolean GStreamerCapturePlugin::buscallback( GstBus *bus, GstMessage* message ) {
        g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));

        switch (GST_MESSAGE_TYPE (message)) {
            case GST_MESSAGE_ERROR: {
                                        GError *err;
                                        gchar *debug;

                                        gst_message_parse_error (message, &err, &debug);
                                        g_print ("Error: %s\n", err->message);
                                        g_error_free (err);
                                        g_free (debug);

                                        break;
                                    }
            case GST_MESSAGE_EOS:
                                    /* end-of-stream */
                                    break;
            default:
                                    /* unhandled message */
                                    break;
        }

        /* we want to be notified again the next time there is a message
         * on the bus, so returning TRUE (FALSE means we want to stop watching
         * for messages on the bus and our callback should not be called again)
         */
        return TRUE;
    }

    void GStreamerCapturePlugin::readFrame(dom::ResizeableRasterPtr theTargetRaster) {
        assert (_myBufferSize <= theTargetRaster->pixels().size());
        theTargetRaster->resize(getFrameWidth(), getFrameHeight());
        asl::AutoLocker<asl::ThreadLock> myAutoLocker(_myBufferLock);
        memcpy(theTargetRaster->pixels().begin(), _myBuffer, _myBufferSize);
    }
    
    bool dumpPads( GstElement* theElement ) {
        AC_PRINT << "dumping pads .."; 
        GstIterator* myPadIterator =  gst_element_iterate_src_pads( theElement );
        GstPad* myPad = NULL;

        while (true) {
            switch (gst_iterator_next( myPadIterator, (void**)&myPad )) {
                case GST_ITERATOR_OK:
                {
                    GstCaps* myCaps = gst_pad_get_caps(myPad);
                    AC_PRINT << "caps: " << gst_caps_to_string( myCaps );
                    gst_object_unref( myPad );
                }
                    break;
                case GST_ITERATOR_DONE:
                    return true;
                default: 
                    return false;
            }
        }
    }

    std::string GStreamerCapturePlugin::getUrl( const std::string & theInputStr ) {
        std::string::size_type myIdx = theInputStr.find( "," );
        if (myIdx != std::string::npos) {
            return theInputStr.substr(0, myIdx);
        }
        return theInputStr; 
    }

    void GStreamerCapturePlugin::load(const std::string & theFilename) {
        // init gstreamer (with fake arguments)
        //
        gst_init( NULL, NULL );

        // grab version information
        // 
        const gchar *nano_str;
        guint major, minor, micro, nano;
      
        gst_version (&major, &minor, &micro, &nano);
      
        if (nano == 1)
          nano_str = "(CVS)";
        else if (nano == 2)
          nano_str = "(Prerelease)";
        else
          nano_str = "";
        
        AC_INFO << "Plugging GStreamer Plugin using GStreamer version "
                << major << "." << minor << "." << micro << "." << std::string(nano_str);

        AC_PRINT << "Using URL: " << getUrl( theFilename );

        std::string myPipelineStr = "rtspsrc location=" + getUrl(theFilename) 
                                    + " ! rtpmp4vdepay ! ffdec_mpeg4 ! ffmpegcolorspace !"
                                    + "capsfilter caps=video/x-raw-rgb ! fakesink "
                                    + "name=fakesink";

        GstElement *myFirstPipeline;
        GError* error = NULL;
        myFirstPipeline = 
            (GstElement*) gst_parse_launch((const gchar*)myPipelineStr.c_str(), &error);
       
        if (error) {
           AC_ERROR << "Cannot link gstreamer elements!" << error;
        }

        GstElement *myPipeline;
        myPipeline = gst_element_factory_make( "pipeline", NULL );
        gst_bin_add( GST_BIN( myPipeline ), myFirstPipeline );

        GstElement* myFakeSink = gst_bin_get_by_name( GST_BIN( myPipeline ), "fakesink" );

        g_object_set( G_OBJECT( myFakeSink ), "signal-handoffs", TRUE, NULL );
        g_signal_connect( myFakeSink, "handoff", 
                          G_CALLBACK( cb_handoff ), 
                          this );

        gst_element_set_state( myPipeline, GST_STATE_PLAYING );
        setFrameWidth(640);
        setFrameHeight(480);
        setPixelFormat(RGB);
    }


    std::string GStreamerCapturePlugin::canDecode( const std::string & theUrl, 
                                                   asl::Ptr<asl::ReadableStreamHandle> 
                                                   theStream)
    {
        if (theUrl.find("rtsp://") != std::string::npos) {
            _myUrl = theUrl;
            return "video/camera";
        } else {
            return "";
        }
    }

    asl::Ptr<CaptureDevice> GStreamerCapturePlugin::instance() const {
        return asl::Ptr<CaptureDevice>(new GStreamerCapturePlugin(getDLHandle()));
    }

    void GStreamerCapturePlugin::stopCapture() {
        CaptureDevice::stopCapture();
    }
    
    void GStreamerCapturePlugin::startCapture() {
        CaptureDevice::startCapture();
    }

    void GStreamerCapturePlugin::pauseCapture() {
        CaptureDevice::pauseCapture();
    }
}
