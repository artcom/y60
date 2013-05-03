#ifndef Y60_ASS_UDP_TRANSPORT_INCLUDED
#define Y60_ASS_UDP_TRANSPORT_INCLUDED

#include <asl/net/UDPConnection.h>
#include <asl/net/UDPSocket.h>
#include <asl/net/Socket.h>

#include "y60_asscore_settings.h"
#include "TransportLayer.h"

namespace y60 {

class UDPTransport : public TransportLayer {
  public:
    UDPTransport(const dom::NodePtr &settings);
    ~UDPTransport();
  
  protected:
    virtual bool settingsChanged(dom::NodePtr theSettings);

    void init(dom::NodePtr theSettings);

    void establishConnection();
    void closeConnection();
    void readData();
    void writeData(const char *theData, size_t theSize);

  private:
    inet::UDPSocket *_mySocket;
    std::string _hostnameString;
    asl::Unsigned16 _myPort;
    char *frame;

    void buildStatusLine();
    void generateGridValues();
    void calculateChecksum();
    void writeStatusToken( const char theToken, unsigned theNumber );

    unsigned char _myFrameCount;
    unsigned _myGenCheckSum;
    unsigned _myGridWidth;
    unsigned _myGridHeight;
    unsigned _myGridSpacing;
    unsigned char _myDimFactor;

    std::vector<unsigned char> _myStatusLine;
    std::vector<unsigned char> _myGridValues;
    std::vector<unsigned char> _myTmpBuffer;
};

}

#endif
