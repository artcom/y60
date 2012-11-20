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

#include "JSproc_functions.h"
#include <y60/jsbase/Documentation.h>

#include <asl/base/proc_functions.h>

namespace jslib {

    static JSBool
    getTotalMemory(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        DOC_BEGIN("Returns total amount of physical system memory in KB");
        DOC_RVAL("The amount of memory in KB.", DOC_TYPE_INTEGER);
        DOC_END;
        *rval = as_jsval(cx, asl::getTotalMemory());
        return JS_TRUE;
    }

    static JSBool
    getFreeMemory(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        DOC_BEGIN("Returns amount of free system memory in KB");
        DOC_RVAL("The amount of memory in KB.", DOC_TYPE_INTEGER);
        DOC_END;
        *rval = as_jsval(cx, asl::getFreeMemory());
        return JS_TRUE;
    }

    static JSBool
    getUsedMemory(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
        DOC_BEGIN("Returns amount of used system memory in KB");
        DOC_RVAL("The amount of memory in KB.", DOC_TYPE_INTEGER);
        DOC_END;
        *rval = as_jsval(cx, asl::getUsedMemory());
        return JS_TRUE;
    }

    static JSBool
    getProcessMemoryUsage(JSContext * cx, JSObject * obj,uintN argc, jsval * argv, jsval * rval) {
        DOC_BEGIN("Returns amount of memory used by this process in KB");
        DOC_RVAL("The amount of memory in KB.", DOC_TYPE_INTEGER);
        DOC_END;
        *rval = as_jsval(cx, asl::getProcessMemoryUsage());
        return JS_TRUE;
    }

    JSFunctionSpec *
    JSProcFunctions::Functions() {
        static JSFunctionSpec myFunctions[] = {
            {"getTotalMemory",        getTotalMemory,        0},
            {"getFreeMemory",         getFreeMemory,         0},
            {"getUsedMemory",         getUsedMemory,         0},
            {"getProcessMemoryUsage", getProcessMemoryUsage, 0},
            {0},
        };
        return myFunctions;
    }
}
