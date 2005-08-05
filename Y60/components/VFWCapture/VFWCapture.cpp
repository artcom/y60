#include "VFWCapture.h"
#include <vfw.h>
#include <iostream>
#include <asl/Logger.h>
#include <asl/Time.h>

extern "C"
EXPORT asl::PlugInBase * y60VFWCapture_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::VFWCapture(myDLHandle);
}

using namespace std;

namespace y60 {
    //static VFWCapture * ourCaptureObject = NULL;

    /**
     *
     * @param theCaptureWindowHandle capture window handle
     * @param theFrameInfo 
     * @return 
     */
    static LRESULT PASCAL frameCallbackProc(HWND theCaptureWindowHandle, LPVIDEOHDR theFrameInfo) {
        cerr << "Callback for Frame" << endl;
        VFWCapture * myCaptureObject = (VFWCapture *) capGetUserData(theCaptureWindowHandle);
        if (myCaptureObject) {
            unsigned char * myImageBuffer = (unsigned char *)theFrameInfo->lpData;    
            myCaptureObject->setGrabbedFrameRGB(myImageBuffer);
        } 
        return true; 
    }

    /**
     *
     * @param theCaptureWindowHandle capture window handle
     * @param theId status code for the current status 
     * @param theStatusText status text string for the current status
     * @return true
     */
    static LRESULT PASCAL statusCallbackProc(HWND theCaptureWindowHandle, int theId, LPSTR theStatusText) 
    { 
        //cerr << "Status Message[" << theId << "]: " << theStatusText << endl;
        return (LRESULT) TRUE; 
    } 

    /**
     * error callback function
     * @param theCaptureWindowHandle capture window handle
     * @param theErrorId error code for the encountered error
     * @param theErrorText error text string for the encountered error
     * @return true if error was displayed
     */
    static LRESULT PASCAL errorCallbackProc(HWND theCaptureWindowHandle, int theErrorId,
        LPSTR theErrorText) 
    { 
        if (!theCaptureWindowHandle) return FALSE; 

        if (theErrorId == 0)            // Starting a new major function. 
            return TRUE;            // Clear out old errors. 

        cerr << "Capture Error [" << theErrorId << "]: " << theErrorText << endl;

        return (LRESULT) TRUE; 
    } 

    static LRESULT PASCAL videoCallbackProc(HWND theCaptureWindowHandle, LPVIDEOHDR theVideoHeader) {
        if (!theCaptureWindowHandle || !theVideoHeader) return FALSE;
        //cerr << "Callback for Stream" << endl;
        VFWCapture * myCaptureObject = (VFWCapture *) capGetUserData(theCaptureWindowHandle);
        if (myCaptureObject) {
            unsigned char * myImageBuffer = (unsigned char *)theVideoHeader->lpData;    
            myCaptureObject->setGrabbedFrameRGB(myImageBuffer);
        } 
        return true; 
    }

    VFWCapture::VFWCapture(asl::DLHandle theDLHandle) : PlugInBase(theDLHandle),
        _myLastGrabbedFrame(0), _myGrabber(0) {
        AC_INFO << "Created VFW Capture";
    }

    VFWCapture::~VFWCapture() {
        if (_myGrabber) {
            // Disables frame callback function
            capSetUserData(_myGrabber, NULL);
            capDriverDisconnect(_myGrabber); 
            capCaptureStop(_myGrabber);
            //capSetCallbackOnFrame(_myGrabber, NULL);     
            //capSetCallbackOnStatus(_myGrabber, NULL);
            capSetCallbackOnError(_myGrabber, NULL);
            capSetCallbackOnVideoStream(_myGrabber, NULL);
            _myGrabber = NULL;
        } 
        if (_myLastGrabbedFrame) {
            delete [] _myLastGrabbedFrame;
            _myLastGrabbedFrame = NULL;
        }
    }


    double VFWCapture::readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster) {
        // Start next capture
        //_myLastGrabbedFrame = 0;
        //capGrabFrame(_myGrabber);
        //capGrabFrameNoStop(_myGrabber);
        _myLock.lock();
        if (_myLastGrabbedFrame && getPlayMode() == y60::PLAY_MODE_PLAY) {
            theTargetRaster->resize(getFrameWidth(), getFrameHeight());
            unsigned myBufferLength = getFrameWidth() * getFrameHeight() * 3;
            //R = Y + 1.403V'
            //G = Y - 0.344U' - 0.714V'
            //B = Y + 1.770U'
            //Uyvy 
            if (myBufferLength == theTargetRaster->pixels().size()) {
                memcpy(theTargetRaster->pixels().begin(), _myLastGrabbedFrame, myBufferLength);
                //_myFirstFrameDelivered = true;
            } else {
                cerr << "### WARNING : Target raster has wrong size. buffer=" << myBufferLength << " raster=" << theTargetRaster->pixels().size() << endl;
            }
        }
        _myLock.unlock();
        return theTime;
    }

    void VFWCapture::load(const std::string & theFilename) {
        //getDevices();
        AC_INFO << "Load " << theFilename;
        // TODO: Read deviceId from URL given in theFilename 
        int myDeviceId = 0;
        // Create frame grabber
        _myGrabber = capCreateCaptureWindow(NULL, WS_POPUP, 0, 0, 1, 1, 0, 0);
        capPreview(_myGrabber, FALSE);
        capSetUserData(_myGrabber, this);
        capOverlay(_myGrabber, FALSE);

        //if (ourCaptureObject) throw Exception("Capture Object already exists", PLUS_FILE_LINE);
        //ourCaptureObject = this;
        // Register the frame callback function
        //if (!capSetCallbackOnFrame(_myGrabber, frameCallbackProc)) {
        //    throw VFWCapture::Exception("capSetCallbackOnFrame() failed.", PLUS_FILE_LINE);
        //}
        //capSetCallbackOnStatus(_myGrabber, statusCallbackProc);
        capSetCallbackOnError(_myGrabber, errorCallbackProc);
        capSetCallbackOnVideoStream(_myGrabber, videoCallbackProc);

        // Connect to a capture driver 
        if (!capDriverConnect(_myGrabber, myDeviceId)) {
            throw VFWCapture::Exception("capDriverConnect() failed.", PLUS_FILE_LINE);
        }

        CAPTUREPARMS myCapParams;
        _myFrameWidth = 720;
        _myFrameHeight = 576;
        //_myFrameWidth = 640;
        //_myFrameHeight = 480;
        unsigned myFrameRate = 25;
        int idx = theFilename.find("width=");
        if (idx != std::string::npos) {
            _myFrameWidth = asl::as_int(theFilename.substr(idx+6));
        }
        idx = theFilename.find("height=");
        if (idx != std::string::npos) {
            _myFrameHeight = asl::as_int(theFilename.substr(idx+7));
        }
        std::string myFourCC = "RGB";
        idx = theFilename.find("format=");
        if (idx != std::string::npos) {
            myFourCC = theFilename.substr(idx+7);
        }

        capCaptureGetSetup(_myGrabber,&myCapParams,sizeof(myCapParams));
        myCapParams.fAbortLeftMouse = FALSE;
        myCapParams.fAbortRightMouse = FALSE;
        myCapParams.fYield = TRUE;
        myCapParams.fCaptureAudio = FALSE;
        //myCapParams.wPercentDropForError = 100;
        myCapParams.dwRequestMicroSecPerFrame = 1000000/myFrameRate;
        //myCapParams.wStepCaptureAverageFrames = 1;
        capCaptureSetSetup(_myGrabber,&myCapParams,sizeof(myCapParams));

        BITMAPINFO myBitmapInfo;
        if (!capGetVideoFormat(_myGrabber, &myBitmapInfo, sizeof(myBitmapInfo))) {
            throw VFWCapture::Exception("capGetVideoFormat() failed.", PLUS_FILE_LINE);
        }
        BITMAPINFOHEADER & myBitmapInfoHeader = myBitmapInfo.bmiHeader; 
        cerr << "Bitmapinfo: Compression: " << myBitmapInfoHeader.biCompression << endl;
        //memset(&myBitmapInfo, 0, sizeof(BITMAPINFO)); 
        //myBitmapInfoHeader.biHeight = _myFrameHeight;
        //myBitmapInfoHeader.biSizeImage = _myFrameWidth * _myFrameHeight * 3; // XXX
        //myBitmapInfoHeader.biWidth = _myFrameWidth;
        //myBitmapInfoHeader.biCompression = BI_RGB;
        _myFrameHeight = myBitmapInfoHeader.biHeight;
        _myFrameWidth = myBitmapInfoHeader.biWidth;
        if (!capSetVideoFormat(_myGrabber, &myBitmapInfo, sizeof(BITMAPINFO))) {
            throw Exception("capSetVideoFormat() failed.", PLUS_FILE_LINE);
        }
        if (_myLastGrabbedFrame) {
            delete[] _myLastGrabbedFrame; 
        }
        _myLastGrabbedFrame = new unsigned char[_myFrameWidth*_myFrameHeight*3];
        setPixelFormat(BGR);

        setFrameCount(UINT_MAX);
        setFrameRate(myFrameRate);

        // Setup video size and image matrix
        setFrameWidth(_myFrameWidth);
        setFrameHeight(_myFrameHeight);
        float myXResize = float(_myFrameWidth) / asl::nextPowerOfTwo(_myFrameWidth);
        float myYResize = float(_myFrameHeight) / asl::nextPowerOfTwo(_myFrameHeight);

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(asl::Vector3f(myXResize, - myYResize, 1.0f));
        myMatrix.translate(asl::Vector3f(0, myYResize, 0));
        setImageMatrix(myMatrix);

    }
    void VFWCapture::setGrabbedFrameRGB(unsigned char* theImageBuffer) {
        _myLock.lock();
        memcpy(_myLastGrabbedFrame, theImageBuffer, _myFrameHeight*_myFrameWidth*3);
        _myLock.unlock();
    }

    void VFWCapture::setGrabbedFrameUYVY(unsigned char* theImageBuffer) {
        int y=0;
        int u=0;
        int v=0;
        _myLock.lock();
        for (int row = 0; row < _myFrameHeight; ++row) {
            for(int col = 0; col < _myFrameWidth; ++col) {
                unsigned char v1 = theImageBuffer[((_myFrameHeight-row)*_myFrameWidth+_myFrameWidth-col)*2];
                unsigned char v2 = theImageBuffer[((_myFrameHeight-row)*_myFrameWidth+_myFrameWidth-col)*2+1];
                y = v2;
                if ((col & 1) == 1) {
                    v = v1;
                } else {
                    u = v1;
                }

                int r = y + (1.370705 * (v-128));
                int g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
                int b = y + (1.732446 * (u-128));
                // Even with proper conversion, some values still need clipping.
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;
                if (r < 0) r = 0;
                if (g < 0) g = 0;
                if (b < 0) b = 0;
                _myLastGrabbedFrame[(row*_myFrameWidth+col)*3] = (unsigned char)r;
                _myLastGrabbedFrame[(row*_myFrameWidth+col)*3+1] = (unsigned char)g;
                _myLastGrabbedFrame[(row*_myFrameWidth+col)*3+2] = (unsigned char)b;
            }
        }
        _myLock.unlock();
    }

    std::string VFWCapture::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
        if (theUrl.find("video://") != std::string::npos) {
            return MIME_TYPE_CAMERA;
        } else {
            return "";
        }
    }

    asl::Ptr<MovieDecoderBase> VFWCapture::instance() const {
        return asl::Ptr<MovieDecoderBase>(new VFWCapture(getDLHandle()));
    }

    void VFWCapture::stopMovie() {
        capCaptureStop(_myGrabber);
        MovieDecoderBase::stopMovie();
    }
    
    void VFWCapture::startMovie(double theStartTime) {
        capCaptureSequenceNoFile(_myGrabber);
        MovieDecoderBase::startMovie(theStartTime);
    }

    void VFWCapture::getDevices() {
        char szDeviceName[80];
        char szDeviceVersion[80];
        for (int wIndex = 0; wIndex < 10; wIndex++) 
        {
            if (capGetDriverDescription (wIndex, szDeviceName, 
                sizeof (szDeviceName), szDeviceVersion, 
                sizeof (szDeviceVersion))) 
            {
                cout << szDeviceName << " : " << szDeviceVersion << endl;
                // Append name to list of installed capture drivers
                // and then let the user select a driver to use.
            }
        } 
    }


}
