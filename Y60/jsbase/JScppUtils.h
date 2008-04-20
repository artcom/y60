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
#include <y60/AcBool.h>
#include <dom/Value.h>

#include <paintlib/plexcept.h>

#include <iostream>
// Undefine stuff to undo Apple namespace pollution
#ifdef check
#undef check
#endif

#ifdef nil
#undef nil
#endif

#ifdef DestroyNotify
#undef DestroyNotify
#endif

//#include <glibmm.h>

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
#define EXITCODE_FILE_NOT_FOUND 4

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

#define ENSURE_ARG(TYPE, VARNAME, INDEX) \
    TYPE VARNAME; \
    if ( ! convertFrom(cx, argv[INDEX], VARNAME)) { \
        throw JSArgMismatch(std::string("Failed to convert argument ") +  \
                        #INDEX  + " to type " + #TYPE, PLUS_FILE_LINE); \
    }



//=============================================================================

JSBool JSA_reportUncaughtException(JSContext *cx, JSErrorReporter onError);

JSBool
JSA_CallFunctionName(JSContext * cx, JSObject * obj, const char * theName, int argc, jsval argv[], jsval* rval);

JSBool
JSA_CallFunction(JSContext * cx, JSObject * obj, JSFunction *fun, int argc, jsval argv[], jsval* rval);

JSBool
JSA_CallFunctionValue(JSContext * cx, JSObject * obj, jsval fval, int argc, jsval argv[], jsval* rval);

void
dumpJSObj(JSContext * cx, JSObject * obj);

JSBool
JSA_hasFunction(JSContext * cx, JSObject * obj, const char * theName);

struct JSConstIntPropertySpec {
    const char      *name;
    intN            tinyid;
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

void dumpJSStack(JSContext *cx, FILE * theTarget = stderr);
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
struct JValueTypeTraits {
};

template <>
struct JValueTypeTraits<float> {
    typedef float self_type;
    typedef float elem_type;
    typedef dom::SimpleValue<self_type> wrapper_type;
    static std::string Postfix() {
        return "f";
    }
    static std::string Name() {
        return "float";
    }
    enum {SIZE = 1};
};

template <>
struct JValueTypeTraits<int> {
    typedef int self_type;
    typedef int elem_type;
    typedef dom::SimpleValue<self_type> wrapper_type;
    static std::string Postfix() {
        return "i";
    }
    static std::string Name() {
        return "int";
    }
    enum {SIZE = 1};
};

template <>
struct JValueTypeTraits<unsigned long> {
    typedef unsigned long self_type;
    typedef unsigned long elem_type;
    typedef dom::SimpleValue<self_type> wrapper_type;
    static std::string Postfix() {
        return "l";
    }
    static std::string Name() {
        return "unsignedLong";
    }
    enum {SIZE = 1};
};

template <>
struct JValueTypeTraits<unsigned int> {
    typedef unsigned int self_type;
    typedef unsigned int elem_type;
    typedef dom::SimpleValue<self_type> wrapper_type;
    static std::string Postfix() {
        return "ui";
    }
    static std::string Name() {
        return "unsignedInt";
    }
    enum {SIZE = 1};
};

jsval as_jsval(JSContext *cx, bool theValue);
jsval as_jsval(JSContext *cx, double theValue);
jsval as_jsval(JSContext *cx, float theValue); 
jsval as_jsval(JSContext *cx, long theValue);
jsval as_jsval(JSContext *cx, unsigned long theValue); 
jsval as_jsval(JSContext *cx, int theValue); 
jsval as_jsval(JSContext *cx, unsigned int theValue); 
jsval as_jsval(JSContext *cx, char theValue); 
jsval as_jsval(JSContext *cx, unsigned char theValue); 
jsval as_jsval(JSContext *cx, const char * theU8String);
jsval as_jsval(JSContext *cx, const std::string & theValue); 
jsval as_jsval(JSContext *cx, const std::basic_string<asl::Unsigned16> & theUTF16String);

template <class T>
jsval as_jsval(JSContext *cx, const std::vector<T> & theVector);

template <class T> struct JSClassTraits;

template <class T>
bool convertFrom(JSContext *cx, jsval theValue, typename dom::ValueWrapper<T>::Type & thePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<T>::Class()) {
                thePtr = JSClassTraits<T>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, double & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, float & theDest);
bool convertFrom(JSContext *cx, jsval theValue, short & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, unsigned short & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, int & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, unsigned int & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, long & theDest);
bool convertFrom(JSContext *cx, jsval theValue, unsigned long & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, char & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, unsigned char & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, bool & theDest);
bool convertFrom(JSContext *cx, jsval theValue, std::string & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, JSObject * & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, asl::Time & theDest); 
bool convertFrom(JSContext *cx, jsval theValue, asl::Block & theDest); 

template <class T>
bool convertFrom(JSContext *cx, jsval theValue, std::vector<T> & theDest);

JSBool JSA_charArrayToString(JSContext *cx, jsval *argv, std::string & theResult);

// Converts an array into a string, if argv is not an array it returns false.
JSBool JSA_ArrayToString(JSContext *cx, jsval *argv, std::string & theResult);

std::string
searchFileRelativeToJSInclude(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                              const std::string & theFile);

}

#endif
