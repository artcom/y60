//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSCELL_RENDERER_INCLUDED_
#define _Y60_ACGTKSHELL_JSCELL_RENDERER_INCLUDED_

#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4512 4413 4244)
#endif //defined(_MSC_VER)
#include <gtkmm/cellrenderer.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/base/string_functions.h>

namespace jslib {

class JSCellRenderer : public JSWrapper<Gtk::CellRenderer, asl::Ptr<Gtk::CellRenderer>, StaticAccessProtocol> {
        JSCellRenderer();  // hide default constructor
    public:
        virtual ~JSCellRenderer() {
        }
        typedef Gtk::CellRenderer NATIVE;
        typedef asl::Ptr<Gtk::CellRenderer> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "CellRenderer";
        }

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_CELL_RENDERER_TEXT,
            PROP_CELL_RENDERER_PIXBUF,
            //PROP_CELL_RENDERER_PROGRESS,
            PROP_CELL_RENDERER_TOGGLE,
            //PROP_CELL_RENDERER_COMBO,
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

        JSCellRenderer(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSCellRenderer & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCellRenderer &>(JSCellRenderer::getJSWrapper(cx,obj));
        }
    private:

};

template <>
struct JSClassTraits<JSCellRenderer::NATIVE>
    : public JSClassTraitsWrapper<JSCellRenderer::NATIVE, JSCellRenderer> {};

jsval
as_jsval(JSContext *cx, JSCellRenderer::OWNERPTR theOwner,
         JSCellRenderer::NATIVE * theCellRenderer);

inline
jsval
as_jsval(JSContext *cx, JSCellRenderer::NATIVE * theCellRenderer) {
    return as_jsval(cx,JSCellRenderer::OWNERPTR(),theCellRenderer);
}

}

#endif



