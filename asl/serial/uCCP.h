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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//    $RCSfile: uCCP.h,v $
//
//     $Author: david $
//
//   $Revision: 1.6 $
//
//=============================================================================

#ifndef ASL_UCCP_INCLUDED
#define ASL_UCCP_INCLUDED

#include "asl_serial_settings.h"

#include <asl/base/Exception.h>
#include <asl/base/Block.h>

#include <limits>
#include <deque>
#include <queue>

namespace asl {

    class SerialDevice;

    DEFINE_EXCEPTION(uCCPException, asl::Exception);

    class ASL_SERIAL_DECL uCCP {
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

            void setVariablePayloadSize(bool theUseLengthByteFlag = true) {
                _myPayloadSize = std::numeric_limits<unsigned>::max();
                _myUseLengthByteFlag = theUseLengthByteFlag;
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
            bool          _myUseLengthByteFlag;

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

#endif  // ASL_UCCP_INCLUDED
