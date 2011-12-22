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

// own header
#include "Movie.h"

#include <limits>
#include <string.h>

#include "MovieDecoderBase.h"
#include "AsyncDecoder.h"
#include "M60Decoder.h"

#include <y60/image/PixelEncoding.h>
#include <y60/base/DecoderManager.h>
#include <y60/base/IScene.h>
#include <asl/base/Logger.h>
#include <asl/zip/PackageManager.h>
#include <asl/base/PlugInManager.h>
#include <asl/base/Assure.h>
#include <asl/base/Dashboard.h>
#include <asl/math/numeric_functions.h>

using namespace dom;
using namespace std;
using namespace asl;

#define DB(x) // x
#define DB2(x) // x


namespace y60 {

    Movie::Movie(dom::Node & theNode):
        Image(theNode),
        FrameCountTag::Plug(theNode),
        CurrentFrameTag::Plug(theNode),
        FrameRateTag::Plug(theNode),
        PlaySpeedTag::Plug(theNode),
        PlayModeTag::Plug(theNode),
        VolumeTag::Plug(theNode),
        LoopCountTag::Plug(theNode),
        CacheSizeTag::Plug(theNode),
        MaxCacheSizeTag::Plug(theNode),
        AudioStreamTag::Plug(theNode),
        AVDelayTag::Plug(theNode),
        AudioTag::Plug(theNode),
        DecoderHintTag::Plug(theNode),
        AspectRatioTag::Plug(theNode),
        dom::DynamicAttributePlug<MovieTimeTag, Movie>(this, &Movie::getMovieTime),
        dom::DynamicAttributePlug<DecoderTag, Movie>(this, &Movie::getDecoderName),
        dom::DynamicAttributePlug<HasAudioTag, Movie>(this, &Movie::hasAudio),
        _myDecoder(),
        _myPlayMode(PLAY_MODE_STOP),
        _myLastDecodedFrame(std::numeric_limits<unsigned>::max()),
        _myLastCurrentTime(-1.0),
        _myCurrentLoopCount(0)
    {
        AC_DEBUG  << "Movie::Movie " << (void*)this;
        if (getNode()) {
            setup();
        }
    }

    void
    Movie::registerDependenciesRegistrators() {
        Image::registerDependenciesRegistrators();
        AC_DEBUG << "Movie::registerDependenciesRegistrators '" << get<NameTag>();
        VolumeTag::Plug::getValuePtr()->setImmediateCallBack(dynamic_cast_Ptr<Movie>(getSelf()), &Movie::setVolume);
        PlayModeTag::Plug::getValuePtr()->setImmediateCallBack(dynamic_cast_Ptr<Movie>(getSelf()), &Movie::setPlayMode);
        ImageSourceTag::Plug::getValuePtr()->setImmediateCallBack(dynamic_cast_Ptr<Movie>(getSelf()), &Movie::setSource);
        // take node attributes on construction time in account
        setSource();
        setPlayMode();
        setVolume();
    }

    void Movie::setup() {
        //check if we have an inline movie, if so open it using FFMpeg
        dom::NodePtr myBinaryElement = getNode().firstChild();
        if (myBinaryElement && !getRasterPtr()) {
            AC_DEBUG << "found an inline movie for " << get<ImageSourceTag>();

            //DK hold this as a member, otherwise our ReadableStream goes out of scope :-(
            _myStreamData = myBinaryElement->firstChild()->nodeValueWrapperPtr();

            asl::ReadableStream * myStream
                = const_cast<asl::ReadableBlock*>(&_myStreamData->accessReadableBlock());

            loadStream( asl::Ptr<asl::ReadableStream>(myStream), get<ImageSourceTag>());
        }
    }



    Movie::~Movie() {
        AC_DEBUG  << "Movie::~Movie " << (void*)this;
    }



    void Movie::stop() {
        AC_DEBUG  << "Movie::stop";
        if (_myDecoder) {
            _myDecoder->stopMovie();
        }
        set<CurrentFrameTag>(0);
        _myLastDecodedFrame = std::numeric_limits<unsigned>::max();
        _myCurrentLoopCount = 0;
        dom::ResizeableRasterPtr myRaster = getRasterPtr();
        if (myRaster) {
            memset(myRaster->pixels().begin(), 0, myRaster->pixels().size());
        }
    }



    void
    Movie::restart(double theCurrentTime) {
        AC_DEBUG  << "Movie::restart";
        _myLastDecodedFrame = std::numeric_limits<unsigned>::max();
        _myDecoder->loop();
        double myMovieTime = _myDecoder->getMovieTime(theCurrentTime);
        decodeFrame(myMovieTime, 0);
    }

    void
    Movie::setPlayMode(MoviePlayMode thePlayMode) {
        AC_DEBUG << "Movie::setPlayMode "
            << asl::getStringFromEnum(thePlayMode, MoviePlayModeStrings);
        // process changes
        switch (thePlayMode) {
        case PLAY_MODE_STOP:
            stop();
            break;
        case PLAY_MODE_PLAY:
            if (_myPlayMode == PLAY_MODE_PAUSE) {
                if (get<CurrentFrameTag>() == static_cast<int>(_myLastDecodedFrame)) {
                    _myDecoder->resumeMovie(getTimeFromFrame(get<CurrentFrameTag>()));
                } else {
                    _myDecoder->startMovie(getTimeFromFrame(get<CurrentFrameTag>()));
                }
            } else if (_myPlayMode == PLAY_MODE_STOP){
                _myDecoder->startMovie(getTimeFromFrame(get<CurrentFrameTag>()));
            } else {
                AC_DEBUG<< "Movie::setPlayMode already playing";
            }
            break;
        case PLAY_MODE_PAUSE:
            if (_myPlayMode == PLAY_MODE_STOP) {
                _myDecoder->startMovie(getTimeFromFrame(get<CurrentFrameTag>()));
            }
            _myDecoder->pauseMovie();
            break;
        case PLAY_MODE_NODISK:
            //XXX check if we need to do something more here
            AC_DEBUG << "Movie set to PLAY_MODE_NODISK";
            break;
        }
        // Synchronize internal and external representation
        _myPlayMode = thePlayMode;
        MoviePlayMode myPlayMode =
            MoviePlayMode(asl::getEnumFromString(get<PlayModeTag>(), MoviePlayModeStrings));
        if (myPlayMode != _myPlayMode) {
            set<PlayModeTag>(asl::getStringFromEnum(thePlayMode, MoviePlayModeStrings));
        }
    }

    double Movie::decodeFrame(double theTime, unsigned theFrame) {
        DB(AC_DEBUG << "Movie::decodeFrame time=" << theTime << " frame=" << theFrame);
        double myReturnTime = theTime;
        if (theFrame != _myLastDecodedFrame) {
            RasterVector myMovieRaster;
            unsigned myRasterCount = getNode().childNodesLength();
            for (unsigned i = 0; i < myRasterCount; i++) {
                myMovieRaster.push_back(getRasterPtr(i));
            }
            myReturnTime = _myDecoder->readFrame(theTime, theFrame, myMovieRaster);
            if (myReturnTime == -1) {
                return myReturnTime;
            } else if (myReturnTime != theTime) {
                _myLastDecodedFrame = getFrameFromTime(myReturnTime);
            } else {
                _myLastDecodedFrame = theFrame; // M60
            }
            set<CurrentFrameTag>(_myLastDecodedFrame);
        }
        return myReturnTime;
    }



    double Movie::getTimeFromFrame(unsigned theFrame) const {
        int FrameCount = get<FrameCountTag>();
        if (FrameCount == -1) {
            return (double)(theFrame) / get<FrameRateTag>();
        } else {
            return (double)(theFrame % get<FrameCountTag>()) / get<FrameRateTag>();
        }
    }



    void
    Movie::ensureMovieFramecount() {
        if (get<FrameCountTag>() == -1) {
            load(AppPackageManager::get().getPtr()->getSearchPath());
            if (get<FrameCountTag>() == -1) {
                MoviePlayMode myPlayMode =
                    MoviePlayMode(asl::getEnumFromString(get<PlayModeTag>(),
                    MoviePlayModeStrings));
                setPlayMode(PLAY_MODE_PLAY);
                _myDecoder->setDecodeAudioFlag(false);
                readFrame();
                int myFrame = -1;
                while (!_myDecoder->getEOF()) {
                    myFrame++;
                    double myMovieTime = getTimeFromFrame(myFrame);
                    decodeFrame(myMovieTime, myFrame);
                }
                _myDecoder->setEOF(false);
                _myDecoder->setDecodeAudioFlag(true);
                restart(0);
                setPlayMode(myPlayMode);
            }
        }
        set<CurrentFrameTag>(0);
        _myLastDecodedFrame = std::numeric_limits<unsigned>::max();
    }



    unsigned Movie::getFrameFromTime(double theTime) {
        double myFrameHelper = theTime * get<FrameRateTag>();
        int myFrame = int(asl::round(myFrameHelper));
        if (get<FrameCountTag>() == -1) {
            return (unsigned)myFrame;
        } else {
            while (myFrame < 0) {
                myFrame += get<FrameCountTag>();
            }
            return (unsigned)(myFrame % get<FrameCountTag>());
        }
    }

    void
    Movie::readFrame() {
        readFrame(_myLastCurrentTime, true);
    }



    void
    Movie::readFrame(double theCurrentTime, bool theIgnoreCurrentTime) {
        DB(AC_TRACE << "Movie::readFrame time=" << theCurrentTime
            << " src=" << get<ImageSourceTag>() << " current playmode=" << getStringFromEnum(_myPlayMode, MoviePlayModeStrings) << " currentFrame=" << get<CurrentFrameTag>());
        DB(AC_TRACE << "                 theIgnoreCurrentTime=" << theIgnoreCurrentTime);
        _myLastCurrentTime = (theIgnoreCurrentTime) ? -1.0 : theCurrentTime;

        if (!_myDecoder) {
            AC_ERROR << "Movie::readFrame not allowed before open";
            return;
        }
        
        // Calculate next frame
        int myNextFrame = 0;
        double myMovieTime = 0.0;
        switch (_myPlayMode) {
        case PLAY_MODE_PAUSE:
            // next frame from currentframe attribute in movie node
            myNextFrame = get<CurrentFrameTag>();
            if (get<FrameCountTag>() != -1) {
                myNextFrame %= get<FrameCountTag>();
            }
            while (myNextFrame < 0) {
                if (get<FrameCountTag>() == -1) {
                    std::string myErrorMsg = string("Movie: ") + get<NameTag>()
                        + " has negative currentframe and a invalid "
                        + "framecount, trying a wraparound with "
                        + "invalid framecount will fail-> calling "
                        + "explicit ensureMovieFramecount() will help";
                    ASSURE_MSG(get<FrameCountTag>() != -1, myErrorMsg.c_str());
                    return;
                }
                myNextFrame += get<FrameCountTag>();
            }
            myMovieTime = getTimeFromFrame(myNextFrame);
            break;
        case PLAY_MODE_PLAY:
            myMovieTime = _myDecoder->getMovieTime(theCurrentTime);
            if (theIgnoreCurrentTime && get<CurrentFrameTag>() != 0) {
                myMovieTime = getTimeFromFrame(get<CurrentFrameTag>());
            }
            myNextFrame = (int)getFrameFromTime(myMovieTime);
            break;
        case PLAY_MODE_STOP:
            return;
        case PLAY_MODE_NODISK:
            //XXX check if we need to do something more here
            AC_DEBUG << "Movie has PLAY_MODE_NODISK";
            break;
        }

        if (myNextFrame < 0) {
            setPlayMode(PLAY_MODE_STOP);
        }
        DB(AC_DEBUG << "Next Frame: " << myNextFrame << ", lastDecodedFrame: "
            << _myLastDecodedFrame << ", MovieTime: " << myMovieTime;)
        decodeFrame(myMovieTime, myNextFrame);

        // check for eof in the decoder
        if (_myDecoder->getEOF()) {
            AC_DEBUG << "Movie has EOF, loopCount=" << _myCurrentLoopCount;
            _myDecoder->setEOF(false);
            if (get<LoopCountTag>() == 0 || ++_myCurrentLoopCount < get<LoopCountTag>()) {
                restart(theCurrentTime);
            } else {
                setPlayMode(PLAY_MODE_STOP);
            }
        }
    }

    bool Movie::getMovieTime(double & theTime) const {
        //DK my workaround for not knowing how to const_cast the asl::Ptr itself
        MovieDecoderBase & myDecoder = const_cast<MovieDecoderBase&>(*_myDecoder);
        theTime = myDecoder.getMovieTime(_myLastCurrentTime);
        DB2(AC_DEBUG << "Movie::getMovieTime systime " << _myLastCurrentTime
            << " got " << theTime;)
        return true;
    }

    bool Movie::getDecoderName(std::string & theName) const {
        MovieDecoderBase & myDecoder = const_cast<MovieDecoderBase&>(*_myDecoder);
        theName = myDecoder.getName();
        DB2(AC_DEBUG << "Movie::getDecoderName got " << theName;)
        return true;
    }

    bool Movie::hasAudio(bool & theHasAudio) const {
        MovieDecoderBase* myDecoder = const_cast<MovieDecoderBase*>(_myDecoder.get());
        theHasAudio = myDecoder->hasAudio();
        return true;
    }

    //callback for 'volume' attribute
    void Movie::setVolume() {
        if (!_myDecoder) {
            return;
        }
        MovieDecoderBase* myDecoder = const_cast<MovieDecoderBase*>(_myDecoder.get());
        Vector2f myVolume = get<VolumeTag>();
        myVolume[0] = asl::clamp(myVolume[0], 0.0f, 1.0f);
        myVolume[1] = asl::clamp(myVolume[1], 0.0f, 1.0f);
        if (!almostEqual(myVolume[0], get<VolumeTag>()[0]) || !almostEqual(myVolume[1], get<VolumeTag>()[1])) {
            set<VolumeTag>(myVolume);
        }
        std::vector<float> myVolumesVec;
        myVolumesVec.push_back(myVolume[0]);
        myVolumesVec.push_back(myVolume[1]);
        myDecoder->setVolumes(myVolumesVec);
    }

    // callback for 'src' attribute
    void Movie::setSource() {
        if (reloadRequired()) {
            load(AppPackageManager::get().getPtr()->getSearchPath());
        }
    }

    // callback for 'playmode' attribute
    void
    Movie::setPlayMode() {
        if (!_myDecoder) {
            return;
        }
        AC_DEBUG<<"playmode attribute in dom changed to "<<get<PlayModeTag>();
        MoviePlayMode myPlayMode =
            MoviePlayMode(asl::getEnumFromString(get<PlayModeTag>(), MoviePlayModeStrings));
        if (myPlayMode != _myPlayMode) {
            setPlayMode(myPlayMode);
            readFrame();
        }
    }
    

    void Movie::load(asl::PackageManager & thePackageManager) {
        /*
        * XXX
        * decide here wether you want to use the stream interface of thePackageManager
        * advantage: you could load mpegs which are in a zip file known to thePackageManager
        * disadvantages: 1) big files (> 200MB) are AFAIK currently not handled by
        *                   thePackageManager
        *                2) loadStream currently does not fallback to loadFile for WMV
        *                   and alike
        * DK
        */
#if 0
        loadStream( thePackageManager.openFile(get<ImageSourceTag>()), get<ImageSourceTag>());
#else
        loadFile( thePackageManager.searchFile(get<ImageSourceTag>()));
#endif
    }



    MovieDecoderBasePtr Movie::getDecoder(const std::string theFilename) {
        MovieDecoderBasePtr myDecoder;
        std::vector<MovieDecoderBasePtr> myDecoders;
        myDecoders = DecoderManager::get().findAllDecoders<MovieDecoderBase>(theFilename);
        std::string myDecoderHint = get<DecoderHintTag>();
        std::string myFileExtension = asl::toLowerCase(asl::getExtension(theFilename));

        // no decoder initialized or no decoder for filetype found
        if (myDecoders.size() == 0) {
            if (myFileExtension == "m60") {
                return MovieDecoderBasePtr(new M60Decoder());
            } else {
                if (myDecoderHint == "" ) { 
                    myDecoderHint = "FFMpegDecoder2";
                }
                asl::PlugInBasePtr myPlugIn =
                    asl::PlugInManager::get().getPlugIn(myDecoderHint);
                if (IDecoderPtr myDecoderPlug = dynamic_cast_Ptr<IDecoder>(myPlugIn)) {
                    AC_INFO << "Plug: " << myDecoderHint << ": as Decoder" << endl;
                    DecoderManager::get().addDecoder(myDecoderPlug);
                    myDecoder = DecoderManager::get().findDecoder<MovieDecoderBase>(theFilename);
                    if (!myDecoder) {
                        throw MovieException(std::string("Sorry, could not find decoder for: ")
                            + theFilename, PLUS_FILE_LINE);
                    }
                } else {
                    throw MovieException(std::string("Unable to plug decoder: ")
                        + myDecoderHint, PLUS_FILE_LINE);
                }
            }
        } else if (myDecoderHint != "") {
            std::vector<MovieDecoderBasePtr>::iterator it;
            for(it = myDecoders.begin(); it != myDecoders.end(); ++it) {
                AC_DEBUG << "possible decoder " << (*it)->getName();
                if ((*it)->getName() == myDecoderHint) {
                    myDecoder = (*it);
                    break;
                }
            }
        } else {
            myDecoder = *(myDecoders.begin());
        }
        return myDecoder->instance();
    }



    void
    Movie::load(const std::string & theTexturePath) {
        loadFile( asl::searchFile(get<ImageSourceTag>(), theTexturePath) );
    }



    void
    Movie::loadStream(asl::Ptr<asl::ReadableStream> theSource, const std::string theUrl) {
        AC_INFO << "Movie::loadStream " << theUrl;
        MovieDecoderBasePtr myDecoder = getDecoder(theUrl);
        if (!myDecoder) {
            throw MovieException(string("Sorry, could not find a streamable decoder for: ")
                + theUrl, PLUS_FILE_LINE);
        }
        _myDecoder = myDecoder->instance();

        _myDecoder->initialize(this);
        _myDecoder->load(theSource, theUrl);

        postLoad();
    }



    void
    Movie::loadFile(const std::string & theUrl) {

        if (_myPlayMode != PLAY_MODE_STOP) {
            setPlayMode(PLAY_MODE_STOP);
        }
        
        // if imagesource is an url do not take the packetmanager or searchfile new url
        const std::string & mySourceFile = get<ImageSourceTag>();
        std::string myFilename;
        if (mySourceFile.find("://") != string::npos) {
            myFilename = mySourceFile;
        } else {
            myFilename = theUrl;
        }
        if (myFilename.empty()) {
            AC_ERROR << "Unable to find url='" << theUrl << "'"
                << ", src='" << mySourceFile << "'";
            return;
        }
        AC_INFO << "Movie::loadFile " << (void*)this << " filename=" << myFilename;

        // First: Look for registered decoders that could handle the source
        _myDecoder = getDecoder(myFilename);
        if (!_myDecoder) {
            throw MovieException(std::string("Sorry, could not find decoder for: ")
                + myFilename, PLUS_FILE_LINE);
        }

        _myDecoder->initialize(this);
        _myDecoder->load(myFilename);
        postLoad();
    }



    void Movie::postLoad() {

        /*
        * UH: theUrl is mangled by the PackageManager and is not necessarily the same
        * as the ImageSourceTag.
        * this leads to multiple load() calls by the TextureManager since reloadRequired checks
        * _myLoadedFilename against ImageSourceTag and finds them to differ.
        */
        _myLoadedFilename = get<ImageSourceTag>();
        if (get<ImageResizeTag>() == IMAGE_RESIZE_PAD) {
            float myXResize = float(get<ImageWidthTag>())
                / asl::nextPowerOfTwo(get<ImageWidthTag>());
            float myYResize = float(get<ImageHeightTag>())
                / asl::nextPowerOfTwo(get<ImageHeightTag>());

            asl::Matrix4f myMatrix;
            myMatrix.makeScaling(asl::Vector3f(myXResize, myYResize, 1.0f));
            set<ImageMatrixTag>(myMatrix);
        }
    }



    bool
    Movie::reloadRequired() {
        std::string mySource = get<ImageSourceTag>();
        if (mySource == "") {
            return false;
        }
        bool rc = !_myDecoder || _myLoadedFilename != mySource;
        AC_TRACE << "Movie::reloadRequired " << rc
            << " with _myLoadedFilename=" << _myLoadedFilename
            << " ImageSourceTag=" << mySource;
        if (_myLoadedFilename != mySource) {
            set<FrameCountTag>(-1);
        }
        return rc;
    }



    dom::ResizeableRasterPtr
        Movie::addRasterValue(dom::ValuePtr theRaster, PixelEncoding theEncoding,
        unsigned theDepth)
    {
        AC_DEBUG << "Movie::addRasterValue '" << get<NameTag>() << "' id=" << get<IdTag>();
        // Setup raster nodes
        dom::DOMString myRasterName = RasterElementNames[theEncoding];
        dom::NodePtr myRasterChild =
            getNode().appendChild(dom::NodePtr(new dom::Element(myRasterName)));
        dom::NodePtr myTextChild = myRasterChild->appendChild(dom::NodePtr(new dom::Text()));

        myTextChild->nodeValueWrapperPtr(theRaster);
        return dynamic_cast_Ptr<dom::ResizeableRaster>(theRaster);
    }

}
