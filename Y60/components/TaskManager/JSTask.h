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
//   $RCSfile: JSTask.h,v $
//   $Author: valentin $
//   $Revision: 1.3 $
//   $Date: 2005/01/18 16:17:48 $
//
//
//=============================================================================

#ifndef _ac_y60_JSTask_h_
#define _ac_y60_JSTask_h_

#include "Task.h"
#include <y60/JSWrapper.h>

namespace jslib {

    class JSTask : public jslib::JSWrapper<y60::Task, asl::Ptr<y60::Task> , jslib::StaticAccessProtocol> {
            JSTask() {}
        public:
            typedef y60::Task NATIVE;
            typedef asl::Ptr<NATIVE> OWNERPTR;
            typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;
    
            JSTask(OWNERPTR theOwner, NATIVE * theNative)
                : Base(theOwner, theNative)
            {}
    
            ~JSTask() {};
            
            static const char * ClassName() {
                return "Task";
            }
            static JSFunctionSpec * Functions();
    
            enum PropertyNumbers {
                PROP_isActive = -100,
                PROP_windows,
                PROP_ABOVE_NORMAL,
                PROP_BELOW_NORMAL,
                PROP_HIGH_PRIORITY,
                PROP_IDLE_PRIORITY,
                PROP_NORMAL_PRIORITY,
                PROP_REALTIME_PRIORITY
            };

            static JSPropertySpec * Properties();

            static JSPropertySpec * StaticProperties();
            static JSFunctionSpec * StaticFunctions();
    
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
    
            static jslib::JSConstIntPropertySpec * ConstIntProperties();
            static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
    
            static JSTask & getObject(JSContext *cx, JSObject * obj) {
                return dynamic_cast<JSTask &>(JSTask::getJSWrapper(cx,obj));
            }
    };

    jsval as_jsval(JSContext *cx, JSTask::OWNERPTR theOwner);
    jsval as_jsval(JSContext *cx, JSTask::OWNERPTR theOwner, JSTask::NATIVE * theNative);

    template <>
    struct JSClassTraits<JSTask::NATIVE> 
        : public JSClassTraitsWrapper<JSTask::NATIVE, JSTask> {};

}


#endif

