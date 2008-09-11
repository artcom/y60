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
//   $RCSfile: Capture.h,v $
//
//   $Author: jens $
//
//   $Revision: 1.2 $
//
//=============================================================================

#ifndef _ac_video_Capture_h_
#define _ac_video_Capture_h_

#include "Movie.h"
#include <y60/base/NodeNames.h>

namespace y60 {

    /*! @addtogroup Y60video */
    /* @{ */

    //                  theTagName    theType      theAttributeName            theDefault
    DEFINE_ATTRIBUT_TAG(NormTag,      std::string, CAPTURE_NORM_ATTRIB,   "PAL");
    DEFINE_ATTRIBUT_TAG(DeviceTag,    unsigned,    CAPTURE_DEVICE_ATTRIB, 0);

    class CaptureDevice;

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
    class Capture : public Image,
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
