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
*/

#ifndef _Y60_ACGTKSHELL_JS_TNT_MEASUREMENT_LIST_INCLUDED_
#define _Y60_ACGTKSHELL_JS_TNT_MEASUREMENT_LIST_INCLUDED_

#include "y60_jsgtk_settings.h"

#include "JSTreeView.h"

#include <y60/jsbase/JSWrapper.h>
#include <y60/acgtk/TNTMeasurementList.h>

#include <asl/base/string_functions.h>

namespace jslib {

class JSTNTMeasurementList : public JSWrapper<acgtk::TNTMeasurementList, asl::Ptr<acgtk::TNTMeasurementList>, StaticAccessProtocol> {
        JSTNTMeasurementList();  // hide default constructor
        typedef JSTreeView JSBASE;
    public:
        virtual ~JSTNTMeasurementList() {
        }
        typedef acgtk::TNTMeasurementList NATIVE;
        typedef asl::Ptr<acgtk::TNTMeasurementList> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "TNTMeasurementList";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_signal_visible_toggled = JSBASE::PROP_END,
            PROP_signal_editable_toggled,
            PROP_signal_right_click,
            PROP_selectedId,
            PROP_signal_name_changed,
            PROP_signal_key_press_event,
            PROP_signal_key_release_event,
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

        JSTNTMeasurementList(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSTNTMeasurementList & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSTNTMeasurementList &>(JSTNTMeasurementList::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSTNTMeasurementList::NATIVE>
    : public JSClassTraitsWrapper<JSTNTMeasurementList::NATIVE, JSTNTMeasurementList> {};

Y60_JSGTK_DECL jsval as_jsval(JSContext *cx, JSTNTMeasurementList::OWNERPTR theOwner, JSTNTMeasurementList::NATIVE * theTNTMeasurementList);

} // namespace

#endif


