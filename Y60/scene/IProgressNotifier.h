//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _Y60_IPROGRESSNOTIFIER_INCLUDED
#define _Y60_IPROGRESSNOTIFIER_INCLUDED

#include <asl/base/Ptr.h>
#include <string>

namespace y60 {

class IProgressNotifier {
    public:
        virtual void onProgress(float theProgress, const std::string & theMessage="") = 0;
};

typedef asl::Ptr<IProgressNotifier> IProgressNotifierPtr;

}
#endif


