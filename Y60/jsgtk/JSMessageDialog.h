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
//   $RCSfile: JSMessageDialog.h,v $
//   $Author: danielk $
//   $Revision: 1.1 $
//   $Date: 2005/01/14 14:36:27 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSMESSAGEDIALOG_INCLUDED_
#define _Y60_ACGTKSHELL_JSMESSAGEDIALOG_INCLUDED_

#include "JSDialog.h"
#include <y60/JSWrapper.h>
#include <gtkmm/messagedialog.h>

#include <asl/string_functions.h>

namespace jslib {

class JSMessageDialog : public JSWrapper<Gtk::MessageDialog, asl::Ptr<Gtk::MessageDialog>, StaticAccessProtocol> {
    private:
        JSMessageDialog();  // hide default constructor
        typedef JSDialog JSBASE;
    public:
        typedef Gtk::MessageDialog NATIVE;
        typedef asl::Ptr<Gtk::MessageDialog> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "MessageDialog";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_MESSAGE_TYPE_INFO = JSBASE::PROP_END,
            PROP_MESSAGE_TYPE_WARNING,
            PROP_MESSAGE_TYPE_QUESTION,
            PROP_MESSAGE_TYPE_ERROR,

            PROP_BUTTONS_TYPE_NONE,
            PROP_BUTTONS_TYPE_OK,
            PROP_BUTTONS_TYPE_CLOSE,
            PROP_BUTTONS_TYPE_CANCEL,
            PROP_BUTTONS_TYPE_YES_NO,
            PROP_BUTTONS_TYPE_OK_CANCEL,
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

        JSMessageDialog(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {
        }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSMessageDialog & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSMessageDialog &>(JSMessageDialog::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSMessageDialog::NATIVE>
    : public JSClassTraitsWrapper<JSMessageDialog::NATIVE, JSMessageDialog> {};

jsval as_jsval(JSContext *cx, JSMessageDialog::OWNERPTR theOwner, JSMessageDialog::NATIVE * theNative);

} // namespace

#endif


