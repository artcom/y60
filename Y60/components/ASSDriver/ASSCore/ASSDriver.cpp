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


using namespace std;
using namespace asl;
using namespace y60;

static const char * DriverStateStrings[] = {
    "no_serial_port",
    "synchronizing",
    "running",
    ""
};

IMPLEMENT_ENUM( y60::DriverState, DriverStateStrings );

namespace y60 {

const unsigned char myMagicToken( 255 ); 

static const char * RAW_RASTER = "ASSRawRaster";
static const char * FILTERED_RASTER = "ASSFilteredRaster";

#ifdef OSX
#undef verify
#endif

ASSDriver::ASSDriver() :
    IRendererExtension("ASSDriver"),
    _myGridSize(0,0),
    _mySyncLostCounter(0),
    _myRawRaster(dom::ValuePtr(0)),
    _myDenoisedRaster(dom::ValuePtr(0)),
    _myScene(0),
    _myNoiseThreshold( 15 ),
    _myComponentThreshold( 5 ),
    _myGainPower(2.0f),
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
    _myReceiveBuffer(256)
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
                //AC_PRINT << "Grid size: " << _myGridSize;
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
}

RasterHandle
ASSDriver::allocateRaster(const std::string & theName) {
    if (_myScene) {
        dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(theName);
        if (myImage) {
            myImage->getFacade<y60::Image>()->createRaster( 
                    _myGridSize[0], _myGridSize[1], 1, y60::GRAY);
            _myRasterNames.push_back( theName );
            return RasterHandle( myImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());
        } else {
            ImageBuilder myImageBuilder(theName, false);
            _myScene->getSceneBuilder()->appendImage(myImageBuilder);
            y60::ImagePtr myImage = myImageBuilder.getNode()->getFacade<y60::Image>();
            myImage->set<y60::IdTag>( theName );
            myImage->set<y60::NameTag>( theName );
            myImage->createRaster( _myGridSize[0], _myGridSize[1], 1, y60::GRAY);

            _myRasterNames.push_back( theName );

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

        unsigned char * myRowPtr = _myRawRaster.raster->pixels().begin() +
                    myRowIdx * _myGridSize[0];
        std::copy(_myFrameBuffer.begin() + 2, _myFrameBuffer.begin() + 2 + _myGridSize[0], myRowPtr);

        _myFrameBuffer.erase( _myFrameBuffer.begin(), _myFrameBuffer.begin() + _myGridSize[0] + 2);
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
    //AC_PRINT << "post buf size: " << _myFrameBuffer.size();
}

template <class PixelT>
struct ThresholdWithPow {
    ThresholdWithPow(const PixelT & theThreshold, float thePower) :
        _myThreshold( theThreshold ),
        _myGainPower( thePower) {}
    PixelT operator () (const PixelT & a) {
        if (a.get() < _myThreshold.get()) {
            return PixelT(0);
        } else {
            return PixelT( (unsigned char)(pow(float(a.get()), _myGainPower) /  pow(255.0f, _myGainPower - 1)));
        }
    }

    PixelT _myThreshold;
    float _myGainPower;
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
            ThresholdWithPow<asl::gray<unsigned char> >( theThreshold, _myGainPower ));
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
    std::vector<int> myOldIDs( _myCursorIDs );
    _myCursorIDs.clear();
    //AC_PRINT << "positions: " << _myPositions.size();
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

            int myID( myOldIDs[ myMinDistIdx ] );
            _myCursorIDs.push_back( myID );
            createEvent( myID, "move", _myPositions[i],
                    applyTransform( _myPositions[i], myTransform),
                    _myRegions[i] );
        } else {
            // new cursor: get new label
            //AC_PRINT << "=== new cursor ===";
            int myNewID( _myIDCounter++ );
            _myCursorIDs.push_back( myNewID );
            createEvent( myNewID, "add", _myPositions[i],
                    applyTransform( _myPositions[i], myTransform),
                    _myRegions[i]);
        }
    }

    for (unsigned i = 0; i < thePreviousPositions.size(); ++i) {
        if ( ! myCorrelationFlags[i]) {
            //AC_PRINT << "=== cursor " << i << " removed ===";

            int myID( myOldIDs[ i ] );
            createEvent( myID, "remove", thePreviousPositions[i],
                    applyTransform( thePreviousPositions[i], myTransform ),
                    _myRegions[i]);
        }
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
        default:
            break;
    }
    //AC_PRINT << "ASSDriver::setState() " << theState << " ...";
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
    if (thePropertyName == "positions") {
        theReturnValue.set( _myPositions );
        return;
    }
    if (thePropertyName == "regions") {
        theReturnValue.set( _myRegions );
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
    }
}

void
ASSDriver::readDataFromPort() {
    try {
        size_t myByteCount = _myReceiveBuffer.size();

        // [DS] If the serial port is removed a non-blocking read returns EAGAIN
        // for some reason. Peek throws an exception which is just what we want.
        _mySerialPort->peek();
        
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
    return (1 + 1 + _myGridSize[0]) * _myGridSize[1];
}

} // end of namespace y60

