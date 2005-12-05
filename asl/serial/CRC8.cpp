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
//    $RCSfile: CRC8.cpp,v $
//
//     $Author: david $
//
//   $Revision: 1.1 $
//
//=============================================================================


#include "CRC8.h"

namespace asl {

char crc8(void *buf, int size )
{
    int i,j;
    char result = 0;
    char *p = (char *)buf;
    
    for ( i = 0 ; i < size ; i++ )
    {
        result ^= *p++;
        for ( j = 0 ; j < 8; j++ )
        {
            if ( result & 0x80 )
            result = (result << 1) ^ 7;
            else
            result <<= 1;
        }
    }
    return result;
}

char crc8i(char init, void *buf, int size )
{
    int i, j;
    char result;
    char *p = (char*)buf;
    
    result = crc8(&init, 1);
    
    for ( i = 0; i < size; i++ )
    {
        result ^= *p++;
        for ( j = 0 ; j < 8; j++ )
        {
            if ( result & 0x80 )
            result = (result << 1) ^ 7;
            else
            result <<= 1;
        }
    }
    return result;
}

}
