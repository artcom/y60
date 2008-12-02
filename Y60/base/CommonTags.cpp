/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
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
//   $RCSfile: CommonTags.h,v $
//
//   $Revision: 1.1 $
//
//   Description:
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "CommonTags.h"
#include <asl/base/Time.h>
#include <asl/base/Block.h>
#include <asl/base/os_functions.h>

namespace y60 {

    IdTag::IdTag() : counter(0) {
        myStartTime = asl::Time().secs() - 1117122059;
    }
    const char * IdTag::getName() {
        return ID_ATTRIB;
    }

    const IdTag::TYPE IdTag::getDefault() {
        asl::Block myIdBlock;
        unsigned int myCounter = ++IdTag::get().counter;
        if (myCounter == 0) {
            IdTag::get().myStartTime = asl::Time().secs() - 1117122059;
        }

        // Most significant bytes first improves legibility and map lookup-speed
        myIdBlock.appendUnsigned8( static_cast<unsigned char>(myCounter) );
        if (myCounter > 0xff) {
            myIdBlock.appendUnsigned8( static_cast<unsigned char>(myCounter >> 8) );
        }
        if (myCounter > 0xffff) {
            myIdBlock.appendUnsigned8( static_cast<unsigned char>(myCounter >> 16) );
        }
        if (myCounter > 0xffffff) {
            myIdBlock.appendUnsigned8( static_cast<unsigned char>(myCounter >> 24) );
        }
        myIdBlock.appendUnsigned16((unsigned short)asl::getThreadId() & 0xFFFF);
        myIdBlock.appendUnsigned(IdTag::get().myStartTime);

        std::string myId;
        binToBase64(myIdBlock, myId);
        return myId;
    }
}
