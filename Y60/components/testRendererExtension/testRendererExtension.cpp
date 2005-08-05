//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: testRendererExtension.cpp,v $
//
//     $Author: martin $
//
//   $Revision: 1.10 $
//
// Description:
//
//=============================================================================

#include <asl/PlugInBase.h>
#include <y60/IRendererExtension.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/QuitFlagSingleton.h>

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace y60;

typedef vector<string> MyVectorOfString;

#define ASSERT(x) if (!(x)) {cerr << "Assertion fialed: " << #x << " " << __FILE__ << " " <<__LINE__ << endl; exit(1);}

class TestRendererExtension :
    public PlugInBase,
    public IRendererExtension,
    public IScriptablePlugin
{
public:
    TestRendererExtension(asl::DLHandle theDLHandle);

	void onUpdateSettings(dom::NodePtr theConfiguration) {};
    void onGetProperty(const std::string & thePropertyName,
                     PropertyValue & theReturnValue) const {};

    void onSetProperty(const std::string & thePropertyName,
                     const PropertyValue & thePropertyValue) {};

	const char * ClassName() {
	    static const char * myClassName = "testRendererExtension";
	    return myClassName;
	}

    void onStartup(jslib::AbstractRenderWindow * theWindow);
    bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow);

	void handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent);
    void onFrame(AbstractRenderWindow * theWindow , double t);
    void onPreRender(AbstractRenderWindow * theRenderer);
    void onPostRender(AbstractRenderWindow * theRenderer);
protected:
    void checkOrderOfCallbacks();
    MyVectorOfString _myStrings;
};

TestRendererExtension :: TestRendererExtension(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    IRendererExtension("TestRendererExtension")
{}

void
TestRendererExtension :: onStartup(jslib::AbstractRenderWindow * theWindow) {
    cerr << "onStartup" << endl;
    _myStrings.push_back("onStartup");
}

bool
TestRendererExtension :: onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    cerr << "onSceneLoaded" << endl;
    _myStrings.push_back("onSceneLoaded");
    return false;
}

void
TestRendererExtension :: handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
    cerr << "handle" << endl;
    //_myStrings.push_back("handle");
}

void
TestRendererExtension :: onFrame(AbstractRenderWindow * theWindow , double t) {
    cerr << "onFrame" << endl;
    _myStrings.push_back("onFrame");
}

void
TestRendererExtension :: onPreRender(AbstractRenderWindow * theRenderer) {
    cerr << "onPreRender" << endl;
    _myStrings.push_back("onPreRender");
}

void
TestRendererExtension :: onPostRender(AbstractRenderWindow * theRenderer) {
    cerr << "onPostRender" << endl;
    _myStrings.push_back("onPostRender");

    if (_myStrings.size() >  2 + 10 * 3) {
        checkOrderOfCallbacks();
    }
}

void
TestRendererExtension :: checkOrderOfCallbacks() {
    ASSERT(_myStrings[0] == "onStartup");
    ASSERT(_myStrings[1] == "onSceneLoaded");
    for(int i=0; i<30; i+=3) {
        ASSERT(_myStrings[i+2] == "onFrame");
        ASSERT(_myStrings[i+3] == "onPreRender");
        ASSERT(_myStrings[i+4] == "onPostRender");
    }
    cerr << "testRendererExtension succeeded, quit" << endl;
    jslib::QuitFlagSingleton::get().setQuitFlag(true); //exit(0);
}

extern "C"
EXPORT PlugInBase* testRendererExtension_instantiatePlugIn(DLHandle myDLHandle) {
    return new TestRendererExtension(myDLHandle);
}

