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

#ifndef _Y60_ACXPSHELL_JSNODELIST_H_INCLUDED_
#define _Y60_ACXPSHELL_JSNODELIST_H_INCLUDED_

#include "y60_jsbase_settings.h"

#include "JSWrapper.h"
#include "JSNode.h"

#include <asl/dom/Value.h>
#include <asl/dom/Nodes.h>
#include <asl/dom/NodeList.h>

namespace jslib {

class Y60_JSBASE_DECL JSNodeList: public JSWrapper<dom::NodeList,dom::NodePtr,StaticAccessProtocol> {
public:
    typedef JSWrapper<dom::NodeList,dom::NodePtr,StaticAccessProtocol> Base;

    static const char * ClassName();
    static JSBool
    item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    appendNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

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

Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, dom::NodePtr theNode, dom::NodeList * theNodeList);
Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, dom::NodeList  & theNodeList);
}

#endif

