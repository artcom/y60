//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSEventBox.h,v $
//   $Author: david $
//   $Revision: 1.4 $
//   $Date: 2005/02/03 15:57:49 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSEVENT_BOX_INCLUDED_
#define _Y60_ACGTKSHELL_JSEVENT_BOX_INCLUDED_

#include "JSBin.h"
#include <y60/JSWrapper.h>
#include <gtkmm/eventbox.h>

#include <asl/string_functions.h>

namespace jslib {

class JSEventBox : public JSWrapper<Gtk::EventBox, asl::Ptr<Gtk::EventBox>, StaticAccessProtocol> {
        JSEventBox();  // hide default constructor
    typedef JSBin JSBASE;
    public:
        virtual ~JSEventBox() {
        }
        typedef Gtk::EventBox NATIVE;
        typedef asl::Ptr<Gtk::EventBox> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "EventBox";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
             PROP_END = JSBASE::PROP_END
        };
        static JSPropertySpec * Properties();

        virtual unsigned long length() const {
            return 1;
        }

        static JSBool getPropertySwitch(NATIVE & theNative, unsigned long theID,
                JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID, JSContext *cx,
                JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSEventBox(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSEventBox & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSEventBox &>(JSEventBox::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSEventBox::NATIVE>
    : public JSClassTraitsWrapper<JSEventBox::NATIVE, JSEventBox> {};

jsval as_jsval(JSContext *cx, JSEventBox::OWNERPTR theOwner, JSEventBox::NATIVE * theEventBox);

} // namespace

#endif



