//============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: uCCP.h,v $
//
//     $Author: david $
//
//   $Revision: 1.6 $
//
//=============================================================================



#include <asl/base/Exception.h>
#include <asl/base/Block.h>

#include <deque>
#include <queue>

namespace asl {

    class SerialDevice;

    DEFINE_EXCEPTION(uCCPException, asl::Exception);

    class uCCP {
        public:
            enum ErrorChecking {
                NO_CHECKING,
                CRC8_CHECKING,
                CHECKSUM_CHECKING
            };

            uCCP(SerialDevice * theTTY);
            virtual ~uCCP() {};

            void recive();
            void send(const std::string & thePacket);

            unsigned pendingPackets() const;
            std::string  popPacket();

            void setFrame(unsigned char theStartToken, unsigned char theEndToken) {
                _myStartToken = theStartToken;
                _myEndToken   = theEndToken;
            }

            void setVariablePayloadSize() {
                _myPayloadSize = UINT_MAX;
            }

            void setFixedPayloadSize(unsigned theSize);

            void setErrorChecking(ErrorChecking theErrorChecking) {
                _myErrorChecking = theErrorChecking;
            }

            std::ostream & printPacketFormat(std::ostream & os) const;

            // Statistics getter
            unsigned getDroppedBytes() const;
            unsigned getCRCErrors() const;
            unsigned getCorruptPackets() const;
            unsigned getPacketsSend() const;
            unsigned getPacketsReceived() const;
            std::ostream & printStatistic(std::ostream & os) const;

        private:
            typedef std::deque<unsigned char> Frame;

            uCCP();
            bool parseFrame(Frame & theFrame);
            bool parseFrameing(Frame & theFrame);
            void unescape(Frame & theFrame);
            bool parseErrorChecking(Frame & theData);
            bool parseSize(Frame & theData);

            void appendEscaped(asl::Block & theBuffer, unsigned char theData);

            SerialDevice *              _myTransportLayer;
            Frame                       _myCurrentFrame;
            std::queue<std::string>     _myPendingPackets;

            // format specification
            unsigned char _myStartToken;
            unsigned char _myEndToken;
            unsigned      _myPayloadSize;
            ErrorChecking _myErrorChecking;

            // statistic
            unsigned _myDroppedBytes;
            unsigned _myCRCErrors;
            unsigned _myCorruptPackets;

            unsigned int _myPacketsSend;
            unsigned int _myPacketsReceived;
    };

    template <class T>
    unsigned char checksum(T theIt, unsigned theSize) {
        unsigned mySum = 0;
        for (unsigned i = 0; i < theSize; ++i) {
            mySum += *theIt++;
        }
        return (unsigned char)(256 - mySum % 256);
    }
}
