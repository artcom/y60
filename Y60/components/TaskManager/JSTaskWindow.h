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
//   $RCSfile: JSTaskWindow.h,v $
//   $Author: valentin $
//   $Revision: 1.3 $
//   $Date: 2005/01/18 16:17:48 $
//
//
//=============================================================================

#ifndef _ac_y60_JSTaskWindow_h_
#define _ac_y60_JSTaskWindow_h_

#include "TaskWindow.h"
#include <y60/jsbase/JSWrapper.h>

namespace jslib {

    class JSTaskWindow : public JSWrapper<y60::TaskWindow, y60::TaskWindowPtr, StaticAccessProtocol> {
            JSTaskWindow() {}
        public:
            typedef y60::TaskWindow NATIVE;
            typedef asl::Ptr<NATIVE> OWNERPTR;
            typedef JSWrapper<NATIVE,OWNERPTR, StaticAccessProtocol> Base;

            JSTaskWindow(OWNERPTR theOwner, NATIVE * theNative)
                : Base(theOwner, theNative)
            {}

            ~JSTaskWindow() {};

            static const char * ClassName() {
                return "TaskWindow";
            }
            static JSFunctionSpec * Functions();
            static JSFunctionSpec * StaticFunctions();

            enum PropertyNumbers {
                PROP_windowName = -100,
                PROP_visible,
                PROP_position
            };

            static JSPropertySpec * Properties();

            virtual unsigned long length() const {
                return 1;
            }

            virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
            virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

            static JSBool
            Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

            static
            JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
                return Base::Construct(cx, theOwner, theNative);
            }

            static JSConstIntPropertySpec * ConstIntProperties();
            static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

            static JSTaskWindow & getObject(JSContext *cx, JSObject * obj) {
                return dynamic_cast<JSTaskWindow &>(JSTaskWindow::getJSWrapper(cx,obj));
            }
    };

    jsval as_jsval(JSContext *cx, JSTaskWindow::OWNERPTR theOwner);
    jsval as_jsval(JSContext *cx, JSTaskWindow::OWNERPTR theOwner, JSTaskWindow::NATIVE * theNative);

    template <>
    struct JSClassTraits<JSTaskWindow::NATIVE>
        : public JSClassTraitsWrapper<JSTaskWindow::NATIVE, JSTaskWindow> {};
}


#endif

