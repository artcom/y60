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

#ifndef _Y60_JSCPPUTILS_INCLUDED_
#define _Y60_JSCPPUTILS_INCLUDED_

#include "y60_jsbase_settings.h"

#include <iostream>
#include <limits>

#include "jssettings.h"

#ifdef USE_LEGACY_SPIDERMONKEY
#include <js/spidermonkey/jspubtd.h>
#include <js/spidermonkey/jsapi.h>
#include <js/spidermonkey/jscntxt.h>
#include <js/spidermonkey/jsnum.h>
#include <js/spidermonkey/jslock.h>
#else
#include <js/jspubtd.h>
#include <js/jsapi.h>
#include <js/jscntxt.h>
#include <js/jsnum.h>
#include <js/jslock.h>
#endif

#include <asl/base/settings.h>
#include <asl/base/string_functions.h>
#include <asl/base/file_functions.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/Time.h>
#include <asl/base/Logger.h>
#include <y60/base/AcBool.h>
#include <asl/dom/Value.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plexcept.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

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

#define IF_NOISY(x)  // x
#define IF_NOISY2(x) // x
#define IF_NOISY_CONV(x) // x
#define IF_NOISY_F(x) // x
#define IF_NOISY_Y(x) x
#define IF_REG(x) // x

#undef min
#undef max

namespace asl {

    Y60_JSBASE_DECL std::string as_string(JSContext *cx, jsval theVal);
    Y60_JSBASE_DECL std::string as_string(JSContext *cx, JSObject *theObj);
    Y60_JSBASE_DECL std::string as_string(JSType theType);
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
        JSRuntime * myRuntime=JS_GetRuntime(cx);(void)myRuntime;\
        JS_ReportError(cx,"%s", asl::as_string(ex).c_str());\
        return JS_FALSE;\
    } catch (std::exception & ex) {\
        JSRuntime * myRuntime=JS_GetRuntime(cx);(void)myRuntime;\
        JS_ReportError(cx,"%s", ex.what());\
        return JS_FALSE;\
    } catch (const PLTextException & ex) {\
        JSRuntime * myRuntime=JS_GetRuntime(cx);(void)myRuntime;\
        JS_ReportError(cx,"%s", asl::as_string(ex).c_str());\
        return JS_FALSE;\
    } catch (...) {\
        JSRuntime * myRuntime=JS_GetRuntime(cx);(void)myRuntime;\
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
#ifdef USE_OLD_JS_ERROR_HANDLING
Y60_JSBASE_DECL JSBool
JSA_reportUncaughtException(JSContext *cx, JSErrorReporter onError);
#endif

Y60_JSBASE_DECL JSBool
JSA_CallFunctionName(JSContext * cx, JSObject * obj, const char * theName, int argc, jsval argv[], jsval* rval);

Y60_JSBASE_DECL JSBool
JSA_CallFunctionName(JSContext * cx, JSObject * theThisObject, JSObject * theObject, const char * theName, uintN argc, jsval argv[], jsval *rval);

Y60_JSBASE_DECL  JSBool
JSA_CallFunction(JSContext * cx, JSObject * obj, JSFunction *fun, int argc, jsval argv[], jsval* rval);

Y60_JSBASE_DECL JSBool
JSA_CallFunctionValue(JSContext * cx, JSObject * obj, jsval fval, int argc, jsval argv[], jsval* rval);

void
dumpJSObj(JSContext * cx, JSObject * obj);

Y60_JSBASE_DECL JSBool
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

Y60_JSBASE_DECL void ensureParamCount(uintN argc, int theMinCount, int theMaxCount, const std::string & where = "");
Y60_JSBASE_DECL void ensureParamCount(uintN argc, int theMinCount, const std::string & where = "");

Y60_JSBASE_DECL void dumpJSStack(JSContext *cx);
Y60_JSBASE_DECL JSStackFrame * getStackFrame(int i, JSContext *cx);
Y60_JSBASE_DECL bool getFileLine(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, const char * & filename, int & lineno);

Y60_JSBASE_DECL void JSA_AddFunctions(JSContext *cx, JSObject *theClassCtr, JSFunctionSpec * theFunctions);
Y60_JSBASE_DECL void JSA_AddProperties(JSContext *cx, JSObject *theClassCtr, JSPropertySpec * theProperties);
Y60_JSBASE_DECL JSBool JSA_DefineConstInts(JSContext *cx, JSObject *obj, JSConstIntPropertySpec *cds);

Y60_JSBASE_DECL std::string ValType(JSContext *cx, jsval theVal);

Y60_JSBASE_DECL void printParams(const char *name, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
Y60_JSBASE_DECL void printParams(const char *name, JSContext *cx, JSObject *obj, JSType type, jsval *vp);
Y60_JSBASE_DECL void printParams(const char *name, JSContext *cx, JSObject *obj, jsval id);
Y60_JSBASE_DECL void printParams(const char *name, JSContext *cx, JSObject *obj);

Y60_JSBASE_DECL JSBool
NoisyAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

Y60_JSBASE_DECL JSBool
NoisyDelProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

Y60_JSBASE_DECL JSBool
NoisyGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

Y60_JSBASE_DECL JSBool
NoisySetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

Y60_JSBASE_DECL JSBool
NoisyEnumerate(JSContext *cx, JSObject *obj);

Y60_JSBASE_DECL JSBool
NoisyResolve(JSContext *cx, JSObject *obj, jsval id);

Y60_JSBASE_DECL JSBool
NoisyNewResolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp);

Y60_JSBASE_DECL JSBool
NoisyConvert(JSContext *cx, JSObject *obj, JSType type, jsval *vp);

#ifndef JS_STATIC_DLL_CALLBACK
#define JS_STATIC_DLL_CALLBACK(x) x
#endif

#ifdef NOISY_FINALIZE
JS_STATIC_DLL_CALLBACK(void)
NoisyFinalize(JSContext *cx, JSObject *obj);
#endif

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

Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, time_t theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, bool theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, double theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, float theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, long theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, unsigned long theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, unsigned long long theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, int theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, unsigned int theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, char theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, unsigned char theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const char * theU8String);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::string & theValue);
Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, const std::basic_string<asl::Unsigned16> & theUTF16String);

template <class T>
jsval as_jsval(JSContext *cx, T*);

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

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, double & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, float & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, short & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, unsigned short & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, int & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, unsigned int & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, long & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, unsigned long & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, char & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, unsigned char & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, bool & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, std::string & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, JSObject * & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Time & theDest);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Block & theDest);

template <class T>
bool convertFrom(JSContext *cx, jsval theValue, std::vector<T> & theDest);

Y60_JSBASE_DECL JSBool JSA_charArrayToString(JSContext *cx, jsval *argv, std::string & theResult);

// Converts an array into a string, if argv is not an array it returns false.
Y60_JSBASE_DECL JSBool JSA_ArrayToString(JSContext *cx, jsval *argv, std::string & theResult);

Y60_JSBASE_DECL std::string
searchFileRelativeToJSInclude(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                              const std::string & theFile);

Y60_JSBASE_DECL bool 
isValidUTF8(const std::string & theU8String);

}

#endif
