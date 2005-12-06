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
//   $RCSfile: JSNodeList.h,v $
//   $Author: pavel $
//   $Revision: 1.5 $
//   $Date: 2005/04/24 00:41:19 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSNODELIST_H_INCLUDED_
#define _Y60_ACXPSHELL_JSNODELIST_H_INCLUDED_

#include "JSWrapper.h"
#include "JSNode.h"

#include <dom/Value.h>
#include <dom/Nodes.h>
#include <dom/NodeList.h>
#include <iostream>

namespace jslib {

template <class NATIVE_LIST>
class JSNodeListBase : public JSWrapper<NATIVE_LIST,dom::NodePtr,StaticAccessProtocol> {
public:
    typedef JSWrapper<NATIVE_LIST,dom::NodePtr,StaticAccessProtocol> Base;

    static const char * ClassName() {
        return "NodeList";
    }
    static JSBool
    item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Returns a node with a given index from the nodelist");
        DOC_PARAM("theIndex", "Index of the node to retrieve", DOC_TYPE_INTEGER);
        DOC_RVAL("The node from the nodelist at the specified index", DOC_TYPE_NODE);
        DOC_END;
        typedef dom::NodePtr (NATIVE_LIST::*MyMethod)(int);
        return Method<NATIVE_LIST>::call((MyMethod)&NATIVE_LIST::item,cx,obj,argc,argv,rval);
    }
    static JSBool
    appendNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Appends a node to the node list");
        DOC_PARAM("theNode", "The xml node to append", DOC_TYPE_NODE);
        DOC_RVAL("The appended node", DOC_TYPE_NODE);
        DOC_END;
        typedef dom::NodePtr (NATIVE_LIST::*MyMethod)(int);
        return Method<NATIVE_LIST>::call((MyMethod)&NATIVE_LIST::append,cx,obj,argc,argv,rval);
    }

    static JSFunctionSpec * Functions() {
        AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<std::endl;
        static JSFunctionSpec myFunctions[] = {
            /* name         native          nargs    */
            {"item",             item,            1},
            {"appendNode",       appendNode,      1},
            //{"replaceChild",     replaceChild,    1},
            //{"setAllNodeValues", setAllNodeValues,    1},
            {0}
        };
        return myFunctions;
    }
    enum PropertyNumbers {PROP_length = -100};

    static JSPropertySpec * Properties() {
        static JSPropertySpec myProperties[] = {
            {"length", PROP_length, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT},   // readonly attribute unsigned long
            {0}
        };
        return myProperties;
    }

    static JSConstIntPropertySpec * ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    static JSPropertySpec * StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    static JSFunctionSpec * StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {{0}};
        return myFunctions;
    }

    virtual unsigned long length() const {
        return this->getNative().length();
    }

    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        switch (theID) {
            case PROP_length:
                *vp = as_jsval(cx, this->getNative().length());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSNodeListBase::getProperty: index %d out of range", theID);
                return JS_FALSE;
        }
    };
    virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        *vp = as_jsval(cx, this->getNative().item(theIndex));
        return JS_TRUE;
    };

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSNodeListBase::setPropertySwitch: index %d out of range", theID);
        return JS_FALSE;
    };
    virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        dom::NodePtr myArgNode1 = JSNode::getNodePtr(cx, *vp);
        this->openNative().item(theIndex) = myArgNode1;
        this->closeNative();
        return JS_TRUE;
    };

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Creates a new node list");
        DOC_END;

        if (JSA_GetClass(cx,obj) != Base::Class()) {
            JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
            return JS_FALSE;
        }
        JSNodeListBase * myNewObject = 0;
        dom::NodePtr myNewNode = dom::NodePtr(new dom::Node);
        if (argc == 0) {
            myNewObject=new JSNodeListBase(myNewNode, &(myNewNode->childNodes()));
        } else {
            JS_ReportError(cx,"Constructor for %s: superflous argument ignored",ClassName());
            myNewObject=new JSNodeListBase(myNewNode, &(myNewNode->childNodes()));
        }
        if (myNewObject) {
            JS_SetPrivate(cx,obj,myNewObject);
            return JS_TRUE;
        }
        JS_ReportError(cx,"JSVector::Constructor: bad parameters");
        return JS_FALSE;
    }
    static
    JSObject * Construct(JSContext *cx, dom::NodePtr theNode, NATIVE_LIST * theList) {
        return Base::Construct(cx, theNode, theList);
    }
    JSNodeListBase(dom::NodePtr theNode, NATIVE_LIST * theNodeList)
        : Base(theNode, theNodeList)
    {}
    static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), 0, 0, 0);
        DOC_CREATE(JSNodeListBase);
        return myClass;
    }
};

typedef JSNodeListBase<dom::NodeList> JSNodeList;

template <>
struct JSClassTraits<dom::NodeList>
    : public JSClassTraitsWrapper<dom::NodeList, JSNodeList> {};

inline
jsval as_jsval(JSContext *cx, dom::NodePtr theNode, dom::NodeList * theNodeList) {
    JSObject * myObject = JSNodeList::Construct(cx, theNode, theNodeList);
    return OBJECT_TO_JSVAL(myObject);
}
}

#endif

