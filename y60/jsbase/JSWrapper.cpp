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

// own header
#include "JSWrapper.h"

#include "JSWrapper.impl"

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
checkArguments(const std::string & theMethodName, uintN argc, jsval *argv,
        unsigned theRequiredArguments)
{
    if (argc != theRequiredArguments) {
        throw BadArgumentException(theMethodName + ": Wrong number of arguments. Got " +
            asl::as_string(argc) + ", expected " + asl::as_string(theRequiredArguments) + ".", PLUS_FILE_LINE);
    }

    checkForUndefinedArguments(theMethodName, argc, argv);
}

bool
isCalledForConversion(JSContext * cx, uintN argc, jsval * argv) {
    if (0 < argc && JSVAL_IS_STRING(argv[0])) {
        JSString * myJSString = JSVAL_TO_STRING(argv[0]);
        if (!strcmp(JS_GetStringBytes(myJSString), "CALLED_FROM_AS_JSVAL")) {
            JS_RemoveRoot(cx,&myJSString);
            return true;
        }
    }
    return false;
}

}


