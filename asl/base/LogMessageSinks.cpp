//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "LogMessageSinks.h"

namespace asl {

ThreadLock StreamPrinter::myLock;
ThreadLock FilePrinter::myLock;

#ifdef WIN32
ThreadLock OutputWindowPrinter::myLock;
#endif

}
