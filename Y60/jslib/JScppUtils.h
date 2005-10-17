//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JScppUtils.h,v $
//   $Author: pavel $
//   $Revision: 1.12 $
//   $Date: 2005/04/24 00:41:19 $
//
//
//=============================================================================

#ifndef _Y60_JSCPPUTILS_INCLUDED_
#define _Y60_JSCPPUTILS_INCLUDED_

#include "jssettings.h"

#include <js/jspubtd.h>
#include <js/jsapi.h>
#include <js/jscntxt.h>
#include <js/jsnum.h>

#include <asl/settings.h>
#include <asl/string_functions.h>
#include <asl/file_functions.h>
#include <asl/numeric_functions.h>
#include <asl/Time.h>
#include <asl/Logger.h>
#include <dom/Value.h>

#include <paintlib/plexcept.h>


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
        JS_ReportError(cx,"%s", asl::as_string(ex).c_str());\
        return JS_FALSE;\
    } catch (std::exception & ex) {\
        JS_ReportError(cx,"%s", ex.what());\
        return JS_FALSE;\
    } catch (const PLTextException & ex) {\
        JS_ReportError(cx,"%s", asl::as_string(ex).c_str());\
        return JS_FALSE;\
    } catch (...) {\
        JS_ReportError(cx,"Unknown Exception caught");\
        return JS_FALSE;\
    }

//=============================================================================


DEFINE_EXCEPTION(JSArgMismatch, asl::Exception);

#define ENSURE_ARG(TYPE, VARNAME, INDEX) \
    TYPE VARNAME; \
    if ( ! convertFrom(cx, argv[INDEX], VARNAME)) { \
        throw JSArgMismatch(std::string("Failed to convert argument ") +  \
                        #INDEX  + " to type " + #TYPE, PLUS_FILE_LINE); \
    }



//=============================================================================

JSBool JSA_reportUncaughtException(JSContext *cx);

JSBool
JSA_CallFunctionName(JSContext * cx, JSObject * obj, const char * theName, int argc, jsval argv[], jsval* rval);

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

inline
jsval as_jsval(JSContext *cx, const std::string & theValue) {
    JSString * myString = JS_NewStringCopyN(cx,theValue.c_str(),theValue.size());
    return STRING_TO_JSVAL(myString);
}

template <class T>
jsval as_jsval(JSContext *cx, const std::vector<T> & theVector) {
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    for (unsigned i = 0; i < theVector.size(); ++i) {
        jsval myValue = as_jsval(cx, theVector[i]);
        if (!myValue || !JS_SetElement(cx, myReturnObject, i, &myValue)) {
            return JS_FALSE;
        }
    }
    return OBJECT_TO_JSVAL(myReturnObject);
}

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

inline
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

inline
bool convertFrom(JSContext *cx, jsval theValue, float & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) &&
        !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        theDest = float(myDoubleDest);
        return true;
    }
    return false;
}

inline
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

inline
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

inline
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
inline
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

inline
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

inline
bool convertFrom(JSContext *cx, jsval theValue, bool & theDest) {
    JSBool myBool;
    if (JS_ValueToBoolean(cx, theValue, &myBool))
    {
        theDest = bool(myBool);
        return true;
    }
    return false;
}

inline
bool convertFrom(JSContext *cx, jsval theValue, std::string & theDest) {
    JSString *myJSStr = JS_ValueToString(cx, theValue);
    if (!myJSStr) {
        return JS_TRUE;
    }
    theDest = JS_GetStringBytes(myJSStr);
    return true;
}

inline
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

inline
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

template <class T>
bool convertFrom(JSContext *cx, jsval theValue, std::vector<T> & theDest) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            jsuint myArrayLength = 0;
            if (JS_HasArrayLength(cx,myArgument,&myArrayLength)) {
                theDest.resize(myArrayLength);
                jsval myArgElem;
                for (int i = 0; i < myArrayLength; ++i) {
                    if (JS_GetElement(cx, myArgument, i, &myArgElem)) {
                        T myResult;
                        if (convertFrom(cx, myArgElem, myResult)) {
                            theDest[i] = myResult;
                        } else {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

inline
bool convertFrom(JSContext *cx, jsval theValue, asl::Time & theDest) {
    jsdouble myDoubleDest = -1;
    if (JS_ValueToNumber(cx, theValue, &myDoubleDest) && !JSDOUBLE_IS_NaN(myDoubleDest) )
    {
        theDest = asl::Time(myDoubleDest);
        return true;
    }
    return false;
}

JSBool JSA_charArrayToString(JSContext *cx, jsval *argv, std::string & theResult);

std::string
searchFileRelativeToJSInclude(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                              const std::string & theFile);

}

#endif
