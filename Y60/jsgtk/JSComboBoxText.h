//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSCOMBOBOXTEXT_INCLUDED_
#define _Y60_ACGTKSHELL_JSCOMBOBOXTEXT_INCLUDED_

#include "JSComboBox.h"

#include <y60/jsbase/JSWrapper.h>
#include <gtkmm/comboboxtext.h>

#include <asl/base/string_functions.h>

namespace jslib {

class JSComboBoxText : public JSWrapper<Gtk::ComboBoxText, asl::Ptr<Gtk::ComboBoxText>, StaticAccessProtocol> {
        JSComboBoxText();  // hide default constructor
        typedef JSComboBox JSBASE;
    public:
        virtual ~JSComboBoxText() {
        }
        typedef Gtk::ComboBoxText NATIVE;
        typedef asl::Ptr<Gtk::ComboBoxText> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "ComboBoxText";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_active_text = JSBASE::PROP_END,
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

        JSComboBoxText(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSComboBoxText & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSComboBoxText &>(JSComboBoxText::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSComboBoxText::NATIVE>
    : public JSClassTraitsWrapper<JSComboBoxText::NATIVE, JSComboBoxText> {};

jsval as_jsval(JSContext *cx, JSComboBoxText::OWNERPTR theOwner, JSComboBoxText::NATIVE * theNative);

} // namespace _Y60_ACGTKSHELL_JSCOMBOBOXTEXT_INCLUDED_

#endif


