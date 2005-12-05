//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSNamedNodeMap.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/11/07 04:50:20 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_NAMEDNODEMAP_H_INCLUDED_
#define _Y60_ACXPSHELL_NAMEDNODEMAP_H_INCLUDED_

#include "JSNode.h"
#include "JSNamedNodeMap.h"
#include "JScppUtils.h"


namespace jslib {

class JSNamedNodeMap : public JSNodeListBase<dom::NamedNodeMap> {
public:
    typedef JSNodeListBase<dom::NamedNodeMap> Base;
    typedef Base::Base Wrapper;

    static const char * ClassName() {
        return "NamedNodeMap";
    }

    static JSFunctionSpec * Functions();
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();


    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSObject * Construct(JSContext *cx, dom::NodePtr theNode, dom::NamedNodeMap * theList);
    JSNamedNodeMap(dom::NodePtr theNode, dom::NamedNodeMap * theNodeList)
        : Base(theNode, theNodeList)
    {}
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject) {
        DOC_CREATE(JSNamedNodeMap);
        return Wrapper::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
    }
};

template <>
struct JSClassTraits<dom::NamedNodeMap>
    : public JSClassTraitsWrapper<dom::NamedNodeMap, JSNamedNodeMap> {};

}

#endif
