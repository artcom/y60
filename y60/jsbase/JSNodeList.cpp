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

// own header
#include "JSNodeList.h"

#include "JSWrapper.impl"

namespace jslib {

    using namespace dom;

    template class JSWrapper<dom::NodeList,dom::NodePtr,StaticAccessProtocol>;

    const char * JSNodeList::ClassName() {
        return "NodeList";
    }

    JSBool
    JSNodeList::item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
            jsval *rval)
    {
        DOC_BEGIN("Returns a node with a given index from the nodelist");
        DOC_PARAM("theIndex", "Index of the node to retrieve", DOC_TYPE_INTEGER);
        DOC_RVAL("The node from the nodelist at the specified index", DOC_TYPE_NODE);
        DOC_END;
        typedef dom::NodePtr (NodeList::*MyMethod)(NodeList::size_type);
        return Method<NodeList>::call((MyMethod)&NodeList::item,cx,obj,argc,argv,rval);
    }

    JSBool
    JSNodeList::appendNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Appends a node to the node list");
        DOC_PARAM("theNode", "The xml node to append", DOC_TYPE_NODE);
        DOC_RVAL("The appended node", DOC_TYPE_NODE);
        DOC_END;
        typedef dom::NodePtr (NodeList::*MyMethod)(dom::NodePtr);
        return Method<NodeList>::call((MyMethod)&NodeList::append,cx,obj,argc,argv,rval);
    }

    JSBool
    JSNodeList::clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("deletes all nodes from the node list");
        DOC_END;
        typedef void (NodeList::*MyMethod)();
        return Method<NodeList>::call((MyMethod)&NodeList::clear,cx,obj,argc,argv,rval);
    }

    JSFunctionSpec * JSNodeList::Functions() {
        AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<std::endl;
        static JSFunctionSpec myFunctions[] = {
            /* name         native          nargs    */
            {"item",             item,            1},
            {"appendNode",       appendNode,      1},
            {"clear",            clear,           0},
            //{"replaceChild",     replaceChild,    1},
            //{"setAllNodeValues", setAllNodeValues,    1},
            {0}
        };
        return myFunctions;
    }

    JSPropertySpec * JSNodeList::Properties() {
        static JSPropertySpec myProperties[] = {
            {"length", PROP_length, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT},   // readonly attribute unsigned long
            {0}
        };
        return myProperties;
    }

    JSConstIntPropertySpec * JSNodeList::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSPropertySpec * JSNodeList::StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSFunctionSpec * JSNodeList::StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {{0}};
        return myFunctions;
    }

    unsigned long JSNodeList::length() const {
        return this->getNative().length();
    }

    JSBool JSNodeList::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        switch (theID) {
            case PROP_length:
                *vp = as_jsval(cx, this->getNative().length());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSNodeList::getProperty: index %d out of range", theID);
                return JS_FALSE;
        }
    };
    JSBool JSNodeList::getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        *vp = as_jsval(cx, this->getNative().item(theIndex));
        return JS_TRUE;
    };

    JSBool JSNodeList::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSNodeList::setPropertySwitch: index %d out of range", theID);
        return JS_FALSE;
    };

    JSBool JSNodeList::setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        dom::NodePtr myArgNode1 = JSNode::getNodePtr(cx, *vp);
        this->openNative().item(theIndex) = myArgNode1;
        this->closeNative();
        return JS_TRUE;
    };

    JSBool
    JSNodeList::Constructor(JSContext *cx, JSObject *obj, uintN argc,
            jsval *argv, jsval *rval) {
        DOC_BEGIN("Creates a new node list");
        DOC_END;

        if (JSA_GetClass(cx,obj) != Base::Class()) {
            JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
            return JS_FALSE;
        }
        JSNodeList * myNewObject = 0;
        dom::NodePtr myNewNode = dom::NodePtr(new dom::Node);
        if (argc == 0) {
            myNewObject=new JSNodeList(myNewNode, &(myNewNode->childNodes()));
        } else {
            JS_ReportError(cx,"Constructor for %s: superflous argument ignored",ClassName());
            myNewObject=new JSNodeList(myNewNode, &(myNewNode->childNodes()));
        }
        if (myNewObject) {
            JS_SetPrivate(cx,obj,myNewObject);
            return JS_TRUE;
        }
        JS_ReportError(cx,"JSVector::Constructor: bad parameters");
        return JS_FALSE;
    }

    JSObject * JSNodeList::Construct(JSContext *cx, dom::NodePtr theNode, NodeList * theList) {
        return Base::Construct(cx, theNode, theList);
    }

    JSNodeList::JSNodeList(dom::NodePtr theNode,
            NodeList * theNodeList)
        : Base(theNode, theNodeList)
    {}

    JSObject * JSNodeList::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), 0, 0, 0);
        DOC_CREATE(JSNodeList);
        return myClass;
    }

    template <>
    struct JSClassTraits<dom::NodeList>
        : public JSClassTraitsWrapper<dom::NodeList, JSNodeList> {};

    jsval as_jsval(JSContext *cx, dom::NodePtr theNode, dom::NodeList * theNodeList) {
        JSObject * myObject = JSNodeList::Construct(cx, theNode, theNodeList);
        return OBJECT_TO_JSVAL(myObject);
    }
    bool convertFrom(JSContext *cx, jsval theValue, dom::NodeList  & theNodeList) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<dom::NodeList>::Class()) {
                    theNodeList = JSClassTraits<dom::NodeList>::getNativeRef(cx,myArgument);
                    return true;
                }
            }
        }
        return false;
    }

}
