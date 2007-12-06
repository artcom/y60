//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "DummyTransport.h"

using namespace y60;
using namespace std;

#define DB(x) //x

DummyTransport::DummyTransport( const dom::NodePtr & theSettings ) :
    TransportLayer( "dummy", theSettings),
    _myFrameCount(0),
    _myGridWidth(15),
    _myGridHeight(5),
    _myGridSpacing(100),
    _myGenCheckSum(0),
    _myDimFactor(248)
{
    DB(AC_TRACE << "DummyTransport()";)
}



DummyTransport::~DummyTransport() {
    DB(AC_TRACE << "~DummyTransport()";)
    stopThread();
}


bool DummyTransport::settingsChanged( dom::NodePtr theSettings) {
    DB(AC_TRACE << "DummyTransport::settingsChanged()";)
    bool myChangedFlag = false;

    myChangedFlag |= settingChanged( theSettings, "TransportLayer", _myTransportName );

    return myChangedFlag;
}



void DummyTransport::establishConnection() {
    DB(AC_TRACE << "DummyTransport::establishConnection()";)
    setState(SYNCHRONIZING);
}



void DummyTransport::readData() {
    DB(AC_TRACE << "DummyTransport::readData()";)
    
    buildStatusLine();
    generateGridValues(); 
    calculateChecksum(); // save the new checksum for the next frame
    ++_myFrameCount;

    _myTmpBuffer.push_back(255);
    _myTmpBuffer.push_back(0);

    _myTmpBuffer.insert( _myTmpBuffer.end(), _myStatusLine.begin(), _myStatusLine.end() );
    
    for (unsigned i = 1; i <= _myGridHeight; i++) {
        _myTmpBuffer.push_back(255);
        _myTmpBuffer.push_back(i);

        _myTmpBuffer.insert( _myTmpBuffer.end(), _myGridValues.begin() + (i-1)*_myGridWidth, 
                             _myGridValues.begin() + i*_myGridWidth );
    }
    

}


void DummyTransport::writeData(const char * theData, size_t theSize) {
    DB(AC_TRACE << "DummyTransport::writeData()";)
}


void DummyTransport::closeConnection() {
    DB(AC_TRACE << "DummyTransport::closeConnection()";)
}


void DummyTransport::buildStatusLine() {
    DB(AC_TRACE << "DummyTransport::buildStatusLine()";)
    
    _myStatusLine.clear();
    char myStatusHeader[] = "V0105S0000I0000M0002F1000";
    unsigned myHeaderSize = strlen(myStatusHeader);

    for (unsigned i = 0; i < myHeaderSize; i++) {
        _myStatusLine.push_back(myStatusHeader[i]);
    }

    writeStatusToken( 'W', _myGridWidth );
    writeStatusToken( 'H', _myGridHeight );
    writeStatusToken( 'G', _myGridSpacing );
    writeStatusToken( 'N', _myFrameCount );
    writeStatusToken( 'C', _myGenCheckSum ); 
}


void DummyTransport::writeStatusToken( const char theToken, unsigned theNumber ) {
    DB(AC_TRACE << "DummyTransport::writeStatusToken()";)
    
    ostringstream myStream;
    myStream << theToken; 
    myStream.width(4); 
    myStream.fill('0'); 
    myStream.setf( ios::hex, ios::basefield );
    myStream << theNumber; 

    string myString = myStream.str();

    _myStatusLine.insert( _myStatusLine.end(), myString.begin(), myString.end() );
    
}

// XXX replace with more sophisticated blob generation (i.e. from mouse data)
void DummyTransport::generateGridValues() {
    DB(AC_TRACE << "DummyTransport::generateGridValues()";)

    _myGridValues.resize(_myGridWidth * _myGridHeight);

    vector<unsigned char>::iterator myIt = _myGridValues.begin();
    while(myIt != _myGridValues.end()) {
        (*myIt++) = rand() % (255 - _myDimFactor);
    }
}


void DummyTransport::calculateChecksum() {
    DB(AC_TRACE << "DummyTransport::calculateChecksum()";)
    _myGenCheckSum = 0;
    vector<unsigned char>::iterator myIt = _myGridValues.begin();
    while (myIt != _myGridValues.end()) {
        _myGenCheckSum += (*myIt++);
    }
}



