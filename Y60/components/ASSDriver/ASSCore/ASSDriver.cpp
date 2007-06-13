//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "ASSDriver.h"

#include <asl/Logger.h>
#include <asl/SerialDeviceFactory.h>
#include <asl/Assure.h>
#include <asl/numeric_functions.h>

#include <y60/ImageBuilder.h>
#include <y60/PixelEncoding.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/Overlay.h>

#include <iostream>
#include <algorithm>
#include <ctype.h>

#ifdef LINUX
#   include <values.h>
#else
#   include <float.h>
#endif


using namespace std;
using namespace asl;
using namespace y60;
using namespace jslib;

static const char * DriverStateStrings[] = {
    "no_serial_port",
    "synchronizing",
    "running",
    "configuring",
    ""
};

IMPLEMENT_ENUM( y60::DriverState, DriverStateStrings );

const char * CMD_ENTER_CONFIG_MODE = "x";
const char * CMD_QUERY_CONFIG_MODE = "c98";
const char * CMD_LEAVE_CONFIG_MODE = "c99";

const char * CMD_CALLIBRATE_TRANSMISSION_LEVEL = "c01";
const char * CMD_PERFORM_TARA = "c02";

namespace y60 {

const unsigned char MAGIC_TOKEN( 255 );


static const char * RAW_RASTER = "ASSRawRaster";
static const char * FILTERED_RASTER = "ASSFilteredRaster";
static const char * MOMENT_RASTER = "ASSMomentRaster";

const std::string OK_STRING("\r\nOK\r\n");
const std::string ERROR_STRING("\r\nERR");
const double COMMAND_TIMEOUT( 10 );

const unsigned int BEST_VERSION( 260 );

const unsigned int MAX_NUM_STATUS_TOKENS( 9 );
const unsigned int BYTES_PER_STATUS_TOKEN( 5 );
const unsigned int MAX_STATUS_LINE_LENGTH( MAX_NUM_STATUS_TOKENS * BYTES_PER_STATUS_TOKEN + 2 );

const float TOUCH_MARKER_LIFE_TIME( 1.0 );

#ifdef OSX
#undef verify
#endif

Box2f
asBox2f( const Box2i & theBox ) {
    return Box2f( Vector2f( theBox[Box2i::MIN][0], theBox[Box2i::MIN][1]),
                Vector2f( theBox[Box2i::MAX][0], theBox[Box2i::MAX][1]));
}


//XXX
void dumpBuffer(std::vector<unsigned char> & theBuffer) {
    if ( ! theBuffer.empty()) {
        cerr << "=========== buffer: " << endl;
        unsigned myMax = asl::minimum( unsigned(theBuffer.size()), unsigned(80));
        cerr.setf( std::ios::hex, std::ios::basefield );
        cerr.setf( std::ios::showbase );
        for (unsigned i = 0; i < /*myMax*/ theBuffer.size(); ++i) {
            if (theBuffer[i] == 0xff) {
                cerr << endl;
            }
            cerr << int(theBuffer[i]) << " ";
        }
        cerr.unsetf( std::ios::hex );
        cerr.unsetf( std::ios::showbase );
        cerr << endl << "==================" << endl;
    }
}
// XXX
void fillBufferWithString(std::vector<unsigned char> & theBuffer, const char * theString) {
    string myString( theString );
    for (unsigned i = 0; i < myString.size(); ++i) {
        theBuffer.push_back( myString[i] );
    }
    AC_PRINT << "================";
    dumpBuffer( theBuffer );
}

ASSDriver::ASSDriver() :
    IRendererExtension("ASSDriver"),
    _myGridSize(0,0),
    _mySyncLostCounter(0),
    _myRawRaster(dom::ValuePtr(0)),
    _myDenoisedRaster(dom::ValuePtr(0)),
    _myMomentRaster(dom::ValuePtr(0)),
    _myScene(0),
    _myNoiseThreshold( 15 ),
    _myComponentThreshold( 5 ),
    _myFirstDerivativeThreshold( 200 ),
    _myGainPower(2.0f),
    _myMinTouchInterval( 0.25 ),
    _myIDCounter( 0 ),
    _myMagicTokenFlag( false ),
    _myUseUSBFlag( false ),
    _myPortNum( -1 ),
    _mySerialPort( 0 ),
    _myBaudRate( 57600 ),
    _myBitsPerSerialWord( 8 ),
    _myParity( SerialDevice::NO_PARITY ),
    _myStopBits( 1 ),
    _myHandshakingFlag( false ),
    _myReceiveBuffer(256),
    _myDebugTouchEventsFlag( 0 ),
    _myLastFrameTime( asl::Time() ),
    _myRunTime(0.0),
    _myLastCommandTime( asl::Time() ),
    _myProbePosition( -1, -1),

    _myGridColor(0.5, 0.5, 0, 1.0),
    _myCursorColor(0.5, 0.5, 0, 1.0),
    _myTouchColor(1.0, 1.0, 1.0, 1.0),
    _myTextColor(1.0, 1.0, 1.0, 1.0),
    _myProbeColor(0.0, 0.75, 0.0, 1.0),

    _myExpectedLine( 0 ),
    _myFirmwareVersion(-1),
    _myFirmwareStatus(-1),
    _myControllerId(-1),
    _myFirmwareMode(-1),
    _myFramerate(-1),
    _myFrameNo(-1),
    _myChecksum(-1),
    _myClampToScreenFlag(false),
    _myWindow( 0 )
{
    setState(NO_SERIAL_PORT);
}

ASSDriver::~ASSDriver() {
    freeSerialPort();
    AC_PRINT << "ASSDriver stats:";
    AC_PRINT << "    sync errors: " << _mySyncLostCounter;
}

bool
ASSDriver::onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    _myWindow = theWindow;
    _myScene = theWindow->getCurrentScene();
    // TODO: reallocate rasters
    return true;
}

void
ASSDriver::onFrame(jslib::AbstractRenderWindow * theWindow, double theTime) {
}

void
ASSDriver::synchronize() {
    if ( ! _myMagicTokenFlag ) {
        for (unsigned i = 0; i < _myFrameBuffer.size(); ++i) {
            if (_myFrameBuffer[i] == MAGIC_TOKEN) {
                //AC_PRINT << "Found MAGIC_TOKEN.";
                _myMagicTokenFlag = true;
                if ( i > 0) {
                    _myFrameBuffer.erase( _myFrameBuffer.begin(), _myFrameBuffer.begin() + i);
                    if ( ! _myFrameBuffer.empty() ) {
                        ASSURE( _myFrameBuffer[0] == MAGIC_TOKEN); 
                    }
                }
                break;
            }
        }
    }
    if ( _myMagicTokenFlag && _myFrameBuffer.size() > 1) {
        ASSURE( _myFrameBuffer[0] == MAGIC_TOKEN );
        if (_myFrameBuffer[1] == 0) {
            //AC_PRINT << "Found status line.";
            setState(RUNNING);
        } else {
            //AC_PRINT << "Found data line: " << int(_myFrameBuffer[1]);
            _myFrameBuffer.erase( _myFrameBuffer.begin(), _myFrameBuffer.begin() + 1);
            _myMagicTokenFlag = false;
        }
    }
}

void
ASSDriver::allocateGridBuffers() {
    _myRasterNames.clear();
    _myRawRaster = allocateRaster(RAW_RASTER);
    _myDenoisedRaster = allocateRaster(FILTERED_RASTER);
    _myMomentRaster = allocateRaster(MOMENT_RASTER);
}

RasterHandle
ASSDriver::allocateRaster(const std::string & theName) {
    if (_myScene) {
        dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(theName);
        if (myImage) {
            myImage->getFacade<y60::Image>()->createRaster( 
                    _myPoTSize[0], _myPoTSize[1], 1, y60::GRAY);
            _myRasterNames.push_back( theName );
            return RasterHandle( myImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());
        } else {
            ImageBuilder myImageBuilder(theName, false);
            _myScene->getSceneBuilder()->appendImage(myImageBuilder);
            y60::ImagePtr myImage = myImageBuilder.getNode()->getFacade<y60::Image>();
            myImage->set<y60::IdTag>( theName );
            myImage->set<y60::NameTag>( theName );
            myImage->createRaster( _myPoTSize[0], _myPoTSize[1], 1, y60::GRAY);

            _myRasterNames.push_back( theName );

            return RasterHandle( myImage->getNode().childNode(0)->childNode(0)->nodeValueWrapperPtr());
        }
    } else {
        RasterHandle myHandle(createRasterValue(y60::GRAY, _myPoTSize[0], _myPoTSize[1]));
        return myHandle;
                
    }
}

unsigned
ASSDriver::readStatusToken( std::vector<unsigned char>::iterator & theIt, const char theToken ) {
    if ( * theIt != theToken ) {
        throw ASSException(string("Failed to parse status token '") + theToken + "'. Got '" +
                char(* theIt) + "'", PLUS_FILE_LINE );
    }
    theIt += 1;
    //string myNumber(theIt, theIt + 4);
    istringstream myStream;
    myStream.setf( std::ios::hex, std::ios::basefield );
    myStream.str(string( theIt, theIt + 4 ));
    //AC_PRINT << "=== parsing '" << myStream.str() << "'";
    theIt += 4;
    unsigned myNumber;
    myStream >>  myNumber;
    return myNumber;
}


unsigned
ASSDriver::getBytesPerStatusLine() {
    if (_myFirmwareVersion < 0) {
        return MAX_STATUS_LINE_LENGTH;
    } else if ( _myFirmwareVersion < BEST_VERSION ) {
        // one token: version
        return BYTES_PER_STATUS_TOKEN + 2;
    } else {
        // nine tokens: version status id mode framerate width height framenumber checksum
        return 9 * BYTES_PER_STATUS_TOKEN + 2;
    }
}

void
ASSDriver::parseStatusLine() {
    //AC_PRINT << "==== parseStatusLine()";
    //dumpBuffer(_myFrameBuffer);
    if ( _myFrameBuffer.size() >= getBytesPerStatusLine() ) {
        ASSURE( _myFrameBuffer[0] == MAGIC_TOKEN );
        ASSURE( _myFrameBuffer[1] == _myExpectedLine );
        std::vector<unsigned char>::iterator myIt = _myFrameBuffer.begin() + 2;
        _myFirmwareVersion = readStatusToken( myIt, 'V' );
        //AC_PRINT << "Firmware version: " << _myFirmwareVersion;
        Vector2i myGridSize;
        if ( _myFirmwareVersion < BEST_VERSION ) {
	    /*
            myGridSize[0] = readStatusToken( myIt, 'W' );
            myGridSize[1] = readStatusToken( myIt, 'H' );
	    */
	    myGridSize[0] = 20;
	    myGridSize[1] = 10;
        } else {
            _myFirmwareStatus = readStatusToken( myIt, 'S' );
            _myControllerId = readStatusToken( myIt, 'I' );
            _myFirmwareMode = readStatusToken( myIt, 'M' );
            _myFramerate = readStatusToken( myIt, 'F' );
            myGridSize[0] = readStatusToken( myIt, 'W' );
            myGridSize[1] = readStatusToken( myIt, 'H' );
            _myFrameNo = readStatusToken( myIt, 'N' );
            _myChecksum = readStatusToken( myIt, 'C' );
        }
        _myFrameBuffer.erase( _myFrameBuffer.begin(), myIt);
        _myExpectedLine = 1;

        if ( _myGridSize != myGridSize ) {
            _myGridSize = myGridSize;
            _myPoTSize[0] = nextPowerOfTwo( _myGridSize[0] );
            _myPoTSize[1] = nextPowerOfTwo( _myGridSize[1] );
            AC_PRINT << "Grid size: " << _myGridSize;
            ASSURE(_myFrameBuffer.front() == MAGIC_TOKEN);
            allocateGridBuffers();
            _myReceiveBuffer.resize( getBytesPerFrame() );
            createTransportLayerEvent( _myIDCounter ++, "configure" );
        }
    }
}

void
ASSDriver::readSensorValues() {
    while ( _myFrameBuffer.size() >= ( _myExpectedLine == 0 ? 
                getBytesPerStatusLine() : _myGridSize[0] + 2 ))
    {
        //dumpBuffer( _myFrameBuffer );
        if ( _myExpectedLine == 0 ) {
            parseStatusLine();
        } else {
            if ( _myFrameBuffer[0] != MAGIC_TOKEN) {
                AC_PRINT << "No MAGIC_TOKEN: '" << int( _myFrameBuffer[0] );
            }
            if ( _myFrameBuffer[1] != _myExpectedLine) {
                AC_PRINT << "Expected line number " << int( _myExpectedLine )
			 << " got " << int(_myFrameBuffer[1]);
            }
            if ( _myFrameBuffer[0] != MAGIC_TOKEN ||
                    _myFrameBuffer[1] != _myExpectedLine)
            {
                AC_PRINT << "Sync error.";
                _mySyncLostCounter++;
                createTransportLayerEvent( _myIDCounter++, "lost_sync" ); 
                setState( SYNCHRONIZING );
                return;
            }
            int myRowIdx = _myFrameBuffer[1] - 1;
            //AC_PRINT << "Got row: " << myRowIdx;

            size_t byteCount = (_myFrameBuffer.begin() + 2 + _myGridSize[0]) - (_myFrameBuffer.begin() + 2);
            unsigned char * myRowPtr = _myRawRaster.raster->pixels().begin() +
                myRowIdx * _myPoTSize[0];
            std::copy(_myFrameBuffer.begin() + 2, _myFrameBuffer.begin() + 2 + _myGridSize[0], myRowPtr);

            _myFrameBuffer.erase( _myFrameBuffer.begin(), _myFrameBuffer.begin() + _myGridSize[0] + 2);

            if ( myRowIdx == _myGridSize[1] - 1) {
                asl::Time myTime;
                double myDeltaT = myTime - _myLastFrameTime;
                //AC_PRINT << "dt: " <<  myDeltaT;
                // XXX
                myDeltaT = 1.0 / 25.0;

                _myRunTime += myDeltaT;
                _myLastFrameTime = myTime;

                processSensorValues( myDeltaT );
                if (_myScene) {
                    _myScene->getSceneDom()->getElementById(RAW_RASTER)->getFacade<y60::Image>()->triggerUpload();
                }
                _myExpectedLine = 0;
            } else {
                _myExpectedLine += 1;
            }
        }
    }
    if ( ! _myCommandQueue.empty() ) {
        if (_myFirmwareVersion >= 0) {
            if (_myFirmwareVersion >= BEST_VERSION) {
                setState( CONFIGURING );
            } else {
                AC_WARNING << "Hardware commands not supported in this firmware version ("
                           << _myFirmwareVersion << ")";
                _myCommandQueue.clear();
            }
        }
    }
    //AC_PRINT << "post buf size: " << _myFrameBuffer.size();
}

template <class PixelT>
struct Threshold{
    Threshold(const PixelT & theThreshold) :
        _myThreshold( theThreshold ) {}
    PixelT operator () (const PixelT & a) {
        if (a.get() < _myThreshold.get()) {
            return PixelT(0);
        } else {
            return PixelT(a.get() - _myThreshold.get());
        }
    }

    PixelT _myThreshold;
};

template <class PixelT>
struct Power {
    Power(float thePower) :
        _myGainPower( thePower) {}
    PixelT operator () (const PixelT & a) {
        return PixelT( (unsigned char)(pow(float(a.get()), _myGainPower) /  pow(255.0f, _myGainPower - 1)));
    }

    float _myGainPower;
};

template <class RASTER>
struct AnalyseMoment {
    void operator() (const RASTER & theRaster) {
        analyseMoments( theRaster, result );
    }
    MomentResults result;
};

template <class PixelT>
struct AnalyseProximity {
    AnalyseProximity() :
        proximity( 0.0 ),
        max( 0 ) {}
    void operator () (const PixelT & a) {
        proximity += sqrt( float( a.get() ) );
        max = maximum( max, a.get() );
    }
    float proximity;
    typename PixelT::value_type max;
};

template <class PixelT>
struct FindMaximum {
    FindMaximum() :
        max( 0 ) {}
    void operator () (const PixelT & a) {
        max = maximum( max, sqrt(float(a.get())));
    }
    float max;
};

void
ASSDriver::updateDerivedRasters()
{
    const y60::RasterOfGRAY & myRawRaster = *
            dom::dynamic_cast_Value<y60::RasterOfGRAY>( & * (_myRawRaster.value) );
    y60::RasterOfGRAY & myDenoisedRaster = *
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );
    y60::RasterOfGRAY & myMomentRaster = *
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myMomentRaster.value) );

    std::transform( myRawRaster.begin(), myRawRaster.end(),
            myDenoisedRaster.begin(),
            Threshold<gray<unsigned char> >( _myNoiseThreshold));

    std::transform( myDenoisedRaster.begin(), myDenoisedRaster.end(),
            myMomentRaster.begin(),
            Power<gray<unsigned char> >( _myGainPower));

    dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );
    dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myMomentRaster.value) );

}

void
ASSDriver::processSensorValues( double theDeltaT ) {

    updateDerivedRasters();

    BlobListPtr myROIs = connectedComponents( _myMomentRaster.raster, _myComponentThreshold);

    std::vector<MomentResults> myCurrentPositions;
    computeCursorPositions( myCurrentPositions, myROIs);
    correlatePositions( myCurrentPositions, myROIs );
    updateCursors(theDeltaT);
}

void
ASSDriver::computeCursorPositions( std::vector<MomentResults> & theCurrentPositions, 
                                   const BlobListPtr & theROIs)
{
    typedef subraster<gray<unsigned char> > SubRaster;
    for (BlobList::const_iterator it = theROIs->begin(); it != theROIs->end(); ++it) {
        Box2i myBox = (*it)->bbox();
        AnalyseMoment<SubRaster> myMomentAnalysis;
        // XXX is this correct ?
        _myMomentRaster.raster->apply( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1],
                                    myBox[Box2i::MAX][0] + 1, myBox[Box2i::MAX][1] + 1, myMomentAnalysis);

        MomentResults myResult = myMomentAnalysis.result;
        myResult.center += Vector2f( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1]);

        //AC_PRINT << "major angle: " << myMomentAnalysis.result.major_angle << " major dir: " << myMomentAnalysis.result.major_dir;
        /*
        AC_PRINT << "moment: w: " << myMomentAnalysis.result.w 
                 << " l: " << myMomentAnalysis.result.l
                 << " major dir: " << myMomentAnalysis.result.major_dir
                 << " minor dir: " << myMomentAnalysis.result.minor_dir;
                 */

        theCurrentPositions.push_back( myResult );
    }

}

template <class N>
N sqr(const N & n) {
    return n * n;
}

void 
ASSDriver::updateCursors(double theDeltaT) {
 y60::RasterOfGRAY & myDenoisedRaster = *
        dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );

    y60::RasterOfGRAY & myRawRaster = *
        dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );

    CursorMap::iterator myCursorIt = _myCursors.begin();
    for(; myCursorIt != _myCursors.end(); ++myCursorIt) {
        computeIntensity(myCursorIt, myRawRaster);
        findTouch(myCursorIt, theDeltaT);

    }
    dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );
    dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );
}

void
ASSDriver::findTouch(CursorMap::iterator & theCursorIt, double theDeltaT) {
    
    float myFirstDerivative = (theCursorIt->second.intensity - theCursorIt->second.previousIntensity) / theDeltaT;
    //float myFirstDerivative = (theCursorIt->second.intensity - theCursorIt->second.getMinIntensity()) / theDeltaT;
    theCursorIt->second.firstDerivative = myFirstDerivative; 
    
    float myTouch = 0;
    if ( myFirstDerivative > _myFirstDerivativeThreshold  &&
        _myRunTime - theCursorIt->second.lastTouchTime > _myMinTouchInterval &&
        theCursorIt->second.intensity > 9) 
    {
        myTouch = theCursorIt->second.intensity;   
        //AC_PRINT << "touched me! at " << _myRunTime;
        theCursorIt->second.lastTouchTime = _myRunTime;
        createEvent( theCursorIt->first, "touch", theCursorIt->second.position,
                applyTransform(theCursorIt->second.position, getTransformationMatrix() ),
                theCursorIt->second.roi );
        _myTouchHistory.push_back( TouchEvent(_myRunTime, theCursorIt->second.position ));
    }
   
    theCursorIt->second.intensityHistory.push_back( theCursorIt->second.intensity );
    if( theCursorIt->second.intensityHistory.size() > MAX_HISTORY_LENGTH ) {
            theCursorIt->second.intensityHistory.pop_front();
        }

    if (_myDebugTouchEventsFlag) {
        cout << _myRunTime << "\t" << theCursorIt->second.intensity << "\t" << myFirstDerivative*0.1 << "\t" << myTouch
            << "\t" << theCursorIt->second.position[0] << endl;
    }

}

void
ASSDriver::computeIntensity(CursorMap::iterator & theCursorIt, const y60::RasterOfGRAY & theRaster) {
    const Vector2f & myPosition = theCursorIt->second.position;
    
    Vector2i myTopLeft;
    myTopLeft[0] = int( floor( myPosition[0] ));
    myTopLeft[1] = int( floor( myPosition[1] ));

    Vector2f myWeight;
    myWeight[0] = myPosition[0] - myTopLeft[0];
    myWeight[1] = myPosition[1] - myTopLeft[1];

    float myHeadRoom = 0.3;
    myWeight[0] *= 1 - 2 * myHeadRoom;
    myWeight[0] += myHeadRoom;
    myWeight[1] *= 1 - 2 * myHeadRoom;
    myWeight[1] += myHeadRoom;

    float myV00 = sqrt( float( theRaster[ theRaster.hsize() * myTopLeft[1] + myTopLeft[0] ].get())); 
    float myV10 = sqrt( float( theRaster[ theRaster.hsize() * myTopLeft[1] + myTopLeft[0] + 1].get()));
    float myV01 = sqrt( float( theRaster[ theRaster.hsize() * (myTopLeft[1] + 1) + myTopLeft[0]].get()));
    float myV11 = sqrt( float( theRaster[ theRaster.hsize() * (myTopLeft[1] + 1) + myTopLeft[0] + 1].get()));

    float myIntensity = myV11 * myWeight[0]  * myWeight[1]+ 
        myV01 * (1 - myWeight[0]) * myWeight[1] +
        myV10 * myWeight[0] * (1 - myWeight[1]) +
        myV00 * (1 - myWeight[0]) * (1 - myWeight[1]);


    int myROIChanged = 0;
    if ( theCursorIt->second.previousRoi.hasPosition() &&
            theCursorIt->second.roi[Box2f::MIN] != theCursorIt->second.previousRoi[Box2f::MIN] ||
            theCursorIt->second.roi[Box2f::MAX] !=  theCursorIt->second.previousRoi[Box2f::MAX])
    {
        myROIChanged = int(myIntensity);
        myIntensity = (theCursorIt->second.previousIntensity + myIntensity) / 2;
    }
    

    if (theCursorIt->second.previousIntensity == 0.0) {
        theCursorIt->second.previousIntensity = myIntensity;
    } else {
        theCursorIt->second.previousIntensity = theCursorIt->second.intensity;
    }
    theCursorIt->second.intensity = myIntensity;

}

asl::Matrix4f
ASSDriver::getTransformationMatrix() {
    Matrix4f myTransform;
    if (_myOverlay ) {
        const y60::Overlay & myOverlay = *(_myOverlay->getFacade<y60::Overlay>());
        const Vector2f & myPosition = myOverlay.get<Position2DTag>();
        const Vector2f & myScale = myOverlay.get<Scale2DTag>();
        const float & myOrientation = myOverlay.get<Rotation2DTag>();

        myTransform.makeIdentity();

        myTransform.translate( Vector3f( 0.5, 0.5, 0));

        myTransform.scale( Vector3f( myScale[0], myScale[1], 0));
        myTransform.translate( Vector3f( myPosition[0], myPosition[1], 0));
        myTransform.rotateZ( myOrientation );
    } else {
        myTransform.makeIdentity();
    }
    return myTransform;
}

void 
ASSDriver::correlatePositions( const std::vector<MomentResults> & theCurrentPositions,
                               const BlobListPtr theROIs)
{

    Matrix4f myTransform = getTransformationMatrix();

    const BlobList & myROIs = * theROIs;

    std::vector<int> myCorrelatedIds;
    for (unsigned i = 0; i < theCurrentPositions.size(); ++i) {
        float myMinDistance = FLT_MAX;
        CursorMap::iterator myMinDistIt  = _myCursors.end();
        CursorMap::iterator myCursorIt  = _myCursors.begin();
        for (; myCursorIt != _myCursors.end(); ++myCursorIt ) {
            if ( find( myCorrelatedIds.begin(), myCorrelatedIds.end(), myCursorIt->first ) ==
                    myCorrelatedIds.end())
            {
                float myDistance = magnitude( myCursorIt->second.position - theCurrentPositions[i].center);
                if (myDistance < myMinDistance) {
                    myMinDistance = myDistance;
                    myMinDistIt = myCursorIt;
                }
            }
        }
        const float DISTANCE_THRESHOLD = 2.0;
        if (myMinDistIt != _myCursors.end() && myMinDistance < DISTANCE_THRESHOLD) {
            // cursor moved
            myCorrelatedIds.push_back( myMinDistIt->first );
            myMinDistIt->second.position = theCurrentPositions[i].center;
            myMinDistIt->second.major_direction = theCurrentPositions[i].major_dir;
            myMinDistIt->second.minor_direction = theCurrentPositions[i].minor_dir;

            myMinDistIt->second.previousRoi = myMinDistIt->second.roi;
            myMinDistIt->second.roi = asBox2f( myROIs[i]->bbox() );
            createEvent( myMinDistIt->first, "move", theCurrentPositions[i].center,
                    applyTransform( theCurrentPositions[i].center, myTransform),
                    asBox2f( myROIs[i]->bbox()) );
        } else {
            // new cursor
            int myNewID( _myIDCounter++ );
            _myCursors.insert( make_pair( myNewID, Cursor( theCurrentPositions[i],
                    asBox2f( myROIs[i]->bbox()) )));
            myCorrelatedIds.push_back( myNewID );
            createEvent( myNewID, "add", theCurrentPositions[i].center,
                    applyTransform( theCurrentPositions[i].center, myTransform),
                    asBox2f( myROIs[i]->bbox() ));
        }
    }


    CursorMap::iterator myIt = _myCursors.begin();
    std::vector<int> myOutdatedCursorIds;
    for (; myIt != _myCursors.end(); ++myIt) {
        if ( find( myCorrelatedIds.begin(), myCorrelatedIds.end(), myIt->first ) ==
                myCorrelatedIds.end())
        {
            // cursor removed
            myOutdatedCursorIds.push_back( myIt->first );
            createEvent( myIt->first, "remove", myIt->second.position,
                    applyTransform( myIt->second.position, myTransform ),
                    myIt->second.roi);
        }
    }

    for (unsigned i = 0; i < myOutdatedCursorIds.size(); ++i) {
        _myCursors.erase( myOutdatedCursorIds[i] );
    }
}

void
ASSDriver::setState( DriverState theState ) {
    _myState = theState;
    switch ( _myState ) {
        case NO_SERIAL_PORT:
            freeSerialPort();
            break;
        case SYNCHRONIZING:
            _myMagicTokenFlag = false;
            _myGridSize = Vector2i(0, 0);
            _myFrameBuffer.clear();
            break;
        case RUNNING:
            _myExpectedLine = 0;
            break;
        case CONFIGURING:
            _myConfigureState = SEND_CONFIG_COMMANDS;
            sendCommand( CMD_ENTER_CONFIG_MODE );
            break;
        default:
            break;
    }
    //AC_PRINT << "ASSDriver::setState() " << theState << " ...";
}

void
ASSDriver::drawGrid() {
    glColor4fv( _myGridColor.begin() );
    glBegin( GL_LINES );
    for (unsigned i = 0; i < _myGridSize[0]; ++i) {
        glVertex2f(i, 0.0);
        glVertex2f(i, _myGridSize[1] - 1);
    }
    for (unsigned i = 0; i < _myGridSize[1]; ++i) {
        glVertex2f( 0.0, i);
        glVertex2f( _myGridSize[0] - 1, i);
    }
    glEnd();
}


void
ASSDriver::drawCircle( const Vector2f & thePosition, float theRadius,
                       unsigned theSubdivision, const Vector4f & theColor)
{
    float myStep = 2 * PI / theSubdivision;

    glColor4fv( theColor.begin() );
    glBegin( GL_LINE_LOOP );
    for (unsigned i = 0; i < theSubdivision; ++i) {
        Vector2f myPosition( theRadius * sin( i * myStep ), theRadius * cos( i * myStep ));
        glVertex2fv( (myPosition + thePosition).begin() );
    }
    glEnd();
}

void
ASSDriver::drawMarkers() {
    // draw origin
    drawCircle( Vector2f(0.0, 0.0), 0.15, 36, _myGridColor );

    // draw cursors
    CursorMap::iterator myIt = _myCursors.begin();
    for (; myIt != _myCursors.end(); ++myIt ) {
        drawCircle( myIt->second.position, 0.15, 36, _myCursorColor );
    }

    // draw touch history
    if ( ! _myTouchHistory.empty() ) {
        //std::vector<TouchEvent>::reverse_iterator myIt = _myTouchHistory.rbegin();
        std::vector<TouchEvent>::iterator myIt = _myTouchHistory.begin();
        while ( myIt != _myTouchHistory.end() ) {
            if (_myRunTime - myIt->birthTime > TOUCH_MARKER_LIFE_TIME) {
                myIt = _myTouchHistory.erase( myIt );
            } else {
                drawCircle( myIt->position, 0.20, 36, _myTouchColor);
                myIt += 1;
            }
        }
    }

    // draw value probe
    if (_myProbePosition[0] >= 0 && _myProbePosition[1] >= 0) {
        drawCircle( _myProbePosition, 0.15, 36, _myProbeColor);
    }
}



void
ASSDriver::onPostRender(jslib::AbstractRenderWindow * theWindow) {
    if ( _myScene && _myOverlay) {
        y60::OverlayPtr myOverlay = _myOverlay->getFacade<y60::Overlay>();

        if ( ! myOverlay->get<VisibleTag>() ) {
            return;
        }

            
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
        glLoadIdentity();

        dom::Node * myOverlayList = _myOverlay->parentNode();
        if ( ! myOverlayList ) {
            throw ASSException("Failed to get owning viewport.", PLUS_FILE_LINE );
        }
        dom::Node * myViewportNode = myOverlayList->parentNode();
        if ( ! myViewportNode ) {
            throw ASSException("Failed to get owning viewport.", PLUS_FILE_LINE );
        }
        y60::ViewportPtr myViewport = myViewportNode->getFacade<y60::Viewport>();

        gluOrtho2D(0.0, myViewport->get<ViewportWidthTag>(), myViewport->get<ViewportHeightTag>(), 0.0);

        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glLoadMatrixf( static_cast<const GLfloat*>( getTransformationMatrix().getData()));

        glDisable( GL_DEPTH_TEST );

        drawGrid();
        drawMarkers();

        glPopMatrix();

        if (_myProbePosition[0] >= 0 && _myProbePosition[1] >= 0) {
            unsigned char myValue = * (_myRawRaster.raster->pixels().begin() +
                int(_myProbePosition[1]) * _myPoTSize[0] + int(_myProbePosition[0]));

            const Vector4f & myOldColor = theWindow->getTextColor();
            theWindow->setTextColor( _myTextColor );
            theWindow->renderText( Vector2f( 50, 50 ), string("Value") +
                    as_string(_myProbePosition) + " = " + as_string(int(myValue)), "Screen15");
            theWindow->setTextColor( myOldColor );
        }
        

        glMatrixMode( GL_PROJECTION );
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );
        glPopAttrib();
    }
}

void
ASSDriver::sendCommand( const std::string & theCommand ) {
    try {
        if (_mySerialPort) {
            AC_PRINT << "Sending command: '" << theCommand << "'";
            std::string myCommand( theCommand );
            myCommand.append("\r");
            _mySerialPort->write( myCommand.c_str(), myCommand.size() );
            _myLastCommandTime = double( asl::Time() );
        } else {
            AC_WARNING << "Can not send command. No serial port.";
            setState( NO_SERIAL_PORT );
        }
    } catch (const SerialPortException & ex) {
        AC_WARNING << ex;
        createTransportLayerEvent( _myIDCounter ++, "lost_communication" );
        setState(NO_SERIAL_PORT);
    }
}

void
ASSDriver::onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const
{
    if (thePropertyName == "gridSize") {
        theReturnValue.set( _myGridSize );
        return;
    }
    if (thePropertyName == "overlay") {
        theReturnValue.set( _myOverlay );
        return;
    }
    if (thePropertyName == "rasterNames") {
        theReturnValue.set( _myRasterNames );
        return;
    }
    if (thePropertyName == "gridColor") {
        theReturnValue.set( _myGridColor );
        return;
    }
    if (thePropertyName == "cursorColor") {
        theReturnValue.set( _myCursorColor );
        return;
    }
    if (thePropertyName == "touchColor") {
        theReturnValue.set( _myTouchColor );
        return;
    }
    if (thePropertyName == "textColor") {
        theReturnValue.set( _myTextColor );
        return;
    }
    if (thePropertyName == "probeColor") {
        theReturnValue.set( _myProbeColor );
        return;
    }
    AC_ERROR << "Unknown property '" << thePropertyName << "'";
}

void
ASSDriver::onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue)
{
    if (thePropertyName == "gridSize") {
        AC_ERROR << "gridSize property is read only";
        return;
    }
    if (thePropertyName == "overlay") {
        _myOverlay = thePropertyValue.get<dom::NodePtr>();
        return;
    }
    if (thePropertyName == "rasterNames") {
        AC_ERROR << "rasterNames property is read only";
        return;
    }
    if (thePropertyName == "gridColor") {
        _myGridColor = thePropertyValue.get<Vector4f>();
        return;
    }
    if (thePropertyName == "cursorColor") {
        _myCursorColor = thePropertyValue.get<Vector4f>();
        return;
    }
    if (thePropertyName == "touchColor") {
        _myTouchColor = thePropertyValue.get<Vector4f>();
        return;
    }
    if (thePropertyName == "textColor") {
        _myTextColor = thePropertyValue.get<Vector4f>();
        return;
    }
    if (thePropertyName == "probeColor") {
        _myProbeColor = thePropertyValue.get<Vector4f>();
        return;
    }

    AC_ERROR << "Unknown property '" << thePropertyName << "'";
}


void
ASSDriver::onUpdateSettings(dom::NodePtr theSettings) {

    dom::NodePtr mySettings = getASSSettings( theSettings );

    getConfigSetting( mySettings, "ComponentThreshold", _myComponentThreshold, 5 );
    getConfigSetting( mySettings, "NoiseThreshold", _myNoiseThreshold, 15 );
    getConfigSetting( mySettings, "GainPower", _myGainPower, 2.0f );
    getConfigSetting( mySettings, "IntensityThreshold", _myIntensityThreshold, 9.0f );
    getConfigSetting( mySettings, "FirstDerivativeThreshold", _myFirstDerivativeThreshold, 25.0f );
    getConfigSetting( mySettings, "DebugTouchEvents", _myDebugTouchEventsFlag, 0 );
    getConfigSetting( mySettings, "ProbePosition", _myProbePosition, Vector2f( -1, -1) );
    getConfigSetting( mySettings, "MinTouchInterval", _myMinTouchInterval, 0.25 );
    getConfigSetting( mySettings, "ClampToScreen", _myClampToScreenFlag, 0);

    bool myPortConfigChanged = false;
    myPortConfigChanged |= getConfigSetting( mySettings, "SerialPort", _myPortNum, -1 );
    myPortConfigChanged |= getConfigSetting( mySettings, "UseUSB", _myUseUSBFlag, false );
    myPortConfigChanged |= getConfigSetting( mySettings, "BaudRate", _myBaudRate, 57600 );
    myPortConfigChanged |= getConfigSetting( mySettings, "BitsPerWord", _myBitsPerSerialWord, 8 );
    myPortConfigChanged |=
            getConfigSetting( mySettings, "Parity", _myParity, SerialDevice::NO_PARITY );
    myPortConfigChanged |= getConfigSetting( mySettings, "StopBits", _myStopBits, 1 );
    myPortConfigChanged |= getConfigSetting( mySettings, "Handshaking", _myHandshakingFlag, false );

    if (myPortConfigChanged) {
        //AC_PRINT << "Serial port configuration changed.";
        setState( NO_SERIAL_PORT );
    }
}

Vector3f 
ASSDriver::applyTransform( const Vector2f & theRawPosition,
                           const Matrix4f & theTransform )
{
    Vector4f myHPosition;
    myHPosition[0] = theRawPosition[0];
    myHPosition[1] = theRawPosition[1];
    myHPosition[2] = 0.0;
    myHPosition[3] = 1.0;
    myHPosition = myHPosition * theTransform;
    Vector3f my3DPosition;
    my3DPosition[0] = myHPosition[0];
    my3DPosition[1] = myHPosition[1];
    my3DPosition[2] = myHPosition[2];
    if ( _myWindow && _myClampToScreenFlag ) {
        y60::CanvasPtr myCanvas = _myWindow->getCanvas()->getFacade<y60::Canvas>();
        my3DPosition[0] = clamp( my3DPosition[0], 0.0f, float(myCanvas->getWidth()) );
        my3DPosition[1] = clamp( my3DPosition[1], 0.0f, float(myCanvas->getHeight()) );
    }
    return my3DPosition;    
}

void 
ASSDriver::processInput() {
    //AC_PRINT << "------------ frame ------- ";
    switch (_myState) {
        case NO_SERIAL_PORT:
            scanForSerialPort();
            break;
        case SYNCHRONIZING:
            readDataFromPort();
            synchronize();
            break;
        case RUNNING:
            readDataFromPort();
            readSensorValues();
            break;
        case CONFIGURING:
            readDataFromPort();
            handleConfigurationCommand();
            break;
    }
}

void
ASSDriver::readDataFromPort() {
    try {
        size_t myMaxBytes = _myReceiveBuffer.size();

        // [DS] If the serial port is removed a non-blocking read returns EAGAIN
        // for some reason. Peek throws an exception which is just what we want.
        _mySerialPort->peek();
        //AC_PRINT << "bytes: " << _mySerialPort->peek();
        
        //AC_PRINT << "bytes: " << myMaxBytes;
        _mySerialPort->read( reinterpret_cast<char*>(& ( * _myReceiveBuffer.begin())),
                myMaxBytes );

        //AC_PRINT << "=== Got " << myByteCount << " bytes.";

        _myFrameBuffer.insert( _myFrameBuffer.end(),
                _myReceiveBuffer.begin(), _myReceiveBuffer.begin() + myMaxBytes );
        //dumpBuffer( _myFrameBuffer );
    } catch (const SerialPortException & ex) {
        AC_WARNING << ex;
        createTransportLayerEvent( _myIDCounter ++, "lost_communication" );
        setState(NO_SERIAL_PORT);
    }
}

void
ASSDriver::scanForSerialPort() {
    if (_myPortNum >= 0 ) {
#ifdef WIN32
        _mySerialPort = getSerialDevice( _myPortNum );
#endif
#ifdef LINUX
        if (_myUseUSBFlag) {
            string myDeviceName("/dev/ttyUSB");
            myDeviceName += as_string( _myPortNum );
            if ( fileExists( myDeviceName ) ) {
                _mySerialPort = getSerialDeviceByName( myDeviceName );
            }
        } else {
            _mySerialPort = getSerialDevice( _myPortNum );
        }
#endif
#ifdef OSX
        // [DS] IIRC the FTDI devices on Mac OS X appear as /dev/tty.usbserial-[devid]
        // where [devid] is a string that is flashed into the FTDI chip. I'll
        // check that the other day.
        AC_PRINT << "TODO: implement device name handling for FTDI USB->RS232 "
            << "virtual TTYs.";
#endif

        if (_mySerialPort) {
            _mySerialPort->open( _myBaudRate, _myBitsPerSerialWord,
                    _myParity, _myStopBits, _myHandshakingFlag);
            setState( SYNCHRONIZING );
        }
    } else {
        AC_PRINT << "scanForSerialPort() No port configured.";
    }
}

void
ASSDriver::freeSerialPort() {
    if (_mySerialPort) {
        delete _mySerialPort;
        _mySerialPort = 0;
    }
}


dom::NodePtr
getASSSettings(dom::NodePtr theSettings) {
    dom::NodePtr mySettings(0);
    if ( theSettings->nodeType() == dom::Node::DOCUMENT_NODE) {
        if (theSettings->childNode(0)->nodeName() == "settings") {
            mySettings = theSettings->childNode(0)->childNode("ASSDriver", 0);
        }
    } else if ( theSettings->nodeName() == "settings") {
        mySettings = theSettings->childNode("ASSDriver", 0);
    } else if ( theSettings->nodeName() == "ASSDriver" ) {
        mySettings = theSettings;
    }
    
    if ( ! mySettings ) {
        throw ASSException(
            std::string("Could not find ASSDriver node in settings: ") +
            as_string( * theSettings), PLUS_FILE_LINE );
    }
    return mySettings;
}

size_t
ASSDriver::getBytesPerFrame() {
    // line format: <start> <lineno> <databytes>
    return (1 + 1 + _myGridSize[0]) * _myGridSize[1] + MAX_STATUS_LINE_LENGTH;
}

void
ASSDriver::handleConfigurationCommand() {
    //dumpBuffer( _myFrameBuffer );
    CommandResponse myResponse = getCommandResponse();
    switch (_myConfigureState) {
        case SEND_CONFIG_COMMANDS:
            //AC_PRINT << "SEND_CONFIG_COMMANDS";
            if (myResponse == RESPONSE_OK) {
                if ( ! _myCommandQueue.empty()) {
                    sendCommand( _myCommandQueue.front() );
                    _myCommandQueue.pop_front();
                    if (_myCommandQueue.empty()) {
                        _myConfigureState = WAIT_FOR_RESPONSE;
                    }
                } else {
                    sendCommand( CMD_LEAVE_CONFIG_MODE );
                    _myConfigureState = WAIT_FOR_EXIT;
                }
            } else if (myResponse == RESPONSE_ERROR || myResponse == RESPONSE_TIMEOUT) {
                AC_ERROR << "Failed to enter config mode: " 
                         << (myResponse == RESPONSE_TIMEOUT ? "timeout" : "error");
                _myCommandQueue.clear();
                setState( SYNCHRONIZING );
            }
            break;
        case WAIT_FOR_RESPONSE:
            //AC_PRINT << "WAIT_FOR_RESPONSE";
            if (myResponse == RESPONSE_OK ) {
                sendCommand( CMD_LEAVE_CONFIG_MODE );
                _myConfigureState = WAIT_FOR_EXIT;
            } else if (myResponse == RESPONSE_ERROR || myResponse == RESPONSE_TIMEOUT) {
                AC_ERROR << "Failed to configure sensor hardware: "
                         << (myResponse == RESPONSE_TIMEOUT ? "timeout" : "error");
                sendCommand( CMD_LEAVE_CONFIG_MODE );
                _myConfigureState = WAIT_FOR_EXIT;
                //setState( SYNCHRONIZING );
            }
            break;
        case WAIT_FOR_EXIT:
            //AC_PRINT << "WAIT_FOR_EXIT";
            if (myResponse == RESPONSE_OK) {
                setState( SYNCHRONIZING );
            } else if (myResponse == RESPONSE_ERROR || myResponse == RESPONSE_TIMEOUT) {
                AC_ERROR << "Failed to leave config mode.";
                setState( SYNCHRONIZING );
            }
            break;
    }
}

CommandResponse 
ASSDriver::getCommandResponse() {
    string myString( (char*)& ( * _myFrameBuffer.begin()), _myFrameBuffer.size());
    transform( myString.begin(), myString.end(), myString.begin(), ::toupper);
    string::size_type myPos = myString.find(OK_STRING);
    //AC_PRINT << "getCommandResponse(): '" << myString << "'";
    if (myPos != string::npos ) {
        string::size_type myEnd = myPos + OK_STRING.size() - 2;
        _myFrameBuffer.erase( _myFrameBuffer.begin() , _myFrameBuffer.begin() + myEnd );
        AC_PRINT << "Got 'OK'";
        return RESPONSE_OK;
    }

    myPos = myString.find(ERROR_STRING);
    if (myPos != string::npos && myPos + ERROR_STRING.size() + 2 + 2 <= myString.size() &&
        myString[ myPos + ERROR_STRING.size() + 2 ] == '\r' && 
        myString[ myPos + ERROR_STRING.size() + 2 + 1 ] == '\n') {
        unsigned myErrorCode = as<unsigned>( myString.substr( myPos + ERROR_STRING.size(), 2 ));
        AC_ERROR << "Got 'ERR': errorcode: " << myErrorCode;
        string::size_type myEnd = myPos + ERROR_STRING.size() + 2;
        _myFrameBuffer.erase( _myFrameBuffer.begin() , _myFrameBuffer.begin() + myEnd );
        return RESPONSE_ERROR;
    }

    if ( double(asl::Time()) - _myLastCommandTime > COMMAND_TIMEOUT) {
        AC_PRINT << "Got timeout";
        return RESPONSE_TIMEOUT;
    }
    return RESPONSE_NONE;
}


void 
ASSDriver::performTara() {
    //AC_PRINT << "ASSDriver::performTara()";
    queueCommand( CMD_PERFORM_TARA );
}

void 
ASSDriver::callibrateTransmissionLevels() {
    //AC_PRINT << "ASSDriver::callibrateTransmissionLevels()";
    queueCommand( CMD_CALLIBRATE_TRANSMISSION_LEVEL );
}

void
ASSDriver::queryConfigMode() {
    //AC_PRINT << "ASSDriver::queryConfigMode()";
    queueCommand( CMD_QUERY_CONFIG_MODE );
}

void
ASSDriver::queueCommand( const char * theCommand ) {
    _myCommandQueue.push_back( string( theCommand ));
}

static JSBool
PerformTara(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
    DOC_BEGIN("");
    DOC_END;
   
    asl::Ptr<ASSDriver> myNative = getNativeAs<ASSDriver>(cx, obj);
    if (myNative) {
        myNative->performTara();
    } else {
        assert(myNative);
    }
    return JS_TRUE;
}

static JSBool
CallibrateTransmissionLevels(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
    DOC_BEGIN("");
    DOC_END;
   
    asl::Ptr<ASSDriver> myNative = getNativeAs<ASSDriver>(cx, obj);
    if (myNative) {
        myNative->callibrateTransmissionLevels();
    } else {
        assert(myNative);
    }
    return JS_TRUE;
}

static JSBool
QueryConfigMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
    DOC_BEGIN("");
    DOC_END;
   
    asl::Ptr<ASSDriver> myNative = getNativeAs<ASSDriver>(cx, obj);
    if (myNative) {
        myNative->queryConfigMode();
    } else {
        assert(myNative);
    }
    return JS_TRUE;
}


JSFunctionSpec * 
ASSDriver::Functions() {
    static JSFunctionSpec myFunctions[] = {
        {"performTara", PerformTara, 0},
        {"callibrateTransmissionLevels", CallibrateTransmissionLevels, 0},
        {"queryConfigMode", QueryConfigMode, 0},
        {0}
    };
    return myFunctions;
}

} // end of namespace y60

