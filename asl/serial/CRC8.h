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
//    $RCSfile: CRC8.h,v $
//
//     $Author: david $
//
//   $Revision: 1.3 $
//
//=============================================================================

#ifndef ASL_CRC8_INCLUDED
#define ASL_CRC8_INCLUDED

#include <asl/Block.h>

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
