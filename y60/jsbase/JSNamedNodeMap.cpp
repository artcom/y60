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
//
//   $RCSfile: JSNamedNodeMap.cpp,v $
//   $Author: pavel $
//   $Revision: 1.3 $
//   $Date: 2005/04/24 00:41:19 $
//
//
//=============================================================================

// own header
#include "JSNamedNodeMap.h"

#include "JSNodeList.h"
#include "JSWrapper.impl"
#include <iostream>

using namespace std;
using namespace dom;

namespace jslib {

template class JSWrapper<dom::NamedNodeMap,dom::NodePtr,StaticAccessProtocol>;

const char * JSNamedNodeMap::ClassName() {
    return "NamedNodeMap";
}

// TODO: add namedItem getter and setter functions
JSFunctionSpec *
JSNamedNodeMap::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name         native          nargs    */
        {"item",             JSNamedNodeMap::item,            1}, // gcc does not allow function pointers to inherited functions
        {"appendNode",       JSNamedNodeMap::appendNode,      1},
        {"getNamedItem",     JSNamedNodeMap::getNamedItem,    1},
        //{"replaceChild",     replaceChild,    1},
//        {"setAllNodeValues",  JSNamedNodeMap::setAllNodeValues,    1},
        {0}
    };
    return myFunctions;
}

enum PropertyNumbers {PROP_length = -100};

JSBool
JSNamedNodeMap::item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
        jsval *rval)
{
    DOC_BEGIN("Returns a node with a given index from the nodelist");
    DOC_PARAM("theIndex", "Index of the node to retrieve", DOC_TYPE_INTEGER);
    DOC_RVAL("The node from the nodelist at the specified index", DOC_TYPE_NODE);
    DOC_END;
    typedef dom::NodePtr (NamedNodeMap::*MyMethod)(NamedNodeMap::size_type);
    return Method<NamedNodeMap>::call((MyMethod)&NamedNodeMap::item,cx,obj,argc,argv,rval);
}

JSBool
JSNamedNodeMap::getNamedItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
        jsval *rval)
{
    DOC_BEGIN("Returns a node with a given name from the nodelist");
    DOC_PARAM("theName", "Name of the node to retrieve", DOC_TYPE_INTEGER);
    DOC_RVAL("The node from the nodelist with the specified name or undefined when node can not be found", DOC_TYPE_NODE);
    DOC_END;
    typedef dom::NodePtr (NamedNodeMap::*MyMethod)(const DOMString & name);
    return Method<NamedNodeMap>::call((MyMethod)&NamedNodeMap::getNamedItem,cx,obj,argc,argv,rval);
}

JSBool
JSNamedNodeMap::appendNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Appends a node to the node list");
    DOC_PARAM("theNode", "The xml node to append", DOC_TYPE_NODE);
    DOC_RVAL("The appended node", DOC_TYPE_NODE);
    DOC_END;
    typedef dom::NodePtr (NamedNodeMap::*MyMethod)(int);
    return Method<NamedNodeMap>::call((MyMethod)&NamedNodeMap::append,cx,obj,argc,argv,rval);
}

JSPropertySpec * JSNamedNodeMap::Properties() {
    static JSPropertySpec myProperties[] = {
        {"length", PROP_length, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned long
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSNamedNodeMap::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSNamedNodeMap::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSNamedNodeMap::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

unsigned long
JSNamedNodeMap::length() const {
    return this->getNative().length();
}

// getproperty handling
JSBool
JSNamedNodeMap::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_length:
                *vp = as_jsval(cx, getNative().length());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSNamedNodeMap::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
};
JSBool
JSNamedNodeMap::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    *vp = as_jsval(cx, getNative().item(theIndex));
    return JS_TRUE;
};

// setproperty handling
JSBool JSNamedNodeMap::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JS_ReportError(cx,"JSNamedNodeMap::setPropertySwitch: index %d out of range", theID);
    return JS_FALSE;
};
JSBool JSNamedNodeMap::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    dom::NodePtr myArgNode1 = JSNode::getNodePtr(cx, *vp);
    openNative().item(theIndex) = myArgNode1;
    closeNative();
    return JS_TRUE;
};

JSBool
JSNamedNodeMap::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs a new NamedNodeMap.");
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSNamedNodeMap * myNewObject = 0;
    dom::NodePtr myNewNode = dom::NodePtr(new dom::Node);
    if (argc == 0) {
        myNewObject=new JSNamedNodeMap(myNewNode, &(myNewNode->attributes()));
    } else {
        JS_ReportError(cx,"Constructor for %s: superflous argument ignored",ClassName());
        myNewObject=new JSNamedNodeMap(myNewNode, &(myNewNode->attributes()));
    }
    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSVector::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSNamedNodeMap::Construct(JSContext *cx, dom::NodePtr theNode, dom::NamedNodeMap * theList) {
    return Base::Construct(cx, theNode, theList);
}
jsval as_jsval(JSContext *cx, dom::NodePtr theNode, dom::NamedNodeMap * theNodeMap) {
    JSObject * myObject = JSNamedNodeMap::Construct(cx, theNode, theNodeMap);
    return OBJECT_TO_JSVAL(myObject);
}

}
