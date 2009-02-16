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
#include "M60Decoder.h"

#include <y60/image/PixelEncoding.h>
#include <y60/base/DecoderManager.h>
#include <y60/base/IScene.h>
#include <asl/base/Logger.h>
#include <asl/zip/PackageManager.h>
#include <asl/base/PlugInManager.h>
#include <asl/base/Assure.h>
#include <asl/base/Dashboard.h>

using namespace dom;
using namespace std;
using namespace asl;

#define DB(x) x
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
        AVDelayTag::Plug(theNode),
        AudioTag::Plug(theNode),
        DecoderHintTag::Plug(theNode),
        FrameBlendFactorTag::Plug(theNode),		
        FrameBlendingTag::Plug(theNode),				
        dom::DynamicAttributePlug<MovieTimeTag, Movie>(this, &Movie::getMovieTime),
        dom::DynamicAttributePlug<DecoderTag, Movie>(this, &Movie::getDecoderName),
        _myDecoder(0),
        _myPlayMode(PLAY_MODE_STOP),
        _myLastDecodedFrame(std::numeric_limits<unsigned>::max()),
        _myLastCurrentTime(-1.0),
        _myCurrentLoopCount(0),
        _myNextUsedBuffer(PRIMARY_BUFFER),
        _myFirstBufferFrame(0),
        _mySecondBufferFrame(0)
    {
        AC_DEBUG  << "Movie::Movie " << (void*)this;
        if (getNode()) {
            setup();
        }
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
        set<PlayModeTag>(asl::getStringFromEnum(PLAY_MODE_STOP, MoviePlayModeStrings));
        set<CurrentFrameTag>(0);
        _myLastDecodedFrame = std::numeric_limits<unsigned>::max();
        _myNextUsedBuffer = PRIMARY_BUFFER;
        _myCurrentLoopCount = 0;
        dom::ResizeableRasterPtr myRaster = getRasterPtr();
        if (myRaster) {
            memset(myRaster->pixels().begin(), 0, myRaster->pixels().size());
        }
    }



    void
        Movie::restart(double theCurrentTime) {
            AC_DEBUG  << "Movie::restart";
            //_myDecoder->stopMovie();
            _myDecoder->startMovie(0);
            _myNextUsedBuffer = PRIMARY_BUFFER;
            double myMovieTime = _myDecoder->getMovieTime(theCurrentTime);
            decodeFrame(myMovieTime, 0);
            //		_myLastDecodedFrame = std::numeric_limits<unsigned>::max();
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
                    _myDecoder->resumeMovie(getTimeFromFrame(get<CurrentFrameTag>()));
                } else if (_myPlayMode == PLAY_MODE_STOP){
                    _myDecoder->startMovie(getTimeFromFrame(get<CurrentFrameTag>()));
                } else {
                    AC_DEBUG<< "Movie::setPlayMode already playing";
                }
                break;
            case PLAY_MODE_PAUSE:
                if (_myPlayMode == PLAY_MODE_STOP) {
                    _myDecoder->startMovie(0);
                }
                _myDecoder->pauseMovie();
                break;
            case PLAY_MODE_NODISK:
                //XXX check if we need to do something more here
                AC_INFO << "Movie set to PLAY_MODE_NODISK";
                break;
            }
            // Synchronize internal and external representation
            _myPlayMode = thePlayMode;
            set<PlayModeTag>(asl::getStringFromEnum(thePlayMode, MoviePlayModeStrings));
    }

    //    
    //
    //    void Movie::decodeFrame(double theTime, unsigned theFrame) {
    //
    //		if (theFrame != _myLastDecodedFrame) {
    //			DB(AC_DEBUG << "Movie::decodeFrame time=" << theTime << " frame=" << theFrame);
    //			if ( _myNextUsedBuffer == PRIMARY_BUFFER) {
    //				_myFirstBufferFrame = theFrame;
    //			} else {
    //				_mySecondBufferFrame = theFrame;
    //			}
    //			_myLastDecodedFrame = theFrame;
    //
    //			//AC_PRINT << "Decode frame #" << theFrame << " into " 
    //            //         << ((_myNextUsedBuffer == PRIMARY_BUFFER) ? 
    //            //             "PRIMARY_BUFFER":"SECONDARY_BUFFER");
    //            
    //			double myReturnTime = _myDecoder->readFrame(theTime, theFrame, 
    //                                                        getRasterPtr(_myNextUsedBuffer));
    //			if (get<FrameBlendingTag>()) {
    //				_myNextUsedBuffer = MovieFrameBuffer((int(_myNextUsedBuffer)+1) % (MAX_BUFFER));
    //			}
    //		}
    //
    //		if (get<FrameBlendingTag>()) {
    //			// If the first buffer holds the later frame, invert blendfactor
    //			if (_myFirstBufferFrame > _mySecondBufferFrame){
    //				set<FrameBlendFactorTag>((1.0-get<FrameBlendFactorTag>()));
    //			}
    //		}

    // TODO: frameblending..
    double Movie::decodeFrame(double theTime, unsigned theFrame) {
        DB(AC_DEBUG << "Movie::decodeFrame time=" << theTime << " frame=" << theFrame);
        RasterVector myMovieRaster;
        unsigned myRasterCount = getNode().childNodesLength();
        for (unsigned i = 0; i < myRasterCount; i++) {
            myMovieRaster.push_back(getRasterPtr(i));
        }
        double myReturnTime = _myDecoder->readFrame(theTime, theFrame, myMovieRaster);
        if (myReturnTime != theTime) {
            _myLastDecodedFrame = getFrameFromTime(myReturnTime);
        } else {
            _myLastDecodedFrame = theFrame; // M60
        }
        set<CurrentFrameTag>(_myLastDecodedFrame);
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
                        _myLastDecodedFrame = myFrame;
                        set<CurrentFrameTag>(myFrame);
                    }
                    _myDecoder->setEOF(false);
                    _myDecoder->setDecodeAudioFlag(true);
                    restart(0);
                    setPlayMode(myPlayMode);
                }
            } 
    }



    unsigned Movie::getFrameFromTime(double theTime) {
        double myFrameHelper = theTime * get<FrameRateTag>();
        int myFrame = 0;
        if (get<FrameBlendingTag>()) {
            myFrame = int(floor(myFrameHelper));
            double myNextFrameTimeRatio = ((myFrame+1)/get<FrameRateTag>() - theTime)  
                / (1.0 / get<FrameRateTag>());
            set<FrameBlendFactorTag>(myNextFrameTimeRatio);
            //AC_PRINT << "Frame : " << myFrame << " at time: " << theTime 
            //         << " and a ratio to next frame: " << get<FrameBlendFactorTag>();
        } else {
            myFrame = int(asl::round(myFrameHelper));
        }
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
            readFrame(0, true);
    }



    void
        Movie::readFrame(double theCurrentTime, bool theIgnoreCurrentTime) {
            DB(AC_DEBUG << "Movie::readFrame time=" << theCurrentTime 
                << " src=" << get<ImageSourceTag>());
            DB(AC_DEBUG << "                 theIgnoreCurrentTime=" << theIgnoreCurrentTime);
            _myLastCurrentTime = theCurrentTime;

            if (!_myDecoder) {
                AC_ERROR << "Movie::readFrame not allowed before open";
                return;
            }
            // Check for playmode changes from javascript
            MoviePlayMode myPlayMode = 
                MoviePlayMode(asl::getEnumFromString(get<PlayModeTag>(), MoviePlayModeStrings));
            if (myPlayMode != _myPlayMode) {
                setPlayMode(myPlayMode);
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
            case PLAY_MODE_NODISK:
                //XXX check if we need to do something more here
                AC_INFO << "Movie has PLAY_MODE_NODISK";
                break;
            }                

            if (myNextFrame < 0) {
                setPlayMode(PLAY_MODE_STOP);
            }
            //double myCurrentTime = myMovieTime;
            DB(AC_DEBUG << "Next Frame: " << myNextFrame << ", lastDecodedFrame: " 
                << _myLastDecodedFrame << ", MovieTime: " << myMovieTime;)
            if (get<FrameBlendingTag>()) {
                // create correct start position -> 
                // frame 0 in PRIMARY_BUFFER && frame 1 in SECONDARY_BUFFER
                if (_myLastDecodedFrame == std::numeric_limits<unsigned>::max()) {
                    _myNextUsedBuffer = PRIMARY_BUFFER;
                    decodeFrame(0,0);
                }
            }
            set<CurrentFrameTag>(myNextFrame);
            // for frame blending we always need a frame in advance
            if (get<FrameBlendingTag>()) {
                myMovieTime += (1.0/get<FrameRateTag>());
                myNextFrame++;
            }
            decodeFrame(myMovieTime, myNextFrame);
            //AC_PRINT << myCurrentTime << " -> " << (get<FrameBlendFactorTag>()*100.0) 
            //         << "% # " << _myFirstBufferFrame << " / " 
            //         << ((1.0-get<FrameBlendFactorTag>())*100.0)<<"% # " << _mySecondBufferFrame;

            //<<<<<<< .mine
            //=======
            //        //AC_PRINT << "Next Frame: " << myNextFrame << ", lastDecodedFrame: " << _myLastDecodedFrame << ", MovieTime: " << myMovieTime << " movie: " << get<ImageSourceTag>();
            //        DB(AC_DEBUG << "Next Frame: " << myNextFrame << ", lastDecodedFrame: " << _myLastDecodedFrame << ", MovieTime: " << myMovieTime;)
            //        if (myNextFrame != _myLastDecodedFrame) {
            //            MAKE_SCOPE_TIMER(Movie_readFrame);
            //            double myDecodedTime = decodeFrame(myMovieTime, myNextFrame);
            //            /*if (!asl::almostEqual(myDecodedTime, myMovieTime, 0.04)) {
            //                AC_WARNING << "Decoded Time=" << myDecodedTime << " differs from Movie Time=" << myMovieTime << ". Delta=" << myMovieTime-myDecodedTime;
            //            }*/
            //        }
            //
            //>>>>>>> .r14897
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
            //AC_PRINT << "--------------------";
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



    void Movie::load() {}



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
        loadFile( thePackageManager.searchFile(get<ImageSourceTag>()) );
#endif
    }



    MovieDecoderBasePtr Movie::getDecoder(const std::string theFilename) {
        MovieDecoderBasePtr myDecoder = 
            DecoderManager::get().findDecoder<MovieDecoderBase>(theFilename);

        const std::string & myDecoderHint = get<DecoderHintTag>();
        if (myDecoderHint != "") {
            std::vector<MovieDecoderBasePtr> myDecoders;
            myDecoders = DecoderManager::get().findAllDecoders<MovieDecoderBase>(theFilename);
            std::vector<MovieDecoderBasePtr>::iterator it;
            bool foundDecoderFlag = false;
            for(it = myDecoders.begin(); it != myDecoders.end(); ++it) {
                AC_DEBUG << "possible decoder " << (*it)->getName();
                if ((*it)->getName() == myDecoderHint) {
                    myDecoder = (*it);
                    foundDecoderFlag = true;
                    break;
                }
            }
            if (!foundDecoderFlag) {
                // we did not find a decoder for the decoderhint, plug it and try to use it
                asl::PlugInBasePtr myPlugIn = 
                    asl::PlugInManager::get().getPlugIn(myDecoderHint);
                if (IDecoderPtr myDecoderPlug = dynamic_cast_Ptr<IDecoder>(myPlugIn)) {
                    AC_INFO << "Plug: " << myDecoderHint << ": as Decoder" << endl;
                    DecoderManager::get().addDecoder(myDecoderPlug);
                    myDecoder = dynamic_cast_Ptr<MovieDecoderBase>(myDecoderPlug);
                } else {
                    throw MovieException(std::string("Unable to plug decoder: ") 
                        + myDecoderHint, PLUS_FILE_LINE);
                }
            }
        }
        //        AC_INFO << "using decoder " << myDecoder->getName() << " for decoding " 
        //                << theFilename << " hint " << myDecoderHint;
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

            const std::string & mySourceFile = get<ImageSourceTag>();
            if (_myDecoder) {
                _myDecoder->stopMovie();
            }
            //set<CurrentFrameTag>(0);
            _myLastDecodedFrame = std::numeric_limits<unsigned>::max();
            _myCurrentLoopCount = 0;

            _myPlayMode = PLAY_MODE_STOP;

            // if imagesource is an url do not take the packetmanager or searchfile new url

            string myFilename;
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
            MovieDecoderBasePtr myDecoder = getDecoder(myFilename);

            if (!myDecoder) {
                // Second: Try m60, by extension
                string myFileExtension = asl::toLowerCase(asl::getExtension(myFilename));
                if (myFileExtension == "m60") {
                    _myDecoder = MovieDecoderBasePtr(new M60Decoder());
                } else {
                    throw MovieException(std::string("Sorry, could not find decoder for: ") 
                        + myFilename, PLUS_FILE_LINE);
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
        Movie::reloadRequired(){
            bool rc = !_myDecoder || _myLoadedFilename != get<ImageSourceTag>();
            AC_TRACE << "Movie::reloadRequired " << rc
                << " with _myLoadedFilename=" << _myLoadedFilename
                << " ImageSourceTag=" << get<ImageSourceTag>();
            if (_myLoadedFilename != get<ImageSourceTag>()) {
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
