//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


#ifndef _ac_y60_components_jsmesh_h_
#define _ac_y60_components_jsmesh_h_

#include "Mesh.h"
#include <y60/JSWrapper.h>

namespace jslib {
    
class JSMesh : public jslib::JSWrapper<y60::Mesh, asl::Ptr<y60::Mesh> , jslib::StaticAccessProtocol> {
        JSMesh() {}
    public:
        typedef y60::Mesh NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSMesh(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        virtual ~JSMesh() {};
        
        static const char * ClassName() {
            return "Mesh";
        }
        static JSFunctionSpec * Functions();
        static JSFunctionSpec * StaticFunctions();

        enum PropertyNumbers {
            PROP_default_window = -100,
            PROP_length,
            PROP_signal_progress,
            PROP_simplify
        };
        static JSPropertySpec * Properties();

        virtual unsigned long length() const {
            return 1;
        }

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }        

        static jslib::JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSMesh & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSMesh &>(JSMesh::getJSWrapper(cx,obj));
        }
};

template <>
struct JSClassTraits<JSMesh::NATIVE> 
    : public JSClassTraitsWrapper<JSMesh::NATIVE, JSMesh> {};


bool convertFrom(JSContext *cx, jsval theValue, JSMesh::OWNERPTR & theMesh);

jsval as_jsval(JSContext *cx, JSMesh::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSMesh::OWNERPTR theOwner, JSMesh::NATIVE * theMesh);

} // namespace jslib
#endif


