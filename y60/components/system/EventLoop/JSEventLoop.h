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


#ifndef _AC_JSEVENTLOOP_H_
#define _AC_JSEVENTLOOP_H_

#include "y60_eventloop_settings.h"

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
