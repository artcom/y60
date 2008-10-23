//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_jslib_JSSynergyServer_h_
#define _ac_jslib_JSSynergyServer_h_

#include "SynergyServer.h"

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.impl>

namespace jslib {

    class JSSynergyServer : public jslib::JSWrapper<SynergyServer, asl::Ptr<SynergyServer> , 
                              jslib::StaticAccessProtocol> 
    {
        JSSynergyServer() {}

        public:

            typedef SynergyServer NATIVE;
            typedef asl::Ptr<NATIVE> OWNERPTR;
            typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;
        
            JSSynergyServer(OWNERPTR theOwner, NATIVE * theNative)
                : Base(theOwner, theNative)
            {}

            virtual ~JSSynergyServer();

            static const char * ClassName() {
                return "SynergyServer";
            }
        
            static JSFunctionSpec * Functions();

//            enum PropertyNumbers {
//            };

            static JSPropertySpec * Properties();
            static JSPropertySpec * StaticProperties();
            static JSFunctionSpec * StaticFunctions();

            virtual unsigned long length() const {
                return 1;
            }

            virtual JSBool getPropertySwitch( unsigned long theID, JSContext *cx, 
                                              JSObject *obj, jsval id, jsval *vp);
            virtual JSBool setPropertySwitch( unsigned long theID, JSContext *cx, 
                                              JSObject *obj, jsval id, jsval *vp);

            static JSBool
            Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

            static
            JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
                return Base::Construct(cx, theOwner, theNative);
            }

            static jslib::JSConstIntPropertySpec * ConstIntProperties();
            static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

            static JSSynergyServer & getObject(JSContext *cx, JSObject * obj) {
                return dynamic_cast<JSSynergyServer &>(JSSynergyServer::getJSWrapper(cx,obj));
            }
    };

    template <>
    struct JSClassTraits<JSSynergyServer::NATIVE>
        : public JSClassTraitsWrapper<JSSynergyServer::NATIVE , JSSynergyServer> {};


    jsval as_jsval( JSContext *cx, JSSynergyServer::OWNERPTR theOwner );
    jsval as_jsval( JSContext *cx, JSSynergyServer::OWNERPTR theOwner, 
                    JSSynergyServer::NATIVE * theSerial );


} // namespace jslib

namespace y60 {
    class JSSynergyServerPlugIn : public asl::PlugInBase, public jslib::IScriptablePlugin {
    public:
    	JSSynergyServerPlugIn(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}
        
    	virtual void initClasses(JSContext * theContext, JSObject * theGlobalObject) {
            jslib::JSSynergyServer::initClass(theContext, theGlobalObject);
  	}
        
   	const char * ClassName() {
    	    static const char * myClassName = "SynergyServerPlugIn";
    	    return myClassName;
    	}
};
}

extern "C"
EXPORT asl::PlugInBase * SynergyServer_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::JSSynergyServerPlugIn(myDLHandle);
}

#endif //_ac_jslib_JSSynergyServer_h_
