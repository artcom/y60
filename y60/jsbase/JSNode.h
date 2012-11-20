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

#ifndef _Y60_ACXPSHELL_JSNODE_INCLUDED_
#define _Y60_ACXPSHELL_JSNODE_INCLUDED_

#include "y60_jsbase_settings.h"

#include "JScppUtils.h"
#include "JSWrapper.h"

#include <asl/base/settings.h>
#include <asl/base/string_functions.h>
#include <asl/base/file_functions.h>
#include <asl/dom/Value.h>
#include <asl/dom/Nodes.h>
#include <asl/dom/Schema.h>
#include <y60/base/DataTypes.h>

namespace jslib {

Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, dom::NodePtr::ptr_type theNode);

inline
jsval as_jsval(JSContext *cx, dom::Node::NodeType theValue) {
    return as_jsval(cx, int(theValue));
}


Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, dom::ValuePtr theValue);

// this function will take a jsval and try create a ValuePtr from it
//bool convertFrom(JSContext *cx, jsval theValue, dom::ValuePtr & theValuePtr);

// this function will take a jsval, try to convert it to theTypeName and create a ValuePtr from it
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, const std::string & theTypeName, dom::ValuePtr & theValuePtr);

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, dom::Node::CloneDepth & theDest);

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, dom::NodePtr & thePtr);

Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, dom::Node & thePtr);
#if 1
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, const dom::Node * & theNode);
#endif
class Y60_JSBASE_DECL JSNode : public JSWrapperBase {
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

    virtual const void * getNativeAdress() const {
        return _myNode.get();
    }

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

