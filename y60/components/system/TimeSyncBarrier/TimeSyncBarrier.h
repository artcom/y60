
#ifndef __Y60_TIME_SYNC_BARRIER_INCLUDED__
#define __Y60_TIME_SYNC_BARRIER_INCLUDED__

#include <asl/base/PlugInBase.h>
#include <y60/jslib/IRendererExtension.h>
#include <y60/jslib/AbstractRenderWindow.h>
#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/jsbase/QuitFlagSingleton.h>

#include <iostream>
#include <vector>
#include <string>

class TimeSyncBarrier :
    public asl::PlugInBase,
    public y60::IRendererExtension,
    public jslib::IScriptablePlugin
{
public:
    TimeSyncBarrier(asl::DLHandle theDLHandle);

    JSFunctionSpec * Functions();

	void onUpdateSettings(dom::NodePtr theConfiguration) {};
    void onGetProperty(const std::string & thePropertyName,
                     y60::PropertyValue & theReturnValue) const {};

    void onSetProperty(const std::string & thePropertyName,
                     const y60::PropertyValue & thePropertyValue) {};

	const char * ClassName() {
	    static const char * myClassName = "TimeSyncBarrier";
	    return myClassName;
	}

    void onStartup(jslib::AbstractRenderWindow * theWindow);
    bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow);

	void handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent);
    void onFrame(jslib::AbstractRenderWindow * theWindow , double t);
    void onPreRender(jslib::AbstractRenderWindow * theRenderer);
    void onPostRender(jslib::AbstractRenderWindow * theRenderer);
    void foo();
};

#endif // __Y60_TIME_SYNC_BARRIER_INCLUDED__

