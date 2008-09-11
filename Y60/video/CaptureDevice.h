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
//   $RCSfile: CaptureDevice.h,v $
//
//   $Author: jens $
//
//   $Revision: 1.2 $
//
//=============================================================================

#ifndef _ac_video_CaptureDevice_h_
#define _ac_video_CaptureDevice_h_

#include "Capture.h"
#include <y60/base/NodeNames.h>
#include <y60/base/NodeValueNames.h>
#include <y60/base/IDecoder.h>
#include <y60/image/PixelEncoding.h>
#include <y60/image/Image.h>
#include <asl/dom/Value.h>

namespace y60 {
    
    /*! @addtogroup Y60video */
    /* @{ */

    class CaptureDevice : public IDecoder {
    public:
        virtual ~CaptureDevice() {};
        /**
         * Stops the currently running capturing. No new frames should be displayed on
         * calls to readframe. Ideally, implementations should drop all connections to
         * the hardware device so that other captures can be performed on this.
         */
        virtual void stopCapture() {;}
        /**
         * Starts capturing on the device.
         */
        virtual void startCapture() {;}
        /**
         * Pauses capturing on the device. Most of the time this has the same effect as stopCapture
         * but implementations may choose this to be less resource intensive for a faster resume than
         * stop. While in pause, the hardware remains locked to this class.
         */
        virtual void pauseCapture() {;}
        /**
         * Reads a frame from the decoder.
         * @param theTargetRaster raster to read the frame into
         * @return real time of the frame return in theTargetRaster
         */
        virtual void readFrame(dom::ResizeableRasterPtr theTargetRaster) = 0;
        virtual void load(const std::string & theFilename) = 0;

        /**
        * @return new instance of the concrete CaptureDevice.
        */
        virtual asl::Ptr<CaptureDevice> instance() const {
            return asl::Ptr<CaptureDevice>(0);
        }

        void parseOptions(const std::string & theURL);
#if 0
        virtual std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>(0)) {
            AC_ERROR << asl::StackTrace();
            AC_ERROR << "canDecode() not implemented in CaptureDevice();
            return "";
        }
#endif
        void initialize(Capture * theParent) {
            _myParent = theParent;
        }

        unsigned getFrameWidth() const {
            return _myParent->get<ImageWidthTag>();
        }
        unsigned getFrameHeight() const {
            return _myParent->get<ImageHeightTag>();
        }
        double getFrameRate() const {
            return _myParent->get<FrameRateTag>();
        }
        void setFrameWidth(unsigned theFrameWidth) {
            _myParent->set<ImageWidthTag>(theFrameWidth);
        }
        void setFrameHeight(unsigned theFrameHeight) {
            _myParent->set<ImageHeightTag>(theFrameHeight);
        }
        void setFrameRate(double theFrameRate) {
            _myParent->set<FrameRateTag>(theFrameRate);
        }
        void setPixelFormat(const PixelEncoding thePixelFormat) {
            _myParent->set<RasterPixelFormatTag>(asl::getStringFromEnum(thePixelFormat, PixelEncodingString));
        }
        void setImageMatrix(const asl::Matrix4f & theMatrix) {
            _myParent->set<ImageMatrixTag>(theMatrix);
        }
        void setName(const std::string & theName) {
            _myParent->set<NameTag>(theName);
        }
        void setDevice(unsigned theId) {
            _myParent->set<DeviceTag>(theId);
        }
        unsigned getDevice() const {
            return _myParent->get<DeviceTag>();
        }
        template <class T>
        bool getOption(const std::string & theOption, T & theValue) {
            std::map<std::string, std::string>::iterator it = _myOptions.find(theOption);
            if (it == _myOptions.end()) {
                return false;
            }
            theValue = asl::as<T>(it->second);
            return true;
        };
    private:
        std::map<std::string, std::string> _myOptions;
        Capture *   _myParent;
    };

    typedef asl::Ptr<CaptureDevice> CaptureDevicePtr;

    /* @} */
}

#endif // _ac_video_CaptureDevice_h_
