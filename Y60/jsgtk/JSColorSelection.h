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
//   $RCSfile: JSColorSelection.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/11/27 16:22:00 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_COLOR_SELECTION_INCLUDED_
#define _Y60_ACGTKSHELL_COLOR_SELECTION_INCLUDED_

#include "JSBox.h"
#include <y60/JSWrapper.h>
#include <gtkmm/colorselection.h>

#include <asl/string_functions.h>

namespace jslib {

class JSColorSelection : public JSWrapper<Gtk::ColorSelection, asl::Ptr<Gtk::ColorSelection>, StaticAccessProtocol> {
    private:
        JSColorSelection();  // hide default constructor
        typedef JSBox JSBASE;
    public:
        virtual ~JSColorSelection() {
        }
        typedef Gtk::ColorSelection NATIVE;
        typedef asl::Ptr<Gtk::ColorSelection> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "ColorSelection";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_current_color = JSBASE::PROP_END,
            PROP_previous_color,
            PROP_has_opacity_control,
            PROP_has_palette,
            PROP_signal_color_changed,
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

        JSColorSelection(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSColorSelection & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSColorSelection &>(JSColorSelection::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSColorSelection::NATIVE>
    : public JSClassTraitsWrapper<JSColorSelection::NATIVE, JSColorSelection> {};

jsval as_jsval(JSContext *cx, JSColorSelection::OWNERPTR theOwner,
               JSColorSelection::NATIVE * theColorSelection);

} // namespace

#endif

