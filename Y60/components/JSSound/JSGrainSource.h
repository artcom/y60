//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _JSGRAINSOURCE_H_
#define _JSGRAINSOURCE_H_

#include <y60/JSWrapper.h>
#include <y60/GrainSource.h>

namespace jslib {

    class JSGrainSource : public jslib::JSWrapper<y60::GrainSource, y60::GrainSourcePtr, jslib::StaticAccessProtocol> {

        JSGrainSource() {};

    public:

        typedef y60::GrainSource NATIVE;
        typedef y60::GrainSourcePtr OWNERPTR;
        typedef jslib::JSWrapper<NATIVE, OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSGrainSource(OWNERPTR theOwner, NATIVE * theNative) :
            Base(theOwner, theNative)
        {}

        ~JSGrainSource() {};

        static const char * ClassName() {
            return "GrainSource";
        }

        virtual unsigned long length() const {
            return 1;
        }

        static JSFunctionSpec * Functions();
        static JSFunctionSpec * StaticFunctions();
        
        enum PropertyNumbers {
            PROP_volume = -100,
            PROP_size,
            PROP_sizejitter,
            PROP_rate,
            PROP_ratejitter,
            PROP_position,
            PROP_positionjitter,
            PROP_ratio
        };
        static jslib::JSConstIntPropertySpec * ConstIntProperties();
        static JSPropertySpec * Properties();
        static JSPropertySpec * StaticProperties();

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
   
        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }        
    
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
    
        static JSGrainSource & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSGrainSource &>(JSGrainSource::getJSWrapper(cx,obj));
        }
    };

    bool convertFrom(JSContext *cx, jsval theValue, y60::GrainSourcePtr & theGrainSource);

    jsval as_jsval(JSContext *cx, JSGrainSource::OWNERPTR theOwner);
    jsval as_jsval(JSContext *cx, JSGrainSource::OWNERPTR theOwner, JSGrainSource::NATIVE * theNative);

    template <>
    struct JSClassTraits<JSGrainSource::NATIVE> 
        : public JSClassTraitsWrapper<JSGrainSource::NATIVE, JSGrainSource> {};

}
    
#endif
