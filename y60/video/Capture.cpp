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

// own header
#include "Capture.h"

#include <asl/base/Logger.h>
#include "CaptureDevice.h"
#include <y60/base/DecoderManager.h>

using namespace dom;
using namespace std;

namespace y60 {

    Capture::Capture(dom::Node & theNode)
        : Image(theNode),
        DeviceTag::Plug(theNode),
        NormTag::Plug(theNode),
        FrameRateTag::Plug(theNode),
        PlayModeTag::Plug(theNode),
        TimeCodeTag::Plug(theNode),
         _myDevice(),
        _myPlayMode(PLAY_MODE_STOP)
    {
        AC_TRACE << "Capture::Capture " << theNode;
    }

    Capture::~Capture() {
    }

    void Capture::stop() {
        AC_INFO << "Capture::stop";
        _myDevice->stopCapture();
        set<PlayModeTag>(asl::getStringFromEnum(PLAY_MODE_STOP, MoviePlayModeStrings));
        dom::ResizeableRasterPtr myRaster = getRasterPtr();
        memset(myRaster->pixels().begin(), 0, myRaster->pixels().size());
    }

    void
    Capture::setPlayMode(MoviePlayMode thePlayMode) {
        AC_DEBUG << "Capture::setPlayMode " << asl::getStringFromEnum(thePlayMode, MoviePlayModeStrings);
        // process changes
        switch (thePlayMode) {
            case PLAY_MODE_STOP:
                stop();
                break;
            case PLAY_MODE_PLAY:
                _myDevice->startCapture();
                break;
            case PLAY_MODE_PAUSE:
                if (_myPlayMode == PLAY_MODE_STOP) {
                    _myDevice->startCapture();
                }
                _myDevice->pauseCapture();
                break;
            case PLAY_MODE_NODISK:
                //XXX check if we should do something else here
                AC_INFO << "Capture Playmode set to PLAY_MODE_NODISK";
                break;
        }

        // Synchronize internal and external representation
        _myPlayMode = thePlayMode;
        set<PlayModeTag>(asl::getStringFromEnum(thePlayMode, MoviePlayModeStrings));
    }

    void
    Capture::readFrame() {
        if (!_myDevice) {
            AC_ERROR << "Capture::readFrame not allowed before open";
            return;
        }
        // Check for playmode changes from javascript
        MoviePlayMode myPlayMode = MoviePlayMode(asl::getEnumFromString(get<PlayModeTag>(), MoviePlayModeStrings));
        if (myPlayMode != _myPlayMode) {
            setPlayMode(myPlayMode);
        }
        _myDevice->readFrame(getRasterPtr());
    }

    void Capture::load(asl::PackageManager & thePackageManager) {

        const std::string & mySourceFile = get<ImageSourceTag>();
        AC_DEBUG << "Capture::load " << mySourceFile;
        open(mySourceFile);
    }

    void
    Capture::load(const std::string & theTexturePath) {
        const std::string & mySourceFile = get<ImageSourceTag>();
        open(mySourceFile);
    }

    void
    Capture::open(const std::string & theUrl) {
        if (_myDevice) {
            _myDevice = CaptureDevicePtr();
        }

        AC_DEBUG << "Capture::open " << (void*)this << " url=" << theUrl;

        // First: Look for registered decoders that could handle the source
        CaptureDevicePtr myDevice = DecoderManager::get().findDecoder<CaptureDevice>(theUrl);

        if (!myDevice) {
            AC_ERROR << "Capture::open(): could not find capture device for '" << theUrl << "'";
            return;
        }
        _myDevice = myDevice->instance();

        _myDevice->initialize(this);
        dom::ValuePtr myRasterValue = getRasterValue();
        if (!myRasterValue) {
            // Create default raster
            createRaster(1,1,1,GRAY);
        }
        _myDevice->load(theUrl);

        /*
         * UH: theUrl is mangled by the PackageManager and is not necessarily the same as the ImageSourceTag
         * this leads to multiple load() calls by the TextureManager since reloadRequired checks
         * _myLoadedFilename against ImageSourceTag and finds them to differ.
         */
        _myLoadedFilename = get<ImageSourceTag>(); // theUrl;

        Image::createRaster(get<ImageWidthTag>(), get<ImageHeightTag>(), 1, getRasterEncoding());

        if (_myPlayMode == PLAY_MODE_PLAY) {
            _myDevice->startCapture();
        }
    }

    bool
    Capture::reloadRequired() const {
        return (!_myDevice || _myLoadedFilename != get<ImageSourceTag>());
    }
}
