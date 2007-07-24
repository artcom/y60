/*__ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
*/
#ifndef _included_asl_checksum_
#define _included_asl_checksum_
#include "zlib.h"

namespace asl {    
    template<class T>
    void
    appendCRC32( unsigned long & theCRC, const T & theValue) {
        const Bytef * myValuePtr = (const Bytef*)(&theValue);
        theCRC = crc32(theCRC, myValuePtr, sizeof(T));
    }
} // end of namespace asl
#endif
