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
//    $RCSfile: CRC8.h,v $
//
//     $Author: david $
//
//   $Revision: 1.3 $
//
//=============================================================================

#ifndef ASL_CRC8_INCLUDED
#define ASL_CRC8_INCLUDED

#include "asl_serial_settings.h"

#include <asl/base/Block.h>

namespace asl {

    char crc8(void *buf, int size );
    char crc8i(char init, void *buf, int size );

    template <class T>
    char crc8i(char theInit, T theIt, unsigned theSize) {
        char myResult = crc8(&theInit, 1);

        for (unsigned i = 0; i < theSize; ++i) {
            myResult ^= *theIt++;
            for (unsigned j = 0; j < 8; ++j) {
                if (myResult & 0x80) {
                    myResult = (myResult << 1) ^ 7;
                } else {
                    myResult <<= 1;
                }
            }
        }
        return myResult;
    }
}

#endif // ASL_CRC8_INCLUDED
