//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_JSCPPUTILS_INCLUDED_
#define _Y60_JSCPPUTILS_INCLUDED_

#include "jssettings.h"

#include <js/jspubtd.h>
#include <js/jsapi.h>
#include <js/jscntxt.h>
#include <js/jsnum.h>
#include <js/jslock.h>

#include <asl/settings.h>
#include <asl/string_functions.h>
#include <asl/file_functions.h>
#include <asl/numeric_functions.h>
#include <asl/Time.h>
#include <asl/Logger.h>
#include <dom/Value.h>

#include <paintlib/plexcept.h>

#ifdef check
#undef check
//#warning undefined check to undo Apple namespace pollution
#endif

#ifdef nil
#undef nil
//#warning undefined nil to undo Apple namespace pollution
#endif

#ifdef DestroyNotify
#undef DestroyNotify
//#warning undefined data to undo Apple namespace pollution
#endif

#include <limits>

#define IF_NOISY(x)  // x
#define IF_NOISY2(x) // x
#define IF_NOISY_CONV(x) // x
#define IF_NOISY_F(x) // x
#define IF_NOISY_Y(x) x
#define IF_REG(x) // x

#undef min
#undef max

namespace asl {

    std::string as_string(JSContext *cx, jsval theVal);
    std::string as_string(JSContext *cx, JSObject *theObj);
    std::string as_string(JSType theType);
}

namespace jslib {

    struct JSCallArgs {
        JSCallArgs(JSContext *the_cx, JSObject *the_obj, uintN the_argc, jsval *the_argv, jsval *the_rval)
            : cx(the_cx), obj(the_obj), argc(the_argc), argv(the_argv), rval(the_rval)
        {}
        JSContext *cx;
        JSObject *obj;
        uintN argc;
        jsval *argv;
        jsval *rval;
    };

    //=============================================================================

#ifdef JS_THREADSAFE
#define DoBeginRequest(cx) JS_BeginRequest((cx))
#define DoEndRequest(cx)   JS_EndRequest((cx))
#else
#define DoBeginRequest(cx) ((void)0)
#define DoEndRequest(cx)   ((void)0)
#endif

    //=============================================================================

#define EXITCODE_RUNTIME_ERROR 3

// Unused?
// #define EXITCODE_FILE_NOT_FOUND 4

#define HANDLE_CPP_EXCEPTION\
    catch (asl::Exception & ex) {\
        JSRuntime * myRuntime=JS_GetRuntime(cx);\
        JS_ReportError(cx,"%s", asl::as_string(ex).c_str());\
        return JS_FALSE;\
    } catch (std::exception & ex) {\
        JSRuntime * myRuntime=JS_GetRuntime(cx);\
        JS_ReportError(cx,"%s", ex.what());\
        return JS_FALSE;\
    } catch (const PLTextException & ex) {\
        JSRuntime * myRuntime=JS_GetRuntime(cx);\
        JS_ReportError(cx,"%s", asl::as_string(ex).c_str());\
        return JS_FALSE;\
    } catch (...) {\
        JSRuntime * myRuntime=JS_GetRuntime(cx);\
        JS_ReportError(cx,"Unknown Exception caught");\
        return JS_FALSE;\
    }

//=============================================================================


DEFINE_EXCEPTION(JSArgMismatch, asl::Exception);
DEFINE_EXCEPTION(UnicodeException, asl::Exception);

/*
 * Unused?
#define ENSURE_ARG(TYPE, VARNAME, INDEX) \
    TYPE VARNAME; \
    if ( ! convertFrom(cx, argv[INDEX], VARNAME)) { \
        throw JSArgMismatch(std::string("Failed to convert argument ") +  \
                        #INDEX  + " to type " + #TYPE, PLUS_FILE_LINE); \
    }
*/

//=============================================================================

JSBool JSA_reportUncaughtException(JSContext *cx, JSErrorReporter onError);

JSBool
JSA_CallFunctionName(JSContext * cx, JSObject * obj, const char * theName, int argc, jsval argv[], jsval* rval);

void
dumpJSObj(JSContext * cx, JSObject * obj);

JSBool
JSA_hasFunction(JSContext * cx, JSObject * obj, const char * theName);

struct JSConstIntPropertySpec {
    const char      *name;
    int8            tinyid;
    int             value;
};

inline
JSClass *
JSA_GetClass(JSContext *cx, JSObject *obj) {
#ifdef JS_THREADSAFE
    return JS_GetClass(cx, obj);
#else
    return JS_GetClass(obj);
#endif
}

void ensureParamCount(uintN argc, int theMinCount, int theMaxCount = 0);

JSStackFrame * getStackFrame(int i, JSContext *cx);
bool getFileLine(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, const char * & filename, int & lineno);

void JSA_AddFunctions(JSContext *cx, JSObject *theClassCtr, JSFunctionSpec * theFunctions);
void JSA_AddProperties(JSContext *cx, JSObject *theClassCtr, JSPropertySpec * theProperties);
JSBool JSA_DefineConstInts(JSContext *cx, JSObject *obj, JSConstIntPropertySpec *cds);

std::string ValType(JSContext *cx, jsval theVal);

void printParams(const char *name, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
void printParams(const char *name, JSContext *cx, JSObject *obj, JSType type, jsval *vp);
void printParams(const char *name, JSContext *cx, JSObject *obj, jsval id);
void printParams(const char *name, JSContext *cx, JSObject *obj);

JSBool
NoisyAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

JSBool
NoisyDelProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

JSBool
NoisyGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

JSBool
NoisySetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

JSBool
NoisyEnumerate(JSContext *cx, JSObject *obj);

JSBool
NoisyResolve(JSContext *cx, JSObject *obj, jsval id);

JSBool
NoisyNewResolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp);

JSBool
NoisyConvert(JSContext *cx, JSObject *obj, JSType type, jsval *vp);

JS_STATIC_DLL_CALLBACK(void)
NoisyFinalize(JSContext *cx, JSObject *obj);

template <class T>
struct JValueTypeTraits;

inline
jsval as_jsval(JSContext *cx, bool theValue) {
    return BOOLEAN_TO_JSVAL(theValue);
}

inline
jsval as_jsval(JSContext *cx, double theValue) {
    jsval myReturnValue;
    JS_NewDoubleValue(cx,theValue,&myReturnValue);
    return myReturnValue;
}

inline
jsval as_jsval(JSContext *cx, float theValue) {
    return as_jsval(cx, double(theValue));
}

inline
jsval as_jsval(JSContext *cx, long theValue) {
    return as_jsval(cx, double(theValue));
}

inline
jsval as_jsval(JSContext *cx, unsigned long theValue) {
    return as_jsval(cx, double(theValue));
}

inline
jsval as_jsval(JSContext *cx, unsigned int theValue) {
    return as_jsval(cx, double(theValue));
}

inline
jsval as_jsval(JSContext *cx, int theValue) {
    return INT_TO_JSVAL(theValue);
}

jsval as_jsval(JSContext *cx, const char * theU8String);

inline
jsval as_jsval(JSContext *cx, const std::string & theValue) {
    return as_jsval(cx, theValue.c_str());
}

inline
jsval as_jsval(JSContext *cx, const std::basic_string<asl::Unsigned16> & theUTF16String) {
    JSString * myString = JS_NewUCStringCopyZ(cx,reinterpret_cast<const jschar*>(theUTF16String.c_str()));
    return STRING_TO_JSVAL(myString);    
}

template <class T>
jsval as_jsval(JSContext *cx, const std::vector<T> & theVector);

template <class T> struct JSClassTraits;

template <class T>
bool convertFrom(JSContext *cx, jsval theValue, typename dom::ValueWrapper<T>::Type & thePtr);

bool convertFrom(JSContext *cx, jsval theValue, double & theDest);

bool convertFrom(JSContext *cx, jsval theValue, float & theDest);

bool convertFrom(JSContext *cx, jsval theValue, unsigned short & theDest); 

bool convertFrom(JSContext *cx, jsval theValue, int & theDest); 

bool convertFrom(JSContext *cx, jsval theValue, unsigned int & theDest); 

bool convertFrom(JSContext *cx, jsval theValue, unsigned long & theDest);

bool convertFrom(JSContext *cx, jsval theValue, long & theDest); 

bool convertFrom(JSContext *cx, jsval theValue, bool & theDest); 

bool convertFrom(JSContext *cx, jsval theValue, std::string & theDest); 

bool convertFrom(JSContext *cx, jsval theValue, JSObject * & theDest);

bool convertFrom(JSContext *cx, jsval theValue, unsigned char & theDest); 

template <class T>
bool convertFrom(JSContext *cx, jsval theValue, std::vector<T> & theDest);

bool convertFrom(JSContext *cx, jsval theValue, asl::Time & theDest); 

JSBool JSA_charArrayToString(JSContext *cx, jsval *argv, std::string & theResult);

// Converts an array into a string, if argv is not an array it returns false.
JSBool JSA_ArrayToString(JSContext *cx, jsval *argv, std::string & theResult);

std::string
searchFileRelativeToJSInclude(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                              const std::string & theFile);

}

#endif
