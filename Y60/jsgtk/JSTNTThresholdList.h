//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSTNTThresholdList.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/11/27 16:22:00 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JS_TNT_THRESHOLD_LIST_INCLUDED_
#define _Y60_ACGTKSHELL_JS_TNT_THRESHOLD_LIST_INCLUDED_

#include "JSTreeView.h"

#include <y60/JSWrapper.h>
#include <acgtk/TNTThresholdList.h>

#include <asl/string_functions.h>

namespace jslib {

class JSTNTThresholdList : public JSWrapper<acgtk::TNTThresholdList, asl::Ptr<acgtk::TNTThresholdList>, StaticAccessProtocol> {
        JSTNTThresholdList();  // hide default constructor
        typedef JSTreeView JSBASE;
    public:
        virtual ~JSTNTThresholdList() {
        }
        typedef acgtk::TNTThresholdList NATIVE;
        typedef asl::Ptr<acgtk::TNTThresholdList> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "TNTThresholdList";
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

        JSTNTThresholdList(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSTNTThresholdList & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSTNTThresholdList &>(JSTNTThresholdList::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSTNTThresholdList::NATIVE>
    : public JSClassTraitsWrapper<JSTNTThresholdList::NATIVE, JSTNTThresholdList> {};

jsval as_jsval(JSContext *cx, JSTNTThresholdList::OWNERPTR theOwner, JSTNTThresholdList::NATIVE * theTNTThresholdList);

} // namespace

#endif


