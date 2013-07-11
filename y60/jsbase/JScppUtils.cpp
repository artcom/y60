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
#include "JScppUtils.h"

#include "QuitFlagSingleton.h"
#include "JSNode.h"
#include "JScppUtils.impl"

#include <asl/base/error_functions.h>

#ifdef USE_LEGACY_SPIDERMONKEY
#include <js/spidermonkey/jsapi.h>
#include <js/spidermonkey/jsprf.h>
#include <js/spidermonkey/jsparse.h>
#include <js/spidermonkey/jsscan.h>
#include <js/spidermonkey/jsemit.h>
#include <js/spidermonkey/jsscript.h>
#include <js/spidermonkey/jsarena.h>
#include <js/spidermonkey/jscntxt.h>
#include <js/spidermonkey/jsdbgapi.h>
#else
#include <js/jsapi.h>
#include <js/jsprf.h>
//#include <js/jsparse.h>
//#include <js/jsscan.h>
#include <js/jsemit.h>
#include <js/jsscript.h>
#include <js/jsarena.h>
#include <js/jscntxt.h>
#include <js/jsdbgapi.h>
#endif

#ifndef _WIN32
#include <boost/locale.hpp>
#endif

using namespace std;
using namespace asl;
using namespace boost::locale;

#define DB(x) // x;

namespace asl {

std::string
as_string(JSContext *cx, jsval theVal) {
    JSString *myJSStr = JS_ValueToString(cx, theVal);
    if (!myJSStr) {
        throw asl::Exception("Value is not a string", PLUS_FILE_LINE);
    }
#ifdef _WIN32
#if _MSC_VER >= 1500
    const LPWSTR myWChars = reinterpret_cast<LPWSTR>(JS_GetStringChars(myJSStr));
#else
    const LPWSTR myWChars = static_cast<LPWSTR>(JS_GetStringChars(myJSStr));
#endif
    AC_SIZE_TYPE myUTF8Size = WideCharToMultiByte(CP_UTF8, 0, myWChars, -1, 0, 0, 0, 0);
    if (myUTF8Size == 0) {
        DWORD myLastError = GetLastError();
        throw jslib::UnicodeException(errorDescription(myLastError), PLUS_FILE_LINE);
    }
    char * myUTF8Chars = new char[myUTF8Size];
    WideCharToMultiByte(CP_UTF8, 0, myWChars, -1, myUTF8Chars, myUTF8Size, 0, 0);
    std::string myResult = std::string(myUTF8Chars);
    delete [] myUTF8Chars;
    return myResult;
#else
    std::string myResult;
    size_t srcLen = JS_GetStringLength(myJSStr);

    // get pointer to 16-bit chars
    asl::Unsigned16 * myData = reinterpret_cast<asl::Unsigned16*>(JS_GetStringChars(myJSStr));

    try {
        // now convert to utf-8 encoded string
        myResult = conv::utf_to_utf<char>(myData, myData+srcLen, conv::stop);
    } catch (conv::conversion_error & ex) {
        throw jslib::UnicodeException(ex.what(), PLUS_FILE_LINE);
    }
    return myResult;
#endif
}

std::string as_string(JSContext *cx, JSObject *theObj) {
    return as_string(cx,OBJECT_TO_JSVAL(theObj));
}

std::string as_string(JSType theType) {
    switch (theType) {
        case JSTYPE_VOID: return "JSTYPE_VOID"; /* undefined */
        case JSTYPE_OBJECT: return "JSTYPE_OBJECT";  /* object */
        case JSTYPE_FUNCTION: return "JSTYPE_FUNCTION"; /* function */
        case JSTYPE_STRING: return "JSTYPE_STRING";  /* string */
        case JSTYPE_NUMBER: return "JSTYPE_NUMBER"; /* number */
        case JSTYPE_BOOLEAN: return "JSTYPE_BOOLEAN";             /* boolean */
        case JSTYPE_LIMIT: return "JSTYPE_LIMIT";
    }
    return "ILLEGAL";
}

}

namespace jslib {


#if _MSC_VER >= 1500
    // used for 'std::time_t'
    jsval as_jsval(JSContext *cx, __int64 theValue) {
        return as_jsval(cx, double(theValue));
    }
#endif

jsval as_jsval(JSContext *cx, bool theValue) {
    return BOOLEAN_TO_JSVAL(theValue);
}

jsval as_jsval(JSContext *cx, double theValue) {
    jsval myReturnValue;
    JS_NewDoubleValue(cx,theValue,&myReturnValue);
    return myReturnValue;
}

jsval as_jsval(JSContext *cx, float theValue) {
    return as_jsval(cx, double(theValue));
}

jsval as_jsval(JSContext *cx, long theValue) {
    return as_jsval(cx, double(theValue));
}

jsval as_jsval(JSContext *cx, unsigned long theValue) {
    return as_jsval(cx, double(theValue));
}

jsval as_jsval(JSContext *cx, unsigned long long theValue) {
    return as_jsval(cx, double(theValue));
}

jsval as_jsval(JSContext *cx, unsigned int theValue) {
    return as_jsval(cx, double(theValue));
}

jsval as_jsval(JSContext *cx, char theValue) {
    return as_jsval(cx, double(theValue));
}

jsval as_jsval(JSContext *cx, unsigned char theValue) {
    return as_jsval(cx, double(theValue));
}

jsval as_jsval(JSContext *cx, int theValue) {
    return INT_TO_JSVAL(theValue);
}

bool
isValidUTF8(const std::string & theU8String) {
#ifdef _WIN32
    AC_SIZE_TYPE myWCharSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, theU8String, -1, 0, 0);
    return myWCharSize != 0;
#else
    std::string::const_iterator it = theU8String.begin();
    while (it != theU8String.end()) {
        utf::code_point cp = utf::utf_traits<char>::decode(it, theU8String.end());
        if (!utf::is_valid_codepoint(cp)) {
            break;
        }
    }
    return it == theU8String.end();
#endif
};

jsval
as_jsval(JSContext *cx, const char * theU8String) {
    // convert from UTF8 to WideChars/UTF16
#ifdef _WIN32
    AC_SIZE_TYPE myWCharSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, theU8String, -1, 0, 0);
    if (myWCharSize == 0) {
        DWORD myLastError = GetLastError();
        ostringstream os;
        os << errorDescription(myLastError) << " '" << theU8String << "' hex:";
        for (unsigned i = 0; i < strlen(theU8String); ++i) {
            os << " " << std::hex << int(reinterpret_cast<const unsigned char*>(theU8String)[i]);
        }
        throw jslib::UnicodeException(os.str(), PLUS_FILE_LINE);
    }
    LPWSTR myWChars = new WCHAR[myWCharSize];
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, theU8String, -1, myWChars, myWCharSize);
    JSString * myString = JS_NewUCStringCopyZ(cx,reinterpret_cast<jschar*>(myWChars));
    delete [] myWChars;
    return STRING_TO_JSVAL(myString);
#else
    JSString * myString;
    try {
        std::basic_string<asl::Unsigned16> utf16String = conv::utf_to_utf<asl::Unsigned16>(theU8String, conv::stop);
        myString = JS_NewUCStringCopyN(cx,utf16String.c_str(), utf16String.size());
    } catch (conv::conversion_error & ex) {
        ostringstream os;
        os << ex.what() << " '" << theU8String << "' hex:";
        for (unsigned i = 0; i < strlen(theU8String); ++i) {
            os << " " << std::hex << int(reinterpret_cast<const unsigned char*>(theU8String)[i]);
        }
        throw jslib::UnicodeException(os.str(), PLUS_FILE_LINE);
    }
    return STRING_TO_JSVAL(myString);
#endif
}

jsval as_jsval(JSContext *cx, const std::string & theValue) {
    JSString * myString;
    try {
        std::basic_string<asl::Unsigned16> utf16String = conv::utf_to_utf<asl::Unsigned16>(theValue, conv::stop);
        myString = JS_NewUCStringCopyN(cx,utf16String.c_str(), utf16String.size());
    } catch (conv::conversion_error & ex) {
        ostringstream os;
        os << ex.what() << " '" << theValue << "' hex:";
        for (unsigned i = 0; i < theValue.size(); ++i) {
            os << " " << std::hex << int(theValue[i]);
        }
        throw jslib::UnicodeException(os.str(), PLUS_FILE_LINE);
    }
    return STRING_TO_JSVAL(myString);
}

jsval as_jsval(JSContext *cx, const std::basic_string<asl::Unsigned16> & theUTF16String) {
    JSString * myString = JS_NewUCStringCopyN(cx,reinterpret_cast<const jschar*>(theUTF16String.c_str()), theUTF16String.size());
    return STRING_TO_JSVAL(myString);
}

template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<bool> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<y60::AcBool> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<unsigned char> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<int> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<unsigned int> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<float> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<double> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<std::string> & theVector);
template Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::vector<std::vector<std::string> > & theVector);

void ensureParamCount(uintN argc, int theMinCount, int theMaxCount, const std::string & where) {
    if ( static_cast<int>(argc) < theMinCount) {
        throw Exception(string("Not enough arguments, ")+as_string(theMinCount)+" expected.", where);
    }
    if (theMaxCount && static_cast<int>(argc) > theMaxCount) {
        throw Exception(string("Too many arguments, ")+as_string(theMaxCount)+" accepted.", where);
    }
};
void ensureParamCount(uintN argc, int theMinCount, const std::string & where) {
    ensureParamCount(argc, theMinCount, 0, where);
};

void
dumpJSStack(JSContext *cx) {
    JSStackFrame* fp;
    JSStackFrame* iter = 0;

    int num = 0;
    while(0 != (fp = JS_FrameIterator(cx, &iter))) {
        if(!JS_IsNativeFrame(cx, fp)) {
            JSScript* script = JS_GetFrameScript(cx, fp);
            jsbytecode* pc = JS_GetFramePC(cx, fp);
            if(script && pc) {
                const char * filename = JS_GetScriptFilename(cx, script);
                int lineno =  JS_PCToLineNumber(cx, script, pc);

                const char * funname = 0;
                JSFunction * fun = JS_GetFrameFunction(cx, fp);
                if(fun) {
                    funname = JS_GetFunctionName(fun);
                }
                std::cerr<<"Stackframe "<<num<<":"<<"  lineno: "<<lineno<<" filename: "<<(filename ? filename : "-")<<" function: "<<(funname ? funname : "-")<<endl;
            }
        } else {
            std::cerr<<"Stackframe "<<num<<":"<<"native"<<endl;
        }
        ++num;
    }
}

JSStackFrame *
getStackFrame(int i, JSContext *cx) {
    /*try*/ {
        JSStackFrame* fp;
        JSStackFrame* iter = 0;
        int num = 0;

        while(0 != (fp = JS_FrameIterator(cx, &iter)))
        {
            if (num == i) {
                return fp;
            }
            ++num;
        }
        return 0;
    } /*HANDLE_CPP_EXCEPTION;*/
}

bool
getFileLine(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, const char * & filename, int & lineno) {
    try {
        uint16 n = 1;
        if (argc > 0 && JSVAL_IS_INT(argv[0])) {
            JS_ValueToUint16(cx, argv[0], &n);
        }

        JSStackFrame * fp = getStackFrame(n,cx);

        if (fp) {
            if(!JS_IsNativeFrame(cx, fp)) {
                JSScript* script = JS_GetFrameScript(cx, fp);
                jsbytecode* pc = JS_GetFramePC(cx, fp);
                if(script && pc)
                {
                    filename = JS_GetScriptFilename(cx, script);
                    lineno =  JS_PCToLineNumber(cx, script, pc);
                    return true;
                }
            }
        }
        return false;
    } HANDLE_CPP_EXCEPTION;
}


JSBool
JSA_DefineConstInts(JSContext *cx, JSObject *obj, JSConstIntPropertySpec *cds) {
    JSBool ok;
    uintN flags = JSPROP_READONLY | JSPROP_PERMANENT;

    for (ok = JS_TRUE; cds->name; cds++) {
        ok = JS_DefineProperty(cx, obj, cds->name, INT_TO_JSVAL(cds->value), NULL, NULL, flags);
        if (!ok) {
            break;
        }
    }
    return ok;
}

void
JSA_AddFunctions(JSContext *cx, JSObject *theClassCtr, JSFunctionSpec * theFunctions) {
    while (theFunctions->name) {
        DB(AC_TRACE << "adding function " << theFunctions->name << " to " << theClassCtr << endl);
        JSFunction * myFunction = JS_DefineFunction(cx, theClassCtr, theFunctions->name, theFunctions->call,
                theFunctions->nargs, 0);
        (void)myFunction;
        DB(AC_TRACE << "return:" << myFunction << endl);
        ++theFunctions;
    }
   // JS_DefineFunctions(cx, theClassCtr, Functions());

}

void
JSA_AddProperties(JSContext *cx, JSObject *theClassCtr, JSPropertySpec * theProperties) {
   JS_DefineProperties(cx, theClassCtr, theProperties);

}


std::string
ValType(JSContext *cx, jsval theVal) {
    std::string result = "{";
    if(JSVAL_IS_BOOLEAN(theVal)) {
        result+="BOOLEAN ";
    }
    if(JSVAL_IS_DOUBLE(theVal)) {
        result+="DOUBLE ";
    }
    if(JSVAL_IS_GCTHING(theVal)) {
        result+="GCTHING ";
    }
    if(JSVAL_IS_INT(theVal)) {
        result+="INT ";
    }
    if(JSVAL_IS_NULL(theVal)) {
        result+="NULL ";
    }
    if(JSVAL_IS_NUMBER(theVal)) {
        result+="NUMBER ";
    }
    if(JSVAL_IS_OBJECT(theVal)) {
        result+="OBJECT ";
    }
    if(JSVAL_IS_PRIMITIVE(theVal)) {
        result+="PRIMITIVE ";
    }
    if(JSVAL_IS_STRING(theVal)) {
        result+="STRING ";
    }
    if(JSVAL_IS_VOID(theVal)) {
        result+="VOID ";
    }
    result+="}";
    return result;
}

void
printParams(const char *name, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    cerr << name
         << "("
         //<<"obj = "<<as_string(cx,obj)
         <<"obj = "<<(void*)obj
         <<", obj->classname = "<<JSA_GetClass(cx,obj)->name
         <<", id='"<<as_string(cx,id)<<"'"
         <<",val="<<ValType(cx,*vp)<<as_string(cx,*vp)
         <<")"<<endl;
}
void
printParams(const char *name, JSContext *cx, JSObject *obj, JSType type, jsval *vp) {
    cerr << name
         << "("
         //<<"obj = "<<as_string(cx,obj)
         <<"obj = "<<(void*)obj
         <<", obj->classname = "<<JSA_GetClass(cx,obj)->name
         <<", type='"<<as_string(type)<<"'"
         <<",val="<<ValType(cx,*vp)<<as_string(cx,*vp)
         <<")"<<endl;
}
void
printParams(const char *name, JSContext *cx, JSObject *obj, jsval id) {
    cerr << name
         << "("
         //<<"obj = "<<as_string(cx,obj)
         <<"obj = "<<(void*)obj
         <<", obj->classname = "<<JSA_GetClass(cx,obj)->name
         <<", id='"<<as_string(cx,id)<<"'"
         <<")"<<endl;
}
void
printParams(const char *name, JSContext *cx, JSObject *obj) {
    cerr << name
         << "("
         //<<"obj = "<<as_string(cx,obj)
         <<"obj = "<<(void*)obj
         <<")"<<endl;
}

JSBool
NoisyAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    IF_NOISY(printParams("addProperty",cx,obj,id,vp));
    return JS_TRUE;
}
JSBool
NoisyDelProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    IF_NOISY(printParams("delProperty",cx,obj,id,vp));
    return JS_TRUE;
}
JSBool
NoisyGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    IF_NOISY(printParams("getProperty",cx,obj,id,vp));
    return JS_TRUE;
}
JSBool
NoisySetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    IF_NOISY(printParams("setProperty",cx,obj,id,vp));
    return JS_TRUE;
}
JSBool
NoisyEnumerate(JSContext *cx, JSObject *obj) {
    IF_NOISY(printParams("enumerate",cx,obj));
    return JS_TRUE;
}

JSBool
NoisyResolve(JSContext *cx, JSObject *obj, jsval id) {
    IF_NOISY(printParams("resolve",cx,obj,id));
    return JS_TRUE;
}

JSBool
NoisyNewResolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp) {
    IF_NOISY(
        printParams("newresolve",cx,obj,id);
        if (flags & JSRESOLVE_QUALIFIED) {
            AC_TRACE << "JSRESOLVE_QUALIFIED"<<endl;
        }
        if (flags & JSRESOLVE_ASSIGNING) {
            AC_TRACE << "JSRESOLVE_ASSIGNING"<<endl;
        }
    )
    *objp = 0;
    return JS_TRUE;
}

JSBool
NoisyConvert(JSContext *cx, JSObject *obj, JSType type, jsval *vp) {
    IF_NOISY_CONV(printParams("convert",cx,obj,type,vp));
    return JS_ConvertStub(cx,obj,type,vp);
}

//#define NOISY_FINALIZE
#ifdef NOISY_FINALIZE
JS_STATIC_DLL_CALLBACK(void)
NoisyFinalize(JSContext *cx, JSObject *obj) {
    IF_NOISY(AC_TRACE << "finalize " << (void*)obj << endl);
}
#endif


// all those following functions are slighly modified versions of private js engine
// functions to allow report of uncaught exception messages when
// calling js functions from c++

#ifdef USE_OLD_JS_ERROR_HANDLING

typedef JSBool
(* JS_DLL_CALLBACK JSDebugErrorHook)(JSContext *cx, const char *message,
                                     JSErrorReport *report, void *closure);


void
JSA_ReportErrorAgain(JSContext *cx, const char *message, JSErrorReport *reportp, JSErrorReporter onError)
{
    if (!message)
        return;

    if (cx->lastMessage)
        free(cx->lastMessage);
    cx->lastMessage = JS_strdup(cx, message);
    if (!cx->lastMessage)
        return;

    /*
     * If debugErrorHook is present then we give it a chance to veto
     * sending the error on to the regular ErrorReporter.
     */
    if (onError) {
        JSDebugErrorHook hook = cx->runtime->debugErrorHook;
        if (hook &&
            !hook(cx, cx->lastMessage, reportp,
                  cx->runtime->debugErrorHookData)) {
            onError = NULL;
        }
    }
    if (onError)
        onError(cx, cx->lastMessage, reportp);
}

 typedef struct JSExnPrivate {
     JSErrorReport *errorReport;
 } JSExnPrivate;


JSErrorFormatString AC_ErrorFormatString[JSErr_Limit] = {
#if JS_HAS_DFLT_MSG_STRINGS
#define MSG_DEF(name, number, count, exception, format) \
    { format, count } ,
#else
#define MSG_DEF(name, number, count, exception, format) \
    { NULL, count } ,
#endif
#include <js/spidermonkey/js.msg>
#undef MSG_DEF
};

static const JSErrorFormatString *
AC_GetErrorMessage(void *userRef, const char *locale, const uintN errorNumber)
{
    if ((errorNumber > 0) && (errorNumber < JSErr_Limit))
        return &AC_ErrorFormatString[errorNumber];
    return NULL;
}


JSErrorReport *
JSA_ErrorFromException(JSContext *cx, jsval exn)
{
    JSObject *obj;
    JSExnPrivate *privateData;
    //jsval privateValue;

    if (JSVAL_IS_PRIMITIVE(exn))
        return NULL;
    obj = JSVAL_TO_OBJECT(exn);
//    if (OBJ_GET_CLASS(cx, obj) != &ExceptionClass)
//        return NULL;

    privateData = (JSExnPrivate*) JS_GetPrivate(cx, obj);
    if (!privateData)
        return NULL;

//    JS_ASSERT(privateData->errorReport);
    return privateData->errorReport;
}

// from mozilla/jsexn.c
JSBool
JSA_reportUncaughtException(JSContext *cx, JSErrorReporter onError)
{
    JSObject *exnObject;
    JSString *str;
    jsval exn;
    JSErrorReport *reportp;
    const char *bytes;

    if (!JS_IsExceptionPending(cx))
        return JS_FALSE;

    if (!JS_GetPendingException(cx, &exn))
        return JS_FALSE;

    /*
     * Because js_ValueToString below could error and an exception object
     * could become unrooted, we root it here.
     */
    if (JSVAL_IS_OBJECT(exn) && exn != JSVAL_NULL) {
        exnObject = JSVAL_TO_OBJECT(exn);
        if (!JS_AddNamedRoot(cx, &exnObject, "exn.report.root"))
            return JS_FALSE;
    } else {
        exnObject = NULL;
    }

#if JS_HAS_ERROR_EXCEPTIONS
    reportp = JSA_ErrorFromException(cx, exn);
#else
    reportp = NULL;
#endif

    str = JS_ValueToString(cx, exn);
    bytes = str ? JS_GetStringBytes(str) : "null";

    if (reportp == NULL) {
        /*
         * XXXmccabe todo: Instead of doing this, synthesize an error report
         * struct that includes the filename, lineno where the exception was
         * originally thrown.
         */
        //JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL,
        JS_ReportErrorNumber(cx, AC_GetErrorMessage, NULL,
                             JSMSG_UNCAUGHT_EXCEPTION, bytes);
    } else {
        /* Flag the error as an exception. */
        reportp->flags |= JSREPORT_EXCEPTION;
        JSA_ReportErrorAgain(cx, bytes, reportp, onError);
    }

    if (exnObject != NULL)
        JS_RemoveRoot(cx, &exnObject);

    // NOTE: spidermonkey exceptions are always fatal because
    //       they indicate interpreter internal errors.
    abort();

    return JS_TRUE;
}
#endif

JSBool
JSA_CallFunctionName(JSContext * cx, JSObject * obj, const char * theName, int argc, jsval argv[], jsval* rval) {
    try {
        JSBool ok = JS_CallFunctionName(cx, obj, theName, argc, argv, rval);
        if (!ok && !QuitFlagSingleton::get().getQuitFlag()) {
            AC_DEBUG << "Exception while calling js function '" << theName << "'" << endl;
#ifdef USE_OLD_JS_ERROR_HANDLING
            JSA_reportUncaughtException(cx, cx->errorReporter);
#endif
        }
        return ok;
    } HANDLE_CPP_EXCEPTION;
}

JSBool
JSA_CallFunction(JSContext * cx, JSObject * obj, JSFunction *fun, int argc, jsval argv[], jsval* rval) {
    try {
        JSBool ok = JS_CallFunction(cx, obj, fun, argc, argv, rval);
        if (!ok && !QuitFlagSingleton::get().getQuitFlag()) {
            AC_DEBUG << "Exception while calling js function" << endl;
#ifdef USE_OLD_JS_ERROR_HANDLING
            JSA_reportUncaughtException(cx, cx->errorReporter);
#endif
        }
        return ok;
    } HANDLE_CPP_EXCEPTION;
}
JSBool
JSA_CallFunctionValue(JSContext * cx, JSObject * obj, jsval fval, int argc, jsval argv[], jsval* rval) {
    try {
        JSBool ok = JS_CallFunctionValue(cx, obj, fval, argc, argv, rval);
        if (!ok && !QuitFlagSingleton::get().getQuitFlag()) {
            AC_DEBUG << "Exception while calling js function " << endl;
#ifdef USE_OLD_JS_ERROR_HANDLING
            JSA_reportUncaughtException(cx, cx->errorReporter);
#endif
        }
        return ok;
    } HANDLE_CPP_EXCEPTION;
}



void
dumpJSObj(JSContext * cx, JSObject * obj) {
    AC_PRINT << "obj = " << (void*)obj
         << ", obj->classname = " << JSA_GetClass(cx,obj)->name;
    AC_PRINT << "Properties: ";
    JSIdArray * myIds = JS_Enumerate(cx, obj);
    if (!myIds) {
        AC_ERROR << "Could not enumerate children for js obj " << (void *)obj;
        return;
    }
    AC_PRINT << "Number of properties: " << myIds->length;
    for (int i=0; i< myIds->length; i++) {
        jsid id = myIds->vector[i];
        AC_PRINT << "  Property " << id << ": ";
        jsval myIdVal;
        if (JS_IdToValue(cx, id, &myIdVal)) {
            string myPropName;
            convertFrom(cx, myIdVal, myPropName);
            AC_PRINT << "    " << myPropName;
//                jsval myValue;
//                JS_GetProperty(cx, myValidOptions, myPropName.c_str(), &myValue);
//                string myValueString;
//                if (myValue != JSVAL_NULL) {
//                    convertFrom(cx, myValue, myValueString);
//                }
        }
    }
    JS_DestroyIdArray(cx, myIds);
}

JSBool
JSA_hasFunction(JSContext * cx, JSObject * obj, const char * theName) {
    try {
        jsval myVal;
        JSBool ok = JS_GetProperty(cx, obj, theName, &myVal);
//        dumpJSObj(cx, obj);
        if  (ok == JS_TRUE && myVal != JSVAL_VOID) {
            if (JS_ValueToFunction(cx, myVal) != NULL) {
                return JS_TRUE;
            } else {
                AC_ERROR << "Property '" << theName << "' is not a function." << endl;
#ifdef USE_OLD_JS_ERROR_HANDLING
                JSA_reportUncaughtException(cx, cx->errorReporter);
#endif
            }
        }
        return JS_FALSE;
    } HANDLE_CPP_EXCEPTION;
}

JSBool
JSA_charArrayToString(JSContext *cx, jsval *argv, string & theResult) {
    // try to convert first argument into an object
    JSObject * myArgument;
    if (!JS_ValueToObject(cx, argv[0], &myArgument)) {
        return JS_TRUE;
    }

    // try to get values from array
    jsuint myArrayLength = 0;
    if (!JS_HasArrayLength(cx, myArgument, &myArrayLength)) {
        return JS_TRUE;
    }

    for (unsigned i = 0; i < myArrayLength; ++i) {
        jsval myArgElement;
        unsigned char  myCharacter;
        if (!JS_GetElement(cx, myArgument, i, &myArgElement)) {
            JS_ReportError(cx, "JSSocket::charArrayToString(): argument %d does not exist", i);
            return JS_FALSE;
        }

        if (!convertFrom(cx, myArgElement, myCharacter)) {
            JS_ReportError(cx, "JSSocket::charArrayToString(): Array element #%d must be a character", i);
            return JS_FALSE;
        }

        theResult.append(1, myCharacter);
    }

    return JS_TRUE;
}

JSBool
JSA_ArrayToString(JSContext * cx, jsval * vp, string & theResult) {
    // try to convert first argument into an object
    JSObject * myJSArray;
    if (!JS_ValueToObject(cx, *vp, &myJSArray)) {
        return JS_FALSE;
    }

    if (!myJSArray) {
        return JS_FALSE;
    }

    if (!JS_IsArrayObject(cx, myJSArray)) {
        return JS_FALSE;
    }

    // try to get values from array
    jsuint myArrayLength = 0;
    if (!JS_GetArrayLength(cx, myJSArray, &myArrayLength)) {
        return JS_FALSE;
    }

    theResult += "[";
    for (unsigned i = 0; i < myArrayLength; ++i) {
        jsval myArrayElement;
        if (!JS_GetElement(cx, myJSArray, i, &myArrayElement)) {
            JS_ReportError(cx, "JSA_ArrayToString: array element %d does not exist.", i);
            return JS_FALSE;
        }

        string myString;
        if (!convertFrom(cx, myArrayElement, myString)) {
            JS_ReportError(cx, "JSA_ArrayToString: Could not convert array element #%d to string.", i);
            return JS_FALSE;
        }

        if (JSVAL_IS_STRING(myArrayElement)) {
            // TODO: escape the string
            theResult += VECTOR_OF_STRING_DELIMITER + myString + VECTOR_OF_STRING_DELIMITER;
        } else {
            theResult += myString;
        }
        if (i < myArrayLength - 1) {
            theResult += ",";
        }
    }
    theResult += "]";

    return JS_TRUE;
}


std::string
searchFileRelativeToJSInclude(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, const std::string & theFile) {
    const char * myCurrentFile;
    int myLine;
    if (!getFileLine(cx, obj, argc, argv, myCurrentFile, myLine)) {
        JS_ReportError(cx, "Failed to determine current JS include file.");
        return "";
    }
    std::string myIncludePath = asl::getDirectoryPart(myCurrentFile);

    std::string myFileWithPath = asl::searchFile(theFile, myIncludePath);
    if (myFileWithPath.empty()) {
          JS_ReportError(cx, "File '%s' not found in %s", theFile.c_str(), myIncludePath.c_str());
          return "";
    }
    return myFileWithPath;
}

bool convertFrom(JSContext *cx, jsval theValue, double & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) &&
        !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        theDest = double(myDoubleDest);
        return true;
    }
    //theDest = asl::Time::double(theValue);
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, float & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) &&
        !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        theDest = (float)myDoubleDest;
        return true;
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, short & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) &&
        !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        if ((myDoubleDest < std::numeric_limits<short>::min()) ||
            (myDoubleDest > std::numeric_limits<short>::max()))
        {
            JS_ReportError(cx, "#WARNING convertFrom: -> unsigned short: value out of range: %g", myDoubleDest);
        }
        theDest = (short)(myDoubleDest);
        return true;
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, unsigned short & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) &&
        !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        if ((myDoubleDest < std::numeric_limits<unsigned short>::min()) ||
            (myDoubleDest > std::numeric_limits<unsigned short>::max()))
        {
            JS_ReportError(cx, "#WARNING convertFrom: -> unsigned short: value out of range: %g", myDoubleDest);
        }
        theDest = (unsigned short)(myDoubleDest);
        return true;
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, char & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) &&
        !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        if ((myDoubleDest < std::numeric_limits<char>::min()) ||
            (myDoubleDest > std::numeric_limits<char>::max()))
        {
            JS_ReportError(cx, "#WARNING convertFrom: -> unsigned short: value out of range: %g", myDoubleDest);
        }
        theDest = (char)(myDoubleDest);
        return true;
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, unsigned char & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) && !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        if ((myDoubleDest < asl::NumericTraits<unsigned char>::min()) ||
            (myDoubleDest > asl::NumericTraits<unsigned char>::max()))
        {
            AC_ERROR << "min="<< asl::NumericTraits<unsigned char>::min();
            AC_ERROR << "max="<< asl::NumericTraits<unsigned char>::max();

            JS_ReportError(cx, "#WARNING convertFrom: -> unsigned char: value out of range: %g", myDoubleDest);
        }
        theDest = (unsigned char)(myDoubleDest);
        return true;
    }
    return false;
}


bool convertFrom(JSContext *cx, jsval theValue, int & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) &&
        !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        if ((myDoubleDest < asl::NumericTraits<int>::min()) ||
            (myDoubleDest > asl::NumericTraits<int>::max()))
        {
            AC_ERROR << "min="<< asl::NumericTraits<int>::min();
            AC_ERROR << "max="<< asl::NumericTraits<int>::max();
            JS_ReportError(cx, "#WARNING convertFrom: -> int: value out of range: %g", myDoubleDest);
        }
        theDest = (int)(myDoubleDest);
        return true;
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, unsigned int & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) &&
        !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        if ((myDoubleDest < asl::NumericTraits<unsigned int>::min()) ||
            (myDoubleDest > asl::NumericTraits<unsigned int>::max()))
        {
            AC_ERROR << "min="<< asl::NumericTraits<unsigned int>::min();
            AC_ERROR << "max="<< asl::NumericTraits<unsigned int>::max();

            JS_ReportError(cx, "#WARNING convertFrom: -> unsigned int: value out of range: %g", myDoubleDest);
        }
        theDest = (unsigned int)(myDoubleDest);
        return true;
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, unsigned long & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) &&
        !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        if ((myDoubleDest < asl::NumericTraits<unsigned long>::min()) ||
            (myDoubleDest > asl::NumericTraits<unsigned long>::max()))
        {
            JS_ReportError(cx, "#WARNING convertFrom: -> unsigned long: value out of range: %g", myDoubleDest);
        }
        theDest = (unsigned long)(myDoubleDest);
        return true;
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, long & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) &&
        !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        if ((myDoubleDest < asl::NumericTraits<long>::min()) ||
            (myDoubleDest > asl::NumericTraits<long>::max()))
        {
            JS_ReportError(cx, "#WARNING convertFrom: -> long: value out of range: %g", myDoubleDest);
        }
        theDest = long(myDoubleDest);
        return true;
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, bool & theDest) {
    JSBool myBool;
    if (JS_ValueToBoolean(cx, theValue, &myBool))
    {
        theDest = 0 != myBool;
        return true;
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, std::string & theDest) {
    theDest = asl::as_string(cx, theValue);
    return true;
}

bool convertFrom(JSContext *cx, jsval theValue, JSObject * & theDest) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            theDest = myArgument;
            return true;
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Time & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) && !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        theDest = asl::Time(myDoubleDest);
        return true;
    }
    return false;
}

template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<double> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<float> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<unsigned short> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<int> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<unsigned int> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<unsigned long> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<long> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<bool> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<std::string> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<JSObject *> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<unsigned char> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<asl::Time> & theVector);
template Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::vector<dom::NodePtr> & theVector);

}
