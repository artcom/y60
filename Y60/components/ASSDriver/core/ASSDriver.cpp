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

#include <iostream>

#ifdef LINUX
#   include <values.h>
#else
#   include <float.h>
#endif


using namespace asl;
using namespace y60;

namespace y60 {

const unsigned char myMagicToken( 255 ); 

static const char * RAW_RASTER = "ASSRawRaster";
static const char * FILTERED_RASTER = "ASSFilteredRaster";

static const char * DriverStateStrings[] = {
    "synchronizing",
    "running",
    ""
};

#ifdef OSX
#undef verify
#endif


IMPLEMENT_ENUM( DriverState, DriverStateStrings );


ASSDriver::ASSDriver(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    IRendererExtension("ASSDriver"),
    _myGridSize(0,0),
    _mySyncLostCounter(0),
    _myRawRaster(dom::ValuePtr(0)),
    _myDenoisedRaster(dom::ValuePtr(0)),
    _myScene(0),
    _myNoiseThreshold( 15 ),
    _myComponentThreshold( 5 ),
    _myPower(2),
    _myIDCounter( 0 )/*,
    _myEventSchema( new dom::Document( oureventxsd ) ),
    _myValueFactory( new dom::ValueFactory() )*/

{
    // XXX [DS] make things configurable
    //_mySerialPort = getSerialDevice(0);
    _mySerialPort = getSerialDeviceByName("/dev/ttyUSB0");
    _mySerialPort->open( 57600, 8, SerialDevice::NO_PARITY, 1, false);
    //_mySerialPort->open( 921600, 8, SerialDevice::NO_PARITY, 1, false);
    setState(SYNCHRONIZING);

}

ASSDriver::~ASSDriver() {
    AC_PRINT << "ASSDriver stats:";
    AC_PRINT << "    sync errors: " << _mySyncLostCounter;
}

bool
ASSDriver::onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    _myScene = theWindow->getCurrentScene();
    return true;
}

void
ASSDriver::onFrame(jslib::AbstractRenderWindow * theWindow, double theTime) {
}

void
ASSDriver::synchronize() {
    // XXX not reentrant safe ... even two ASSDrivers cause problems
    // TODO: refactor
    static int myLineStart = -1;
    static int myLineEnd   = -1;
    static int myMaxLine = 0;
    static bool myMagicTokenFlag = false;
    // first look for the beginning of a line
    if (myLineStart < 0) {
        for (unsigned i = 0; i < _myBuffer.size(); ++i) {
            if (myMagicTokenFlag) {
                myMaxLine = _myBuffer[i];
                myMagicTokenFlag = false;
                myLineStart = i;
                AC_PRINT << "Got line start";
                break;
            } else if (_myBuffer[i] == myMagicToken) {
                myMagicTokenFlag = true;
            }
        }
    }
    // then find the end of that line and determine the number of columns
    if (myLineStart >= 0 && myLineEnd < 0) {
        for (unsigned i = myLineStart + 1; i < _myBuffer.size(); ++i) {
            if (_myBuffer[i] == myMagicToken) {
                myLineEnd = i;
                _myGridSize[0] = myLineEnd - myLineStart - 1;
                AC_PRINT << "Got line end";
                break;
            }
        }
    }

    // now scan for the maximum line number to determine the number of rows
    if (_myGridSize[0] > 0 && _myGridSize[1] == 0) {
        ASSURE( _myBuffer[ myLineEnd ] == myMagicToken );
        for (unsigned i = myLineEnd; i < _myBuffer.size(); i += _myGridSize[0] + 2) {
            ASSURE( _myBuffer[i] == myMagicToken );
            if (_myBuffer[i + 1] == 1 && myMaxLine > 0) {
                _myGridSize[1] = myMaxLine;
                AC_PRINT << "Grid size: " << _myGridSize;
                _myBuffer.erase( _myBuffer.begin(), _myBuffer.begin() + i);
                ASSURE(_myBuffer.front() == myMagicToken);
                // reset the statics so we could resynchronize ... at least in theory
                myLineStart = -1;
                myLineEnd = -1;
                myMaxLine = 0;
                myMagicTokenFlag = false;
                allocateGridBuffers();
                setState( RUNNING );
                createSyncEvent( _myIDCounter ++ );
                return;
            } else {
                myMaxLine = _myBuffer[i + 1];
            }
        }
    }
}

void
ASSDriver::allocateGridBuffers() {
    _myRawRaster = allocateRaster(RAW_RASTER);
    _myDenoisedRaster = allocateRaster(FILTERED_RASTER);
}

RasterHandle
ASSDriver::allocateRaster(const std::string & theName) {
    if (_myScene) {
        dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(theName);
        if (myImage) {
            myImage->getFacade<y60::Image>()->createRaster( 
                    _myGridSize[0], _myGridSize[1], 1, y60::GRAY);
            return RasterHandle( myImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());
        } else {
            ImageBuilder myImageBuilder(theName, false);
            _myScene->getSceneBuilder()->appendImage(myImageBuilder);
            y60::ImagePtr myImage = myImageBuilder.getNode()->getFacade<y60::Image>();
            myImage->set<y60::IdTag>( theName );
            myImage->createRaster( _myGridSize[0], _myGridSize[1], 1, y60::GRAY);
            return RasterHandle( myImage->getNode().childNode(0)->childNode(0)->nodeValueWrapperPtr());
        }
    } else {
        RasterHandle myHandle(createRasterValue(y60::GRAY, _myGridSize[0], _myGridSize[1]));
        return myHandle;
                
    }
}

void
ASSDriver::readSensorValues() {
    //AC_PRINT << "====";
    //AC_PRINT << "pre buf size: " << _myBuffer.size();
    bool myNewDataFlag = false;
    while ( _myGridSize[0] + 2 < _myBuffer.size() ) {
        if ( _myBuffer[0] != myMagicToken ) {
            AC_PRINT << "Sync error.";
            _mySyncLostCounter++;
            setState( SYNCHRONIZING );
            return;
        }
        int myRowIdx = _myBuffer[1] - 1;
        //AC_PRINT << "Got row: " << myRowIdx;

        unsigned char * myRowPtr = _myRawRaster.raster->pixels().begin() +
                    myRowIdx * _myGridSize[0];
        std::copy(_myBuffer.begin() + 2, _myBuffer.begin() + 2 + _myGridSize[0], myRowPtr);
        //*(_myRawRaster.raster->pixels().begin() + (_myGridSize[1] / 2) * _myGridSize[0] + _myGridSize[0] / 2) = 254;

        _myBuffer.erase( _myBuffer.begin(), _myBuffer.begin() + _myGridSize[0] + 2);
        if ( myRowIdx == _myGridSize[1] - 1) {
            asl::Time myTime;
            double myDeltaT = myTime - _myLastFrameTime;
            //AC_PRINT << "Got Frame (dt = " << myDeltaT << ")";
            _myLastFrameTime = myTime;

            processSensorValues( myDeltaT );
        }
        myNewDataFlag = true;
    }

    if (myNewDataFlag && _myScene) {
        _myScene->getSceneDom()->getElementById(RAW_RASTER)->getFacade<y60::Image>()->triggerUpload();
        
    }
    //AC_PRINT << "post buf size: " << _myBuffer.size();
}

template <class PixelT>
struct ThresholdWithPow {
    ThresholdWithPow(const PixelT & theThreshold, int thePower) :
        _myThreshold( theThreshold ),
        _myPower( thePower) {}
    PixelT operator () (const PixelT & a) {
        if (a.get() < _myThreshold.get()) {
            return PixelT(0);
        } else {
            return PixelT( (unsigned char)(pow(a.get(), _myPower) /  pow(255, _myPower - 1)));
        }
    }

    PixelT _myThreshold;
    float _myPower;
};

template <class RASTER>
struct AnalyseMoment {
    void operator() (const RASTER & theRaster) {
        analyseMoments( theRaster, result );
    }
    MomentResults result;
};

void
ASSDriver::createThresholdedRaster(RasterHandle & theInput,
                                   RasterHandle & theOutput,
                                   const unsigned char theThreshold)
{
    const y60::RasterOfGRAY * myInput =
            dom::dynamic_cast_Value<y60::RasterOfGRAY>( & * (theInput.value) );
    y60::RasterOfGRAY * myOutput =
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (theOutput.value) );
    std::transform( myInput->begin(), myInput->end(),
            myOutput->begin(),
            ThresholdWithPow<asl::gray<unsigned char> >( theThreshold, _myPower ));
    dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (theOutput.value) );
}

void
ASSDriver::processSensorValues( double theDeltaT ) {
    createThresholdedRaster( _myRawRaster, _myDenoisedRaster, _myNoiseThreshold );

    BlobListPtr myROIs = connectedComponents( _myDenoisedRaster.raster, _myComponentThreshold);

    std::vector<Vector2f> myPreviousPositions( _myPositions );

    computeCursorPositions( myROIs );
    correlatePositions( myPreviousPositions );

}

void
ASSDriver::computeCursorPositions( const BlobListPtr & theROIs) {
    _myPositions.clear();
    _myRegions.clear();

    typedef asl::subraster<gray<unsigned char> > SubRaster;
    for (BlobList::const_iterator it = theROIs->begin(); it != theROIs->end(); ++it) {
        Box2i myBox = (*it)->bbox();
        myBox[Box2i::MAX] += Vector2i(1, 1);
        AnalyseMoment<SubRaster> myMomentAnalysis;
        _myDenoisedRaster.raster->apply( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1],
                                    myBox[Box2i::MAX][0], myBox[Box2i::MAX][1], myMomentAnalysis);

        Vector2f myPosition = Vector2f( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1])
                + myMomentAnalysis.result.center;

        //AC_PRINT << "center: " << myPosition;
        _myPositions.push_back( myPosition );
        _myRegions.push_back( Box2f( Vector2f( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1]),
                                     Vector2f( myBox[Box2i::MAX][0], myBox[Box2i::MAX][1])));
    }

}

void 
ASSDriver::correlatePositions( const std::vector<asl::Vector2f> & thePreviousPositions )
{

    typedef TransformHierarchyFacade THF;
    Matrix4f myTransform;
    if (_myTransform) {
        myTransform = _myTransform->getFacade<THF>()->get<GlobalMatrixTag>();
    } else {
        myTransform.makeIdentity();
    }


    std::vector<bool> myCorrelationFlags(thePreviousPositions.size(), false);
    std::vector<Unsigned64> myOldIDs( _myCursorIDs );
    _myCursorIDs.clear();
    for (unsigned i = 0; i < _myPositions.size(); ++i) {
        float myMinDistance = FLT_MAX;
        int myMinDistIdx  = -1;
        for (unsigned j = 0; j < thePreviousPositions.size(); ++j) {
            if ( ! myCorrelationFlags[j] ) {
                float myDistance = magnitude( _myPositions[i] - thePreviousPositions[j]);
                if (myDistance < myMinDistance) {
                    myMinDistance = myDistance;
                    myMinDistIdx = j;
                }
            }
        }
        const float DISTANCE_THRESHOLD = 2.0;
        if (myMinDistIdx >= 0 && myMinDistance < DISTANCE_THRESHOLD) {
            // cursor moved: copy label from previous analysis
            myCorrelationFlags[myMinDistIdx] = true;

            Unsigned64 myID( myOldIDs[ myMinDistIdx ] );
            _myCursorIDs.push_back( myID );
            createEvent( myID, "move", _myPositions[i],
                    applyTransform( _myPositions[i], myTransform) );
        } else {
            // new cursor: get new label
            //AC_PRINT << "=== new cursor ===";
            Unsigned64 myNewID( _myIDCounter++ );
            _myCursorIDs.push_back( myNewID );
            createEvent( myNewID, "add", _myPositions[i],
                    applyTransform( _myPositions[i], myTransform ));
        }
    }

    for (unsigned i = 0; i < thePreviousPositions.size(); ++i) {
        if ( ! myCorrelationFlags[i]) {
            //AC_PRINT << "=== cursor " << i << " removed ===";

            Unsigned64 myID( myOldIDs[ i ] );
            createEvent( myID, "remove", thePreviousPositions[i],
                    applyTransform( thePreviousPositions[i], myTransform ));
        }
    }
}

void
ASSDriver::setState( DriverState theState ) {
    _myState = theState;
    switch ( _myState ) {
        case SYNCHRONIZING:
            _myGridSize = Vector2i(0, 0);
            _myBuffer.clear();
            break;
        case RUNNING:
            break;
        default:
            break;
    }
    AC_PRINT << theState << " ...";
}

void
ASSDriver::onPostRender(jslib::AbstractRenderWindow * theWindow) {
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
    if (thePropertyName == "componentThreshold") {
        theReturnValue.set( _myComponentThreshold );
        return;
    }
    if (thePropertyName == "noiseThreshold") {
        theReturnValue.set( _myNoiseThreshold );
        return;
    }
    if (thePropertyName == "positions") {
        theReturnValue.set( _myPositions );
        return;
    }
    if (thePropertyName == "regions") {
        theReturnValue.set( _myRegions );
        return;
    }
    if (thePropertyName == "gainPower") {
        theReturnValue.set( _myPower );
        return;
    }
    if (thePropertyName == "transform") {
        theReturnValue.set( _myTransform );
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
    if (thePropertyName == "componentThreshold") {
        _myComponentThreshold = thePropertyValue.get<unsigned char>();
        return;
    }
    if (thePropertyName == "noiseThreshold") {
        _myNoiseThreshold = thePropertyValue.get<unsigned char>();
        return;
    }
    if (thePropertyName == "gainPower") {
        _myPower = thePropertyValue.get<int>();
        return;
    }
    if (thePropertyName == "positions") {
        AC_ERROR << "positions property is read only";
        return;
    }
    if (thePropertyName == "regions") {
        AC_ERROR << "regions property is read only";
        return;
    }
    if (thePropertyName == "transform") {
        _myTransform = thePropertyValue.get<dom::NodePtr>();
        return;
    }
    AC_ERROR << "Unknown property '" << thePropertyName << "'";
}

void
ASSDriver::onUpdateSettings(dom::NodePtr theSettings) {
    AC_PRINT << "TODO: onUpdateSettings";
}

Vector3f 
ASSDriver::applyTransform( const Vector2f & theRawPosition,
                           const asl::Matrix4f & theTransform )
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
    std::vector<unsigned char> myBuffer;
    size_t myByteCount = _mySerialPort->peek();
    myBuffer.resize( myByteCount );
    _mySerialPort->read( reinterpret_cast<char*>(& ( * myBuffer.begin())), myByteCount );

    _myBuffer.insert( _myBuffer.end(), myBuffer.begin(), myBuffer.end() );

    AC_PRINT << "Got " << myByteCount << " bytes.";

    switch (_myState) {
        case SYNCHRONIZING:
            synchronize();
            break;
        case RUNNING:
            readSensorValues();
            break;
    }
}

} // end of namespace y60

