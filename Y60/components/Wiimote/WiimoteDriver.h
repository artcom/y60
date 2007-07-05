//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_WIIMOTE_DRIVER_INCLUDED_
#define _Y60_WIIMOTE_DRIVER_INCLUDED_

#ifdef WIN32
#   include "Win32mote.h"
#elif defined( LINUX )
#   include "Liimote.h"
#elif defined( OSX )
// TODO
#endif

#include "WiiEvent.h"

#include <asl/Ptr.h>
#include <asl/PlugInBase.h>
#include <asl/Vector234.h>
#include <asl/ThreadLock.h>
#include <y60/IEventSource.h>
#include <y60/IScriptablePlugin.h>

#include <vector>
#include <queue>
#include <map>

namespace y60 {

     
class WiimoteDriver :
    public asl::PlugInBase,
    public jslib::IScriptablePlugin,
    public y60::IEventSource
{ 
    public:

        WiimoteDriver(asl::DLHandle theDLHandle);
        ~WiimoteDriver();
        virtual y60::EventPtrList poll();

        void requestStatusReport(unsigned theId);
        void requestStatusReport();

        void setLEDs(int theIndex, bool led1, bool led2, bool led3, bool led4);
        void setLED(int theWiimoteIndex, int theLEDIndex, bool theState);
        void setRumble(int theIndex, bool on);

        unsigned getNumWiimotes() const;

        void onSetProperty(const std::string & thePropertyName,
                const PropertyValue & thePropertyValue);
        void onGetProperty(const std::string & thePropertyName,
                PropertyValue & theReturnValue) const;
        void onUpdateSettings(dom::NodePtr theSettings);

        JSFunctionSpec * Functions();

        static JSBool SetRumble(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool SetLEDs(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool SetLED(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool RequestStatusReport(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        const char * ClassName();

    private:
        WiiReportMode _myDefaultReportMode;

        std::vector<WiiRemotePtr> _myWiimotes;
};

typedef asl::Ptr<WiimoteDriver> WiimotePtr;

}
#endif // _Y60_WIIMOTE_DRIVER_INCLUDED_

