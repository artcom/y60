//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSCELL_RENDERER_TEXT_INCLUDED_
#define _Y60_ACGTKSHELL_JSCELL_RENDERER_TEXT_INCLUDED_

#include "JSCellRenderer.h"

#include <y60/JSWrapper.h>
#include <gtkmm/cellrenderertext.h>

#include <asl/string_functions.h>

namespace jslib {

class JSCellRendererText : public JSWrapper<Gtk::CellRendererText, asl::Ptr<Gtk::CellRendererText>,
                                            StaticAccessProtocol> 
{
    private:
        JSCellRendererText();  // hide default constructor
        typedef JSCellRenderer JSBASE;
    public:
        virtual ~JSCellRendererText() {
        }
        typedef Gtk::CellRendererText NATIVE;
        typedef asl::Ptr<Gtk::CellRendererText> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "CellRendererText";
        }

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_signal_edited = JSBASE::PROP_END,
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

        JSCellRendererText(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSCellRendererText & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCellRendererText &>(JSCellRendererText::getJSWrapper(cx,obj));
        }
    private:

};

template <>
struct JSClassTraits<JSCellRendererText::NATIVE>
    : public JSClassTraitsWrapper<JSCellRendererText::NATIVE, JSCellRendererText> {};

jsval
as_jsval(JSContext *cx, JSCellRendererText::NATIVE * theCellRenderer);

}

#endif



