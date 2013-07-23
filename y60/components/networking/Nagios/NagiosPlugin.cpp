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

#include <asl/net/net.h>
#include <asl/net/INetEndpoint.h>
#include <asl/ipc/ConduitAcceptor.h>
#include <asl/ipc/TCPPolicy.h>
#include <asl/base/PlugInBase.h>
#include <y60/jslib/IRendererExtension.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/jslib/AbstractRenderWindow.h>
#include <y60/scene/Scene.h>

#include <iostream>

#include "StatusServer.h"

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
        static const char * myClassName = "Nagios";
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
_myStatusServer(),
_myPort(2346)
{
}

void
NagiosPlugin :: onStartup(jslib::AbstractRenderWindow * theWindow) {
    if ( ! _myStatusServer ) {
        _myStatusServer = asl::Ptr<ConduitAcceptor<TCPPolicy> >(
            new ConduitAcceptor<TCPPolicy>(TCPPolicy::Endpoint("INADDR_ANY", _myPort),
            StatusServer::create));
        _myStatusServer->start();
    } else {
        AC_INFO << "Nagios server allready running. Reusing instance.";
    }
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
    } else if (thePropertyName == "timeout") {
        theReturnValue.set<asl::Signed32>(StatusServer::readFrameTimeout());
    } else if (thePropertyName == "text") {
        theReturnValue.set<std::string>(StatusServer::readStatusText());
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
        if (_myPort == 0 && _myStatusServer) {
            AC_INFO << "stopping nagios plugin";
            _myStatusServer->stop();
        }
    } else if (thePropertyName == "timeout") {
        StatusServer::writeFrameTimeout(thePropertyValue.get<asl::Signed32>());
    } else if (thePropertyName == "text") {
        StatusServer::writeStatusText(thePropertyValue.get<std::string>());
    } else {
        std::cerr << "### WARNING: Unknown property: " << thePropertyName << std::endl;
    }
}

extern "C"
EXPORT PlugInBase* Nagios_instantiatePlugIn(DLHandle myDLHandle) {
    return new NagiosPlugin(myDLHandle);
}

