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
//   $RCSfile: JSNode.cpp,v $
//   $Author: christian $
//   $Revision: 1.19 $
//   $Date: 2005/04/28 17:12:58 $
//
//=============================================================================


#include "JSNode.h"
#include "JSNodeList.h"
#include "JSNamedNodeMap.h"

#include "JSBox.h"
#include "JSTriangle.h"
#include "JSLine.h"
#include "JSMatrix.h"
#include "JSQuaternion.h"
#include "JSFrustum.h"
#include "JSVector.h"
#include "JSResizeableRaster.h"
#include "JSResizeableVector.h"
#include "JSAccessibleVector.h"
#include "JSEvent.h"
#include "JSPlane.h"

#include <asl/os_functions.h>
#include <asl/Logger.h>
#include <y60/DataTypes.h>

#include <iostream>
#include <fstream>
#include <map>

using namespace std;
using namespace asl;

namespace jslib {

#define DB(x)  x

struct TypeConverterMap {
    static std::map<std::string,int> ourKnownTypes;

    typedef jsval (*JSValFromValuePtr)(JSContext *cx, dom::ValuePtr theSrcValue);
    typedef bool (*ValuePtrFromJSVal)(JSContext *cx, jsval theSrcValue, dom::ValuePtr & theDestValue);


private:
    static std::vector<JSValFromValuePtr> _myJSValFromValuePtrFunctions;
    static std::vector<ValuePtrFromJSVal> _myValuePtrFromJSValFunctions;

    template<class T>
    static bool convertFromSimpleValue(JSContext *cx, jsval theSrcValue, dom::ValuePtr & theDestValue) {
        T mySimpleValue;
        if (convertFrom(cx, theSrcValue, mySimpleValue)) {
            theDestValue = dom::ValuePtr(new dom::SimpleValue<T>(mySimpleValue, 0));
            return true;
        }
        AC_ERROR << "conversion failed in TypeConverter::convertFromSimpleValue";
        return false;
    }

    template<class T>
    static jsval jsvalByValue(JSContext *cx, dom::ValuePtr theValue) {
        const T * myTPtr = dom::dynamic_cast_Value<T>(&(*theValue));
        if (myTPtr) {
            return as_jsval(cx, *myTPtr);
        }
        AC_ERROR << "conversion failed in TypeConverter::jsValByValue";
        return JSVAL_VOID;
    }

    template<class T>
    static jsval jsvalByReference(JSContext *cx, dom::ValuePtr theValue) {
        asl::Ptr<typename dom::ValueWrapper<T>::Type, dom::ThreadingModel> myTPtr
            = dynamic_cast_Ptr<typename dom::ValueWrapper<T>::Type>(theValue);
        if (myTPtr) {
            //AC_TRACE << "conversion succeeded in TypeConverter::jsValByReference <" << myTPtr->name() << ">";
            return as_jsval(cx, myTPtr);

        } else {
            //DK: i found by trial and error that this was necessary
            //    for some VectorOfVector2f conversions as exemplified in testVectorOfString.tst.js
            return jsvalByValue<T>(cx, theValue);
        }
        AC_ERROR << "conversion failed in TypeConverter::jsValByReference";
        return JSVAL_VOID;
    }

    static bool convertFromString(JSContext *cx, jsval theSrcValue, dom::ValuePtr & theDestValue) {
        //AC_TRACE << "conversion succeeded in TypeConverter::convertFromString";
        theDestValue = dom::ValuePtr(new dom::StringValue(as_string(cx, theSrcValue), 0));
        return true;
    }

    static jsval jsvalFromString(JSContext *cx, dom::ValuePtr theValue) {
        //AC_TRACE << "conversion succeeded in TypeConverter::jsvalFromString";
        return as_jsval(cx, theValue->getString());
    }

    static jsval jsvalFromIDValue(JSContext *cx, dom::ValuePtr theValue) {
        dom::IDValue * myIdValue = dynamic_cast<dom::IDValue *>(&*theValue);
        if (myIdValue) {
            //AC_TRACE << "conversion succeeded in TypeConverter::jsvalFromIDValue";
            return as_jsval(cx, theValue->getString());
        }
        AC_ERROR << "conversion failed in TypeConverter::jsValFromIDValue";
        return JSVAL_VOID;
    }

public:
    static ValuePtrFromJSVal findConvertFrom(const std::string & theTypeName) {
        if (ourKnownTypes.find(theTypeName) != ourKnownTypes.end()) {
            return _myValuePtrFromJSValFunctions[ ourKnownTypes[theTypeName] ];
        }
        AC_DEBUG << "found no converter JS to C++ for type <" << theTypeName << ">";
        return 0;
    }
    static JSValFromValuePtr findAsJsval(const std::string & theTypeName) {
        if (ourKnownTypes.find(theTypeName) != ourKnownTypes.end()) {
            return _myJSValFromValuePtrFunctions[ ourKnownTypes[theTypeName] ];
        }
        AC_DEBUG << "found no converter C++ to JS for type <" << theTypeName << ">";
        return 0;
    }

    #define REGISTER_BYVALUE_CONVERTER(TYPE) \
        ourKnownTypes[typeid( TYPE ).name()] = myIndex++; \
        _myValuePtrFromJSValFunctions.push_back( convertFromSimpleValue< TYPE > ); \
        _myJSValFromValuePtrFunctions.push_back( jsvalByValue< TYPE > )

    #define REGISTER_BYREFERENCE_CONVERTER(TYPE) \
        ourKnownTypes[typeid( TYPE ).name()] = myIndex++; \
        _myValuePtrFromJSValFunctions.push_back( convertFromSimpleValue< TYPE > ); \
        _myJSValFromValuePtrFunctions.push_back( jsvalByReference< TYPE > )

    static void init() {
        if ( ! ourKnownTypes.empty()) {
            return;
        }

        int myIndex = 0;

        ourKnownTypes[typeid(dom::StringValue).name()] = myIndex++;
        _myValuePtrFromJSValFunctions.push_back( convertFromString );
        _myJSValFromValuePtrFunctions.push_back( jsvalFromString );

        ourKnownTypes[typeid(std::string).name()] = myIndex++;
        _myValuePtrFromJSValFunctions.push_back( convertFromString );
        _myJSValFromValuePtrFunctions.push_back( jsvalFromString );

        ourKnownTypes[typeid(dom::IDValue).name()] = myIndex++;
        _myValuePtrFromJSValFunctions.push_back( convertFromString ); //TODO ??
        _myJSValFromValuePtrFunctions.push_back( jsvalFromIDValue );

        REGISTER_BYVALUE_CONVERTER(int);
        REGISTER_BYVALUE_CONVERTER(unsigned int);
        REGISTER_BYVALUE_CONVERTER(long);
        REGISTER_BYVALUE_CONVERTER(float);
        REGISTER_BYVALUE_CONVERTER(double);
        REGISTER_BYVALUE_CONVERTER(bool);

        REGISTER_BYREFERENCE_CONVERTER(Vector2f);
        REGISTER_BYREFERENCE_CONVERTER(Vector3f);
        REGISTER_BYREFERENCE_CONVERTER(Vector4f);
        REGISTER_BYREFERENCE_CONVERTER(Vector2d);
        REGISTER_BYREFERENCE_CONVERTER(Vector3d);
        REGISTER_BYREFERENCE_CONVERTER(Vector4d);
        REGISTER_BYREFERENCE_CONVERTER(Vector2i);
        REGISTER_BYREFERENCE_CONVERTER(Vector3i);
        REGISTER_BYREFERENCE_CONVERTER(Vector4i);
        REGISTER_BYREFERENCE_CONVERTER(Matrix4f);
        REGISTER_BYREFERENCE_CONVERTER(Quaternionf);
        REGISTER_BYREFERENCE_CONVERTER(Planef);
        REGISTER_BYREFERENCE_CONVERTER(Frustum);
        REGISTER_BYREFERENCE_CONVERTER(Box3f);
        REGISTER_BYREFERENCE_CONVERTER(Triangle<TriangleNumber>);
    }
};


std::map<std::string,int> TypeConverterMap::ourKnownTypes;
std::vector<TypeConverterMap::JSValFromValuePtr> TypeConverterMap::_myJSValFromValuePtrFunctions;
std::vector<TypeConverterMap::ValuePtrFromJSVal> TypeConverterMap::_myValuePtrFromJSValFunctions;


#define JSVAL_FROM_VECTORVALUE(TYPE) \
    { \
        TYPE * myTPtr = dynamic_cast< TYPE *>(&(*theValue)); \
        if (myTPtr) { \
            return as_jsval(cx, theValue, myTPtr); \
        } \
    }

jsval as_jsval(JSContext *cx, dom::ValuePtr theValue) {

    if (!theValue) {
        return JSVAL_VOID;
    }

    TypeConverterMap::init();
    //AC_TRACE << "looking for C++ to JS conversion type " << theValue->getTypeInfo().name() << endl;
    TypeConverterMap::JSValFromValuePtr myConverter
        = TypeConverterMap::findAsJsval(theValue->getTypeInfo().name());

    if (myConverter) {
        //AC_TRACE << " got a converter for <" << theValue->getTypeInfo().name() << ">";
        //AC_TRACE << " value as string is  " << theValue->getString();
        jsval myDestValue = (*myConverter)(cx, theValue);
        if (myDestValue != JSVAL_VOID) {
            return myDestValue;
        }
    }

    // following are not registered (they are able to dispatch multiple types...)
    // Attention: put derived classes before their bases [DS]
    JSVAL_FROM_VECTORVALUE(dom::ResizeableVector);
    JSVAL_FROM_VECTORVALUE(dom::AccessibleVector);
    JSVAL_FROM_VECTORVALUE(dom::ResizeableRaster);

    AC_WARNING << " as_jsval:: using string conversion for type " << theValue->getTypeInfo().name()
               << " value as string " << theValue->getString();
    return as_jsval(cx, theValue->getString());
}

// this function will take theValue, try to convert it to theTypeName and create theValuePtr from it
bool convertFrom(JSContext *cx, jsval theValue, const std::string & theTypeName,
                 dom::ValuePtr & theValuePtr) {

    TypeConverterMap::init();

    //AC_TRACE << "looking for JS to C++ conversion type  " << theTypeName << endl;
    TypeConverterMap::ValuePtrFromJSVal myConverter = TypeConverterMap::findConvertFrom(theTypeName);

    if (myConverter) {
        //AC_TRACE << " got a converter for <" << theTypeName << ">";
        //AC_TRACE << " value as string is  " << as_string(cx, theValue);
        if ( (*myConverter)(cx, theValue, theValuePtr) ) {
            return true;
        }
    }
    AC_WARNING << " convertFrom:: using string conversion for type " << theTypeName
               << " value as string " << as_string(cx, theValue);

    theValuePtr = dom::ValuePtr(new dom::StringValue(as_string(cx, theValue), 0));
    return true;
}

bool convertFrom(JSContext *cx, jsval theValue, dom::Node::CloneDepth & theDest) {
    JSBool myBool;
    if (JS_ValueToBoolean(cx, theValue, &myBool))
    {
        theDest = dom::Node::CloneDepth(myBool);
        return true;
    }
    return false;
}

JSNode *
JSNode::getJSNodePtr(JSContext *cx, JSObject *obj) {
    if (JSA_GetClass(cx,obj) != JSNode::Class()) {
        JS_ReportError(cx,"getImpl: class type '%s' expected",JSNode::ClassName());
        return 0;
    }
    JSNode * myJSNode = static_cast<JSNode*>(JS_GetPrivate(cx,obj));
    if (!myJSNode) {
        JS_ReportError(cx,"JSNode::getJSNodePtr: internal error, binding object does not exist");
    }
    return myJSNode;
}

JSNode & JSNode::getJSNode(JSContext *cx, JSObject *obj) {
    JSNode * myJSNode = getJSNodePtr(cx,obj);
    if (!myJSNode) {
        JS_ReportError(cx,"JSNode::getJSNode: internal error, binding object does not exist");
        static dom::NodePtr myDummyPtr = dom::NodePtr(new dom::Node);
        static JSNode myDummyJSNode(myDummyPtr);
        return myDummyJSNode;
    }
    return *myJSNode;
}

dom::NodePtr &
JSNode::getNodePtrRef(JSContext *cx, jsval theJSValue) {
    JSObject * myArgument;
    if (!JS_ValueToObject(cx, theJSValue, &myArgument)) {
        AC_ERROR << "getNativeNode: bad argument type" << endl;
    }
    return getNodePtrRef(cx, myArgument);
}

dom::NodePtr
JSNode::getNodePtr(JSContext *cx, JSObject *obj) {
    JSNode * myJSNode = getJSNodePtr(cx,obj);
    if (myJSNode) {
        return myJSNode->_myNode;
    } else {
        return dom::NodePtr(0);
    }
}

dom::NodePtr
JSNode::getNodePtr(JSContext *cx, jsval theJSValue) {
    JSObject * myArgument;
    if (!JS_ValueToObject(cx, theJSValue, &myArgument)) {
        AC_ERROR << "getNativeNode: bad argument type" << endl;
        return dom::NodePtr(0);
    }
    return getNodePtr(cx, myArgument);
}

JSBool
JSNode::toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the node as string representation.");
    DOC_END;
    std::string myStringRep = asl::as_string(*getNodePtr(cx,obj));
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
replaceChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Replaces the child node oldChild with newChild in the list of children, and returns the oldChild node.");
    DOC_PARAM("newChild", "", DOC_TYPE_NODE);
    DOC_PARAM("oldChild", "", DOC_TYPE_NODE);
    DOC_RVAL("The node replaced", DOC_TYPE_NODE);
    DOC_END;
    return Method<dom::Node>::call(&dom::Node::replaceChild, cx, obj, argc, argv, rval);
}
static JSBool
removeChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Removes the child node indicated by oldChild from the list of children, and returns it.");
    DOC_PARAM("oldChild", "", DOC_TYPE_NODE);
    DOC_RVAL("The node removed.", DOC_TYPE_NODE);
    DOC_END;
    return Method<dom::Node>::call(&dom::Node::removeChild, cx, obj, argc, argv, rval);
}
static JSBool
appendChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Adds the node newChild to the end of the list of children of this node.");
    DOC_PARAM("newChild", "", DOC_TYPE_NODE);
    DOC_RVAL("The node added", DOC_TYPE_NODE);
    DOC_END;
    typedef dom::NodePtr (dom::Node::*MyMethod)(dom::NodePtr);
    return Method<dom::Node>::call((MyMethod)&dom::Node::appendChild, cx, obj, argc, argv, rval);
}
static JSBool
hasChildNodes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns whether this node has any children.");
    DOC_RVAL("Returns true if this node has any children, false otherwise.", DOC_TYPE_BOOLEAN);
    DOC_END;
    return Method<dom::Node>::call(&dom::Node::hasChildNodes, cx, obj, argc, argv, rval);
}
static JSBool
cloneNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a duplicate of this node. If true, recursively clone the subtree under the specified node; if false, clone only the node itself (and its attributes, if it is an Element).");
    DOC_PARAM("theDeepCopyFlag", "", DOC_TYPE_BOOLEAN);
    DOC_RVAL("The duplicate node.", DOC_TYPE_NODE);
    DOC_END;
    typedef dom::NodePtr (dom::Node::*MyMethod)(dom::Node::CloneDepth) const;
    return Method<dom::Node>::call((MyMethod)&dom::Node::cloneNode, cx, obj, argc, argv, rval);
}
static JSBool
isSameNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns whether this node is the same node as the given one.");
    DOC_PARAM("theOtherNode", "The node to compare this node with.", DOC_TYPE_NODE);
    DOC_RVAL("Returns true if the nodes are the same, false otherwise.", DOC_TYPE_BOOLEAN);
    DOC_END;
    typedef bool (dom::Node::*MyMethod)(const dom::NodePtr) const;
    return Method<dom::Node>::call((MyMethod)&dom::Node::isSameNode, cx, obj, argc, argv, rval);
}
static JSBool
addSchema(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Adds a schema to this document.");
    DOC_PARAM("Schema node", "", DOC_TYPE_NODE);
    DOC_PARAM("Namespace Prefix", "", DOC_TYPE_STRING);
    DOC_END;
    typedef void (dom::Node::*MyMethod)(const dom::Node &, const dom::DOMString &);
    JSBool result = Method<dom::Node>::call((MyMethod)&dom::Node::addSchema, cx, obj, argc, argv, rval);
    return result;
}

static JSBool
useFactoriesImpl(JSContext *cx, dom::Node & theNode, const std::string & theFactories) {
    asl::Ptr<dom::ValueFactory> myFactory = asl::Ptr<dom::ValueFactory>(new dom::ValueFactory());
    std::vector<string> myFactoryNames = asl::splitString(string(theFactories),", \t\n\r;");
    for (unsigned i = 0; i < myFactoryNames.size(); ++i) {
        if (myFactoryNames[i] == "w3c-schema") {
            dom::registerStandardTypes(*myFactory);
        } else if (myFactoryNames[i] == "y60") {
            y60::registerSomTypes(*myFactory);
        } else if (myFactoryNames[i] == "som") {
            // [DS] som factory is deprecated for x60 models only.
            // Other schemas using it still have to register it beforehand.
            // So this is defenitly the wrong place for this warning.
            //AC_WARNING << "'som' factory is deprecated";
            y60::registerSomTypes(*myFactory);
        } else {
            JS_ReportError(cx,"JSNode::useFactories: Could not find factory '%s'",myFactoryNames[i].c_str());
            return JS_FALSE;
        }
    }
    theNode.setValueFactory(myFactory);
    return JS_TRUE;
}
static JSBool
useFactories(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Attaches type factories to this document.");
    DOC_RVAL("Comma seperated list of factory names. Currently only 'w3c-schema' and 'y60' are supported", DOC_TYPE_STRING);
    DOC_END;
    if (argc != 1) {
        JS_ReportError(cx,"JSNode::useFactories: wrong number of parameters: %d, 1 expected", argc);
        return JS_FALSE;
    }
    dom::NodePtr myNode;
    std::string myString;
    if (convertFrom(cx, OBJECT_TO_JSVAL(obj),myNode) &&
        convertFrom(cx, argv[0],myString) )
    {
        return useFactoriesImpl(cx,*myNode,myString);
    }
    return JS_FALSE;
}
static JSBool
parse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Parses document from theXMLString.");
    DOC_PARAM("theXMLString", "", DOC_TYPE_STRING);
    DOC_RVAL("true if construction was valid", DOC_TYPE_BOOLEAN);
    DOC_END;
    return Method<dom::Node>::call(&dom::Node::parseAll, cx, obj, argc, argv, rval);
}
static JSBool
parseFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Parses a document from a file");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_END;
    return Method<dom::Node>::call(&dom::Node::parseFile, cx, obj, argc, argv, rval);
}

static JSBool
saveFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Saves this document as file");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (argc != 1) {
            JS_ReportError(cx,"JSNode::saveFile() - Wrong number of parameters: %d, 1 expected", argc);
            return JS_FALSE;
        }

        dom::NodePtr myNode;
        if (!convertFrom(cx, OBJECT_TO_JSVAL(obj),myNode)) {
            JS_ReportError(cx,"JSNode::saveFile() - Could not convert object to node");
            return JS_FALSE;
        }

        std::string myFilename;
        if(!convertFrom(cx, argv[0], myFilename))  {
            JS_ReportError(cx,"JSNode::saveFile() - Could not convert first argument, string expected");
            return JS_FALSE;
        }

        myFilename = asl::expandEnvironment(myFilename);

        std::ofstream myFile(myFilename.c_str());
        if (!myFile) {
            JS_ReportError(cx, "JSNode::saveFile() - "
                    "Can not open file '%s' for writing.", myFilename.c_str());
            return JS_FALSE;
        }
        myFile << *myNode;
        if (!myFile) {
            JS_ReportError(cx, "JSNode::saveFile() - "
                    "Can not write text file '%s' for writing.", myFilename.c_str());
            return JS_FALSE;
        }
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}
static JSBool
childNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns a child node of this node");
    DOC_PARAM("theIndex", "Child index", DOC_TYPE_INTEGER);
    DOC_RESET;
    DOC_PARAM("theNodeName", "Child node name", DOC_TYPE_STRING);
    DOC_RESET;
    DOC_PARAM("theNodeName", "Child node name", DOC_TYPE_STRING);
    DOC_PARAM("theIndex", "Index of all children with a given node name", DOC_TYPE_INTEGER);
    DOC_RVAL("Child node", DOC_TYPE_NODE);
    DOC_END;
    if (argc == 1) {
        if (JSVAL_IS_INT(argv[0])) {
            typedef dom::NodePtr (dom::Node::*MyMethod)(int);
            return Method<dom::Node>::call((MyMethod)&dom::Node::childNode, cx, obj, argc, argv, rval);
        }
        typedef dom::NodePtr (dom::Node::*MyMethod)(const dom::DOMString &);
        return Method<dom::Node>::call((MyMethod)&dom::Node::childNode, cx, obj, argc, argv, rval);
    } else {
        if (argc == 2) {
            typedef dom::NodePtr (dom::Node::*MyMethod)(const dom::DOMString &,int);
            return Method<dom::Node>::call((MyMethod)&dom::Node::childNode, cx, obj, argc, argv, rval);
        }
    }
    JS_ReportError(cx,"JSNode::childNode: wrong number of parameters: %d, 1 or 2 expected", argc);
    return JS_FALSE;
}

static JSBool
childNodesLength(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the number of child nodes of this node. A child node name can be given.");
    DOC_RESET;
    DOC_PARAM("theChildName", "Child node name", DOC_TYPE_STRING);
    DOC_END;
    if (argc == 0) {
        typedef int (dom::Node::*MyMethod)() const;
        return Method<dom::Node>::call((MyMethod)&dom::Node::childNodesLength, cx, obj, argc, argv, rval);
    } else {
        if (argc == 1) {
            typedef int (dom::Node::*MyMethod)(const dom::DOMString &) const;
            return Method<dom::Node>::call((MyMethod)&dom::Node::childNodesLength, cx, obj, argc, argv, rval);
        }
    }
    JS_ReportError(cx,"JSNode::childNodesLength: wrong number of parameters: %d, 0 or 1 expected", argc);
    return JS_FALSE;
}
static JSBool
getElementById(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Finds a node in the whole document by its id.");
    DOC_PARAM("Id Value", "", DOC_TYPE_STRING);
    DOC_RESET;
    DOC_PARAM("Id value", "", DOC_TYPE_STRING);
    DOC_PARAM("Id attribute name", "", DOC_TYPE_STRING);
    DOC_RVAL("The found node, or null", DOC_TYPE_NODE);
    DOC_END;
    dom::DOMString myId;
    dom::DOMString myIdAttribute = "id";
    dom::NodePtr myNode;
    convertFrom(cx, OBJECT_TO_JSVAL(obj),myNode);

    if (argc == 1 ) {
        convertFrom(cx, argv[0], myId);
    } else if (argc == 2) {
        convertFrom(cx, argv[0], myId);
        convertFrom(cx, argv[1], myIdAttribute);
    } else {
        JS_ReportError(cx,"JSNode::getElementById: wrong number of parameters: %d, 1 or 2 expected", argc);
        return JS_FALSE;
    }
    dom::NodePtr myRetNode = myNode->getElementById(myId, myIdAttribute);
    *rval = as_jsval(cx, myRetNode);
    return JS_TRUE;
}
static JSBool
getAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Retrieves an attribute value by name.");
    DOC_PARAM("Name", "", DOC_TYPE_STRING);
    DOC_RVAL("The typed attribute value as typed js object.", DOC_TYPE_OBJECT);
    DOC_END;
    typedef dom::ValuePtr (dom::Node::*MyMethod)(const dom::DOMString &);
    return Method<dom::Node>::call((MyMethod)&dom::Node::getAttributeValueWrapperPtr, cx, obj, argc, argv, rval);
    return JS_FALSE;
}

static JSBool
addEventListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Registers an event listener, depending on the useCapture parameter, on the capture phase of the DOM event flow or its target and bubbling phases");
    DOC_PARAM("Eventtype", "", DOC_TYPE_STRING);
    DOC_PARAM("Listener", "", DOC_TYPE_OBJECT);
    DOC_PARAM("useCapture", "", DOC_TYPE_BOOLEAN);
    DOC_END;
    return Method<dom::Node>::call(&dom::Node::addEventListener, cx, obj, argc, argv, rval);
}
static JSBool
removeEventListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Removes an event listeners from the default group");
    DOC_PARAM("Eventtype", "", DOC_TYPE_STRING);
    DOC_PARAM("Listener", "", DOC_TYPE_OBJECT);
    DOC_PARAM("useCapture", "", DOC_TYPE_BOOLEAN);
    DOC_END;
    return Method<dom::Node>::call(&dom::Node::removeEventListener, cx, obj, argc, argv, rval);
}
static JSBool
dispatchEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sends an event into the dom event model");
    DOC_PARAM("The Event", "", DOC_TYPE_OBJECT);
    DOC_END;
    return Method<dom::Node>::call(&dom::Node::dispatchEvent, cx, obj, argc, argv, rval);
}
JSBool
debugValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("prints debug information about the value.");
    DOC_END;
    dom::NodePtr myNode = JSNode::getNodePtr(cx,obj);
    myNode->nodeValueWrapperPtr()->printDependendGraph();
    myNode->nodeValueWrapperPtr()->printPrecursorGraph();
    return JS_TRUE;
}
JSFunctionSpec *
JSNode::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        /* name         native          nargs    */
        //            {"insertBefore",     insertBefore,    2},
        {"replaceChild",     replaceChild,    2},
        {"removeChild",      removeChild,     1},
        {"appendChild",      appendChild,     1},
        {"hasChildNodes",    hasChildNodes,   1},
        {"cloneNode",        cloneNode,       1},
        {"toString",         toString,        0},
        {"isSameNode",       isSameNode,      1},
        {"addSchema",        addSchema,       2},
        {"useFactories",     useFactories,    1},
        {"parse",            parse,           1},
        {"parseFile",        parseFile,       1},
        {"saveFile",         saveFile,        1},
        {"childNode",        childNode,       2},
        {"childNodesLength", childNodesLength,1},
        {"getElementById",   getElementById,  2},
        {"getAttribute",     getAttribute,    1},
        {"addEventListener",     addEventListener,       3},
        {"removeEventListener",  removeEventListener,    3},
        {"dispatchEvent",        dispatchEvent,          3},
        {"debugValue",        debugValue,         0},
       {0}
    };
    return myFunctions;
}

static JSBool
createNode(dom::Node::NodeType theNodeType, unsigned int theExpectedArgc,
           JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    if (argc != theExpectedArgc) {
        JS_ReportError(cx,"JSNode::createNode: wrong number of parameters: %d, %d expected", argc, theExpectedArgc);
        return JS_FALSE;
    }
    std::string myArg0;
    std::string myArg1;
    if (argc > 0) {
        myArg0 = as_string(cx, argv[0]);
    }
    if (argc > 1) {
        myArg1 = as_string(cx, argv[1]);
    }
    dom::NodePtr myNewNode;
    try {
        switch (argc) {
            case 0: myNewNode = dom::NodePtr(new dom::Node(theNodeType, 0));
                break;
            case 1: myNewNode = dom::NodePtr(new dom::Node(theNodeType,myArg0, 0));
                break;
            case 2: myNewNode = dom::NodePtr(new dom::Node(theNodeType,myArg0,myArg1, 0));
                break;
            default:
                JS_ReportError(cx,"JSNode::createNode: can not expect %d argument", theExpectedArgc);
                return JS_FALSE;
        }
    }
    catch (asl::Exception & ex) {
        JS_ReportError(cx,"JSNode::createNode: exception: %s", asl::as_string(ex).c_str());
        return JS_FALSE;
    }
    catch (...) {
        JS_ReportError(cx,"JSNode::createNode: unknown exception");
        return JS_FALSE;
    }
    if (myNewNode) {
        JSObject * myNewObject = JSNode::Construct(cx, myNewNode);
        *rval = OBJECT_TO_JSVAL(myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSNode::createNode: new failed");
    return JS_FALSE;
}

static JSBool
createElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates an element node.");
    DOC_PARAM("theNodename", "", DOC_TYPE_STRING);
    DOC_RVAL("The new node", DOC_TYPE_NODE);
    DOC_END;
    return createNode(dom::Node::ELEMENT_NODE, 1, cx, obj, argc, argv, rval);
}
static JSBool
createDocumentFragment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a document fragment node.");
    DOC_RVAL("The new node", DOC_TYPE_NODE);
    DOC_END;
    return createNode(dom::Node::DOCUMENT_FRAGMENT_NODE, 0, cx, obj, argc, argv, rval);
}
static JSBool
createTextNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a text node.");
    DOC_PARAM("theText", "", DOC_TYPE_STRING);
    DOC_RVAL("The new node", DOC_TYPE_NODE);
    DOC_END;
    return createNode(dom::Node::TEXT_NODE, 1, cx, obj, argc, argv, rval);
}
static JSBool
createComment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a comment node.");
    DOC_PARAM("theComment", "The comment", DOC_TYPE_STRING);
    DOC_RVAL("The new node", DOC_TYPE_NODE);
    DOC_END;
    return createNode(dom::Node::COMMENT_NODE, 1, cx, obj, argc, argv, rval);
}
static JSBool
createCDATASection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a CDATA section node.");
    DOC_PARAM("theData", "The CDATA", DOC_TYPE_STRING);
    DOC_RVAL("The new node", DOC_TYPE_NODE);
    DOC_END;
    return createNode(dom::Node::CDATA_SECTION_NODE, 1, cx, obj, argc, argv, rval);
}
static JSBool
createProcessingInstruction(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a processing instruction node.");
    DOC_PARAM("theName", "", DOC_TYPE_STRING);
    DOC_PARAM("theValue", "", DOC_TYPE_STRING);
    DOC_RVAL("The new node", DOC_TYPE_NODE);
    DOC_END;
    return createNode(dom::Node::PROCESSING_INSTRUCTION_NODE, 2, cx, obj, argc, argv, rval);
}
static JSBool
createAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a attribute node.");
    DOC_PARAM("theName", "", DOC_TYPE_STRING);
    DOC_RVAL("The new node", DOC_TYPE_NODE);
    DOC_END;
    return createNode(dom::Node::ATTRIBUTE_NODE, 1, cx, obj, argc, argv, rval);
}
static JSBool
createEntityReference(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a entitiy reference node.");
    DOC_PARAM("theName", "", DOC_TYPE_STRING);
    DOC_RVAL("The new node", DOC_TYPE_NODE);
    DOC_END;
    return createNode(dom::Node::ENTITY_REFERENCE_NODE, 1, cx, obj, argc, argv, rval);
}
static JSBool
createDocument(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new document node");
    DOC_RVAL("The new node", DOC_TYPE_NODE);
    DOC_END;
    return createNode(dom::Node::DOCUMENT_NODE, 0, cx, obj, argc, argv, rval);
}
JSFunctionSpec *
JSNode::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        /* name         native          nargs    */
        {"createElement",              createElement,           1},
        {"createDocumentFragment",     createDocumentFragment,  0},
        {"createTextNode",             createTextNode,          1},
        {"createComment",              createComment,           1},
        {"createCDATASection",         createCDATASection,      1},
        {"createProcessingInstruction",createProcessingInstruction,2},
        {"createAttribute",            createAttribute,         1},
        {"createEntityReference",      createEntityReference,   1},
        {"createDocument",             createDocument,   0},
        //  {"createNodeList",             createNodeList,          0},
        {0}
    };
    return myFunctions;
}


enum PropertyNumbers {PROP_nodeName,
PROP_nodeValue,
//PROP_nodeValueTyped,
PROP_nodeType,
PROP_parentNode,
PROP_rootNode,
PROP_childNodes,
PROP_firstChild,
PROP_lastChild,
PROP_previousSibling,
PROP_nextSibling,
PROP_attributes,
PROP_ok,
PROP_schema,
PROP_nodeUniqueId,
PROP_nodeVersion,
PROP_nodeValueTypeName,

// const static property ids:
PROP_ELEMENT_NODE,
PROP_ATTRIBUTE_NODE,
PROP_TEXT_NODE,
PROP_CDATA_SECTION_NODE,
PROP_ENTITY_REFERENCE_NODE,
PROP_ENTITY_NODE,
PROP_PROCESSING_INSTRUCTION_NODE,
PROP_COMMENT_NODE,
PROP_DOCUMENT_NODE,
PROP_DOCUMENT_TYPE_NODE,
PROP_DOCUMENT_FRAGMENT_NODE,
PROP_NOTATION_NODE
};

JSPropertySpec *
JSNode::Properties() {
    static JSPropertySpec myProperties[] = {
        {"nodeName",        PROP_nodeName,       JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute DOMString
        {"nodeValue",       PROP_nodeValue,      JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},   //          attribute DOMString
        {"nodeType",        PROP_nodeType,       JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute unsigned short
        {"parentNode",      PROP_parentNode,     JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // acIDOMNode
        {"rootNode",        PROP_rootNode,       JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // acIDOMNode
        {"childNodes",      PROP_childNodes,     JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // acIDOMNodeList
        {"firstChild",      PROP_firstChild,     JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute acIDOMNode
        {"lastChild",       PROP_lastChild,      JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute acIDOMNode
        {"previousSibling", PROP_previousSibling,JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute acIDOMNode
        {"nextSibling",     PROP_nextSibling,    JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute acIDOMNode
        {"attributes",      PROP_attributes,     JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute acIDOMNamedNodeMap
        {"ok",              PROP_ok,             JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute bool
        {"schema",          PROP_schema,         JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute acIDOMNode
        {"nodeVersion",     PROP_nodeVersion,    JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute string
        {"nodeUniqueId",    PROP_nodeUniqueId,   JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute string
        {"nodeValueTypeName",PROP_nodeValueTypeName,JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},   // readonly attribute string
        {0}
    };
    return myProperties;
}

#define DEFINE_NODETYPE_PROPERTY(NAME) { #NAME, PROP_ ## NAME , dom::Node::NAME }

JSConstIntPropertySpec *
JSNode::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        DEFINE_NODETYPE_PROPERTY(ELEMENT_NODE),
            DEFINE_NODETYPE_PROPERTY(ATTRIBUTE_NODE),
            DEFINE_NODETYPE_PROPERTY(TEXT_NODE),
            DEFINE_NODETYPE_PROPERTY(CDATA_SECTION_NODE),
            DEFINE_NODETYPE_PROPERTY(ENTITY_REFERENCE_NODE),
            DEFINE_NODETYPE_PROPERTY(ENTITY_NODE),
            DEFINE_NODETYPE_PROPERTY(PROCESSING_INSTRUCTION_NODE),
            DEFINE_NODETYPE_PROPERTY(COMMENT_NODE),
            DEFINE_NODETYPE_PROPERTY(DOCUMENT_NODE),
            DEFINE_NODETYPE_PROPERTY(DOCUMENT_TYPE_NODE),
            DEFINE_NODETYPE_PROPERTY(DOCUMENT_FRAGMENT_NODE),
            DEFINE_NODETYPE_PROPERTY(NOTATION_NODE),
            DEFINE_NODETYPE_PROPERTY(ELEMENT_NODE),
        {0}
    };
    return myProperties;
};

JSPropertySpec *
JSNode::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSBool
JSNode::getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    try {
        IF_NOISY(printParams("JSNode::getProperty",cx,obj,id,vp));
        dom::NodePtr myNode = getNodePtr(cx,obj);
        if (!myNode) {
            JS_ReportError(cx, "JSNode::getProperty: bad object type");
            return JS_FALSE;
        }
        if (JSVAL_IS_INT(id)) {
            int myIndex = JSVAL_TO_INT(id);

            switch (myIndex) {
                case PROP_nodeName:
                    *vp = as_jsval(cx, myNode->nodeName());
                    return JS_TRUE;
                case PROP_nodeValue:
                    //*vp = as_jsval(cx, myNode->nodeValue());
                    *vp = as_jsval(cx, myNode->nodeValueWrapperPtr());
                    return JS_TRUE;
                case PROP_nodeType:
                    *vp = as_jsval(cx, myNode->nodeType());
                    break;
                case PROP_parentNode:
                    {
                        dom::Node * myParent = myNode->parentNode();
                        if (myParent && myParent->self()) {
                            *vp = as_jsval(cx, myParent->self().lock());
                            return JS_TRUE;
                        }
                    }
                    break;
                case PROP_rootNode:
                    {
                        dom::Node * myRoot = myNode->getRootNode();
                        if (myRoot->self()) {
                            *vp = as_jsval(cx, myRoot->self().lock());
                            return JS_TRUE;
                         }
                    }
                    break;
                case PROP_childNodes:
                    *vp = as_jsval(cx, myNode, &myNode->childNodes());
                    break;
                case PROP_firstChild:
                    *vp = as_jsval(cx, myNode->firstChild());
                    break;
                case PROP_lastChild:
                    *vp = as_jsval(cx, myNode->lastChild());
                    break;
                case PROP_previousSibling:
                    *vp = as_jsval(cx, myNode->previousSibling());
                    break;
                case PROP_nextSibling:
                    *vp = as_jsval(cx, myNode->nextSibling());
                    break;
                case PROP_attributes:
                    *vp = as_jsval(cx, myNode, &myNode->attributes());
                    break;
                case PROP_ok:
                    *vp = as_jsval(cx, myNode->operator bool());
                    break;
                case PROP_schema:
                    {
                        const dom::NodePtr mySchemaNode = myNode->getSchema();
                        *vp = as_jsval(cx, mySchemaNode);
                    }
                    break;
                case PROP_nodeValueTypeName:
                    *vp = as_jsval(cx, myNode->nodeValueTypeName());
                    break;
               case PROP_nodeVersion:
                   *vp = as_jsval(cx, asl::as_string(myNode->nodeVersion()));
                    break;
               case PROP_nodeUniqueId:
                    *vp = as_jsval(cx, asl::as_string(myNode->getUniqueId()));
                    break;
            }
        } else {
            JSString *myJSStr = JS_ValueToString(cx, id);
            std::string myProperty = JS_GetStringBytes(myJSStr);
            AC_TRACE << "JSNode::getProperty:" << myProperty << endl;
            AC_TRACE << "JSNode::getProperty: nodeName = " <<myNode->nodeName() << endl;
            AC_TRACE << "JSNode::getProperty: myNode = " <<*myNode << endl;

            // First: Check if property exists as attribute
            dom::NodePtr myAttrNode = myNode->attributes().getNamedItem(myProperty);
            if (myAttrNode) {
                AC_TRACE << "JSNode::getProperty: myAttrNode = " <<*myAttrNode;
                *vp = as_jsval(cx, myAttrNode->nodeValueWrapperPtr());
                return JS_TRUE;
            }

            // Second: Check if there are dynamic or property plug attributes in the facade
            if (myNode->hasFacade()) {
                dom::FacadePtr myFacade = myNode->getFacade();
                if (myFacade) {
                    AC_TRACE << "JSNode::hasFacade " << myProperty << endl;
                    dom::NodePtr myAttrNode = myFacade->getNamedItem(myProperty);
                    if (myAttrNode) {
                        AC_TRACE << "JSNode::getProperty: myAttrNode-Facade = " <<*myAttrNode;
                        *vp = as_jsval(cx, myAttrNode->nodeValueWrapperPtr());
                        return JS_TRUE;
                    }                    
		            // 2.5: Check for children
                    dom::NodePtr myChildNode = myFacade->getChildNode(myProperty);
                    if (myChildNode) {
                        AC_TRACE << "JSNode::getProperty: myAttrNode-Facade = " <<*myChildNode;
                        *vp = as_jsval(cx, myChildNode);
                        return JS_TRUE;
                    }                    				
					// ask the facades propertylist (features, properties, etc)
                    dom::NodePtr myPropertyNode = myFacade->getProperty(myProperty);
                    if (myPropertyNode) {
                        *vp = as_jsval(cx, myPropertyNode->nodeValueWrapperPtr());
                        return JS_TRUE;
					}

                }
            }

            // Third: Check again for attributes, because the faced creates default values.
            myAttrNode = myNode->attributes().getNamedItem(myProperty);
            if (myAttrNode) {
                IF_NOISY(AC_TRACE << "JSNode::getProperty: myAttrNode = " <<*myAttrNode << endl);
                *vp = as_jsval(cx, myAttrNode->nodeValueWrapperPtr());
                return JS_TRUE;
            }
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION
}

JSBool
JSNode::setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    try {
        IF_NOISY(printParams("JSVector::setProperty",cx,obj,id,vp));
        dom::NodePtr myNode = getNodePtr(cx,obj);
        if (!myNode) {
            JS_ReportError(cx, "JSNode::setProperty: bad object type");
            return JS_FALSE;
        }
        if (JSVAL_IS_INT(id)) {
            int myIndex = JSVAL_TO_INT(id);
            switch (myIndex) {
            case PROP_nodeValue:
                myNode->nodeValue(as_string(cx,*vp));
                return JS_TRUE;
            }
        } else {
            JSString *myJSStr = JS_ValueToString(cx, id);
            std::string myProperty = JS_GetStringBytes(myJSStr);
            IF_NOISY(
                AC_TRACE << "JSNode::setProperty:" << myProperty << endl;
                AC_TRACE << "JSNode::setProperty: nodeName = " << myNode->nodeName() << endl;
                AC_TRACE << "JSNode::setProperty: myNode = " << *myNode << endl;
            )
            dom::NodePtr myAttrNode = myNode->attributes().getNamedItem(myProperty);
            if (myAttrNode) {
                IF_NOISY(AC_TRACE << "JSNode::setProperty: myAttrNode = " <<*myAttrNode << endl);
                // Try to convert array to strings
                std::string myResult;
                if (!JSA_ArrayToString(cx, vp, myResult)) {
                    myResult = as_string(cx,*vp);
                }
                myAttrNode->nodeValue(myResult);
                return JS_TRUE;
            } else {
                if (myNode->hasFacade()) {
                    dom::FacadePtr myFacade = myNode->getFacade();
                    if (myFacade) {
                        dom::NodePtr myAttrNode = myFacade->getNamedItem(myProperty);
                        if (myAttrNode) {
                            IF_NOISY(AC_TRACE << "JSNode::setProperty: myAttrNode-Facade = " <<*myAttrNode << endl);
#define FACADE_ATTRIBUTES_WRITE_PROTECED
#ifdef FACADE_ATTRIBUTES_WRITE_PROTECED
                            AC_ERROR << "JSNode::setProperty: facade attribute '"<<myProperty<<"' is not writeable."<<endl;
                            return JS_FALSE;
#else
                            myAttrNode->nodeValue(as_string(cx,*vp));
                            return JS_TRUE;
#endif
                        }
						// ask the facades propertylist (features, properties, etc)
                        dom::NodePtr myPropertyNode = myFacade->getProperty(myProperty);
                        if (myPropertyNode) {
                            myPropertyNode->nodeValue(as_string(cx,*vp));
                            return JS_TRUE;
						}
                    }
                }

            }
            // retry to check for attrribute again; it may have been created by the facade
            myAttrNode = myNode->attributes().getNamedItem(myProperty);
            if (myAttrNode) {
                IF_NOISY(AC_TRACE << "JSNode::setProperty: myAttrNode-post = " <<*myAttrNode << endl);
                myAttrNode->nodeValue(as_string(cx,*vp));
                return JS_TRUE;
            } else {
                myNode->appendAttribute(myProperty, as_string(cx,*vp));
            }
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION
}

JSBool
JSNode::newResolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp)
{
    IF_NOISY(
        printParams("newresolve",cx,obj,id);
        if (flags & JSRESOLVE_QUALIFIED) {
            AC_TRACE << "JSRESOLVE_QUALIFIED"<<endl;
        }
        if (flags & JSRESOLVE_ASSIGNING) {
            AC_TRACE << "JSRESOLVE_ASSIGNING"<<endl;
        }
        AC_TRACE << "objp=" << (void*)*objp<<endl;
    )
    if (!JSVAL_IS_INT(id)) {
        JSString *myJSStr = JS_ValueToString(cx, id);
        std::string myProperty = JS_GetStringBytes(myJSStr);

        if(!JS_GetPrivate(cx,obj)) {
            // obj is a class object, dont do anything
            *objp = 0;
            return JS_TRUE;
        }
        dom::NodePtr myNode = getNodePtr(cx,obj);
        if (!myNode) {
            JS_ReportError(cx, "JSNode::setProperty: bad object type");
            return JS_FALSE;
        }

        IF_NOISY(
            AC_TRACE << "JSNode::newresolve: " << myProperty << endl;
            AC_TRACE << "JSNode::newresolve: nodeName = " << myNode->nodeName() << endl;
            AC_TRACE << "JSNode::newresolve: myNode = " << *myNode << endl;
        )

        dom::NodePtr myAttrNode = myNode->attributes().getNamedItem(myProperty);
        if (myAttrNode) {
            IF_NOISY(AC_TRACE << "JSNode::newResolve: found attribute: "<< myProperty << endl);

            dom::ValuePtr myVal = myAttrNode->nodeValueWrapperPtr();
            jsval myJSVal = as_jsval(cx,myVal);

            if (!JS_DefineProperty(cx, obj, myProperty.c_str(),
                myJSVal, 0,0, JSPROP_ENUMERATE))
            {
                JS_ReportError(cx, "JSNode::newResolve: JS_DefineProperty '%s' failed", myProperty.c_str());
                return JS_FALSE;
            }
            *objp = obj;
            IF_NOISY(AC_TRACE << "JSNode::newResolve: return objp= "<< (void*)(*objp) << endl);
            return JS_TRUE;
        } else {
            IF_NOISY(AC_TRACE << "JSNode::newResolve: attribute: "<< myProperty << " does not exist." << endl);
            *objp = 0;
            return JS_TRUE;
        }
    }
    *objp = 0;
    return JS_TRUE;
}

JSBool
JSNode::customConvert(JSContext *cx, JSObject *obj, JSType type, jsval *vp)
{
    IF_NOISY(printParams("customConvert",cx,obj,type,vp));
    /*
    if (type == JSTYPE_BOOLEAN) {
    dom::NodePtr myNode = getNodePtr(cx,obj);
    bool myNodeOK = *myNode;
    *vp = as_jsval(cx, myNodeOK);
    return JS_TRUE;
    }
    */
    return JS_ConvertStub(cx,obj,type,vp);
}

void
JSNode::finalizeImpl(JSContext *cx, JSObject *obj)
{
    IF_NOISY_F(AC_TRACE << "finalize "<<ClassName() << (void*)obj << endl);
    JSNode * myImpl = static_cast<JSNode*>(JS_GetPrivate(cx,obj));
    delete myImpl;
}

class JSClassSingleton :
    public asl::Singleton<JSClassSingleton>
{
public:
    friend class asl::SingletonManager;
    JSClass * getClass() {
        return &_myJSClass;
    }
private:
    JSClassSingleton() {
        JSClass myTempClass = {
            JSNode::ClassName(),                   // const char          *name;
            JSCLASS_HAS_PRIVATE |                  // uint32              flags;
            //JSCLASS_HAS_PRIVATE |               /* objects have private slot */
            //JSCLASS_NEW_ENUMERATE |             /* has JSNewEnumerateOp hook */
            JSCLASS_NEW_RESOLVE //|                 /* has JSNewResolveOp hook */
            //JSCLASS_PRIVATE_IS_NSISUPPORTS |    /* private is (nsISupports *) */
            //JSCLASS_SHARE_ALL_PROPERTIES |      /* all properties are SHARED */
            //JSCLASS_NEW_RESOLVE_GETS_START      //JSNewResolveOp gets starting
            //object in prototype chain
            //passed in via *objp in/out
            //parameter */
            ,
            /* Mandatory non-null function pointer members. */
            NoisyAddProperty,    // JSPropertyOp        addProperty;
            NoisyDelProperty,    // JSPropertyOp        delProperty;
            JSNode::getProperty,         // JSPropertyOp        getProperty;
            JSNode::setProperty,         // JSPropertyOp        setProperty;
            NoisyEnumerate,      // JSEnumerateOp       enumerate;
            //NoisyResolve,        // JSResolveOp         resolve;
            (JSResolveOp)JSNode::newResolve,  // JSResolveOp         resolve;
            JSNode::customConvert,        // JSConvertOp         convert;
            JSNode::finalizeImpl,       // JSFinalizeOp        finalize;

            JSCLASS_NO_OPTIONAL_MEMBERS
            /* Optionally non-null members start here. */
            // JSGetObjectOps      getObjectOps;
            // JSCheckAccessOp     checkAccess;
            // JSNative            call;
            // JSNative            construct;
            // JSXDRObjectOp       xdrObject;
            // JSHasInstanceOp     hasInstance;
            // JSMarkOp            mark;
            // jsword              spare;
        };
        _myJSClass = myTempClass;
    }
private:
    JSClass _myJSClass;
};

JSClass *
JSNode::Class() {
    return JSClassSingleton::get().getClass();
}

JSObject *
JSNode::Construct(JSContext *cx, dom::NodePtr theNode) {
    JSObject * myNewObj = JS_ConstructObject(cx, Class(), 0, 0);
    dom::NodePtr & myNode = JSNode::getNodePtrRef(cx,myNewObj);
    myNode = theNode;
    return myNewObj;
}

JSBool
JSNode::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructing a node from scratch or from a string.");
    DOC_END;
    IF_NOISY2(AC_TRACE << "Constructor argc =" << argc << endl);
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSNode * myNewObject = 0;
    if (argc == 0) {
        myNewObject=new JSNode(dom::NodePtr(new dom::Node));
    } else {
        if (argc == 1) {
            IF_NOISY(AC_TRACE << "Node::Constructor: constructing Node from string"<<endl);
            std::string myXmlText = as_string(cx, argv[0]);
            myNewObject=new JSNode(dom::NodePtr(new dom::Document(myXmlText)));
        }
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSVector::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSNode::initClass(JSContext *cx, JSObject *theGlobalObject) {
    DOC_CREATE(JSNode);
    JSObject * myProtoObj = JS_InitClass(cx, theGlobalObject, NULL, Class(),
        /* native constructor function and min arg count */
        Constructor, 0,
        /* prototype object properties and methods -- these
        will be "inherited" by all instances through
        delegation up the instance's prototype link. */
        Properties(), Functions(),

        /* class constructor properties and methods */
        //static_props, static_methods
        0, StaticFunctions()
        );
    jsval myConstructorFuncObjVal;
    if (JS_GetProperty(cx, theGlobalObject, ClassName(), &myConstructorFuncObjVal)) {
        JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
        JSA_DefineConstInts(cx, myConstructorFuncObj, ConstIntProperties());
    }
    return myProtoObj;
}

jsval as_jsval(JSContext *cx, dom::NodePtr theNode) {
    if (theNode) {
        JSObject * myReturnObject = JSNode::Construct(cx, theNode);
        return OBJECT_TO_JSVAL(myReturnObject);
    } else {
        return JSVAL_NULL;
    }
}

bool convertFrom(JSContext *cx, jsval theValue, dom::NodePtr & thePtr) {
    if (JSVAL_IS_NULL(theValue)) {
        thePtr = dom::NodePtr(0);
        return true;
    }
    if ( JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<dom::Node>::Class()) {
                thePtr = JSClassTraits<dom::Node>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, dom::Node & theNode) {
    if (JSVAL_IS_NULL(theValue)) {
        return false;
    }
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<dom::Node>::Class()) {
                theNode = *JSClassTraits<dom::Node>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

}
