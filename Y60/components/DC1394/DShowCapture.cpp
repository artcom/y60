
#include "DShowCapture.h"
#include <asl/Logger.h>
#include <asl/Time.h>
#include <asl/assure.h>

#include <dshow.h>
#include <iostream>

extern "C"
EXPORT asl::PlugInBase * y60DShowCapture_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::DShowCapture(myDLHandle);
}

using namespace std;

namespace y60 {
    DShowCapture::DShowCapture(asl::DLHandle theDLHandle) : PlugInBase(theDLHandle), _myGraph(0) {
        CoInitialize(0);
    }

    DShowCapture::~DShowCapture() {
        if (_myGraph) {
            delete _myGraph;
        }
        CoUninitialize();
    }

    void DShowCapture::readFrame(dom::ResizeableRasterPtr theTargetRaster) {
        unsigned char * myData = _myGraph->lockImage();
        if (myData) {
            theTargetRaster->resize(getFrameWidth(), getFrameHeight());
            unsigned myBufferLength = _myGraph->getHeight() * _myGraph->getWidth() * 3;
            if (myBufferLength == theTargetRaster->pixels().size()) {
                memcpy(theTargetRaster->pixels().begin(), myData, myBufferLength);
            } else {
                AC_WARNING << "Target raster has wrong size. buffer=" << myBufferLength << " raster=" << theTargetRaster->pixels().size();
            }
            _myGraph->unlockImage();
        }
    }

    void DShowCapture::load(const std::string & theFilename) {
        AC_DEBUG << "DShowCapture::load " << theFilename;

        int myFrameWidth = getFrameWidth();
        int myFrameHeight = getFrameHeight();
        unsigned myFrameRate = (unsigned)getFrameRate();
        unsigned myInputPinNumber = 0;
        unsigned myDeviceId = getDevice();
        unsigned myWhitebalanceU;
        unsigned myWhitebalanceV;
        unsigned myShutter;
        unsigned myGain;

        if (myFrameWidth == 0) {
            myFrameWidth = 320;
        }
        if (myFrameHeight == 0) {
            myFrameHeight = 240;
        }

        ASSURE(NULL == _myGraph);
        bool myRestartGraph = false;
        if (_myGraph) {
            myRestartGraph = _myGraph->isRunning();
            delete _myGraph;
        }
        _myGraph = new DShowGraph();

        setPixelFormat(BGR);

        int idx = theFilename.find("width=");
        if (idx != std::string::npos) {
            myFrameWidth = asl::as_int(theFilename.substr(idx+6));
        }
        idx = theFilename.find("height=");
        if (idx != std::string::npos) {
            myFrameHeight = asl::as_int(theFilename.substr(idx+7));
        }
        idx = theFilename.find("fps=");
        if (idx != std::string::npos) {
            myFrameRate = asl::as_int(theFilename.substr(idx+4));
        }
        idx = theFilename.find("input=");
        if (idx != std::string::npos) {
            myInputPinNumber = asl::as_int(theFilename.substr(idx+6));
        }
        idx = theFilename.find("device=");
        if (idx != std::string::npos && theFilename.substr(idx+7).length() > 0) {
            myDeviceId = asl::as_int(theFilename.substr(idx+7));
        }
        idx = theFilename.find("whitebalanceb=");
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
        std::vector<std::string> myDevices = _myGraph->enumDevices();
        if (myDeviceId >= myDevices.size()) {
            throw DShowCapture::Exception("No such Device. Highest available DeviceId is: " + myDevices.size()-1, PLUS_FILE_LINE);
        } 

        setName(myDevices[myDeviceId]);
        setFrameRate(myFrameRate);
        setFrameHeight(myFrameHeight);
        setFrameWidth(myFrameWidth);
        setDevice(myDeviceId);

        // Setup video size and image matrix
        float myXResize = float(myFrameWidth) / asl::nextPowerOfTwo(myFrameWidth);
        float myYResize = float(myFrameHeight) / asl::nextPowerOfTwo(myFrameHeight);
        _myGraph->setDesiredVideoFormat(myFrameWidth, myFrameHeight, myFrameRate, 24);

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(asl::Vector3f(myXResize, - myYResize, 1.0f));
        myMatrix.translate(asl::Vector3f(0, myYResize, 0));
        setImageMatrix(myMatrix);
        // Start Capturing
		_myGraph->CaptureLive(myDeviceId, myInputPinNumber); // set this to false if you don't want the video format dialog.
        if (myRestartGraph) {
            _myGraph->Play();
        }
        _myGraph->setCameraParams(myWhitebalanceU, myWhitebalanceV, myShutter, myGain);
    }

    std::string DShowCapture::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
        if (theUrl.find("dshow://") != std::string::npos) {
            return MIME_TYPE_CAMERA;
        } else {
            return "";
        }
    }

    asl::Ptr<CaptureDevice> DShowCapture::instance() const {
        return asl::Ptr<CaptureDevice>(new DShowCapture(getDLHandle()));
    }

    void DShowCapture::stopCapture() {
        _myGraph->Stop();
        CaptureDevice::stopCapture();
    }
    
    void DShowCapture::startCapture() {
        _myGraph->Play();
        CaptureDevice::startCapture();
    }

    void DShowCapture::pauseCapture() {
        _myGraph->Stop();
        CaptureDevice::pauseCapture();
    }
}
