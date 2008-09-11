//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSCOMBOBOX_INCLUDED_
#define _Y60_ACGTKSHELL_JSCOMBOBOX_INCLUDED_

#include "JSBin.h"

#include <y60/jsbase/JSWrapper.h>
#include <gtkmm/combobox.h>

#include <asl/base/string_functions.h>

namespace jslib {

class JSComboBox : public JSWrapper<Gtk::ComboBox, asl::Ptr<Gtk::ComboBox>, StaticAccessProtocol> {
        JSComboBox();  // hide default constructor
        typedef JSBin JSBASE;
    public:
        virtual ~JSComboBox() {
        }
        typedef Gtk::ComboBox NATIVE;
        typedef asl::Ptr<Gtk::ComboBox> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "ComboBox";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_active_text = JSBASE::PROP_END,
            PROP_active_index,
            PROP_signal_changed,
            PROP_END
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

        JSComboBox(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSComboBox & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSComboBox &>(JSComboBox::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSComboBox::NATIVE>
    : public JSClassTraitsWrapper<JSComboBox::NATIVE, JSComboBox> {};

jsval as_jsval(JSContext *cx, JSComboBox::OWNERPTR theOwner, JSComboBox::NATIVE * theNative);

} // namespace

#endif


