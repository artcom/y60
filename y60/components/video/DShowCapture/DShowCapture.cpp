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

#include "DShowCapture.h"
#include <asl/base/Logger.h>
#include <asl/base/Time.h>
#include <asl/base/assure.h>
#include <asl/base/Block.h>

#include <dshow.h>
#include <iostream>


#ifdef WIN32
// Disable FFMPeg Warning in Windows
#pragma warning(push)
#pragma warning(disable:4244)
#define EMULATE_INTTYPES
#endif
extern "C" {
#include <libavcodec/avcodec.h>

#if LIBAVCODEC_VERSION_INT >= ((51<<16)+(38<<8)+0)
#   include <libswscale/swscale.h>
#endif
}

extern "C"
EXPORT asl::PlugInBase * DShowCapture_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::DShowCapture(myDLHandle);
}

using namespace std;

namespace y60 {
    DShowCapture::DShowCapture(asl::DLHandle theDLHandle) : PlugInBase(theDLHandle), _myGraph(0), _myDeinterlaceFlag(false) {
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
                if (_myDeinterlaceFlag) {
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,20,0)
        PixelFormat
#else
        int
#endif
                    myPixelFormat = PIX_FMT_BGR24;
                    unsigned myBytesPerPixel = 3;
                    unsigned int myLineSizeBytes = _myGraph->getWidth() * myBytesPerPixel;

                    AVPicture myYUVPict;
                    asl::Ptr<asl::Block> myBlock(new asl::Block(myBufferLength));
                    unsigned size = _myGraph->getWidth() * _myGraph->getHeight();
                    myYUVPict.data[0] = myBlock->begin();
                    myYUVPict.data[1] = myYUVPict.data[0] + size;
                    myYUVPict.data[2] = myYUVPict.data[1] + size;

                    myYUVPict.linesize[0] = _myGraph->getWidth();
                    myYUVPict.linesize[1] = _myGraph->getWidth();
                    myYUVPict.linesize[2] = _myGraph->getWidth();

                    AVPicture mySrcPict;
                    mySrcPict.data[0] = myData;
                    mySrcPict.data[1] = myData+1;
                    mySrcPict.data[2] = myData+2;

                    mySrcPict.linesize[0] = myLineSizeBytes;
                    mySrcPict.linesize[1] = myLineSizeBytes;
                    mySrcPict.linesize[2] = myLineSizeBytes;

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,20,0)
        PixelFormat
#else
        int
#endif
                    myDestPixelFormat = PIX_FMT_YUV444P;
#if LIBAVCODEC_VERSION_INT < ((51<<16)+(38<<8)+0)
                    img_convert(&myYUVPict, myDestPixelFormat, &mySrcPict, myPixelFormat,
                                _myGraph->getWidth(), _myGraph->getHeight());
#else
                    int mySWSFlags = SWS_FAST_BILINEAR;//SWS_BICUBIC;
                    SwsContext * img_convert_ctx = sws_getContext(_myGraph->getWidth(), _myGraph->getHeight(),
                        myPixelFormat,
                        _myGraph->getWidth(), _myGraph->getHeight(),
                        myDestPixelFormat,
                        mySWSFlags, NULL, NULL, NULL);
                    sws_scale(img_convert_ctx, mySrcPict.data,
                        mySrcPict.linesize, 0, _myGraph->getHeight(),
                        myYUVPict.data, myYUVPict.linesize);

                    sws_freeContext(img_convert_ctx);
#endif

                    avpicture_deinterlace(&myYUVPict, &myYUVPict, myDestPixelFormat,  _myGraph->getWidth(), _myGraph->getHeight());

                    AVPicture myDestPict;
                    myDestPict.data[0] = theTargetRaster->pixels().begin();
                    myDestPict.data[1] = theTargetRaster->pixels().begin()+1;
                    myDestPict.data[2] = theTargetRaster->pixels().begin()+2;
                    myDestPict.linesize[0] = myLineSizeBytes;
                    myDestPict.linesize[1] = myLineSizeBytes;
                    myDestPict.linesize[2] = myLineSizeBytes;

#if LIBAVCODEC_VERSION_INT < ((51<<16)+(38<<8)+0)
                    img_convert(&myDestPict, myPixelFormat, &myYUVPict, myDestPixelFormat,
                                _myGraph->getWidth(), _myGraph->getHeight());
#else
                    img_convert_ctx = sws_getCachedContext(img_convert_ctx, _myGraph->getWidth(), _myGraph->getHeight(),
                        myDestPixelFormat,
                        _myGraph->getWidth(), _myGraph->getHeight(),
                        myPixelFormat,
                        mySWSFlags, NULL, NULL, NULL);
                    sws_scale(img_convert_ctx, myYUVPict.data,
                        myYUVPict.linesize, 0, _myGraph->getHeight(),
                        myDestPict.data, myDestPict.linesize);

                    sws_freeContext(img_convert_ctx);
#endif

                } else {
                    memcpy(theTargetRaster->pixels().begin(), myData, myBufferLength);
                }
            } else {
                AC_WARNING << "Target raster has wrong size. buffer=" << myBufferLength << " raster=" << theTargetRaster->pixels().size();
            }
            _myGraph->unlockImage();
        }
    }

    void DShowCapture::load(const std::string & theFilename) {
        AC_DEBUG << "DShowCapture::load " << theFilename;

        int myFrameWidth = 0;//getFrameWidth();
        int myFrameHeight = 0;//getFrameHeight();
        unsigned myFrameRate = static_cast<unsigned>(getFrameRate());
        unsigned myBitsPerPixel = 24;
        unsigned myInputPinNumber = 0;
        unsigned myDeviceId = getDevice();
        unsigned myWhitebalanceU = 0;
        unsigned myWhitebalanceV = 0;
        unsigned myShutter = 0;
        unsigned myGain = 0;
        bool myAVT_Camera_Flag = false;

        if (myFrameWidth == 0) {
            myFrameWidth = 640;
        }
        if (myFrameHeight == 0) {
            myFrameHeight = 480;
        }

        ASSURE(NULL == _myGraph);
        bool myRestartGraph = false;
        if (_myGraph) {
            myRestartGraph = _myGraph->isRunning();
            delete _myGraph;
        }

        std::string::size_type idx = theFilename.find("width=");
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
        idx = theFilename.find("bpp=");
        if (idx != std::string::npos) {
            myBitsPerPixel = asl::as_int(theFilename.substr(idx+4));
        }
        idx = theFilename.find("input=");
        if (idx != std::string::npos) {
            myInputPinNumber = asl::as_int(theFilename.substr(idx+6));
        }
        idx = theFilename.find("device=");
        if (idx != std::string::npos && theFilename.substr(idx+7).length() > 0) {
            myDeviceId = asl::as_int(theFilename.substr(idx+7));
        }
        idx = theFilename.find("avtcamera=");
        if (idx != std::string::npos && theFilename.substr(idx+10).length() > 0) {
            myAVT_Camera_Flag = asl::as_int(theFilename.substr(idx+10)) == 1;
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

        idx = theFilename.find("deinterlace=");
        if (idx != std::string::npos && theFilename.substr(idx+12).length() > 0) {
            _myDeinterlaceFlag = asl::as_int(theFilename.substr(idx+12)) == 1 ? true:false;
        }

        if (myAVT_Camera_Flag) {
            _myGraph = new AVTDShowGraph();
        } else {
            _myGraph = new DShowGraph();
        }

        setPixelFormat(BGR);


        std::vector<std::string> myDevices = _myGraph->enumDevices();
        std::string myDeviceName = "unknown";
        if (myDeviceId >= myDevices.size()) {
            //throw DShowCapture::Exception("No such Device. Highest available DeviceId is: " + myDevices.size()-1, PLUS_FILE_LINE);
        } else {
            myDeviceName = myDevices[myDeviceId];
        }

        setName(myDeviceName);
        setFrameRate(myFrameRate);
        setFrameHeight(myFrameHeight);
        setFrameWidth(myFrameWidth);
        setDevice(myDeviceId);

        // Setup video size and image matrix
        float myXResize = float(myFrameWidth) / asl::nextPowerOfTwo(myFrameWidth);
        float myYResize = float(myFrameHeight) / asl::nextPowerOfTwo(myFrameHeight);

        _myGraph->setDesiredVideoFormat(myFrameWidth, myFrameHeight, myFrameRate, myBitsPerPixel);

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(asl::Vector3f(1.0f, - 1.0f, 1.0f));
        myMatrix.translate(asl::Vector3f(0, 1.0f, 0));
        setImageMatrix(myMatrix);
        // Start Capturing
        _myGraph->CaptureLive(myDeviceId, myInputPinNumber); // set this to false if you don't want the video format dialog.
        if (myRestartGraph) {
            _myGraph->Play();
        }
        _myGraph->setCameraParams(myWhitebalanceU, myWhitebalanceV, myShutter, myGain);
    }

    std::string DShowCapture::canDecode(const std::string & theUrl,
                                        asl::Ptr<asl::ReadableStreamHandle> theStream)
    {
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
