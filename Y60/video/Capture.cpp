#include "Capture.h"
#include <asl/Logger.h>
#include "CaptureDevice.h"
#include <y60/DecoderManager.h>

using namespace dom;
using namespace std;

namespace y60 {

    Capture::Capture(dom::Node & theNode) : Image(theNode),
        FrameRateTag::Plug(theNode),
        PlayModeTag::Plug(theNode),
        DeviceTag::Plug(theNode),
        NormTag::Plug(theNode), _myDevice(0),
        _myPlayMode(PLAY_MODE_STOP)
    {
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
        AC_DEBUG << "Movie::setPlayMode " << asl::getStringFromEnum(thePlayMode, MoviePlayModeStrings);
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

        AC_DEBUG << "Capture::open " << (void*)this << " url=" << theUrl;

        // First: Look for registered decoders that could handle the source
        CaptureDevicePtr myDevice = DecoderManager::get().findDecoder<CaptureDevice>(theUrl);

        if (!myDevice) {
            _myDevice = CaptureDevicePtr(0);
            return;
        }
        _myDevice = myDevice->instance();

        _myDevice->initialize(this);
        _myDevice->load(theUrl);

        /*
         * UH: theUrl is mangled by the PackageManager and is not necessarily the same as the ImageSourceTag
         * this leads to multiple load() calls by the TextureManager since reloadRequired checks
         * _myLoadedFilename against ImageSourceTag and finds them to differ.
         */
        _myLoadedFilename = get<ImageSourceTag>(); // theUrl;

        Image::createRaster(_myDevice->getPixelFormat());
        getRasterPtr()->resize(get<ImageWidthTag>(), get<ImageHeightTag>());

    }

    bool
    Capture::reloadRequired() const {
        return (!_myDevice || _myLoadedFilename != get<ImageSourceTag>());
    }
}
