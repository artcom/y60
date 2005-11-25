//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSproc_functions.cpp,v $
//   $Author: ulrich $
//   $Revision$
//   $Date$
//
//=============================================================================

#include "JSproc_functions.h"
#include <y60/Documentation.h>

#include <asl/proc_functions.h>

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
