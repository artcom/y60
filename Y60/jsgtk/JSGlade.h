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
//   $RCSfile: JSGlade.h,v $
//   $Author: martin $
//   $Revision: 1.1 $
//   $Date: 2004/10/27 16:30:11 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSGLADE_INCLUDED_
#define _Y60_ACGTKSHELL_JSGLADE_INCLUDED_

#include <y60/JSWrapper.h>
#include <libglademm/xml.h>

#include <asl/string_functions.h>

namespace jslib {

class JSGlade : public JSWrapper<Gnome::Glade::Xml, Glib::RefPtr<Gnome::Glade::Xml>, StaticAccessProtocol>
{
        JSGlade() {}
    public:
        typedef Gnome::Glade::Xml NATIVE;
        typedef Glib::RefPtr<Gnome::Glade::Xml> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Glade";
        }
        static JSFunctionSpec * Functions();

        virtual unsigned long length() const {
            return 1;
        }

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        //virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner) {
            return Base::Construct(cx, theOwner, 0);
        }

        JSGlade(OWNERPTR theOwner)
            : Base(theOwner, 0)
        {}

        static JSPropertySpec * Properties();
        // static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSGlade & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSGlade &>(JSGlade::getJSWrapper(cx,obj));
        }

    private:
};


template <>
struct JSClassTraits<JSGlade::NATIVE> 
    : public JSClassTraitsWrapper<JSGlade::NATIVE, JSGlade> {};

jsval as_jsval(JSContext *cx, JSGlade::OWNERPTR theOwner);
//jsval as_jsval(JSContext *cx, JSGlade::OWNERPTR theOwner, JSGlade::NATIVE * theSerial);

}

#endif


