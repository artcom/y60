//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSNODELIST_H_INCLUDED_
#define _Y60_ACXPSHELL_JSNODELIST_H_INCLUDED_

#include "JSWrapper.h"
#include "JSNode.h"

// #include <dom/Value.h>
#include <dom/Nodes.h>
#include <dom/NodeList.h>

namespace jslib {

class JSNodeList: public JSWrapper<dom::NodeList,dom::NodePtr,StaticAccessProtocol> {
public:
    typedef JSWrapper<dom::NodeList,dom::NodePtr,StaticAccessProtocol> Base;

    static const char * ClassName();
    static JSBool
    item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    appendNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSFunctionSpec * Functions();
    enum PropertyNumbers {PROP_length = -100};
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();
    virtual unsigned long length() const;
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, 
            jsval id, jsval *vp); 
    virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, 
            jsval id, jsval *vp); 
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj,
            jsval id, jsval *vp);
    virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, 
            jsval id, jsval *vp);
    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); 
    static JSObject * Construct(JSContext *cx, dom::NodePtr theNode, 
            dom::NodeList * theList);

    JSNodeList(dom::NodePtr theNode, dom::NodeList * theNodeList);

    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject); 
};

jsval as_jsval(JSContext *cx, dom::NodePtr theNode, dom::NodeList * theNodeList);

}

#endif

