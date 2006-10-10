//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSNodeList.h"

namespace jslib {

using namespace dom;

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
        typedef dom::NodePtr (NodeList::*MyMethod)(int);
        return Method<NodeList>::call((MyMethod)&NodeList::item,cx,obj,argc,argv,rval);
    }

    JSBool
    JSNodeList::appendNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Appends a node to the node list");
        DOC_PARAM("theNode", "The xml node to append", DOC_TYPE_NODE);
        DOC_RVAL("The appended node", DOC_TYPE_NODE);
        DOC_END;
        typedef dom::NodePtr (NodeList::*MyMethod)(int);
        return Method<NodeList>::call((MyMethod)&NodeList::append,cx,obj,argc,argv,rval);
    }

    JSFunctionSpec * JSNodeList::Functions() {
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

    JSObject * JSNodeList::Construct(JSContext *cx, dom::NodePtr theNode, 
            NodeList * theList) {
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

}
