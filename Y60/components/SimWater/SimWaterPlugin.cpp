//============================================================================
//
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "SimWater.h"

#include <asl/PlugInBase.h>
/*
#include <y60/IRendererExtension.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/Documentation.h>

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace y60;

class SimWaterPlugin :
    public PlugInBase,
    public IRendererExtension,
    public IScriptablePlugin
{
    public:
        SimWaterPlugin(asl::DLHandle theDLHandle)
        virtual ~SimWaterPlugin(); 

        JSFunctionSpec * Functions();

        void onUpdateSettings(dom::NodePtr theConfiguration) {
            AC_PRINT << "SimWaterPlugin::onUpdateSettings()";
        };
        void onGetProperty(const std::string & thePropertyName,
                PropertyValue & theReturnValue) const
        {
            AC_PRINT << "SimWaterPlugin::onGetProperty()";
        };

        void onSetProperty(const std::string & thePropertyName,
                const PropertyValue & thePropertyValue)
        {
            AC_PRINT << "SimWaterPlugin::onSetProperty()";
        };

        void onStartup(jslib::AbstractRenderWindow * theWindow)  {
            AC_PRINT << "SimWaterPlugin::onStartup()";
        };
        bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
            AC_PRINT << "SimWaterPlugin::onSceneLoaded()";
            _myWaterSimulation = WaterRepresentationPtr(
                new WaterSimulation( _mySimulationWidth, _mySimulationHeight, _myWaterDamping ));
            return true;
        };
        void handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
            AC_PRINT << "SimWaterPlugin::handle()";
        };
        void onFrame(jslib::AbstractRenderWindow * theWindow , double t) {
            AC_DEBUG << "SimWaterPlugin::onFrame()";
        };

        void onPreRender(jslib::AbstractRenderWindow * theRenderer) {
            AC_DEBUG << "SimWaterPlugin::onPreRender()";
        };
        void onPostRender(jslib::AbstractRenderWindow * theRenderer) {
            AC_DEBUG << "SimWaterPlugin::onPostRender()";
        };

        const char * ClassName() {
            static const char * myClassName = "SimWater";
            return myClassName;
        }

    private:
        SimWaterPlugin();
        WaterSimulationPtr     _myWaterSimulation;
        WaterRepresentationPtr _myWaterRepresentation;
};

SimWaterPlugin::SimWaterPlugin(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    IRendererExtension("SimWater")
{}

SimWaterPlugin::~SimWaterPlugin() {
}


static JSBool
Foo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    AC_PRINT << "LETS GO FOO";
    return JS_TRUE;
}


JSFunctionSpec *
SimWaterPlugin::Functions() {
    static JSFunctionSpec myFunctions[] = {
        {"foo", Foo, 0},
        {0}
    };
    return myFunctions;
}
*/

extern "C"
EXPORT asl::PlugInBase* SimWater_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new SimWater(myDLHandle);
}

