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
//
//    $RCSfile: QuicktimeDecoder.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.6 $
//       $Date: 2005/04/01 17:03:27 $
//
//  Quicktime movie decoder.
//
//=============================================================================

#include "QuicktimeDecoder.h"

#include <y60/video/Movie.h>

#include <asl/base/Logger.h>
#include <asl/base/file_functions.h>
#include <y60/video/Movie.h>

#include <qt/QTML.h>
#include <qt/TextUtils.h>

#define DB(x) //x

using namespace std;

extern "C"
EXPORT asl::PlugInBase * y60QuicktimeDecoder_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::QuicktimeDecoder(myDLHandle);
}

namespace y60 {
    #include <qt/MoviesFormat.h>

    // QuicktimeDecoder
    QuicktimeDecoder::QuicktimeDecoder(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle), _myLastDecodedFrame(UINT_MAX),_myInternalMovieTime(0), _myFrameTimeStep(0)
    {}

    QuicktimeDecoder::~QuicktimeDecoder() {
        closeMovie();
        ExitMovies();
        TerminateQTML();
    }

    asl::Ptr<MovieDecoderBase> QuicktimeDecoder::instance() const {
        AC_DEBUG<<"QuicktimeDecoder::instance()";
        InitializeQTML(0);
        EnterMovies();
        return asl::Ptr<MovieDecoderBase>(new QuicktimeDecoder(getDLHandle()));
    }

    std::string
    QuicktimeDecoder::canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream) {
        if (asl::toLowerCase(asl::getExtension(theUrl)) == "mov") {
            AC_INFO << "QuicktimeDecoder can decode :" << theUrl << endl;
            return MIME_TYPE_MOV;
        } else {
            AC_INFO << "QuicktimeDecoder can not decode :" << theUrl << " responsible for extension mov" << endl;
            return "";
        }
    }

    void
    QuicktimeDecoder::closeMovie() {
        AC_DEBUG << "QuicktimeDecoder::closeMovie";
        PixMapHandle myPixMap = GetGWorldPixMap(_myOffScreenWorld);
        DisposeScreenBuffer(myPixMap);
        DisposeGWorld(_myOffScreenWorld);
        MovieDecoderBase::closeMovie();
    }

    void
    QuicktimeDecoder::stopMovie() {
        AC_DEBUG<<"QuicktimeDecoder::stopMovie()";
        _myLastDecodedFrame = UINT_MAX;
        _myInternalMovieTime = 0;

        MovieDecoderBase::stopMovie();
    }

    bool
    QuicktimeDecoder::hasVideo() const {
        return true;
    }

    bool
    QuicktimeDecoder::hasAudio() const {
        return false;
    }

    void
    QuicktimeDecoder::load(const std::string & theFilename) {
        AC_DEBUG<<"QuicktimeDecoder::load theFilename: "<<theFilename;
        asl::Time myLoadStartTime;

        OSErr                   myErr;
        short                   myFileRefNum;
        char                    myFullPath[255];

        // qt can not handle filepaths like this: "./foo.mov",
        // which comes from the PacketManager -> cut it off
        if (theFilename.size() > 2 && theFilename.substr(0,2) == "./" ) {
            std::string myNewFilename = theFilename.substr(2,theFilename.size() - 2);
            strcpy (myFullPath, myNewFilename.c_str());
        } else {
            strcpy (myFullPath, theFilename.c_str());
        }
        c2pstr( (char*)myFullPath );
        FSSpec sfFile;
        FSMakeFSSpec (0, 0L, ConstStr255Param(myFullPath), &sfFile);

        myErr = OpenMovieFile (&sfFile, &myFileRefNum, fsRdPerm);
        if (myErr != noErr) {
            throw QuicktimeDecoderException(string("Movie ") + theFilename +
                " could not be loaded, check existence.", PLUS_FILE_LINE);
        }

        myErr = NewMovieFromFile (&_myMovie, myFileRefNum, 0, 0, newMovieActive, 0);
        if (myErr != noErr) {
            throw QuicktimeDecoderException(string("Movie ") + theFilename +
                " could not be loaded, check existence.", PLUS_FILE_LINE);
        }
        
        CloseMovieFile(myFileRefNum);

        Rect movieBounds;
        GetMovieBox(_myMovie, &movieBounds);
        Movie * myMovie = getMovie();
        unsigned myQTTargetPixelFormat;
        // get movie pixelformat
        GWorldPtr myOrigGWorld = NULL;
        GetMovieGWorld(_myMovie, &myOrigGWorld, NULL);
        PixMapHandle myPortPixMap = myOrigGWorld->portPixMap;
        AllowPurgePixels(myPortPixMap);
        OSType myMoviePixelType = (*myPortPixMap)->pixelFormat;
        myQTTargetPixelFormat = myMoviePixelType;

        switch (myMoviePixelType) {
            case k32BGRAPixelFormat:
            case k32RGBAPixelFormat:
            case k32ABGRPixelFormat:
                myQTTargetPixelFormat = k32BGRAPixelFormat;
				{AC_TRACE << "Using BGRA pixels";}
                myMovie->createRaster(movieBounds.right, movieBounds.bottom, 1, y60::BGRA);
                break;
            case k24BGRPixelFormat:
            case k16LE555PixelFormat:
            case k16LE5551PixelFormat:
            case k16BE565PixelFormat:
            case k16LE565PixelFormat:
            case kYUVSPixelFormat:
            case kYUVUPixelFormat:
            case kYVU9PixelFormat:
            case kYUV411PixelFormat:
            case kYVYU422PixelFormat:
            case kUYVY422PixelFormat:
            case kYUV211PixelFormat:
            case k2vuyPixelFormat:
            default:
                AC_TRACE << "Using BGR pixels";
                myQTTargetPixelFormat = k24BGRPixelFormat;
                myMovie->createRaster(movieBounds.right, movieBounds.bottom, 1, y60::BGR);
                break;
        }

        QTNewGWorld(&_myOffScreenWorld, myQTTargetPixelFormat, &movieBounds, 0, 0, 0);
        unsigned myFrameCount = getFramecount(_myMovie);
        TimeScale ts = GetMovieTimeScale(_myMovie);
        unsigned myDuration = (GetMovieDuration(_myMovie)*1000)/ts;

        double myFrameRate = myFrameCount*1000/myDuration;
        if (!myFrameRate) {
            throw QuicktimeDecoderException(string("Movie ") + theFilename +
                ": could not get framecount and framerate.", PLUS_FILE_LINE);
            //myFrameRate = 25;
        }
        AC_DEBUG<<"framerate: "<<myFrameRate;
        myMovie->set<FrameRateTag>(myFrameRate);

        TimeValue   myTimeScale = GetMovieTimeScale(_myMovie);
        AC_INFO << "myTimeScale : " << myTimeScale;

        _myFrameTimeStep = (TimeValue)(myTimeScale/ myMovie->get<FrameRateTag>());

        double myDurationInSeconds = float(GetMovieDuration(_myMovie)) / GetMovieTimeScale(_myMovie);
        myMovie->set<FrameCountTag>(long(myDurationInSeconds * myFrameRate));

        SetGWorld(_myOffScreenWorld, NULL);
        SetMovieGWorld(_myMovie, _myOffScreenWorld, NULL);
        asl::Time myLoadEndTime;
        AC_DEBUG << "Load file:" << theFilename << ", time " << (myLoadEndTime - myLoadStartTime) << "s";
        

    }

    unsigned
    QuicktimeDecoder::getFramecount(::Movie theMovie) {
        int      frameCount = 0;
        OSType      whichMediaType = VIDEO_TYPE;
        short flags = nextTimeMediaSample + nextTimeEdgeOK; // nextTimeStep;
        TimeValue   duration;
        TimeValue   theTime = 0;


        while (theTime >= 0) {
            frameCount++;
            GetMovieNextInterestingTime( theMovie,            
                                         flags,               
                                         1,                   
                                         &whichMediaType,     
                                         theTime,             
                                         1,                   
                                         &theTime,            
                                         &duration );         

            // after the first interesting time, don't include the time we
            //  are currently at.

            flags = nextTimeMediaSample;
        } // while
        if (frameCount == 0) {
            return INT_MAX;
        } else {
            return frameCount;
        }

    }

    double
    QuicktimeDecoder::readFrame(double theTime, unsigned theFrame, RasterVector theTargetRaster) {
        AC_INFO << "Timestamp : " << theTime << "Read frame: " << theFrame << ", last decoded frame: " << _myLastDecodedFrame;

        if (static_cast<int>(theFrame) >= (getFrameCount() -1) ) {
            setEOF(true);
            return theTime;
        }
        // Handle frame wrap around
        if (_myLastDecodedFrame > theFrame) {
            decodeFrame(0, theTargetRaster[0]);
        }

        if (theFrame != _myLastDecodedFrame) {
            decodeFrame(theFrame, theTargetRaster[0]);
        }

        theTargetRaster[0]->resize(getFrameWidth(), getFrameHeight());
        return theTime;
    }

    void QuicktimeDecoder::decodeFrame(unsigned theFrameNumber, dom::ResizeableRasterPtr theTargetRaster)
    {
        TimeValue myDuration;

        SetGWorld(_myOffScreenWorld, NULL);

        // get the next frame of the source movie
        short   myFlags     = nextTimeMediaSample;
        OSType  myMediaType = FOUR_CHAR_CODE('vide');//VIDEO_TYPE;

        // if this is the first frame, include the frame we are currently on
        if (theFrameNumber == 0) {
            myFlags |= nextTimeEdgeOK;
        }
        ::TimeValue myDesiredMovieTime = theFrameNumber * _myFrameTimeStep;
        AC_INFO << "myDesiredMovieTime: " << myDesiredMovieTime;

        // skip to the next interesting time and get the duration for that frame
        GetMovieNextInterestingTime( _myMovie,
                                     myFlags,
                                     1,
                                     &myMediaType,
                                     myDesiredMovieTime,
                                     0,
                                     &_myInternalMovieTime,
                                     &myDuration );
        // set the time for the frame and give time to the movie toolbox
        SetMovieTimeValue(_myMovie, _myInternalMovieTime);


        // *** this does the actual drawing into the GWorld ***
        MoviesTask(_myMovie,0);

        PixMapHandle myPixMap = GetGWorldPixMap(_myOffScreenWorld);
        Ptr baseAddr = GetPixBaseAddr(myPixMap);

        memcpy(theTargetRaster->pixels().begin(), baseAddr, theTargetRaster->pixels().size());

        _myLastDecodedFrame = theFrameNumber;
        
    }
}

