//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: DecoderManager.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//=============================================================================

#include "DecoderManager.h"
#include <asl/Logger.h>

namespace y60 {

void
DecoderManager::addDecoder(IDecoderPtr theDecoder) {
    // Check if a decoder of this type already exists
    for (unsigned i = 0; i < _myDecoders.size(); ++i) {
        if (typeid(*_myDecoders[i]) == typeid(*theDecoder)) {
            AC_TRACE << " decoder already registered - skipping";
            return;
        }
    }
    _myDecoders.push_back(theDecoder);
    AC_TRACE << _myDecoders.size() << " decoders are now registered";
}

}
