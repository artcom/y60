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

#ifndef ASS_OSC_RECEIVER_INCLUDED
#define ASS_OSC_RECEIVER_INCLUDED

#include "y60_oscclient_settings.h"

#include <asl/base/Ptr.h>

#include <asl/math/Box.h>
#include <asl/math/Vector234.h>

#include <asl/net/UDPConnection.h>

#include <oscpack/osc/OscOutboundPacketStream.h>

namespace y60 {

class ASSOscReceiver {
public:

    ASSOscReceiver(std::string theHost, asl::Unsigned16 theSourcePort, asl::Unsigned16 theDestinationPort);

    ~ASSOscReceiver();

    // build a configure event - used for communicating array configuration.
    void buildConfigureEvent(asl::Signed32 theWidth, asl::Signed32 theHeight);

    // build a TL event - used for array/driver state notification
    void buildTransportLayerEvent(const std::string& theType);

    // build a cursor event
    void buildCursorEvent(const std::string&   theType,
                          int                  theId,
                          const asl::Vector3f& thePosition,
                          float                theIntensity);

    // install a bounding box for this receiver
    void restrictToRegion(const asl::Box2f& theRegion);

    // prepare the receiver for a new cycle,
    // clearing all state from the previous cycle
    void prepare();

    // send everything built in this cycle
    void send();

    // reset connection state
    void reset();

private:

    static const unsigned ourBufferSize = 1024;

    char* _myBuffer;

    std::string _myHost;
    asl::Unsigned16 _mySourcePort;
    asl::Unsigned16 _myDestinationPort;

    inet::UDPConnection _myConnection;

    osc::OutboundPacketStream _myStream;
    bool _myDidOverflow;

    bool _myEmpty;
    bool _myConfigured;

    asl::Unsigned64 _myBundleCounter;

    bool _myBounded;
    asl::Box2f _myRegion;
};

typedef asl::Ptr<ASSOscReceiver> ASSOscReceiverPtr;

} // end of namespace y60

#endif // ASS_OSC_RECEIVER_INCLUDED
