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
//   $RCSfile: JSNamedNodeMap.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/11/07 04:50:20 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_NAMEDNODEMAP_H_INCLUDED_
#define _Y60_ACXPSHELL_NAMEDNODEMAP_H_INCLUDED_

#include "y60_jsbase_settings.h"

#include "JSNode.h"
#include "JScppUtils.h"
#include "JSNodeList.h"
#include "JSWrapper.h"

namespace jslib {

class Y60_JSBASE_DECL JSNamedNodeMap : public JSWrapper<dom::NamedNodeMap,dom::NodePtr,StaticAccessProtocol> {
public:
    typedef JSWrapper<dom::NamedNodeMap,dom::NodePtr,StaticAccessProtocol> Base;

    static const char * ClassName();

    static JSBool
    item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    appendNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSBool
    getNamedItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static JSFunctionSpec * Functions();
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();


    virtual unsigned long length() const;
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
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), 0, 0, 0);
        DOC_CREATE(JSNamedNodeMap);
        return myClass;
    }
};

template <>
struct JSClassTraits<dom::NamedNodeMap>
    : public JSClassTraitsWrapper<dom::NamedNodeMap, JSNamedNodeMap> {};

Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, dom::NodePtr theNode, dom::NamedNodeMap * theNodeMap);

}


#endif
