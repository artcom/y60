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
//
//   $RCSfile: Capture.h,v $
//
//   $Author: jens $
//
//   $Revision: 1.2 $
//
//=============================================================================

#ifndef _ac_video_Capture_h_
#define _ac_video_Capture_h_

#include "y60_video_settings.h"

#include <asl/zip/PackageManager.h>
#include <y60/base/NodeNames.h>

#include "Movie.h"
#include "MovieDecoderBase.h"

namespace y60 {

    /*! @addtogroup Y60video */
    /* @{ */

    //                  theTagName    theType      theAttributeName            theDefault
    DEFINE_ATTRIBUTE_TAG(NormTag,      std::string, CAPTURE_NORM_ATTRIB,   "PAL", Y60_VIDEO_DECL);
    DEFINE_ATTRIBUTE_TAG(DeviceTag,    unsigned,    CAPTURE_DEVICE_ATTRIB, 0, Y60_VIDEO_DECL);

    class Y60_VIDEO_DECL CaptureDevice;

    /**
     * @ingroup y60-video
     * abstract class for capture devices. Capture devices playback live
     * video from an input device like a camera. Unlike movies they are
     * driven by external hardware and i.E. cannot be paused that easily.
     * Also unlike movies, it has a device and norm attribute. The norm
     * attribute is used to define the video norm of analog capture devices.
     * Some Values are PAL-B, NTSC, ...
     *
     * @see MovieDecoder
     */
    class Y60_VIDEO_DECL Capture : public Image,
        public DeviceTag::Plug,
        public NormTag::Plug,
        public FrameRateTag::Plug,
        public PlayModeTag::Plug
    {
    public:
        Capture(dom::Node & theNode);
        virtual ~Capture();

        IMPLEMENT_FACADE(Capture);

        void readFrame();
        /**
         * Opens the capture device
         * @param thePackageManager Capture device to open
         * @bug Since a capture device obviously cannot be packed into a
         *      package, calls will have an undefined behavior.
         */
        virtual void load(asl::PackageManager & thePackageManager);
        /**
         * Opens the capture device given in theTexturePath
         * @param theTexturePath Capture device to open
         */
        virtual void load(const std::string & theTexturePath = ".");
        /**
         * @retval true if a reload of the device is necessary, because some
         *         parameters changed.
         */
        virtual bool reloadRequired() const;

        MoviePlayMode getPlayMode() const {
            return _myPlayMode;
        }
        DEFINE_EXCEPTION(CaptureException, asl::Exception);
    private:
        Capture();

        // Overwrite the Image::load() method to avoid auto-loading mechanism
        void load() {}

        void stop();
        void setPlayMode(MoviePlayMode thePlayMode);
        void open(const std::string & theSourceFile);
        asl::Ptr<CaptureDevice>    _myDevice;
        std::string                _myLoadedFilename;
        MoviePlayMode         _myPlayMode;
    };

    typedef asl::Ptr<Capture, dom::ThreadingModel> CapturePtr;

    /* @} */

}

#endif // _ac_video_Capture_h_
