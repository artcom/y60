//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSSEPARATOR_TOOL_ITEM_INCLUDED_
#define _Y60_ACGTKSHELL_JSSEPARATOR_TOOL_ITEM_INCLUDED_

#include "JSToolItem.h"
#include <y60/jsbase/JSWrapper.h>
#include <gtkmm/separatortoolitem.h>

#include <asl/base/string_functions.h>

namespace jslib {

class JSSeparatorToolItem : public JSWrapper<Gtk::SeparatorToolItem, asl::Ptr<Gtk::SeparatorToolItem>, StaticAccessProtocol> {
        JSSeparatorToolItem();  // hide default constructor
    typedef JSToolItem JSBASE;
    public:
        virtual ~JSSeparatorToolItem() {
        }
        typedef Gtk::SeparatorToolItem NATIVE;
        typedef asl::Ptr<Gtk::SeparatorToolItem> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "SeparatorToolItem";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
             PROP_END = JSBASE::PROP_END
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

        JSSeparatorToolItem(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSSeparatorToolItem & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSSeparatorToolItem &>(JSSeparatorToolItem::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSSeparatorToolItem::NATIVE>
    : public JSClassTraitsWrapper<JSSeparatorToolItem::NATIVE, JSSeparatorToolItem> {};

jsval as_jsval(JSContext *cx, JSSeparatorToolItem::OWNERPTR theOwner, JSSeparatorToolItem::NATIVE * theSeparatorToolItem);

} // namespace

#endif



