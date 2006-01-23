//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: Movie.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.9 $
//       $Date: 2005/04/22 14:58:47 $
//
//=============================================================================


#include "Movie.h"
#include "MovieDecoderBase.h"
#include <y60/PixelEncoding.h>
#include <y60/DecoderManager.h>
#include <asl/Logger.h>

#include "M60Decoder.h"

#include <asl/PackageManager.h>

#include <string.h>

using namespace dom;
using namespace std;

#define DB2(x) // x
#define DB(x) // x



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
        AVDelayTag::Plug(theNode),
        AudioTag::Plug(theNode),
        DecoderHintTag::Plug(theNode),
        dom::DynamicAttributePlug<MovieTimeTag, Movie>(this, &Movie::getMovieTime),
        dom::DynamicAttributePlug<DecoderTag, Movie>(this, &Movie::getDecoderName),
        _myDecoder(0),
        _myLastDecodedFrame(UINT_MAX),
        _myLastCurrentTime(-1.0),
        _myCurrentLoopCount(0),
        _myPlayMode(PLAY_MODE_STOP)
    {
        AC_DEBUG  << "Movie::Movie " << (void*)this;
        setup();
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
        _myDecoder->stopMovie();
        set<PlayModeTag>(asl::getStringFromEnum(PLAY_MODE_STOP, MoviePlayModeStrings));
        set<CurrentFrameTag>(0);
        _myLastDecodedFrame = UINT_MAX;
        _myCurrentLoopCount = 0;
        dom::ResizeableRasterPtr myRaster = getRasterPtr();
        memset(myRaster->pixels().begin(), 0, myRaster->pixels().size());
    }

    void
    Movie::restart() {
        AC_DEBUG  << "Movie::restart";
        _myDecoder->stopMovie();
        _myDecoder->startMovie(0);
        decodeFrame(0.0, 0);
    }

    void
    Movie::setPlayMode(MoviePlayMode thePlayMode) {
        AC_DEBUG << "Movie::setPlayMode " << asl::getStringFromEnum(thePlayMode, MoviePlayModeStrings);
        // process changes
        switch (thePlayMode) {
            case PLAY_MODE_STOP:
                stop();
                break;
            case PLAY_MODE_PLAY:
                if (_myPlayMode == PLAY_MODE_PAUSE) {
                    _myDecoder->resumeMovie(getTimeFromFrame(get<CurrentFrameTag>()));
                } else {
                    _myDecoder->startMovie(getTimeFromFrame(get<CurrentFrameTag>()));
                }
                break;
            case PLAY_MODE_PAUSE:
                if (_myPlayMode == PLAY_MODE_STOP) {
                    _myDecoder->startMovie(0);
                }
                _myDecoder->pauseMovie();
                break;
        }

        // Synchronize internal and external representation
        _myPlayMode = thePlayMode;
        set<PlayModeTag>(asl::getStringFromEnum(thePlayMode, MoviePlayModeStrings));
    }

    double Movie::decodeFrame(double theTime, unsigned theFrame) {
        DB(AC_DEBUG << "Movie::decodeFrame time=" << theTime << " frame=" << theFrame);
        double myReturnTime = _myDecoder->readFrame(theTime, theFrame, getRasterPtr());
        if (myReturnTime != theTime) {
            _myLastDecodedFrame = getFrameFromTime(myReturnTime);
        } else {
            _myLastDecodedFrame = theFrame; // M60
        }
        set<CurrentFrameTag>(_myLastDecodedFrame);
        return myReturnTime;
    }

    double Movie::getTimeFromFrame(unsigned theFrame) const {
        AC_DEBUG << "getTimeFromFrame count " << get<FrameCountTag>() << " framerate " << get<FrameRateTag>();
        AC_DEBUG << "returning " << (double)(theFrame % get<FrameCountTag>()) / get<FrameRateTag>();
 
        return (double)(theFrame % get<FrameCountTag>()) / get<FrameRateTag>();
    }

    unsigned Movie::getFrameFromTime(double theTime) const {
        AC_DEBUG << "getFrameFromTime theTime " << theTime;
        double myFrameHelper = theTime * get<FrameRateTag>();
        int myFrame = int(myFrameHelper < 0 ? ceil(myFrameHelper) : floor(myFrameHelper));
        while (myFrame < 0) {
            myFrame += get<FrameCountTag>();
        }
        AC_DEBUG << "returning " << myFrame << " mod " << get<FrameCountTag>();
        return (unsigned)(myFrame % get<FrameCountTag>());
    }

    void
    Movie::readFrame() {
        readFrame(0, true);
    }

    void
    Movie::readFrame(double theCurrentTime, bool theIgnoreCurrentTime) {
        DB(AC_DEBUG << "Movie::readFrame time=" << theCurrentTime);
        _myLastCurrentTime = theCurrentTime;

        if (!_myDecoder) {
            AC_ERROR << "Movie::readFrame not allowed before open";
            return;
        }
        // Check for playmode changes from javascript
        MoviePlayMode myPlayMode = MoviePlayMode(asl::getEnumFromString(get<PlayModeTag>(), MoviePlayModeStrings));
        if (myPlayMode != _myPlayMode) {
            setPlayMode(myPlayMode);
        }

        // Calculate next frame
        int myNextFrame;
        double myMovieTime;
        switch (_myPlayMode) {
            case PLAY_MODE_PAUSE:
                // next frame from currentframe attribute in movie node
                myNextFrame = get<CurrentFrameTag>();
                while (myNextFrame < 0) {
                    myNextFrame += get<FrameCountTag>();
                }
                myMovieTime = getTimeFromFrame(myNextFrame);
                break;
            case PLAY_MODE_PLAY:
                if (theIgnoreCurrentTime) {
                    myMovieTime = 0;
                } else {
                    myMovieTime = _myDecoder->getMovieTime(theCurrentTime);
                }
                myNextFrame = (int)getFrameFromTime(myMovieTime);
                break;
            case PLAY_MODE_STOP:
                myNextFrame = _myLastDecodedFrame;
                return;
        }

        if (myNextFrame < 0) {
            setPlayMode(PLAY_MODE_STOP);
        }

        DB(AC_TRACE << "Next Frame: " << myNextFrame << ", lastDecodedFrame: " << _myLastDecodedFrame << ", MovieTime: " << myMovieTime;)
        if (myNextFrame != _myLastDecodedFrame) {
            double myDecodedTime = decodeFrame(myMovieTime, myNextFrame);
            /*if (!asl::almostEqual(myDecodedTime, myMovieTime, 0.04)) {
                AC_WARNING << "Decoded Time=" << myDecodedTime << " differs from Movie Time=" << myMovieTime << ". Delta=" << myMovieTime-myDecodedTime;
            }*/
        }

        // check for eof in the decoder
        if (_myDecoder->getEOF()) {
            AC_DEBUG << "Movie has EOF, loopCount=" << _myCurrentLoopCount;
            _myDecoder->setEOF(false);
            if (get<LoopCountTag>() == 0 || ++_myCurrentLoopCount < get<LoopCountTag>()) {
                restart();
            } else {
                setPlayMode(PLAY_MODE_STOP);
            }
        }
    }

    bool Movie::getMovieTime(double & theTime) const {
        //DK my workaround for not knowing how to const_cast the asl::Ptr itself
        MovieDecoderBase & myDecoder = const_cast<MovieDecoderBase&>(*_myDecoder);
        theTime = myDecoder.getMovieTime(_myLastCurrentTime);
        AC_DEBUG << "Movie::getMovieTime systime " << _myLastCurrentTime 
                 << " got " << theTime;
        return true;
    }
    
    bool Movie::getDecoderName(std::string & theName) const {
        AC_DEBUG << "Movie::getDecoderName";
        MovieDecoderBase & myDecoder = const_cast<MovieDecoderBase&>(*_myDecoder);
        theName = myDecoder.getName();
        AC_DEBUG << "Movie::getDecoderName got " << theName;
        return true;
    }
    
    void Movie::load(asl::PackageManager & thePackageManager) {
        /*
         * XXX
         * decide here wether you want to use the stream interface of thePackageManager
         * advantage: you could load mpegs which are in a zip file known to thePackageManager
         * disadvantages: 1) big files (> 200MB) are AFAIK currently not handled by thePackageManager
         *                2) loadStream currently does not fallback to loadFile for WMV and alike
         * DK
         */
#if 0
        loadStream( thePackageManager.openFile(get<ImageSourceTag>()), get<ImageSourceTag>());
#else
        loadFile( thePackageManager.searchFile(get<ImageSourceTag>()) );
#endif
    }

    MovieDecoderBasePtr Movie::getDecoder(const std::string theFilename) {

        MovieDecoderBasePtr myDecoder = DecoderManager::get().findDecoder<MovieDecoderBase>(theFilename);

        const std::string & myDecoderHint = get<DecoderHintTag>();
        if (myDecoderHint != "") {
            std::vector<MovieDecoderBasePtr> myDecoders; 
            myDecoders = DecoderManager::get().findAllDecoders<MovieDecoderBase>(theFilename);
            std::vector<MovieDecoderBasePtr>::iterator it;
            for(it = myDecoders.begin(); it != myDecoders.end(); ++it) {
                AC_DEBUG << "possible decoder " << (*it)->getName();
                if ((*it)->getName() == myDecoderHint) {
                    myDecoder = (*it);
                    break;
                }
            }
        }
        AC_INFO << "using decoder " << myDecoder->getName() << " for decoding " << theFilename << " hint " << myDecoderHint;
        return myDecoder;
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
            throw MovieException(string("Sorry, could not find a streamable decoder for: ") + theUrl, PLUS_FILE_LINE);
        }
        _myDecoder = myDecoder->instance();

        _myDecoder->initialize(this);
        _myDecoder->load(theSource, theUrl);

        postLoad();
    }


    void
    Movie::loadFile(const std::string & theUrl) {

        const std::string & mySourceFile = get<ImageSourceTag>();
        _myPlayMode = PLAY_MODE_STOP;

        // if imagesource is an url do not take the packetmanaged or searchfiled new url

        string myFilename;
        if (mySourceFile.find("://") != string::npos) {
            myFilename = mySourceFile;
        } else {
            myFilename = theUrl;
        }
        if (myFilename.empty()) {
            AC_ERROR << "Unable to find url=" << theUrl << " filename=" << myFilename;
            return;
        }
        AC_INFO << "Movie::loadFile " << (void*)this << " filename=" << myFilename;

        // First: Look for registered decoders that could handle the source
        MovieDecoderBasePtr myDecoder = getDecoder(myFilename);

        if (!myDecoder) {
            // Second: Try m60, by extension
            string myFileExtension = asl::toLowerCase(asl::getExtension(myFilename));
            if (myFileExtension == "m60") {
                _myDecoder = MovieDecoderBasePtr(new M60Decoder());
            } else {
                throw MovieException(std::string("Sorry, could not find decoder for: ") + myFilename, PLUS_FILE_LINE);
            }
        } else {
            _myDecoder = myDecoder->instance();
        }

        _myDecoder->initialize(this);
        _myDecoder->load(myFilename);
        postLoad();
    }

    void Movie::postLoad() {

        /*
         * UH: theUrl is mangled by the PackageManager and is not necessarily the same as the ImageSourceTag
         * this leads to multiple load() calls by the TextureManager since reloadRequired checks
         * _myLoadedFilename against ImageSourceTag and finds them to differ.
         */
        _myLoadedFilename = get<ImageSourceTag>();

        Image::createRaster(_myDecoder->getPixelFormat());
        getRasterPtr()->resize(get<ImageWidthTag>(), get<ImageHeightTag>());
        getRasterPtr()->clear();
    }

    bool
    Movie::reloadRequired() const {
        bool rc = !_myDecoder || _myLoadedFilename != get<ImageSourceTag>();
        AC_TRACE << "Movie::reloadRequired " << rc
                 << " with _myLoadedFilename=" << _myLoadedFilename
                 << " ImageSourceTag=" << get<ImageSourceTag>();
        return rc;
    }
}
