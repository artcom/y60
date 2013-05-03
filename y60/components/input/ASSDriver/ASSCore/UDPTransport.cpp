#include "UDPTransport.h"
#include "ASSUtils.h"
#include "ASSDriver.h"

#include <asl/base/file_functions.h>
#include <y60/base/SettingsParser.h>

#define FRAMESIZE 4096
#define PROXY_HDR 14*4+32
#define UDP_HDR 8
//#define TOUCHSIZE 4*5*3
#define TOUCHSIZE 64

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
    char receiveBuffer[4252];
    size_t bytesReceived = sizeof(receiveBuffer);

    _mySocket->receiveFrom(0, 0, receiveBuffer, bytesReceived);
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
      cout << endl;
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
