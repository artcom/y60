/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: 
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      poor
//    formatting              :      ok
//    documentation           :      disaster
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
//    interfaces              :      fair
//    confidence              :      fair
//    integration             :      ok
//    dependencies            :      ok
//    error handling          :      ok
//    logging                 :      notapp
//    cheesyness              :      ok
//
//    overall review status   :      ok
//
//    recommendations: move this to its own component, 
//                     check interfaces, 
//                     remove CVS log 

/** The "Station" protokoll is a simple UDP-based broadcast protocol that basically performs
fragmentation and defragmentation of messages that do not fit into a single UDP packet.
It is intended for use in local area networks within one broadcast range.

The protocol is called "station" because it acts like a radio broadcast station. Every station can
broadcast, and the broadcast is picked up by all listening stations.

In addition to fragmentation/defragmentation, the protocol offers two additional features:

1) A transparent 32-Bit Network id that allows to create pseudo-multicast groups
2) Compression of the payload when the uncompressed payload does not fit into one packet

The protocol is not designed for high throughput, but for low-latency and synchronisation purposes.
It also features extensive error checking and reporting. When one packet of a multi-packet message
is lost, the whole message is discarded, no retransmission occurs. Message boundaries are always
preserved. Duplicate messages are not discarded, and nothing is done to ensure ordering of messages,
although on a LAN the ordering of messages is generally preserved. However, message integrity is
strictly preserved.

With 2008 machines and gigabit ethernet, maximum throughput will be in the range of 10-30 MBytes/sec.

Usage:

The interface is quite simple: it consists of the openStation(), closeStation(), broadcast() and receive() calls.

broadcast() takes a block of memory as sole argument and broadcasts it.
receive() is non-blocking and returns the oldest fully received message when available, together with the 
originating host adress and station id.

Caveeats: 
Altough the protocol is fairly simple and easy to use, there are a few things to consider:
1) Under Linux, unfortunately the SO_REUSEPORT socket option does not exist, which limits the number of
receiving stations with the same port number on a single linux host to one. Although this can be solved
by using a simple port forwarder, it is not nice. In practice it is often desirable that multiple
independent processes can listen to the same broadcast.

Please note that you can always provide different port numbers for sending and receiving, so there is
no limit for the number of transmit-only stations on a single Linux host when they use different
port numbers.

On Windows and OSX however however multiple port bindings are allowed. (Windows does not have the
SO_REUSEPORT option, but allowing multiple port bindings is default.)

The proper solution for Linux systems would be to use multicast instead of broadcast. In this case,
the SO_REUSEADDR option hase the same effect like SO_REUSEPORT and should work also properly
with Linux.

2) Currently the protocol adds 40 Bytes overhead, which could be easily reduced to less than the half,
but has not be considered to be worth the problems it might add.

3) The protocol will not perform well when packet loss is high and the messages are very long; the
chances of getting even one message through drops exponentially with the message length:
prob_msg_lost = prob_packet_lost ^ packets_per_message 

4) The implementation is non-blocking and well suited to run in a single threaded environment; therefore
receive() should not be called in a tight loop when there is no work to be performed. A typical usage is
to check once per frame for new messages that have arrived. For operation in a multi-threaded environment,
the code either needs to be modified to support blocking operation as well, or support for using select()
needs to be added; exposing the file descriptor should be enough to do that.
*/

#ifndef AC_STATION_H_INCLUDED
#define AC_STATION_H_INCLUDED

#include "asl_net_settings.h"

#include <asl/base/Exception.h>
#include <asl/base/Ptr.h>
#include <asl/base/Block.h>

#include <sys/types.h>

#ifndef _WIN32
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif

#ifdef _WIN32
  #include <winsock2.h>
#endif

#ifdef OSX
  #include <netinet/in.h>
#endif

#include <string.h>

#include <string>
#include <map>
#include <vector>
#include <algorithm>

class ASL_NET_EXPORT Station {
public:
    DEFINE_NAMED_EXCEPTION(Exception,Station::Exception,asl::Exception);
    DEFINE_NAMED_EXCEPTION(SocketFailed,Station::SocketFailed,Exception);
    DEFINE_NAMED_EXCEPTION(ConnectFailed,Station::ConnectFailed,Exception);
    DEFINE_NAMED_EXCEPTION(SetSockOptFailed,Station::SetSockOptFailed,Exception);
    DEFINE_NAMED_EXCEPTION(BindFailed,Station::BindFailed,Exception);
    DEFINE_NAMED_EXCEPTION(SendFailed,Station::SendFailed,Exception);
    DEFINE_NAMED_EXCEPTION(RecvFailed,Station::RecvFailed,Exception);
    DEFINE_NAMED_EXCEPTION(InvalidPacket,Station::InvalidPacket,Exception);
    DEFINE_NAMED_EXCEPTION(SizeMismatch,Station::SizeMismatch,Exception);
    DEFINE_NAMED_EXCEPTION(EnableNonBlockingFailed,Station::EnableNonBlockingFailed,Exception);

    enum constants { MTU_SIZE = 1400, STATION_MESSAGE_MAGIC = 0xfeedbee1};

    Station() : _good(false) {};
    Station(unsigned long theBroadcastAddress, // IP-Broadcast Adress
            unsigned short theBroadcastPort,   // IP-Port Adress
            unsigned short theReceivePort,     // IP-Port Adress
            unsigned long ownIPAddress,        // supply own Adresse to avoid receiving self-sent packets
            unsigned long theStationID,        // arbitrary unique ID to distinguish different stations
                                               // on a single  host (geht z.Z . unter Linux nicht wg. fehlendem SO_REUSEPORT
            unsigned long theNetworkID)        // arbitrary group id, must be the same number for all participating nodes
        : _good(false)
    {
        openStation(theBroadcastAddress, theBroadcastPort, theReceivePort, ownIPAddress, theStationID, theNetworkID);
    };
    void openStation(unsigned long theBroadcastAddress,
                     unsigned short theBroadcastPort,
                     unsigned short theReceivePort,
                     unsigned long ownIPAddress,
                     unsigned long theStationID,
                     unsigned long theNetworkID);
    void openStationDefault(unsigned long theBroadcastAddress = defaultBroadcastAddress(),
                     unsigned short theBroadcastPort = defaultBroadcastPort(),
                     unsigned short theReceivePort = defaultBroadcastPort(),
                     unsigned long ownIPAddress = defaultIgnoreAddress(),
                     unsigned long theStationID = defaultStationID(),
                     unsigned long theNetworkID = defaultNetworkID())
    {
        openStation(theBroadcastAddress, theBroadcastPort, theReceivePort, ownIPAddress, theStationID, theNetworkID);
    }
    void closeStation();
    void broadcast(const asl::ReadableBlock & theData);
    bool receive(asl::ResizeableBlock & theData, unsigned long & theSenderAddress, unsigned long & theStationID);
    operator const void*() const { return _good ? this : NULL;}
    unsigned long getOutgoingMessageNumber() const {
        return _myOutgoingMessageNumber;
    }

    static unsigned long defaultBroadcastAddress();
    static unsigned short defaultBroadcastPort();
    static unsigned long defaultIgnoreAddress();
    static unsigned long defaultNetworkID();
    static unsigned long defaultOwnIPAddress();
    static unsigned long defaultStationID();
    static bool disableReceivingFlag();

    union Descriptor {
        unsigned long long _together;
        struct {
            unsigned long _myIPAddress;
            unsigned long _myStationID;
        } _separate;
    };

    struct Statistic {
        Statistic() : _myMessageCount(0), _myPacketCount(0), _myByteCount(0) {}
        Statistic(int theMessageCount, int thePacketCount, int theByteCount)
            : _myMessageCount(theMessageCount), _myPacketCount(thePacketCount), _myByteCount(theByteCount) {}
        int _myMessageCount;
        int _myPacketCount;
        int _myByteCount;
        Statistic & operator+=(const Statistic & theOther);
    };

    const Statistic & getTransmitStatistic() const {
        return _myTransmitStatistic;
    }
    const Statistic & getReceiveStatistic() const {
        return _myReceiveStatistic;
    }
    const Statistic & getReceiveErrorStatistic() const {
        return _myReceiveErrorStatistic;
    }

    typedef std::map<unsigned long long, Statistic> ReceiveStatisticByStation;

    const ReceiveStatisticByStation & getReceiveStatisticByStation() const {
        return _myReceiveStatisticByStation;
    }
    void resetStatistic();
    static unsigned long getLocalHostAddress();
private:
    struct Header {
        unsigned long _myMagicNumber;
        unsigned long _mySrcAddress;
        unsigned long _myNetworkID;
        unsigned long _myStationID;
        unsigned long _myMessageNumber;
        unsigned long _myMessageSize;
        unsigned long _myMessagePartCount;
        unsigned long _myMessagePartNumber;
        unsigned long _myMessagePartSize;
        unsigned long _myUncompressedMessageSize;
    };
    struct Packet {
        enum constants { PAYLOAD_SIZE = MTU_SIZE - sizeof(Header) };
        Header _myHeader;
        char   _myData[MTU_SIZE];
    };

    unsigned short _myBroadcastPort;
    unsigned short _myReceivePort;
    unsigned long _myBroadcastAddress;
    unsigned long _myNetworkID;
    unsigned long _myStationID;
    unsigned long _ownIPAddress;
    unsigned long _myFlags;
    struct sockaddr_in _fromAddress;
    struct sockaddr_in _toAddress;
    int _myFileDescriptor;
    int _myOutgoingMessageNumber;
    bool _good;

    std::map<unsigned long long,std::vector<asl::Ptr<Packet> > > _myIncomingPackets;

    Statistic _myReceiveStatistic;
    Statistic _myReceiveErrorStatistic;
    Statistic _myTransmitStatistic;
    ReceiveStatisticByStation _myReceiveStatisticByStation;
};

std::ostream & operator<<(std::ostream&, const Station::Statistic &);
Station::Statistic operator-(const Station::Statistic & theOne, const Station::Statistic & theOther);
Station::Statistic operator/(const Station::Statistic & theOne, double theTime);
Station::Statistic maximum(const Station::Statistic & theOne, const Station::Statistic & theOther);
#endif


