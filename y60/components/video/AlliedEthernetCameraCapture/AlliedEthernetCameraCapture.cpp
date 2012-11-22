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
*/

#include "AlliedEthernetCameraCapture.h"
#include <asl/base/Logger.h>
#include <asl/base/Time.h>
#include <asl/base/assure.h>
#include <asl/base/Block.h>
#include <y60/base/DecoderManager.h>
#include <asl/base/Dashboard.h>
#include <asl/base/Auto.h>

#include <iostream>

#include <Winsock2.h>


#ifdef WIN32
// Disable FFMPeg Warning in Windows
#pragma warning(push)
#pragma warning(disable:4244)
#define EMULATE_INTTYPES
#endif
extern "C" {
#   include <libavcodec/avcodec.h>
#   include <libswscale/swscale.h>
}

using namespace std;
using namespace asl;

extern "C"
EXPORT asl::PlugInBase * AlliedEthernetCameraCapture_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::AlliedEthernetCameraCapture(myDLHandle);
}

namespace y60 {
    AlliedEthernetCameraCapture::AlliedEthernetCameraCapture(asl::DLHandle theDLHandle) 
        : PlugInBase(theDLHandle), PosixThread(),
            _myIP(""),
            _myMsgQueue(),
            _myDestinationPixelFormat(RGB),        
            _myMaxCacheSize(2)
    {
    }

    AlliedEthernetCameraCapture::~AlliedEthernetCameraCapture() {
        if (_myGCamera.Opened) {
            PvCameraClose(_myGCamera.Handle);

            _myGCamera.Handle = NULL;
        }
    }

    bool AlliedEthernetCameraCapture::CameraGrab() {
        tPvUint32 count,connected;
        tPvCameraInfo list;

        count = PvCameraList(&list,1,&connected);
        if(count == 1)
        {
            _myGCamera.UID = list.UniqueId;
            return true;
        
        }
        else
            return false;
    }

    // open the camera
    bool AlliedEthernetCameraCapture::CameraSetup()
    {
        // open the camera and allocate the buffers we need for streaming
        if(!PvCameraOpen(_myGCamera.UID,ePvAccessMaster,&(_myGCamera.Handle)))
        {
            unsigned long FrameSize = 0;

            // Auto adjust the packet size to max supported by the network, up to a max of 8228.
            // NOTE: In Vista, if the packet size on the network card is set lower than 8228,
            //       this call may break the network card's driver. See release notes.
            //
            //PvCaptureAdjustPacketSize(GCamera.Handle,8228);
        
            // how big should the frame buffers be?
            if(!PvAttrUint32Get(_myGCamera.Handle,"TotalBytesPerFrame",&FrameSize))
            {
                bool failed = false;
        
                if(failed)
                {
                    PvCameraClose(_myGCamera.Handle);
                    _myGCamera.Handle = NULL;
                    return false;
                }
                else {
                    return true;
                }
            }
            else
            {
                PvCameraClose(_myGCamera.Handle);
                _myGCamera.Handle = NULL;
                return false;
            }
        }
        else
            return false;
    }

    void AlliedEthernetCameraCapture::run() {
        // decoder loop
        bool isDone = false;
        while (!shouldTerminate() && !isDone) {
            if (_myMsgQueue.size() >= _myMaxCacheSize) {
                asl::msleep(10);
                yield();
                continue;
            }

            VideoMsgPtr myVideoFrame = createFrame(0.0);
            _myGCamera.Frame.ImageBuffer = myVideoFrame->getBuffer(0);
            _myGCamera.Frame.ImageBufferSize = getFrameWidth() * getFrameHeight();
            {
                MAKE_SCOPE_TIMER(PvCaptureQueueFrame);
                PvCaptureQueueFrame(_myGCamera.Handle,&(_myGCamera.Frame),NULL);
            }
            tPvErr Err;
            {

                MAKE_SCOPE_TIMER(PvCaptureWaitForFrameDone);
                Err = PvCaptureWaitForFrameDone(_myGCamera.Handle,&_myGCamera.Frame, PVINFINITE);
                if (!Err) {
                    if (_myGCamera.Frame.Status == ePvErrSuccess) {
                        _myMsgQueue.push_back(myVideoFrame);
                    }
                }
            }
        }
    }
    VideoMsgPtr AlliedEthernetCameraCapture::createFrame(double theTimestamp) {
        AC_DEBUG << "AlliedEthernetCameraCapture::createFrame";
        vector<unsigned> myBufferSizes;
        switch (_myDestinationPixelFormat) {
            case RGBA:
                myBufferSizes.push_back(getFrameWidth() * getFrameHeight() * 4);
                break;
            case GRAY:
                myBufferSizes.push_back(getFrameWidth() * getFrameHeight() * 1);
                break;
            case YUV420:
                myBufferSizes.push_back(getFrameWidth() * getFrameHeight());
                myBufferSizes.push_back(getFrameWidth() * getFrameHeight() / 4);
                myBufferSizes.push_back(getFrameWidth() * getFrameHeight() / 4);
                break;
            default:
                myBufferSizes.push_back(getFrameWidth() * getFrameHeight() * 3);
                break;
        }
        return VideoMsgPtr(new VideoMsg(VideoMsg::MSG_FRAME, theTimestamp, myBufferSizes));
    }

    void AlliedEthernetCameraCapture::readFrame(dom::ResizeableRasterPtr theTargetRaster) {
        if (_myMsgQueue.size() < 1) {
            return;
        }
        VideoMsgPtr myVideoMsg = _myMsgQueue.pop_front();
        theTargetRaster->resize(getFrameWidth(), getFrameHeight());
        switch (_myDestinationPixelFormat) {
            case GRAY:
                memcpy(theTargetRaster->pixels().begin(), myVideoMsg->getBuffer(0),
                        theTargetRaster->pixels().size());
                break;
            default:
                AC_ERROR << "Sorry, yet unsupported pixelformat: " << _myDestinationPixelFormat;
                break;
        }
    }

    void AlliedEthernetCameraCapture::load(const std::string & theFilename) {
        std::string::size_type idx = theFilename.find("ip=");
         
        if (idx != std::string::npos) {
            string myTmp = theFilename.substr(idx+3);
            std::string::size_type endIdx = myTmp.find(";");
            _myIP = myTmp.substr(0, endIdx);
        }
        if(!PvInitialize()) { 
            bool myCameraFoundFlag = false;
            unsigned long myIp = inet_addr(_myIP.c_str());
            for (unsigned int i = 0; i < 10 ; i++) {
                if(!PvCameraOpenByAddr(myIp,ePvAccessMaster,&(_myGCamera.Handle))) {
                    myCameraFoundFlag = true;
                    break;
                }
                Sleep(250);
            }
            if (myCameraFoundFlag) {
                // grab a camera from the list
                if(CameraGrab())
                {
                    // setup the camera
                    _myGCamera.Opened = CameraSetup();
                    unsigned long myCaptureWidth;
                    unsigned long myCaptureHeight;
                    unsigned long myImageSize;
                    unsigned long myPixeltype;
                    PvAttrUint32Get(_myGCamera.Handle, "Width", &myCaptureWidth);
                    PvAttrUint32Get(_myGCamera.Handle, "Height", &myCaptureHeight);
                    PvAttrUint32Get(_myGCamera.Handle, "TotalBytesPerFrame", &myImageSize);
                    PvAttrUint32Get(_myGCamera.Handle, "PixelFormat", &myPixeltype);
                    setFrameHeight(myCaptureHeight);
                    setFrameWidth(myCaptureWidth);
                    AC_INFO << "myImageSize: " << myImageSize;
                    AC_INFO << "myCaptureWidth: " << myCaptureWidth;
                    AC_INFO << "myCaptureHeight: " << myCaptureHeight;
                    AC_INFO << "myPixeltype : " << myPixeltype;
                    _myDestinationPixelFormat = GRAY;
                        
                }
            } else {
                throw AlliedEthernetCameraCapture::Exception("Sorry, could not find Allied Ethernet Camera: ", PLUS_FILE_LINE);
            }
        } else {
            throw AlliedEthernetCameraCapture::Exception("Sorry, failed to initialize the Allied Ethernet Camera: ", PLUS_FILE_LINE);
        }

    }

    std::string AlliedEthernetCameraCapture::canDecode(const std::string & theUrl,
                                        asl::Ptr<asl::ReadableStreamHandle> theStream)
    {
        if (theUrl.find("alliedethernet://") != std::string::npos) {
            return MIME_TYPE_CAMERA;
        } else {
            return "";
        }
    }

    asl::Ptr<y60::CaptureDevice> AlliedEthernetCameraCapture::instance() const {
        return asl::Ptr<CaptureDevice>(new AlliedEthernetCameraCapture(getDLHandle()));
    }

    void AlliedEthernetCameraCapture::stopCapture() {
        CaptureDevice::stopCapture();
        PvCommandRun(_myGCamera.Handle, "AcquisitionStop");
    }

    void AlliedEthernetCameraCapture::startCapture() {
        _myMsgQueue.clear();
        _myMsgQueue.reset();
        CaptureDevice::startCapture();
        if(!PvCaptureStart(_myGCamera.Handle)) {
            PvAttrEnumSet(_myGCamera.Handle, "AcquisitionMode", "Continuous");
            PvAttrEnumSet(_myGCamera.Handle, "FrameStartTriggerMode", "Freerun");

            if (PvCommandRun(_myGCamera.Handle, "AcquisitionStart")) {
            }
        } else {
        }


        if (!isUnjoined()) {
            AC_DEBUG << "Forking FFMpegDecoder Thread";
            PosixThread::fork();
        } else {
            AC_DEBUG << "Thread already running. No forking.";
        }
    }

    void AlliedEthernetCameraCapture::pauseCapture() {
        CaptureDevice::pauseCapture();
    }



}
