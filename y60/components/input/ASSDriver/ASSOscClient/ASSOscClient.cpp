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

#include "ASSOscClient.h"

#include <y60/base/SettingsParser.h>

namespace y60 {

ASSOscClient::ASSOscClient()
{
}

ASSOscClient::~ASSOscClient()
{
}

void
ASSOscClient::poll()
{
	for(unsigned i = 0; i < _myReceivers.size(); i++) {
		ASSOscReceiverPtr myReceiver = _myReceivers[i];

		myReceiver->prepare();
	}

    processInput();

	for(unsigned i = 0; i < _myReceivers.size(); i++) {
		ASSOscReceiverPtr myReceiver = _myReceivers[i];

		myReceiver->send();
	}
}

void
ASSOscClient::createEvent(int theId, const std::string & theType,
        const asl::Vector2f & theRawPosition, const asl::Vector3f & thePosition3D,
        const asl::Box2f & theROI, float theIntensity, const ASSEvent & theEvent)
{
	for(unsigned i = 0; i < _myReceivers.size(); i++) {
		ASSOscReceiverPtr myReceiver = _myReceivers[i];

		myReceiver->buildCursorEvent(theType,
									 theId,
									 thePosition3D,
									 theIntensity);
	}
}

void
ASSOscClient::createTransportLayerEvent(const std::string & theType)
{
    if(theType != "configure") {
        for(unsigned i = 0; i < _myReceivers.size(); i++) {
    		ASSOscReceiverPtr myReceiver = _myReceivers[i];

            myReceiver->buildTransportLayerEvent(theType);
        }
    }
}

void
ASSOscClient::onUpdateSettings(dom::NodePtr theSettings) {
    AC_INFO << "Reconfiguring osc sender";

    ASSDriver::onUpdateSettings(theSettings);

    dom::NodePtr mySettings = getASSSettings(theSettings);

    int mySourcePort = 0;
    getConfigSetting(mySettings, "OscSourcePort", mySourcePort, 3333);

    _myReceivers.clear();

    dom::NodePtr myReceiversNode = mySettings->childNode("OscReceivers");
    if (myReceiversNode) {
    	unsigned myNumReceivers = myReceiversNode->childNodesLength("OscReceiver");
        for (unsigned i = 0; i < myNumReceivers; i++) {
            dom::NodePtr myReceiverNode = myReceiversNode->childNode("OscReceiver", i);

            std::string myHost = myReceiverNode->getAttributeString("host");
            asl::Unsigned16 myPort = myReceiverNode->getAttributeValue<asl::Unsigned16>("port");

            AC_INFO << "Sending from port " << mySourcePort << " to " << myHost << ":" << myPort;

            ASSOscReceiverPtr myReceiver(new ASSOscReceiver(myHost, mySourcePort, myPort));

            if (myReceiverNode->getAttribute("region")) {
                asl::Box2f myRegion = myReceiverNode->getAttributeValue<asl::Box2f>("region");

                myReceiver->restrictToRegion(myRegion);
            }

            _myReceivers.push_back(myReceiver);
        }
    }
}

} // end of namespace y60
