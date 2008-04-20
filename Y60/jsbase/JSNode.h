//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSNODE_INCLUDED_
#define _Y60_ACXPSHELL_JSNODE_INCLUDED_

#include "JScppUtils.h"
#include "JSWrapper.h"

#include <asl/settings.h>
#include <asl/string_functions.h>
#include <asl/file_functions.h>
#include <dom/Value.h>
#include <dom/Nodes.h>
#include <dom/Schema.h>
#include <y60/DataTypes.h>

namespace jslib {

jsval as_jsval(JSContext *cx, dom::NodePtr theNode);

inline
jsval as_jsval(JSContext *cx, dom::Node::NodeType theValue) {
    return as_jsval(cx, int(theValue));
}


jsval as_jsval(JSContext *cx, dom::ValuePtr theValue);

// this function will take a jsval and try create a ValuePtr from it
//bool convertFrom(JSContext *cx, jsval theValue, dom::ValuePtr & theValuePtr);

// this function will take a jsval, try to convert it to theTypeName and create a ValuePtr from it
bool convertFrom(JSContext *cx, jsval theValue, const std::string & theTypeName, dom::ValuePtr & theValuePtr);

bool convertFrom(JSContext *cx, jsval theValue, dom::Node::CloneDepth & theDest);

bool convertFrom(JSContext *cx, jsval theValue, dom::NodePtr & thePtr);

bool convertFrom(JSContext *cx, jsval theValue, dom::Node & thePtr);
#if 1
bool convertFrom(JSContext *cx, jsval theValue, const dom::Node * & theNode);
#endif
class JSNode {
public:
    typedef dom::NodePtr NativeValuePtr;

    static const char * ClassName() {
        return "Node";
    }
    static
    JSNode * getJSNodePtr(JSContext *cx, JSObject *obj);
    static
    JSNode & getJSNode(JSContext *cx, JSObject *obj);
    static
    dom::NodePtr & getNodePtrRef(JSContext *cx, JSObject *obj) {
        return getJSNode(cx,obj)._myNode;
    }
    static
    dom::NodePtr & getNodePtrRef(JSContext *cx, jsval theJSValue);
    static
    dom::NodePtr getNodePtr(JSContext *cx, JSObject *obj);
    static
    dom::NodePtr getNodePtr(JSContext *cx, jsval theJSValue);

    static JSBool
    toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSFunctionSpec * Functions();
    static JSFunctionSpec * StaticFunctions();
    static JSPropertySpec * Properties();
    static JSPropertySpec * StaticProperties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSBool getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    static JSBool setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    static JSBool newResolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp);
    static JSBool customConvert(JSContext *cx, JSObject *obj, JSType type, jsval *vp);
    static void finalizeImpl(JSContext *cx, JSObject *obj);

    static JSClass * Class();

    JSNode(dom::NodePtr p) : _myNode(p) {}
    JSNode(const dom::Node & theNode) : _myNode(new dom::Node(theNode)) {}
    ~JSNode() {
        IF_NOISY_F(AC_TRACE << "~JSNode<"<<ClassName()<<">()" << std::endl);
    }
    static
    JSObject * Construct(JSContext *cx, dom::NodePtr theNode);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
private:
    dom::NodePtr _myNode;
};
#if 1
template <>
struct JSClassTraits<dom::Node> {
    static JSClass * Class() {
        return JSNode::Class();
    }
    static dom::NodePtr getNativeOwner(JSContext *cx, JSObject * theJSObj) {
        return JSNode::getNodePtr(cx, theJSObj);
    }
    static dom::Node & openNativeRef(JSContext *cx, JSObject * theJSObj) {
        return *JSNode::getNodePtr(cx, theJSObj);
    }
    static void closeNativeRef(JSContext *cx, JSObject * theJSObj) {
    }
    static const dom::Node & getNativeRef(JSContext *cx, JSObject * theJSObj) {
        return *JSNode::getNodePtr(cx, theJSObj);
    }
};
#else
template <>
struct JSClassTraits<dom::Node> : public JSClassTraitsWrapper<dom::Node, JSNode> {};
#endif

}

#endif

