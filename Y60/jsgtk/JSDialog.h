//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSDIALOG_INCLUDED_
#define _Y60_ACGTKSHELL_JSDIALOG_INCLUDED_

#include "JSWindow.h"
#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
#pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/dialog.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/base/string_functions.h>

namespace jslib {

class JSDialog : public JSWrapper<Gtk::Dialog, asl::Ptr<Gtk::Dialog>, StaticAccessProtocol> {
    private:
        JSDialog();  // hide default constructor
        typedef JSWindow JSBASE;
    public:
        typedef Gtk::Dialog NATIVE;
        typedef asl::Ptr<Gtk::Dialog> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Dialog";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_signal_response = JSBASE::PROP_END,
            PROP_RESPONSE_NONE,
            PROP_RESPONSE_REJECT,
            PROP_RESPONSE_ACCEPT,
            PROP_RESPONSE_DELETE_EVENT,
            PROP_RESPONSE_OK,
            PROP_RESPONSE_CANCEL,
            PROP_RESPONSE_CLOSE,
            PROP_RESPONSE_YES,
            PROP_RESPONSE_NO,
            PROP_RESPONSE_APPLY,
            PROP_RESPONSE_HELP,
            PROP_vbox,
            PROP_has_separator,
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

        JSDialog(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {
        }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSDialog & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSDialog &>(JSDialog::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSDialog::NATIVE>
    : public JSClassTraitsWrapper<JSDialog::NATIVE, JSDialog> {};

jsval as_jsval(JSContext *cx, JSDialog::OWNERPTR theOwner, JSDialog::NATIVE * theNative);

} // namespace

#endif


