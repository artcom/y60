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

//own header
#include "CommonTags.h"

#include <asl/base/Time.h>
#include <asl/base/Block.h>
#include <asl/base/os_functions.h>

namespace y60 {

    IdTag::IdTag()
        : counter(0), myStartTime(asl::Time().secs() - 1117122059)
    {
    }
    const char * IdTag::getName() {
        return ID_ATTRIB;
    }

    const IdTag::TYPE IdTag::getDefault() {
        asl::Block myIdBlock;
        unsigned int myCounter = ++IdTag::get().counter;
#if defined(Y60_BASE_UNIQUE_IDS)
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
#elif defined(Y60_BASE_COUNTER_IDS)
        myIdBlock.appendUnsigned(myCounter);
#else
#error No ID generation algorithm configured.
#endif
        std::string myId;
        binToBase64(myIdBlock, myId);
        return myId;
    }
}
