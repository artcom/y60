//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSCOLOR_SELECTION_DIALOG_INCLUDED_
#define _Y60_ACGTKSHELL_JSCOLOR_SELECTION_DIALOG_INCLUDED_

#include "JSDialog.h"
#include <y60/jsbase/JSWrapper.h>
#include <gtkmm/colorselection.h>

#include <asl/base/string_functions.h>

namespace jslib {

class JSColorSelectionDialog : public JSWrapper<Gtk::ColorSelectionDialog, asl::Ptr<Gtk::ColorSelectionDialog>, StaticAccessProtocol> {
    private:
        JSColorSelectionDialog();  // hide default constructor
        typedef JSDialog JSBASE;
    public:
        typedef Gtk::ColorSelectionDialog NATIVE;
        typedef asl::Ptr<Gtk::ColorSelectionDialog> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "ColorSelectionDialog";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_cancel_button = JSBASE::PROP_END,
            PROP_help_button,
            PROP_ok_button,
            PROP_colorselection,
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

        JSColorSelectionDialog(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {
        }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSColorSelectionDialog & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSColorSelectionDialog &>(JSColorSelectionDialog::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSColorSelectionDialog::NATIVE>
    : public JSClassTraitsWrapper<JSColorSelectionDialog::NATIVE, JSColorSelectionDialog> {};

jsval as_jsval(JSContext *cx, JSColorSelectionDialog::OWNERPTR theOwner, JSColorSelectionDialog::NATIVE * theNative);

} // namespace

#endif


