//=============================================================================
// Copyright (C) 2004,2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include <qt/QTML.h>
#include <qt/TextUtils.h>

#include <asl/Logger.h>
#include <asl/file_functions.h>

#define DB(x) //x

using namespace std;

extern "C"
EXPORT asl::PlugInBase * y60QuicktimeDecoder_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::QuicktimeDecoder(myDLHandle);
}
Movie QTURL_NewMovieFromURL (char *theURL);

namespace y60 {

    // QuicktimeDecoder
    QuicktimeDecoder::QuicktimeDecoder(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle), _myLastDecodedFrame(UINT_MAX),_myInternalMovieTime(0)
    {}

    QuicktimeDecoder::~QuicktimeDecoder() {
        closeMovie();
    }

    asl::Ptr<MovieDecoderBase> QuicktimeDecoder::instance() const {
        InitializeQTML(0);        
        EnterMovies();        
        return asl::Ptr<MovieDecoderBase>(new QuicktimeDecoder(getDLHandle()));
    }

    std::string
    QuicktimeDecoder::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
        if (asl::toLowerCase(asl::getExtension(theUrl)) == "mov") {
            AC_INFO << "QuicktimeDecoder1 can decode :" << theUrl << endl;
            return MIME_TYPE_MOV;
        } else {
            AC_INFO << "QuicktimeDecoder1 can not decode :" << theUrl << " responsible for extension mov" << endl;
            return "";
        }
    }

    void
    QuicktimeDecoder::closeMovie() {
        AC_DEBUG << "QuicktimeDecoder::closeMovie";
    }

    void
    QuicktimeDecoder::stopMovie() {
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
        asl::Time myLoadStartTime;
        
        OSErr                   myErr;
        short                   myFileRefNum;            
        char                    myFullPath[255];        
        
        //_myMovie = QTURL_NewMovieFromURL((char*)theFilename.c_str()); 
        
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

        PixelEncoding myPixelEncoding = getPixelFormat();
        QTNewGWorld(&_myOffScreenWorld, k32RGBAPixelFormat, &movieBounds, 0, 0, 0);
//   setPixelFormat(PixelEncoding(RGBA));
        
        Movie * myMovie = getMovie();
        myMovie->setPixelEncoding(PixelEncoding(RGBA));
        myMovie->set<FrameRateTag>(25);
        myMovie->set<ImageWidthTag>(movieBounds.right);
        myMovie->set<ImageHeightTag>(movieBounds.bottom);

        // Setup video size and image matrix
        float myXResize = float(movieBounds.right) / asl::nextPowerOfTwo(movieBounds.right);
        float myYResize = float(movieBounds.bottom) / asl::nextPowerOfTwo(movieBounds.bottom);

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(asl::Vector3f(myXResize, myYResize, 1.0f));
        myMovie->set<ImageMatrixTag>(myMatrix);


        double myDurationInSeconds = float(GetMovieDuration(_myMovie)) / GetMovieTimeScale(_myMovie);
        myMovie->set<FrameCountTag>(long(myDurationInSeconds * 25));
        
        
        SetGWorld(_myOffScreenWorld, NULL);
        SetMovieGWorld(_myMovie, _myOffScreenWorld, NULL);
        
        asl::Time myLoadEndTime;
        AC_INFO << "Load file:" << theFilename << ", time " << (myLoadEndTime - myLoadStartTime) << "s";
    }

    double
    QuicktimeDecoder::readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster) {
        AC_INFO << "Read frame: " << theFrame << ", last decoded frame: " << _myLastDecodedFrame;

        if (theFrame >= getFrameCount() -1) {
            setEOF(true);
            return theTime;
        }
        // Handle frame wrap around
        if (_myLastDecodedFrame > theFrame) {
            decodeFrame(0, theTargetRaster);
        }

        if (theFrame != _myLastDecodedFrame) {
            decodeFrame(theFrame, theTargetRaster);
        }
        
        theTargetRaster->resize(getFrameWidth(), getFrameHeight());
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

        // skip to the next interesting time and get the duration for that frame
        GetMovieNextInterestingTime(_myMovie,
                  myFlags,
                  1,
                  &myMediaType,
                  _myInternalMovieTime,
                  0,
                  &_myInternalMovieTime,
                  &myDuration);
        // set the time for the frame and give time to the movie toolbox
        SetMovieTimeValue(_myMovie, _myInternalMovieTime);
        
        // *** this does the actual drawing into the GWorld ***
        MoviesTask(_myMovie,0);
        Ptr baseAddr = GetPixBaseAddr(GetGWorldPixMap(_myOffScreenWorld)); 
        QTGetPixMapHandleRowBytes(GetGWorldPixMap(_myOffScreenWorld));    
        memcpy(theTargetRaster->pixels().begin(), baseAddr, theTargetRaster->pixels().size());

        _myLastDecodedFrame = theFrameNumber;

    }    
}

Movie QTURL_NewMovieFromURL (char *theURL)
{
  Movie    myMovie = NULL;
  Handle    myHandle = NULL;
  Size    mySize = 0;

  mySize = (Size)strlen(theURL) + 1;
  cout << " mySize: " << mySize << endl;
  myHandle = NewHandleClear(mySize);
  BlockMove(theURL, *myHandle, mySize);
  
  NewMovieFromDataRef(&myMovie, newMovieActive, 0, myHandle, URLDataHandlerSubType);

  if (myHandle != NULL) {
    DisposeHandle(myHandle);
  }

  return myMovie;

}   
