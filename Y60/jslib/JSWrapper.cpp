//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSWrapper.h"

namespace jslib {

void
checkForUndefinedArguments(const std::string & theMethodName, uintN argc, jsval *argv) {
    for (unsigned i = 0; i < argc; ++i) {
        if (JSVAL_IS_VOID(argv[i])) {
            throw BadArgumentException(theMethodName + ": Argument " + asl::as_string(i) + " is undefined.", PLUS_FILE_LINE);
        }
    }
}

void
checkArguments(const std::string & theMethodName, uintN argc, jsval *argv, unsigned theRequiredArguments) {
    if (argc != theRequiredArguments) {
        throw BadArgumentException(theMethodName + ": Wrong number of arguments. Got " +
            asl::as_string(argc) + ", expected " + asl::as_string(theRequiredArguments) + ".", PLUS_FILE_LINE);
    }

    checkForUndefinedArguments(theMethodName, argc, argv);
}

}


