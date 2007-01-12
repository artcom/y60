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

#include <y60/PixelEncoding.h>

#include <iostream>

using namespace asl;
using namespace y60;

const unsigned char myMagicToken( 200 ); // all values above 200 are currently reserved
                                         // this will be changed to one escape character
                                         // (255) in a future version

static const char * DriverStateStrings[] = {
    "synchronizing",
    "running",
    ""
};

IMPLEMENT_ENUM( DriverState, DriverStateStrings );


ASSDriver::ASSDriver(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    IRendererExtension("ASSDriver"),
    _myGridSize(0,0),
    _mySyncLostCounter(0),
    _myRawRaster(0)
{
    _mySerialPort = getSerialDevice(0);
    _mySerialPort->open( 57600, 8, SerialDevice::NO_PARITY, 1, false);
    setState(SYNCHRONIZING);
}

ASSDriver::~ASSDriver() {
    AC_PRINT << "ASSDriver stats:";
    AC_PRINT << "    sync errors: " << _mySyncLostCounter;
}

bool
ASSDriver::onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    return true;
}

void
ASSDriver::onFrame(jslib::AbstractRenderWindow * theWindow, double theTime) {
}

void
ASSDriver::synchronize() {
    static int myLineStart = -1;
    static int myLineEnd   = -1;
    static int myMaxLine = 0;
    //AC_PRINT << "buf size: " << _myBuffer.size();
    // first look for the beginning of a line
    if (myLineStart < 0) {
        for (unsigned i = 0; i < _myBuffer.size(); ++i) {
            if (_myBuffer[i] > myMagicToken) {
                myLineStart = i;
                myMaxLine = _myBuffer[i] - myMagicToken;
                break;
            }
        }
    }
    // then find the end of that line and determine the number of columns
    if (myLineStart >= 0 && myLineEnd < 0) {
        for (unsigned i = myLineStart + 1; i < _myBuffer.size(); ++i) {
            if (_myBuffer[i] > myMagicToken) {
                myLineEnd = i;
                _myGridSize[0] = myLineEnd - myLineStart - 1;
                if ( myMaxLine < _myBuffer[i] - myMagicToken ) {
                    myMaxLine = _myBuffer[i] - myMagicToken;
                }
                break;
            }
        }
    }
    // now scan for the maximum line number to determine the number of rows
    if (_myGridSize[0] > 0 && _myGridSize[1] == 0) {
        for (unsigned i = myLineEnd; i < _myBuffer.size(); i += _myGridSize[0] + 1) {
            ASSURE( myMagicToken < _myBuffer[i]);
            if (_myBuffer[i] - myMagicToken < myMaxLine) {
                _myGridSize[1] = myMaxLine;
                AC_PRINT << "Grid size: " << _myGridSize;
                _myBuffer.erase( _myBuffer.begin(), _myBuffer.begin() + i);
                ASSURE(_myBuffer.front() > myMagicToken);
                // reset the statics so we could resynchronize ... at least in theory
                myLineStart = -1;
                myLineEnd = -1;
                myMaxLine = 0;
                allocateGridBuffers();
                setState( RUNNING );
                return;
            } else {
                myMaxLine = _myBuffer[i] - myMagicToken;
            }
            myLineEnd = i;
        }
    }
}

void
ASSDriver::allocateGridBuffers() {
    _myRawRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(
                createRasterValue(y60::GRAY, _myGridSize[0], _myGridSize[1]));
}

void
ASSDriver::readSensorValues() {
    //AC_PRINT << "====";
    //AC_PRINT << "pre buf size: " << _myBuffer.size();
    while ( _myGridSize[0] < _myBuffer.size() ) {
        if ( _myBuffer[0] < myMagicToken ) {
            AC_PRINT << "Sync error.";
            _mySyncLostCounter++;
            setState( SYNCHRONIZING );
            return;
        }
        int myRowIdx = _myBuffer[0] - myMagicToken - 1;
        //AC_PRINT << "Got row: " << myRowIdx;

        // TODO: copy data
        unsigned char * myRowPtr = _myRawRaster->pixels().begin() +
                    myRowIdx * _myGridSize[0];
        std::copy(_myBuffer.begin(), _myBuffer.begin() + _myGridSize[0], myRowPtr);

        _myBuffer.erase( _myBuffer.begin(), _myBuffer.begin() + _myGridSize[0] + 1);
        if ( myRowIdx == _myGridSize[1] - 1) {
            asl::Time myTime;
            double myDeltaT = myTime - _myLastFrameTime;
            AC_PRINT << "Got Frame (dt = " << myDeltaT << ")";
            _myLastFrameTime = myTime;
        }
    }
    //AC_PRINT << "post buf size: " << _myBuffer.size();
}

void
ASSDriver::setState( DriverState theState ) {
    _myState = theState;
    switch ( _myState ) {
        case SYNCHRONIZING:
            _myGridSize = Vector2i(0, 0);
            _myBuffer.clear();
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
    /*
    if (thePropertyName == "rawRaster") {
        theReturnValue.set( _myRawRaster );
        return;
    }
    AC_ERROR << "Unknown property '" << thePropertyName << "'";
*/
}

void
ASSDriver::onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue)
{
    /*
    if (thePropertyName == "rawRaster") {
        AC_ERROR << "rawRaster property is read only";
        return;
    }
    AC_ERROR << "Unknown property '" << thePropertyName << "'";
*/
}

void
ASSDriver::onUpdateSettings(dom::NodePtr theSettings) {
}

y60::EventPtrList 
ASSDriver::poll() {
    std::vector<unsigned char> myBuffer;
    unsigned myByteCount = _mySerialPort->peek();
    myBuffer.resize( myByteCount );
    _mySerialPort->read( reinterpret_cast<char*>(& ( * myBuffer.begin())), myByteCount );

    _myBuffer.insert( _myBuffer.end(), myBuffer.begin(), myBuffer.end() );

    switch (_myState) {
        case SYNCHRONIZING:
            synchronize();
            break;
        case RUNNING:
            readSensorValues();
            break;
    }
    return y60::EventPtrList();
}

/*
void 
ASSDriver::initClasses(JSContext * theContext, JSObject *theGlobalObject) {
}
*/

extern "C"
EXPORT asl::PlugInBase* ASSDriver_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new ASSDriver(myDLHandle);
}



