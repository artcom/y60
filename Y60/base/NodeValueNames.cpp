//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "NodeValueNames.h"

// [DS, TS] This can't be in the y60 namespace because the ms compiler
// b0rks. So just keep it in global space which does no harm anyway
// since it only creates a class which itself will be in y60.

VERIFY_ENUM(y60::TargetBuffers, y60::TargetBuffersStrings);
VERIFY_ENUM(y60::BlendEquation, y60::BlendEquationStrings);

