//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSLABEL_INCLUDED_
#define _Y60_ACGTKSHELL_JSLABEL_INCLUDED_

#include "JSMisc.h"

#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
#pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/label.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/base/string_functions.h>

namespace jslib {

class JSLabel : public JSWrapper<Gtk::Label, asl::Ptr<Gtk::Label>, StaticAccessProtocol> {
    private:
        JSLabel();  // hide default constructor
        typedef JSMisc JSBASE;
    public:
        virtual ~JSLabel() {
        }
        typedef Gtk::Label NATIVE;
        typedef asl::Ptr<Gtk::Label> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Label";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_text = JSBASE::PROP_END,
            PROP_label,
            PROP_use_markup,
            PROP_use_underline,
            PROP_justify,
            PROP_ellipsize, // missing in current gtkmm
            PROP_width_chars,
            PROP_max_width_chars,
            PROP_line_wrap,
            PROP_selectable,
            PROP_angle,
            PROP_single_line_mode,
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

        JSLabel(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSLabel & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSLabel &>(JSLabel::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSLabel::NATIVE>
    : public JSClassTraitsWrapper<JSLabel::NATIVE, JSLabel> {};

jsval as_jsval(JSContext *cx, JSLabel::OWNERPTR theOwner, JSLabel::NATIVE * theLabel);

} // namespace

#endif

