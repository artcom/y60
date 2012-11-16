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

#include "JShashing_functions.h"
#include <y60/jsbase/Documentation.h>

#include <crypto++/sha.h>

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JScppUtils.impl>
#include <y60/jsbase/JSBlock.h>

namespace jslib {

    static JSBool
    sha1FromBlock(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Converts a Block into a sha1 checksum.");
        DOC_PARAM("theBlock", "A Block", DOC_TYPE_OBJECT);
        DOC_PARAM_OPT("theAmountOfBytesToUse", "use this many bytes to calculate the sha sum", DOC_TYPE_INTEGER, 0);
        DOC_RVAL("A SHA-1 checksum as hex string", DOC_TYPE_STRING);
        DOC_END;
        if (argc == 1 || argc == 2) {
            if (JSVAL_IS_VOID(argv[0])) {
                JS_ReportError(cx, "sha1FromBlock(): Argument #%d is undefined", 1);
                return JS_FALSE;
            }

            asl::Block* myContentBlockPtr = 0;
            if (!convertFrom(cx, argv[0], myContentBlockPtr)) {
                JS_ReportError(cx, "sha1FromBlock(): argument #1 must be a Block");
                return JS_FALSE;
            }

            // copy block due to garbage collection
            asl::Ptr<asl::ReadableBlock> myReadableBlockPtr(new asl::Block(*myContentBlockPtr));

            unsigned myAmountOfBytesToUse = myReadableBlockPtr->size();
            if(argc == 2) {
                if (!convertFrom(cx, argv[1], myAmountOfBytesToUse)) {
                    JS_ReportError(cx, "sha1FromBlock(): argument #2 must be an integer");
                    return JS_FALSE;
                }
                myAmountOfBytesToUse = asl::minimum(myAmountOfBytesToUse, myContentBlockPtr->size());
            }
            unsigned char* myDigestBuffer = new unsigned char[CryptoPP::SHA1::DIGESTSIZE];
            CryptoPP::SHA1().CalculateDigest(myDigestBuffer, myReadableBlockPtr->begin(),
                myAmountOfBytesToUse);
            std::string myFinalHexEncodedShaSum;
            asl::binToString(myDigestBuffer, CryptoPP::SHA1::DIGESTSIZE, myFinalHexEncodedShaSum);
            delete [] myDigestBuffer;
            *rval = as_jsval(cx, myFinalHexEncodedShaSum);
            return JS_TRUE;
        }
        JS_ReportError(cx,"sha1FromBlock: bad number of arguments should be one, got %d", argc);
        return JS_FALSE;
    }

    JSFunctionSpec *
    JSHashingFunctions::Functions() {
        static JSFunctionSpec myFunctions[] = {
            {"sha1FromBlock",   sha1FromBlock, 2},
            {0},
        };
        return myFunctions;
    }
}
