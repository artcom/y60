//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSFILECHOOSERDIALOG_INCLUDED_
#define _Y60_ACGTKSHELL_JSFILECHOOSERDIALOG_INCLUDED_

#include "JSDialog.h"
#include <y60/JSWrapper.h>
#include <gtkmm/filechooserdialog.h>

#include <asl/string_functions.h>

namespace jslib {

class JSFileChooserDialog : public JSWrapper<Gtk::FileChooserDialog, asl::Ptr<Gtk::FileChooserDialog>, StaticAccessProtocol> {
    private:
        JSFileChooserDialog();  // hide default constructor
        typedef JSDialog JSBASE;
    public:
        typedef Gtk::FileChooserDialog NATIVE;
        typedef asl::Ptr<Gtk::FileChooserDialog> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "FileChooserDialog";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_FILE_CHOOSER_ACTION_OPEN = JSBASE::PROP_END,
            PROP_FILE_CHOOSER_ACTION_SAVE,
            PROP_FILE_CHOOSER_ACTION_SELECT_FOLDER,
            PROP_FILE_CHOOSER_ACTION_CREATE_FOLDER,
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

        JSFileChooserDialog(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {
        }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSFileChooserDialog & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSFileChooserDialog &>(JSFileChooserDialog::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSFileChooserDialog::NATIVE>
    : public JSClassTraitsWrapper<JSFileChooserDialog::NATIVE, JSFileChooserDialog> {};

jsval as_jsval(JSContext *cx, JSFileChooserDialog::OWNERPTR theOwner, JSFileChooserDialog::NATIVE * theNative);

} // namespace

#endif


