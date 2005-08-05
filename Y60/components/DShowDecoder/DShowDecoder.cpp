
#include "DShowDecoder.h"

#include <asl/file_functions.h>
#include <asl/os_functions.h>

using namespace std;
using namespace asl;

extern "C"
EXPORT asl::PlugInBase * y60DShowDecoder_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::DShowDecoder(myDLHandle);
}

namespace y60 {
    asl::Ptr<MovieDecoderBase> DShowDecoder::instance() const {
        return asl::Ptr<MovieDecoderBase>(new DShowDecoder(getDLHandle()));
    }

    DShowDecoder::DShowDecoder(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle),
        _myGraph(0), _myMediaControl(0), _myBasicAudio(0), _myMediaSeeking(0)
        {}

    std::string DShowDecoder::canDecode(const std::string & theUrl, 
            asl::ReadableStream * theStream) {
        
        if (asl::toLowerCase(asl::getExtension(theUrl)) == "wmv" || 
            asl::toLowerCase(asl::getExtension(theUrl)) == "avi") {
            AC_INFO << "DShowDecoder can decode :" << theUrl << endl;
            return MIME_TYPE_WMV;
            
        } else {
            AC_INFO << "DShowDecoder can not decode : " << theUrl 
                    << " will currently only work with extensions wmv, avi" << endl;
            return "";
        }
    }

    void DShowDecoder::load(const std::string & theFilename) {
        AC_DEBUG << "DShowDecoder::load " << theFilename;
		string myErrorMsg;
        HRESULT hr;
		hr = CreateGraph(theFilename.c_str(), &_myGraph, &_mySampleGrabber);
		if ( ! hResultIsOk(hr, myErrorMsg)) {
            AC_ERROR << "load " << myErrorMsg;
		} else {
            AC_INFO << "load graph ok";
        }
		setupMovieAttributes();        
    }

    void
    DShowDecoder::startMovie(double theStartTime) {
		string myErrorMsg;
		HRESULT hr;
		hr = _myMediaControl->Run();
		if ( ! hResultIsOk(hr, myErrorMsg)) {
            AC_ERROR << "startMovie " << myErrorMsg;
		}
	}

	void
    DShowDecoder::stopMovie() {
		_myMediaControl->Stop(); 
        if (_myMediaSeeking) {
            //rewind
            string myErrorMsg;
            HRESULT hr;
            REFERENCE_TIME myStartPos = 0;           

            hr = _myMediaSeeking->SetPositions(&myStartPos, 
                    AM_SEEKING_AbsolutePositioning 
                    | AM_SEEKING_Segment | AM_SEEKING_NoFlush,
                    NULL, AM_SEEKING_NoPositioning);
            if ( ! hResultIsOk(hr, myErrorMsg)) {
                AC_ERROR << "SetPositions " << myErrorMsg;
            }
        }
	}

	void DShowDecoder::pauseMovie() {
		_myMediaControl->Pause(); 	
	}

    void DShowDecoder::setVolume(float theVolume) {
        if (_myBasicAudio) {
            long myDSoundVolume = long((theVolume - 1.0) * 10000);
            _myBasicAudio->put_Volume(myDSoundVolume);
        }
    }

    float DShowDecoder::getVolume() {
        long myDSoundVolume = 0;
        if (_myBasicAudio) {
            _myBasicAudio->get_Volume(&myDSoundVolume);
        }
        return float(myDSoundVolume + 10000) / 10000;
    }
    
    double DShowDecoder::readFrame(double theTime, unsigned theFrame, 
                dom::ResizeableRasterPtr theTargetRaster) {
        //AC_DEBUG << "DShowDecoder::readFrame ";
		string myErrorMsg;
		HRESULT hr;
        long mySampleSize;
        hr = _mySampleGrabber->GetCurrentBuffer(&mySampleSize, NULL);
		if ( ! hResultIsOk(hr, myErrorMsg)) {
            AC_WARNING << "readFrame getCurrentBuffer " << myErrorMsg << " at " << theTime;
			return theTime;
		}

		//make theTargetRaster dirty so it gets updated
		theTargetRaster->resize(getFrameWidth(), getFrameHeight());

        if (mySampleSize == theTargetRaster->pixels().size()) {
            _mySampleGrabber->GetCurrentBuffer(&mySampleSize, 
                    (long *)theTargetRaster->pixels().begin());
            
        } else {
            AC_ERROR << "window sample buffer is of size " 
                << mySampleSize << " at " << theTime;
        }

        // adjust volume
        float myVolume = getMovie()->get<VolumeTag>();
        if (!asl::almostEqual(getVolume(), myVolume)) {
            setVolume(myVolume);
        }
        
        //check for EOF
        if (_myMediaSeeking) {
            long long myCurrentP, myStopP;
            _myMediaSeeking->GetPositions(&myCurrentP, &myStopP);
            if (myCurrentP >= myStopP) {
                setEOF(true);
            }
            AC_DEBUG << myCurrentP << " - " << myStopP;
        }
        
        return theTime;
    }


	void DShowDecoder::setupMovieAttributes() {
		int myVideoWidth, myVideoHeight;
		const float myFrameRate = 25.0;
		unsigned long myFrameCount;

		string myErrorMsg;
		HRESULT hr;

        hr = _myGraph->QueryInterface(IID_IMediaControl, (void **)&_myMediaControl);
		if ( ! hResultIsOk(hr, myErrorMsg)) {
            AC_ERROR << "MediaControl Interface not available " << myErrorMsg;
        }
        
        hr = _myGraph->QueryInterface(IID_IBasicAudio, (void **)&_myBasicAudio);
		if ( ! hResultIsOk(hr, myErrorMsg)) {
            AC_WARNING << "BasicAudio Interface not available " << myErrorMsg;
        }

        hr = _myGraph->QueryInterface(IID_IMediaSeeking, (void **)&_myMediaSeeking);
		if ( ! hResultIsOk(hr, myErrorMsg)) {
            //the MediaSeeking interf. may not be present... 
            //then seeking & looping wont work
            AC_INFO << "MediaSeeking Interface not available " << myErrorMsg;
		} 
            
		AM_MEDIA_TYPE myMediaType;
		_mySampleGrabber->GetConnectedMediaType(&myMediaType); 

		if (myMediaType.formattype != FORMAT_VideoInfo) {
			AC_ERROR << "media type is not a video format... ";
			throw Exception();
		}
		// Setup pixel format
		GUID mySubType = myMediaType.subtype;
		if (mySubType == MEDIASUBTYPE_RGB24) {
			setPixelFormat(BGR);
		} else if (mySubType == MEDIASUBTYPE_RGB32) {
			setPixelFormat(BGRA);
		} else {
			AC_ERROR << "pixel format is not valid... ";
			throw Exception();
		}

		VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)(myMediaType.pbFormat);

		myVideoWidth = pVih->bmiHeader.biWidth;
		myVideoHeight = pVih->bmiHeader.biHeight;

		// We want the absolute height, and don't care about up-down orientation.
		if (myVideoHeight < 0) myVideoHeight *= -1;

        // Setup video size and image matrix
        setFrameWidth(myVideoWidth);
        setFrameHeight(myVideoHeight);

		float myXResize = float(myVideoWidth) / asl::nextPowerOfTwo(myVideoWidth);
        float myYResize = float(myVideoHeight) / asl::nextPowerOfTwo(myVideoHeight);

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(asl::Vector3f(myXResize, - myYResize, 1.0f));
        myMatrix.translate(asl::Vector3f(0, myYResize, 0));
        setImageMatrix(myMatrix);

		setFrameRate(myFrameRate);
			

        if (_myMediaSeeking) {
			long long myDuration;
			_myMediaSeeking->GetDuration(&myDuration);
			myFrameCount = unsigned((double(myDuration)*myFrameRate)/10e6);
            setFrameCount(myFrameCount);
		} else {
            setFrameCount(UINT_MAX);
        }

        AC_DEBUG << "Video: frame=" << myVideoWidth << "x" << myVideoHeight << " pixelFormat=" << asl::getStringFromEnum(getPixelFormat(), y60::PixelEncodingString);
        AC_DEBUG << "       fps=" << myFrameRate << " duration=" << double(myFrameCount) / myFrameRate << " s" << " frameCount=" << myFrameCount;

		//FreeMediaType(mt);

	}
}

