//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JS_TNT_MEASUREMENT_LIST_INCLUDED_
#define _Y60_ACGTKSHELL_JS_TNT_MEASUREMENT_LIST_INCLUDED_

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

jsval as_jsval(JSContext *cx, JSTNTMeasurementList::OWNERPTR theOwner, JSTNTMeasurementList::NATIVE * theTNTMeasurementList);

} // namespace

#endif


