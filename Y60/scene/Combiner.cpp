//=============================================================================
// Copyright (C) 1993-`date +%Y`, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Combiner.h"

#include <y60/NodeNames.h>
#include <asl/string_functions.h>
#include <asl/Logger.h>


using namespace asl;

namespace y60 {

    Combiner::Combiner() {
        AC_TRACE << "Combiner::Combiner " << (void*) this;
    }

    Combiner::~Combiner() {
        AC_TRACE << "Combiner::~Combiner " << (void*) this;
    }
}
