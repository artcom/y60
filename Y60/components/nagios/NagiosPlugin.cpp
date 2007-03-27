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

#include <asl/net.h>
#include <asl/INetEndpoint.h>
#include <asl/ConduitAcceptor.h>
#include <asl/TCPPolicy.h>
#include <asl/PlugInBase.h>
#include <y60/IRendererExtension.h>
#include <y60/IScriptablePlugin.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/Scene.h>

#include <iostream>

#include "StatusServer.h"

using namespace std;
using namespace asl;
using namespace jslib;
using namespace inet;
using namespace dom;
using namespace y60;

    class NagiosPlugin :
        public PlugInBase,
        public y60::IRendererExtension,
        public IScriptablePlugin
    {
    public:
        NagiosPlugin(asl::DLHandle theDLHandle);

        void onStartup(jslib::AbstractRenderWindow * theWindow);

        bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow);

        void handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent);
        void onFrame(AbstractRenderWindow * theWindow , double t);

        void onPreRender(AbstractRenderWindow * theRenderer);
        void onPostRender(AbstractRenderWindow * theRenderer);

        void onGetProperty(const std::string & thePropertyName,
                         PropertyValue & theReturnValue) const;

        void onSetProperty(const std::string & thePropertyName,
                         const PropertyValue & thePropertyValue);
		const char * ClassName() {
		    static const char * myClassName = "NagiosPlugin";
		    return myClassName;
		}

		void onUpdateSettings(dom::NodePtr theSettings) { }

    private:
        asl::Ptr<ConduitAcceptor<TCPPolicy> > _myStatusServer;
        asl::Unsigned16 _myPort;
    };

NagiosPlugin :: NagiosPlugin(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    IRendererExtension("NagiosPlugin"),
    _myPort(2346),
    _myStatusServer(0)
{
}

void
NagiosPlugin :: onStartup(jslib::AbstractRenderWindow * theWindow) {
    inet::initSockets();
    _myStatusServer = asl::Ptr<ConduitAcceptor<TCPPolicy> >(
            new ConduitAcceptor<TCPPolicy>(TCPPolicy::Endpoint("INADDR_ANY", _myPort), StatusServer::create)); 
    _myStatusServer->start();
}

bool
NagiosPlugin :: onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    return true;
}

void
NagiosPlugin :: handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
}

void
NagiosPlugin :: onFrame(AbstractRenderWindow * theWindow , double t) {
    StatusServer::writeTick();
}

void
NagiosPlugin :: onPreRender(AbstractRenderWindow * theRenderer) {
}

void
NagiosPlugin :: onPostRender(AbstractRenderWindow * theRenderer) {
}

void
NagiosPlugin::onGetProperty(const std::string & thePropertyName,
                 PropertyValue & theReturnValue) const
{
    if (thePropertyName == "port") {
        theReturnValue.set<asl::Unsigned16>(_myPort);
    } else {
        //cerr << "### WARNING: Unknown property: " << thePropertyName << endl;
    }
}

void
NagiosPlugin::onSetProperty(const std::string & thePropertyName,
                 const PropertyValue & thePropertyValue)
{
    if (thePropertyName == "port") {
        _myPort = thePropertyValue.get<asl::Unsigned16>();
    } else {
        cerr << "### WARNING: Unknown property: " << thePropertyName << endl;
    }
}

extern "C"
EXPORT PlugInBase* NagiosPlugin_instantiatePlugIn(DLHandle myDLHandle) {
    return new NagiosPlugin(myDLHandle);
}
