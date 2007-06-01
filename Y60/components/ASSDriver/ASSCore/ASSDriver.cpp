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

#include <y60/ImageBuilder.h>
#include <y60/PixelEncoding.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/JSScriptablePlugin.h>

#include <iostream>

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

const unsigned char myMagicToken( 255 ); 

static const char * RAW_RASTER = "ASSRawRaster";
static const char * FILTERED_RASTER = "ASSFilteredRaster";
static const char * MOMENT_RASTER = "ASSMomentRaster";

const std::string OK_STRING("OK");
const std::string ERROR_STRING("ERR");
const double COMMAND_TIMEOUT( 0.2 );

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
    cerr << "buffer: '";
    for (unsigned i = 0; i < theBuffer.size(); ++i) {
        cerr << theBuffer[i];
    }
    cerr << "'" << endl;
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
    _myLineStart( -1 ),
    _myLineEnd( -1 ),
    _myMaxLine( 0 ),
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
    _myDumpValuesFlag( 0 ),
    _myLastCommandTime( asl::Time() )
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
    _myScene = theWindow->getCurrentScene();
    // TODO: reallocate rasters
    return true;
}

void
ASSDriver::onFrame(jslib::AbstractRenderWindow * theWindow, double theTime) {
}

void
ASSDriver::synchronize() {
    // first look for the beginning of a line
    if (_myLineStart < 0) {
        for (unsigned i = 0; i < _myFrameBuffer.size(); ++i) {
            if (_myMagicTokenFlag) {
                _myMaxLine = _myFrameBuffer[i];
                _myMagicTokenFlag = false;
                _myLineStart = i;
                //AC_PRINT << "Got line start";
                break;
            } else if (_myFrameBuffer[i] == myMagicToken) {
                _myMagicTokenFlag = true;
            }
        }
    }
    // then find the end of that line and determine the number of columns
    if (_myLineStart >= 0 && _myLineEnd < 0) {
        for (unsigned i = _myLineStart + 1; i < _myFrameBuffer.size(); ++i) {
            if (_myFrameBuffer[i] == myMagicToken) {
                _myLineEnd = i;
                _myGridSize[0] = _myLineEnd - _myLineStart - 1;
                //AC_PRINT << "Got line end";
                break;
            }
        }
    }

    // now scan for the maximum line number to determine the number of rows
    if (_myGridSize[0] > 0 && _myGridSize[1] == 0) {
        ASSURE( _myFrameBuffer[ _myLineEnd ] == myMagicToken );
        for (unsigned i = _myLineEnd; i < _myFrameBuffer.size(); i += _myGridSize[0] + 2) {
            ASSURE( _myFrameBuffer[i] == myMagicToken );
            if (_myFrameBuffer[i + 1] == 1 && _myMaxLine > 0) {
                _myGridSize[1] = _myMaxLine;
                _myPoTSize[0] = nextPowerOfTwo( _myGridSize[0] );
                _myPoTSize[1] = nextPowerOfTwo( _myGridSize[1] );
                AC_PRINT << "Grid size: " << _myGridSize;
                _myFrameBuffer.erase( _myFrameBuffer.begin(), _myFrameBuffer.begin() + i);
                ASSURE(_myFrameBuffer.front() == myMagicToken);
                allocateGridBuffers();
                _myReceiveBuffer.resize( getBytesPerFrame() );
                createTransportLayerEvent( _myIDCounter ++, "configure" );
                setState( RUNNING );
                return;
            } else {
                _myMaxLine = _myFrameBuffer[i + 1];
            }
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

void
ASSDriver::readSensorValues() {
    //AC_PRINT << "====";
    //AC_PRINT << "pre buf size: " << _myFrameBuffer.size();
    bool myNewDataFlag = false;
    while ( _myGridSize[0] + 2 < _myFrameBuffer.size() ) {
        if ( _myFrameBuffer[0] != myMagicToken ) {
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


        /*
        int myPixel = _myGridSize[0] * (_myGridSize[1] / 2) + (_myGridSize[0] / 2);
        AC_PRINT << "value: " << int(*(_myRawRaster.raster->pixels().begin() +
                    myPixel));
        */

        _myFrameBuffer.erase( _myFrameBuffer.begin(), _myFrameBuffer.begin() + _myGridSize[0] + 2);
        if ( myRowIdx == _myGridSize[1] - 1) {

            asl::Time myTime;
            double myDeltaT = myTime - _myLastFrameTime;
            //AC_PRINT << "dt: " <<  myDeltaT;
            // XXX
            //cout << "deltaT " << myDeltaT << endl;
            myDeltaT = 1.0 / 25.0;

            _myRunTime += myDeltaT;
            //cout << _myRunTime << "\t" << myDeltaT << endl; // XXX

            //AC_PRINT << "Got Frame (dt = " << myDeltaT << ")";
            _myLastFrameTime = myTime;

            processSensorValues( myDeltaT );
        }
        myNewDataFlag = true;
    }

    if (myNewDataFlag && _myScene) {
        _myScene->getSceneDom()->getElementById(RAW_RASTER)->getFacade<y60::Image>()->triggerUpload();
        
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
                applyTransform(theCursorIt->second.position, getTransormationMatrix() ),
                theCursorIt->second.roi );
    }
   
    theCursorIt->second.intensityHistory.push_back( theCursorIt->second.intensity );
    if( theCursorIt->second.intensityHistory.size() > MAX_HISTORY_LENGTH ) {
            theCursorIt->second.intensityHistory.pop_front();
        }

    if (_myDumpValuesFlag) {
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
ASSDriver::getTransormationMatrix() {
    typedef TransformHierarchyFacade THF;
    Matrix4f myTransform;
    if (_myTransform) {
        myTransform = _myTransform->getFacade<THF>()->get<GlobalMatrixTag>();
    } else {
        myTransform.makeIdentity();
    }
    return myTransform;
}

void 
ASSDriver::correlatePositions( const std::vector<MomentResults> & theCurrentPositions,
                               const BlobListPtr theROIs)
{

    Matrix4f myTransform = getTransormationMatrix();

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
            _myLineStart = -1;
            _myLineEnd = -1;
            _myMaxLine = 0;
            _myMagicTokenFlag = false;
            _myGridSize = Vector2i(0, 0);
            _myFrameBuffer.clear();
            break;
        case RUNNING:
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
ASSDriver::onPostRender(jslib::AbstractRenderWindow * theWindow) {
}

void
ASSDriver::sendCommand( const std::string & theCommand ) {
    try {
        if (_mySerialPort) {
            //AC_PRINT << "Sending command: '" << theCommand << "'";
            std::string myCommand( theCommand );
            myCommand.append("\r");
            _mySerialPort->write( myCommand.c_str(), myCommand.size() );
            _myLastCommandTime = asl::Time();
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
    if (thePropertyName == "maxOccuringValue") {
        theReturnValue.set( myMagicToken );
        return;
    }
    if (thePropertyName == "transform") {
        theReturnValue.set( _myTransform );
        return;
    }
    if (thePropertyName == "rasterNames") {
        theReturnValue.set( _myRasterNames );
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
    if (thePropertyName == "maxOccuringValue") {
        AC_ERROR << "maxOccuringValue property is read only";
        return;
    }
    if (thePropertyName == "transform") {
        _myTransform = thePropertyValue.get<dom::NodePtr>();
        return;
    }
    if (thePropertyName == "rasterNames") {
        AC_ERROR << "rasterNames property is read only";
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
    getConfigSetting( mySettings, "DumpValues", _myDumpValuesFlag, 0 );
    getConfigSetting( mySettings, "MinTouchInterval", _myMinTouchInterval, 0.25 );

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
        size_t myByteCount = _myReceiveBuffer.size();

        // [DS] If the serial port is removed a non-blocking read returns EAGAIN
        // for some reason. Peek throws an exception which is just what we want.
        _mySerialPort->peek();
        
        //AC_PRINT << "bytes: " << myByteCount;
        _mySerialPort->read( reinterpret_cast<char*>(& ( * _myReceiveBuffer.begin())),
                myByteCount );

        //AC_PRINT << "=== Got " << myByteCount << " bytes.";

        _myFrameBuffer.insert( _myFrameBuffer.end(),
                _myReceiveBuffer.begin(), _myReceiveBuffer.begin() + myByteCount );
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
            if ( _myCommandQueue.empty() ) {
                setState( SYNCHRONIZING );
            } else {
                setState( CONFIGURING );
            }
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
    return (1 + 1 + _myGridSize[0]) * _myGridSize[1];
}

void
ASSDriver::handleConfigurationCommand() {
    //dumpBuffer( _myFrameBuffer );
    CommandResponse myResponse = getCommandResponse();
    switch (_myConfigureState) {
        case SEND_CONFIG_COMMANDS:
            //AC_PRINT << "SEND_CONFIG_COMMANDS";
            if (myResponse == RESPONSE_OK) {
                sendCommand( _myCommandQueue.front() );
                _myCommandQueue.pop_front();
                if (_myCommandQueue.empty()) {
                    _myConfigureState = WAIT_FOR_RESPONSE;
                }
            } else if (myResponse == RESPONSE_ERROR || myResponse == RESPONSE_TIMEOUT) {
                AC_ERROR << "Failed to enter config mode.";
                _myCommandQueue.clear();
                setState( SYNCHRONIZING );
            }
            break;
        case WAIT_FOR_RESPONSE:
            //AC_PRINT << "WAIT_FOR_RESPONSE";
            if (myResponse == RESPONSE_OK) {
                sendCommand( CMD_LEAVE_CONFIG_MODE );
                _myConfigureState = WAIT_FOR_EXIT;
            } else if (myResponse == RESPONSE_ERROR || myResponse == RESPONSE_TIMEOUT) {
                AC_ERROR << "Failed to configure sensor hardware.";
                setState( SYNCHRONIZING );
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
    string::size_type myPos = myString.find(OK_STRING);
    if (myPos != string::npos && myPos + OK_STRING.size() + 2 <= myString.size()) {
        string::size_type myEnd = minimum( myPos + OK_STRING.size() + 2, _myFrameBuffer.size());
        _myFrameBuffer.erase( _myFrameBuffer.begin() , _myFrameBuffer.begin() + myEnd );
        return RESPONSE_OK;
    }
    myPos = myString.find(ERROR_STRING);
    if (myPos != string::npos  && myPos + ERROR_STRING.size() + 2 + 2 <= myString.size()) {
        unsigned myErrorCode = as<unsigned>( myString.substr( myPos + ERROR_STRING.size(), 2 ));
        AC_ERROR << "Errorcode: " << myErrorCode;
        string::size_type myEnd = minimum( myPos + ERROR_STRING.size() + 2 + 2, _myFrameBuffer.size());
        _myFrameBuffer.erase( _myFrameBuffer.begin() , _myFrameBuffer.begin() + myEnd );
        return RESPONSE_ERROR;
    }

    if ( asl::Time() - _myLastCommandTime > COMMAND_TIMEOUT) {
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
ASSDriver::queueCommand( const char * theCommand ) {
    _myCommandQueue.push_back( string( theCommand ));
    if (_myState != CONFIGURING && _myState != NO_SERIAL_PORT) {
        setState( CONFIGURING );
    }
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


JSFunctionSpec * 
ASSDriver::Functions() {
    static JSFunctionSpec myFunctions[] = {
        {"performTara", PerformTara, 0},
        {"callibrateTransmissionLevels", CallibrateTransmissionLevels, 0},
        {0}
    };
    return myFunctions;
}

} // end of namespace y60

