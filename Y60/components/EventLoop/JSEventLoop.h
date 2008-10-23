//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


#ifndef _AC_JSEVENTLOOP_H_
#define _AC_JSEVENTLOOP_H_

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/input/Event.h>
#include <y60/input/IEventSink.h>
#include <y60/jsbase/JSWrapper.h>

namespace jslib {

    class JSEventLoop : public asl::PlugInBase, public jslib::IScriptablePlugin, 
                        public y60::IEventSink
    {

    public:

        typedef JSEventLoop NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE, OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSEventLoop( asl::DLHandle theDLHandle ) : asl::PlugInBase( theDLHandle ) {};

        const char * ClassName() {
            static const char * myClassName = "EventLoop";
            return myClassName;
        }

        virtual JSFunctionSpec * StaticFunctions();
        virtual JSFunctionSpec * Functions();

        void onUpdateSettings( dom::NodePtr theSettings ) {
        }
        
        void go( JSContext *cx, JSObject *obj );
        void handle( y60::EventPtr theEvent );
        
    private:

        JSContext * _myJSContext;
        JSObject * _myEventListener;

    };

}

#endif //_AC_JSEVENTLOOP_H_
