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
//   Description: Wrapper for JSGLResourceManager Singelton 
//
//=============================================================================

#ifndef _Y60_JSLIB_JSGLRESOURCEMANAGER_INCLUDED_
#define _Y60_JSLIB_JSGLRESOURCEMANAGER_INCLUDED_

#include "JSWrapper.h"

#include <y60/GLResourceManager.h>

namespace jslib {

class JSGLResourceManager : public JSWrapper<y60::GLResourceManager,asl::Ptr<y60::GLResourceManager>, StaticAccessProtocol> {
public:
    typedef y60::GLResourceManager NATIVE;
    typedef asl::Ptr<y60::GLResourceManager> OWNERPTR;
    typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

    static const char * ClassName() {
        return "GLResourceManager";
    }
    static JSFunctionSpec * Functions();
    
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    
    static JSFunctionSpec * StaticFunctions();

    virtual unsigned long length() const {
        return 0;
    }
    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static
    JSObject * Construct(JSContext *cx, asl::Ptr<y60::GLResourceManager> theGLResourceManager, y60::GLResourceManager * theNative) {
        return Base::Construct(cx, theGLResourceManager, theNative);
    }
    
    JSGLResourceManager(asl::Ptr<y60::GLResourceManager> theOwner, y60::GLResourceManager * theGLResourceManager)
        : Base(theOwner, theGLResourceManager)
    {}

    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
private:
    JSGLResourceManager(); // hide default constructor
    JSGLResourceManager(const JSGLResourceManager &); // hide copy constructor
    JSGLResourceManager & operator=(const JSGLResourceManager &); // hide assignment operator
};

template <>
struct JSClassTraits<y60::GLResourceManager> : public JSClassTraitsWrapper<y60::GLResourceManager, JSGLResourceManager> {};
/*  Only static functions

bool convertFrom(JSContext *cx, jsval theValue, y60::GLResourceManager *& theGLResourceManager);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<y60::GLResourceManager> & theGLResourceManager);

jsval as_jsval(JSContext *cx, JSGLResourceManager::OWNERPTR & theOwner);
*/
}

#endif


