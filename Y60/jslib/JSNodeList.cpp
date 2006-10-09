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
#include "JSNodeList.impl"

namespace jslib {

template class JSNodeListBase<dom::NodeList>;

jsval as_jsval(JSContext *cx, dom::NodePtr theNode, dom::NodeList * theNodeList) {
    JSObject * myObject = JSNodeListBase<dom::NodeList>::Construct(cx, theNode, theNodeList);
    return OBJECT_TO_JSVAL(myObject);
}

}
