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
*/

#include "UDPTransport.h"
#include "ASSUtils.h"
#include "ASSDriver.h"

#include <asl/base/file_functions.h>
#include <y60/base/SettingsParser.h>

#define UDP_HDR 8
#define PROXY_HDR 14*4+32
#define TOUCHSIZE 64
//#define TOUCHSIZE 4*5*3 TODO: there's something wrong
#define FRAMESIZE 4256

using namespace std;
using namespace asl;
using namespace y60;
using namespace inet;

namespace y60 {

UDPTransport::UDPTransport(const dom::NodePtr & theSettings) :
  TransportLayer( "udp", theSettings ),
  _myFrameCount(0),
  _myGenCheckSum(0),
  _myGridWidth(64),
  _myGridHeight(64),
  _myGridSpacing(10) {

  AC_DEBUG << "UDPTransport::UDPTransport()";

  init(theSettings);
}

UDPTransport::~UDPTransport() {
  AC_DEBUG << "UDPTransport::~UDPTransport()";

  stopThread();
  closeConnection();
}

bool UDPTransport::settingsChanged(dom::NodePtr theSettings) {
  AC_DEBUG << "UDPTransport::settingsChanged()";
  
  bool changedFlag = false;

  changedFlag |= settingChanged(theSettings, "TransportLayer", _myTransportName);
  changedFlag |= settingChanged(theSettings, "IpAddress", _hostnameString);
  changedFlag |= settingChanged(theSettings, "Port", _myPort);

  return changedFlag;
}

void UDPTransport::init(dom::NodePtr theSettings) {
  getConfigSetting(theSettings, "IpAddress", _hostnameString, std::string("127.0.0.1"));
  getConfigSetting<asl::Unsigned16>(theSettings, "Port", _myPort, 10001);
}

void UDPTransport::establishConnection() {
  AC_DEBUG << "UDPTransport::establishConnection()";

  try {
    _mySocket = new inet::UDPSocket(INADDR_ANY, _myPort);

    if(_mySocket->isValid()) {
      setState(SYNCHRONIZING);

      char trashBuffer[1024];
      size_t bytesReceived = sizeof(trashBuffer);
      _mySocket->receiveFrom(0, 0, trashBuffer, bytesReceived);
    }
  }
  catch(SocketException &e) {
    AC_WARNING << "Error creating the UDP socket: " << e.where();
    _mySocket = 0;
  }
}

void UDPTransport::readData() {
  AC_DEBUG << "UDPTransport::readData()";

  try {
    char receiveBuffer[UDP_HDR+PROXY_HDR+TOUCHSIZE+FRAMESIZE+6600];

    _mySocket->receiveFrom(0, 0, receiveBuffer, sizeof(receiveBuffer));

    frame = receiveBuffer+UDP_HDR+PROXY_HDR+TOUCHSIZE;

    buildStatusLine();
    generateGridValues();
    calculateChecksum();
    ++_myFrameCount;

    _myTmpBuffer.clear();
    _myTmpBuffer.push_back(255);
    _myTmpBuffer.push_back(0);
    _myTmpBuffer.insert(_myTmpBuffer.end(), _myStatusLine.begin(), _myStatusLine.end());

    for (unsigned char i = 1; i <= _myGridHeight; i++) {
        _myTmpBuffer.push_back(255);
        _myTmpBuffer.push_back(i);

        _myTmpBuffer.insert(_myTmpBuffer.end(), _myGridValues.begin() + (i-1)*_myGridWidth, _myGridValues.begin() + i*_myGridWidth);
    }
    
    receivedData((char*)&_myTmpBuffer[0], _myTmpBuffer.size());
  
    /*
    cout << "##############" << endl;
    for(int i=0; i<64; i+=4) {
      for(int j=0; j<64; j+=4) {
        cout << (int) frame[i*64+j] << "\t";
      }
      cout << endl << endl << endl;
    }
    */
  }
  catch(SocketException &e) {
    AC_WARNING << "Error receiving data: " << e.where();
    connectionLost();
  }
}

void UDPTransport::writeData(const char * theData, size_t theSize) {
  AC_DEBUG << "UDPTransport::writeData()";

  asl::Unsigned32 clientIpAddress = getHostAddress(_hostnameString.c_str());

  if(!_mySocket) {
    AC_WARNING << "Cannot write to socket: socket doesn't exist";
    setState(NOT_CONNECTED);
  }

  try {
    _mySocket->sendTo(clientIpAddress, _myPort, theData, theSize);
  }
  catch(SocketException &e) {
    AC_WARNING << "Error sending data: " << e.where();
    connectionLost();
  }
}

void UDPTransport::closeConnection() {
  AC_DEBUG << "UDPTransport::closeConnection()";

  if(_mySocket) {
    delete _mySocket;
    _mySocket = 0;
  }
}

void UDPTransport::buildStatusLine() {
    AC_DEBUG << "UDPTransport::buildStatusLine()";

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

void UDPTransport::writeStatusToken( const char theToken, unsigned theNumber ) {
    AC_DEBUG << "UDPTransport::writeStatusToken()";

    ostringstream myStream;
    myStream << theToken;
    myStream.width(4);
    myStream.fill('0');
    myStream.setf( ios::hex, ios::basefield );
    myStream << theNumber;

    string myString = myStream.str();

    _myStatusLine.insert( _myStatusLine.end(), myString.begin(), myString.end() );
}

void UDPTransport::generateGridValues() {
    AC_DEBUG << "UDPTransport::generateGridValues()";

    _myGridValues.resize(_myGridWidth * _myGridHeight);
    vector<unsigned char>::iterator myIt = _myGridValues.begin();

    for(int i=0; i<_myGridHeight*_myGridWidth; i++) {
      (*myIt++) = frame[i];
    }
}

void UDPTransport::calculateChecksum() {
    AC_DEBUG << "UDPTransport::calculateChecksum()";

    _myGenCheckSum = 0;
    vector<unsigned char>::iterator myIt = _myGridValues.begin();

    while (myIt != _myGridValues.end()) {
        _myGenCheckSum += (*myIt++);
    }
}
}
