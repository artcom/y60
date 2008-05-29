/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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
*/

#ifndef AC_STATION_H_INCLUDED
#define AC_STATION_H_INCLUDED

#include <asl/Exception.h>
#include <asl/Ptr.h>
#include <asl/Block.h>

#include <sys/types.h>

#ifndef WIN32
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif

#ifdef WIN32
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

class Station {
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
            unsigned short theBroadcastPort,   // IP-Port Adresse
            unsigned short theReceivePort,     // IP-Port Adresse
            unsigned long ownIPAddress,        // eigene Adresse angeben, um eigene Pakete auszufiltern
            unsigned long theStationID,        // beliebige ID zur Unterscheidung verschiedener Teilnehmer
                                               // auf einem Host (geht z.Z . unter Linux nicht wg. fehlendem SO_REUSEPORT
            unsigned long theNetworkID)        // beliebige ID, muss bei allen Teilnehmern identisch sein
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
    operator bool() const { return _good;}
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

//==============================================================================
//
// $Log: Station.h,v $
// Revision 1.6  2004/10/30 15:34:04  pavel
// completed windows station
// changed station interface to use asl::Block
//
// Revision 1.5  2004/09/10 12:50:29  david
//  - mac os x port in progress
//
// Revision 1.4  2004/07/22 14:34:06  christian
// - fixed bug in Station.h
// - turned off station test again, because there are several issues
// - added z-lib to extern install to allow station test build
//
// Revision 1.3  2004/07/17 18:25:15  pavel
// fixed station build problems for windows; however, station has not been tested yet
//
// Revision 1.2  2003/07/01 13:59:00  uzadow
// Initial windows port.
//
// Revision 1.1.1.1  2003/05/12 14:20:23  uzadow
// no message
//
// Revision 1.1.1.1  2002/09/17 15:37:04  wolfger
// initial checkin
//
// Revision 1.2  2002/09/06 18:14:00  valentin
// merged linuxport_2nd_try branch into trunk
// -branch tag before merge : branch_before_merge_final
// -trunk tag before merge  : trunk_before_merge
//
//
// -Merge conflicts:
//  image/glutShowImage/Makefile
//  image/glutShowImage/glShowImage.C
//  loader/libPfIv/pfiv.c++
//  lso/script/vrfm/Intercom.h
//  tools/Pfconvert/pfconvert.c
//  vrap/libMover/HMD.c++
//
//  - compiled && testrun
//  - commit
//  - merged trunk tag : trunk_after_merge
//
// Revision 1.1.2.8  2002/08/30 13:41:11  martin
// std-ized header files
//
// Revision 1.1.2.7  2002/06/06 20:17:49  pavel
// fixed broken WhiteBoardStation protocol - first usable version
//
// Revision 1.1.2.6  2002/05/28 22:25:25  pavel
// added facilities for multiple Station users on one host
//
// Revision 1.1.2.5  2002/05/16 01:26:59  pavel
// improved Station.h dependencies, added station packet payload compression
//
// Revision 1.1.2.4  2002/05/15 19:42:41  pavel
// major station protocol change
//
// Revision 1.1.2.3  2002/05/13 12:36:21  pavel
// some interface modifications due to first integration
//
// Revision 1.1.2.2  2002/05/12 23:26:33  pavel
// first possibly working version of Station with Whiteboard
//
// Revision 1.1.2.1  2002/05/11 21:44:36  pavel
// initial version, just compiles but has never been executed
//
//==============================================================================
