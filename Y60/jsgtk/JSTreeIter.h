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
//   $RCSfile: JSTreeIter.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/12/22 17:30:14 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSTREEITER_INCLUDED_
#define _Y60_ACGTKSHELL_JSTREEITER_INCLUDED_

#include <y60/JSWrapper.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/treeiter.h>
#include <asl/string_functions.h>

namespace jslib {

class JSTreeIter : public JSWrapper<Gtk::TreeIter, asl::Ptr<Gtk::TreeIter>, StaticAccessProtocol> {
        JSTreeIter();  // hide default constructor
    public:
        virtual ~JSTreeIter() {
        }
        typedef Gtk::TreeIter NATIVE;
        typedef asl::Ptr<Gtk::TreeIter> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "TreeIter";
        }

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_children_size = -100,
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

        JSTreeIter(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {
        }

        // static JSConstIntPropertySpec * ConstIntProperties();
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSTreeIter & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSTreeIter &>(JSTreeIter::getJSWrapper(cx,obj));
        }
    private:

};

template <>
struct JSClassTraits<JSTreeIter::NATIVE>
    : public JSClassTraitsWrapper<JSTreeIter::NATIVE, JSTreeIter> {};

jsval as_jsval(JSContext *cx, JSTreeIter::OWNERPTR theOwner, JSTreeIter::NATIVE * theNative);

bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<Gtk::TreeIter> & theNativePtr);

}

#endif




