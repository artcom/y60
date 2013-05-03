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
