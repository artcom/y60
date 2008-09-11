//============================================================================
//
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "SimWater.h"

#include <asl/base/PlugInBase.h>

extern "C"
EXPORT asl::PlugInBase* SimWater_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new SimWater(myDLHandle);
}

