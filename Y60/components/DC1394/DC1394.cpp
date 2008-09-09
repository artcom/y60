
#include "DC1394.h"
#include <asl/Logger.h>
#include <asl/Time.h>
#include <iostream>

#include <dc1394/dc1394.h>
#include <libraw1394/raw1394.h>

#include <map>
#include <vector>

extern "C"
EXPORT asl::PlugInBase * y60DC1394_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::DC1394(myDLHandle);
}

using namespace std;
using namespace asl;

#define ASSERT_DC1394(x) if ( x != DC1394_SUCCESS ) { throw asl::Exception(string("dc1394 error: ")+asl::as_string(x), PLUS_FILE_LINE); }

namespace y60 {

    const int MAX_PORTS = 4;
    
    DC1394::DC1394(asl::DLHandle theDLHandle) : PlugInBase(theDLHandle), // _myFirewire(0),
            _myDevices(0), _myDeviceCount(0), _hasRingBuffer(false), _isTransmitting(false)
    {
        /*
        _myFirewire = raw1394_new_handle();
        if (!_myFirewire) {
            throw asl::Exception("can not open firewire bus", PLUS_FILE_LINE);
        }
        AC_WARNING << "found firewire!";
        */
        scanBus();
        AC_WARNING << "found " << _myDeviceCount << " cameras";
    }

    DC1394::~DC1394() {
        freeRingBuffer();
    }

    void 
    DC1394::scanBus() {

         dc1394camera_list_t * list;
         dc1394error_t err;
         dc1394_t * d;

         d = dc1394_new();
         err = dc1394_camera_enumerate( d, &list );
         DC1394_ERR_RTN( err, "Failed to enumerate cameras");

         if (list->num == 0) {
             dc1394_log_error("No cameras found");
             return 1;
         }

         int j = 0; 
         for (int i = 0; i < list->num; i++) {
             if (j >= MAX_CAMERAS) {
                 break;
             }
             cameras[j] = dc1394_camera_new( d, list->ids[i].guid );
             if (!cameras[j]) {
                 dc1394_log_warning( "Failed to initialize camera with guid %llx", 
                                     list->ids[i].guid );
                 continue;
             }
             j++;
         }
         dc1394_camera_free_list( list );

         if (j == 0) {
            dc1394_log_error("No cameras found");
         }
    }

    void DC1394::readFrame(dom::ResizeableRasterPtr theTargetRaster) {
        dc1394camera_t * myCamera = &getDeviceHandle();
        dc1394error_t myStatus = dc1394_capture_dma(&myCamera, 1, DC1394_VIDEO1394_POLL);
        if (myStatus == DC1394_NO_FRAME) {
            return;
        }
        ASSERT_DC1394(myStatus);

        unsigned char * myData = dc1394_capture_get_dma_buffer(myCamera);
        if (myData) {
            unsigned int myWidth = 0;
            unsigned int myHeight = 0;
            myStatus = dc1394_get_image_size_from_video_mode(myCamera, myCamera->video_mode, &myWidth, &myHeight);
            ASSERT_DC1394(myStatus);
            
            dc1394color_coding_t myColorCoding;
            myStatus = dc1394_get_color_coding_from_video_mode(myCamera, myCamera->video_mode, &myColorCoding);
            ASSERT_DC1394(myStatus);

            theTargetRaster->resize(myWidth, myHeight);
            myStatus = dc1394_convert_to_RGB8(myData, theTargetRaster->pixels().begin(), myWidth, myHeight, DC1394_BYTE_ORDER_UYVY, myColorCoding, 8);
            ASSERT_DC1394(myStatus);
        }
        myStatus = dc1394_capture_dma_done_with_buffer(myCamera);
        ASSERT_DC1394(myStatus);
    }

    void DC1394::load(const std::string & theFilename) {
        dc1394camera_t * myCamera = &getDeviceHandle();

        AC_DEBUG << "DC1394::load " << theFilename;
        parseOptions(theFilename);

        unsigned myFrameRate = (unsigned)getFrameRate();
        unsigned myInputPinNumber = 0;
        unsigned myDeviceId = getDevice();
        unsigned myWhitebalanceU;
        unsigned myWhitebalanceV;
        unsigned myShutter;
        unsigned myGain;
        string myVideoModeString="";

        setPixelFormat(RGB);

        getOption("fps", myFrameRate);
        getOption("video_mode", myVideoModeString);
        getOption("device", myDeviceId);

        string::size_type idx = theFilename.find("whitebalanceb=");
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
        
        if (myDeviceId >= _myDeviceCount) {
            throw asl::Exception("No such Device. Highest available DeviceId is: " + _myDeviceCount-1, PLUS_FILE_LINE);
        } 
      
        dc1394video_mode_t myVideoMode;
        if (myVideoModeString == "160x120_YUV444") {
            myVideoMode = DC1394_VIDEO_MODE_160x120_YUV444;
        } else if (myVideoModeString == "320x240_YUV422") {
            myVideoMode = DC1394_VIDEO_MODE_320x240_YUV422;
        } else if (myVideoModeString == "640x480_YUV411") {
            myVideoMode = DC1394_VIDEO_MODE_640x480_YUV411;
        } else if (myVideoModeString == "640x480_YUV422") {
            myVideoMode = DC1394_VIDEO_MODE_640x480_YUV422;
        } else if (myVideoModeString == "640x480_RGB8") {
            myVideoMode = DC1394_VIDEO_MODE_640x480_RGB8;
        } else if (myVideoModeString == "640x480_MONO8") {
            myVideoMode = DC1394_VIDEO_MODE_640x480_MONO8;
        } else {
            throw Exception("Unknown video mode", PLUS_FILE_LINE);
        }
        dc1394error_t myStatus = dc1394_video_set_mode(myCamera, myVideoMode);
        ASSERT_DC1394(myStatus);
       
        unsigned int myWidth;
        unsigned int myHeight;
        myStatus = dc1394_get_image_size_from_video_mode(myCamera, myVideoMode, &myWidth, &myHeight);
        ASSERT_DC1394(myStatus);

        setName(string(getDeviceHandle().vendor)+"/"+string(getDeviceHandle().model));
        setFrameRate(myFrameRate);
        setFrameHeight(myHeight);
        setFrameWidth(myWidth);
        setDevice(myDeviceId);

        // Setup video size and image matrix
        float myXResize = float(myWidth) / asl::nextPowerOfTwo(myWidth);
        float myYResize = float(myHeight) / asl::nextPowerOfTwo(myHeight);
        //setDesiredVideoFormat(myWidth, myFrameHeight, myFrameRate, 24);

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(asl::Vector3f(myXResize, - myYResize, 1.0f));
        myMatrix.translate(asl::Vector3f(0, myYResize, 0));
        setImageMatrix(myMatrix);
        // Start Capturing
        initRingBuffer();
        dc1394_video_set_transmission(&getDeviceHandle(), DC1394_ON);
        _isTransmitting = true;
    }

    std::string DC1394::canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream) {
        if (theUrl.find("video://") != std::string::npos) {
            return MIME_TYPE_CAMERA;
        } else {
            return "";
        }
    }

    asl::Ptr<CaptureDevice> DC1394::instance() const {
        return asl::Ptr<CaptureDevice>(new DC1394(getDLHandle()));
    }

    void DC1394::stopCapture() {
        dc1394_video_set_transmission(&getDeviceHandle(), DC1394_OFF);
        _isTransmitting = false;
        CaptureDevice::stopCapture();
    }
    
    void DC1394::startCapture() {
        if (!_hasRingBuffer) {
            throw asl::Exception("can not start capture: not DMA buffer initailized", PLUS_FILE_LINE); 
        }
        dc1394_video_set_transmission(&getDeviceHandle(), DC1394_ON);
        _isTransmitting = true;
        CaptureDevice::startCapture();
    }

    void DC1394::pauseCapture() {
        dc1394_video_set_transmission(&getDeviceHandle(), DC1394_OFF);
        _isTransmitting = false;
        CaptureDevice::pauseCapture();
    }

    void 
    DC1394::initRingBuffer() {
        if (_hasRingBuffer) {
            freeRingBuffer();
        }
        dc1394error_t myStatus = dc1394_video_set_iso_speed (&getDeviceHandle(), DC1394_ISO_SPEED_400);
        ASSERT_DC1394(myStatus);

        myStatus = dc1394_capture_setup_dma(&getDeviceHandle(), 10, true);
        ASSERT_DC1394(myStatus);
        _hasRingBuffer = true;
    }

    void 
    DC1394::freeRingBuffer() {
        if (_isTransmitting) {
            dc1394_video_set_transmission(&getDeviceHandle(), DC1394_OFF);
            _isTransmitting = false;
        }
        if (_hasRingBuffer) {
            dc1394error_t myStatus = dc1394_capture_stop (&getDeviceHandle());
            ASSERT_DC1394(myStatus);
            _hasRingBuffer = false;
        }
    }
}
