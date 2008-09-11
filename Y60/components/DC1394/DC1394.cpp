
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
        scanBus();
        AC_WARNING << "found " << _myDeviceCount << " cameras";
    }

    DC1394::~DC1394() {
        freeRingBuffer();
        resetBus();
    }

    void 
    DC1394::scanBus() {

         dc1394camera_list_t * myCamList;
         dc1394error_t myError;
         dc1394_t * d;

         d = dc1394_new();
         myError = dc1394_camera_enumerate( d, &myCamList );

         if (myError) {
             AC_ERROR <<  "Failed to enumerate cameras: Error " << myError;
             return;
         }

         if (myCamList->num == 0) {
             AC_ERROR << "No cameras found";
             return;
         }

         for (int i = 0; i < myCamList->num; i++) {
             dc1394camera_t * myCamera = dc1394_camera_new( d, myCamList->ids[i].guid );
             if (!myCamera) {
                 AC_ERROR << "Failed to initialize camera with guid " 
                          << myCamList->ids[i].guid ;
                 continue;
             } else {
                _myDevices.push_back(myCamera);
             }
         }
         _myDeviceCount = _myDevices.size();
         dc1394_camera_free_list( myCamList );
         dc1394_free( d );
    }

    void
    DC1394::resetBus() {

        for (unsigned i = 0; i < _myDevices.size(); i++) {
            dc1394_reset_bus( _myDevices[i] );
            dc1394_camera_free( _myDevices[i] );
        }

    }

    void DC1394::readFrame(dom::ResizeableRasterPtr theTargetRaster) {
        dc1394camera_t * myCamera = &getDeviceHandle();
        dc1394video_frame_t * myFrame = NULL;

        dc1394error_t myStatus = dc1394_capture_dequeue( myCamera, 
                                                         DC1394_CAPTURE_POLICY_POLL, 
                                                         &myFrame );
        ASSERT_DC1394(myStatus);

        if (myFrame) {
            unsigned int myWidth = 0;
            unsigned int myHeight = 0;
            myStatus = dc1394_get_image_size_from_video_mode( myCamera, 
                                                              _myVideoMode, 
                                                              &myWidth, &myHeight);
            ASSERT_DC1394(myStatus);
            
            dc1394color_coding_t myColorCoding;
            myStatus = dc1394_get_color_coding_from_video_mode( myCamera, 
                                                                _myVideoMode, 
                                                                &myColorCoding);
            ASSERT_DC1394(myStatus);

            theTargetRaster->resize(myWidth, myHeight);
            myStatus = dc1394_convert_to_RGB8( myFrame->image, 
                                               theTargetRaster->pixels().begin(), 
                                               myWidth, myHeight, DC1394_BYTE_ORDER_UYVY, 
                                               myColorCoding, 8 );
            ASSERT_DC1394(myStatus);

            dc1394_capture_enqueue( myCamera, myFrame );
            ASSERT_DC1394(myStatus);
        }
    }

    template<typename T>
    void DC1394::getOption( const std::string & theOptionString, 
                            const std::string & theUrl,
                            T & theOption ) 
    {

        std::string::size_type idx = theUrl.find( theOptionString + "=" );
        std::string::size_type optpos = idx + theOptionString.length() + 1;

        if (idx != std::string::npos) 
        {
            std::string myValStr = theUrl.substr( optpos ); 
            if (!myValStr.empty()) {
                idx = myValStr.find(",");
                if (idx != std::string::npos) {
                    myValStr = myValStr.substr( 0, idx );
                }
                theOption = asl::as<T>( myValStr ); 
            }
        } 

    }

    void DC1394::load(const std::string & theFilename) {

        dc1394error_t myStatus;
        dc1394camera_t * myCamera = &getDeviceHandle();

        AC_DEBUG << "DC1394::load " << theFilename;

        unsigned myFrameRate = (unsigned)getFrameRate();
        unsigned myInputPinNumber = 0;
        unsigned myDeviceId = getDevice();
        unsigned myWhitebalanceB;
        unsigned myWhitebalanceR;
        unsigned myShutter;
        unsigned myGain;
        std::string myVideoModeString="";

        setPixelFormat(RGB);

        getOption("fps", theFilename, myFrameRate);
        getOption("video_mode", theFilename, myVideoModeString);
        getOption("device", theFilename, myDeviceId);
        getOption("whitebalanceb", theFilename, myWhitebalanceB );
        getOption("whitebalancer", theFilename, myWhitebalanceR );
        getOption("shutter", theFilename, myShutter);
        getOption("gain", theFilename, myGain);

        if (myDeviceId >= _myDeviceCount) {
            throw asl::Exception( "No such Device. Highest available DeviceId is: " 
                                  + _myDeviceCount-1, PLUS_FILE_LINE);
        } 
      
        if (myVideoModeString == "160x120_YUV444") {
            _myVideoMode = DC1394_VIDEO_MODE_160x120_YUV444;
        } else if (myVideoModeString == "320x240_YUV422") {
            _myVideoMode = DC1394_VIDEO_MODE_320x240_YUV422;
        } else if (myVideoModeString == "640x480_YUV411") {
            _myVideoMode = DC1394_VIDEO_MODE_640x480_YUV411;
        } else if (myVideoModeString == "640x480_YUV422") {
            _myVideoMode = DC1394_VIDEO_MODE_640x480_YUV422;
        } else if (myVideoModeString == "640x480_RGB8") {
            _myVideoMode = DC1394_VIDEO_MODE_640x480_RGB8;
        } else if (myVideoModeString == "640x480_MONO8") {
            _myVideoMode = DC1394_VIDEO_MODE_640x480_MONO8;
        } else {
            dc1394video_modes_t myModes;
            myStatus = dc1394_video_get_supported_modes( myCamera, &myModes );
            std::string myModeList = "VideoModes: ";
            for (unsigned i = 0; i < myModes.num; i++) {
                myModeList += myModes.modes[i] + " "; 
            }
            AC_PRINT << myModeList; 
            throw Exception("Unknown video mode", PLUS_FILE_LINE);
        }

        myStatus = dc1394_video_set_mode(myCamera, _myVideoMode);
        ASSERT_DC1394(myStatus);
       
        unsigned int myWidth;
        unsigned int myHeight;
        myStatus = dc1394_get_image_size_from_video_mode(myCamera, _myVideoMode, &myWidth, &myHeight);
        ASSERT_DC1394(myStatus);

        setName(string(getDeviceHandle().vendor)+"/"+string(getDeviceHandle().model));
        setFrameRate(myFrameRate);
        setFrameHeight(myHeight);
        setFrameWidth(myWidth);
        setDevice(myDeviceId);

        // Setup video size and image matrix
        float myXResize = float(myWidth) / asl::nextPowerOfTwo(myWidth);
        float myYResize = float(myHeight) / asl::nextPowerOfTwo(myHeight);

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

        myStatus = dc1394_capture_setup(&getDeviceHandle(), 10, 
                                        DC1394_CAPTURE_FLAGS_DEFAULT);
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
