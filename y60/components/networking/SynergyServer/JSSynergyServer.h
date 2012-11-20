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

#ifndef _ac_jslib_JSSynergyServer_h_
#define _ac_jslib_JSSynergyServer_h_

#include "y60_synergyserver_settings.h"

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


    Y60_SYNERGYSERVER_DECL jsval as_jsval( JSContext *cx, JSSynergyServer::OWNERPTR theOwner );
    Y60_SYNERGYSERVER_DECL jsval as_jsval( JSContext *cx, JSSynergyServer::OWNERPTR theOwner,
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

#endif //_ac_jslib_JSSynergyServer_h_
