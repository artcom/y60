//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSFRAME_INCLUDED_
#define _Y60_ACGTKSHELL_JSFRAME_INCLUDED_

#include "JSBin.h"
#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
#pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/frame.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/base/string_functions.h>

namespace jslib {

class JSFrame : public JSWrapper<Gtk::Frame, asl::Ptr<Gtk::Frame>, StaticAccessProtocol> {
        JSFrame();  // hide default constructor
    typedef JSBin JSBASE;
    public:
        virtual ~JSFrame() {
        }
        typedef Gtk::Frame NATIVE;
        typedef asl::Ptr<Gtk::Frame> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Frame";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {            
            PROP_label = JSBASE::PROP_END,
            PROP_label_widget,
            PROP_shadow_type,
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

        JSFrame(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSFrame & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSFrame &>(JSFrame::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSFrame::NATIVE>
    : public JSClassTraitsWrapper<JSFrame::NATIVE, JSFrame> {};

jsval as_jsval(JSContext *cx, JSFrame::OWNERPTR theOwner, JSFrame::NATIVE * theFrame);

} // namespace

#endif



