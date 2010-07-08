/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _Y60_ACGTKSHELL_JSCELL_RENDERER_TEXT_INCLUDED_
#define _Y60_ACGTKSHELL_JSCELL_RENDERER_TEXT_INCLUDED_

#include "y60_jsgtk_settings.h"

#include "JSCellRenderer.h"

#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
    #pragma warning(push,1)
    //#pragma warning(disable:4512 4413 4250)
#endif //defined(_MSC_VER)
#include <gtkmm/cellrenderertext.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/base/string_functions.h>

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

Y60_JSGTK_DECL jsval as_jsval(JSContext *cx, JSCellRendererText::NATIVE * theCellRenderer);

}

#endif



