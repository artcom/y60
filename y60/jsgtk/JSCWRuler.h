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

#ifndef TNT_JS_CW_RULER
#define TNT_JS_CW_RULER

#include "y60_jsgtk_settings.h"

#include "JSDrawingArea.h"

#include <y60/acgtk/CWRuler.h>

#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
#pragma warning(push,1)
    // supress warnings caused by gtk in vc++
    //#pragma warning(disable:4413 4244 4512 4250)
#endif //defined(_MSC_VER)
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/container.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/base/string_functions.h>

namespace jslib {

class JSCWRuler : public JSWrapper<acgtk::CWRuler, asl::Ptr<acgtk::CWRuler>,
                                      StaticAccessProtocol>
{
        JSCWRuler();  // hide default constructor
        typedef JSDrawingArea JSBASE;
    public:
        virtual ~JSCWRuler() {
        }
        typedef acgtk::CWRuler NATIVE;
        typedef asl::Ptr<acgtk::CWRuler> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "CWRuler";
        }

        static JSFunctionSpec * Functions();


        enum PropertyNumbers {
            PROP_value_range = JSBASE::PROP_END,
            PROP_window_center,
            PROP_window_width,
            PROP_lower,
            PROP_upper,
            PROP_mode,
            PROP_signal_center_changed,
            PROP_signal_width_changed,
            PROP_signal_lower_changed,
            PROP_signal_upper_changed,
            PROP_MODE_THRESHOLD,
            PROP_MODE_CENTER_WIDTH,
            PROP_MODE_LOWER_UPPER,
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

        JSCWRuler(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSCWRuler & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCWRuler &>(JSCWRuler::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSCWRuler::NATIVE>
    : public JSClassTraitsWrapper<JSCWRuler::NATIVE, JSCWRuler> {};

Y60_JSGTK_DECL jsval as_jsval(JSContext *cx, JSCWRuler::NATIVE * theWindow);
Y60_JSGTK_DECL bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<acgtk::CWRuler> & theCWRuler);

} // namespace

#endif


