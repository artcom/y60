//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSPROGRESSBAR_INCLUDED_
#define _Y60_ACGTKSHELL_JSPROGRESSBAR_INCLUDED_

#include "JSWidget.h"
#include <y60/JSWrapper.h>
#include <gtkmm/progressbar.h>

#include <asl/string_functions.h>

namespace jslib {

class JSProgressBar : public JSWrapper<Gtk::ProgressBar, asl::Ptr<Gtk::ProgressBar>, StaticAccessProtocol> {
        JSProgressBar();  // hide default constructor
    typedef JSWidget JSBASE;
    public:
        virtual ~JSProgressBar() {
        }
        typedef Gtk::ProgressBar NATIVE;
        typedef asl::Ptr<Gtk::ProgressBar> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "ProgressBar";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
             PROP_text = JSBASE::PROP_END,
             PROP_fraction,
             PROP_pulse_step,
             PROP_orientation,
             PROP_ellipsize,
             PROP_PROGRESS_LEFT_TO_RIGHT,
             PROP_PROGRESS_RIGHT_TO_LEFT,
             PROP_PROGRESS_TOP_TO_BOTTOM,
             PROP_PROGRESS_BOTTOM_TO_TOP,
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

        JSProgressBar(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSProgressBar & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSProgressBar &>(JSProgressBar::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSProgressBar::NATIVE>
    : public JSClassTraitsWrapper<JSProgressBar::NATIVE, JSProgressBar> {};

jsval as_jsval(JSContext *cx, JSProgressBar::OWNERPTR theOwner, JSProgressBar::NATIVE * theProgressBar);

} // namespace

#endif



