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
//
// Dieters Sensor Array...
//
//=============================================================================

#include "DSADriver.h"

#include <asl/math/Vector234.h>
#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>
#include <asl/dom/Nodes.h>
#include <y60/input/TouchEvent.h>
#include <y60/base/DataTypes.h>
#include <y60/input/EventDispatcher.h>

using namespace std;

namespace y60 {

    DSADriver::DSADriver () : _myInterpolateFlag(false) {
    }

    DSADriver::~DSADriver () {
    }

    void DSADriver::calibrate(unsigned int thePortId, const std::string & theFileName) {
        SensorServerList::iterator it = _mySensorServers.find(thePortId);
        if (it != _mySensorServers.end()) {
            std::string myData = asl::readFile(theFileName);
            AC_PRINT<<myData;
            it->second->calibrate(myData);
        }
    }

    std::string DSADriver::getStatus() {
        std::string myStatus;
        for (SensorServerList::iterator ssli = _mySensorServers.begin();
             ssli != _mySensorServers.end(); ++ssli) {
            myStatus += std::string("portId:") + asl::as_string(ssli->first) + " status: " + ssli->second->getStatus() + " ";
        }
        return myStatus;
    }

    void DSADriver::setStatusInterval(unsigned int theInterval) {
        for (SensorServerList::iterator ssli = _mySensorServers.begin();
             ssli != _mySensorServers.end(); ++ssli) {
            ssli->second->_myStatusInterval = theInterval;
        }
    }

    void DSADriver::onUpdateSettings(dom::NodePtr theConfiguration) {
        const dom::NodePtr & myConfigNode = theConfiguration->childNode("DSADriver");
        if (!myConfigNode) {
            AC_ERROR << "DSADriver: No <DSADriver> element found in configuration";
            return;
        }

        const dom::NodePtr & myPortsNode = myConfigNode->childNode("Ports");
        if (myPortsNode) {
            for (unsigned i = 0; i < myPortsNode->childNodesLength("Port"); ++i) {
                const dom::NodePtr & myPortNode = myPortsNode->childNode("Port", i);
                unsigned myPortID = asl::as<int>(myPortNode->getAttribute("id")->nodeValue());
                unsigned myBaudRate = asl::as<int>(myPortNode->getAttribute("baudrate")->nodeValue());
                std::string myComPort = myPortNode->getAttribute("comport")->nodeValue();

                AC_DEBUG << "DSADriver: Port=" << myPortID << " on comport=" << myComPort << " @ " << myBaudRate;
                SensorServerPtr mySensorServer(new SensorServer());
                mySensorServer->openDevice(myComPort, myBaudRate);
                _mySensorServers[myPortID] = mySensorServer;
            }
        }

        const dom::NodePtr & myArraysNode = myConfigNode->childNode("SensorArrays");
        if (myArraysNode) {
            for (unsigned i = 0; i < myArraysNode->childNodesLength("SensorArray"); ++i) {
                const dom::NodePtr & myArrayNode = myArraysNode->childNode("SensorArray", i);
                unsigned myArrayID = asl::as<int>(myArrayNode->getAttribute("id")->nodeValue());
                std::string myArrayName = myArrayNode->getAttribute("name")->nodeValue();


                if (myArrayNode->getAttribute("interpolate")) {
                    _myInterpolateFlag = asl::as<bool>(myArrayNode->getAttribute("interpolate")->nodeValue());
                }
                AC_DEBUG << "DSADriver: interpolate=" << _myInterpolateFlag;

                SensorArrayPtr mySensorArray;
                asl::Vector2i myGridSize(0,0);

                if (myArrayNode->getAttribute("gridsize")) {
                    myGridSize = asl::as<asl::Vector2i>(myArrayNode->getAttribute("gridsize")->nodeValue());
                }
                mySensorArray = SensorArrayPtr(new SensorArray(myArrayName,myGridSize));
                _mySensorArray[myArrayID] = mySensorArray;

                /*
                 * regular grid
                 * [portID,controllerID,bitNumber]
                 */
                if (myGridSize[0] && myGridSize[1]) {
                    if(myArrayNode->childNodesLength() > 0) {
                        std::string myData = (*myArrayNode)("#text").nodeValue();
                        typedef std::vector< std::vector< asl::Vector3i > > VectorOfVectorOfVector3i;
                        VectorOfVectorOfVector3i mySensorMapping = asl::as<VectorOfVectorOfVector3i>(myData);

                        for (int row = 0; row < myGridSize[1]; ++row) {
                            for (int col = 0; col < myGridSize[0]; ++col) {

                                unsigned myPortId, myControllerId, myBitNumber;
                                myPortId = mySensorMapping[row][col][0];
                                myControllerId = mySensorMapping[row][col][1];
                                myBitNumber = mySensorMapping[row][col][2];
                                if (myPortId == (unsigned)-1 ||
                                        myControllerId == (unsigned)-1 ||
                                        myBitNumber == (unsigned)-1) {
                                    AC_WARNING << "DSADriver: Ignoring sensor " << myPortId << "/" << myControllerId << "/" << myBitNumber;
                                    continue;
                                }

                                SensorServerList::iterator it = _mySensorServers.find(myPortId);
                                if (it == _mySensorServers.end()) {
                                    AC_WARNING << "DSADriver: No such port for sensor " << myPortId << "/" << myControllerId << "/" << myBitNumber << "; sensor ignored";
                                } else {
                                    mySensorArray->addSensor(myPortId,
                                            myControllerId,
                                            myBitNumber,
                                            asl::Vector2i(col,row));
                                }
                            }
                        }
                    } else {
                        AC_WARNING << "you specified a gridsize: "<< myGridSize <<" so you have to add sensormappings or the DSADriver isn't working";
                    }
                }
            }
        }
    }

    vector<y60::EventPtr> DSADriver::poll()
    {
        vector<y60::EventPtr> myEvents;
        for (SensorServerList::iterator ssli = _mySensorServers.begin();
             ssli != _mySensorServers.end(); ++ssli) {

            SensorServer::SensorData mySensorData;
            ssli->second->poll(mySensorData);
            for (SensorArrayList::iterator sali = _mySensorArray.begin();
                 sali != _mySensorArray.end(); ++sali) {

                std::vector<asl::Vector2i> myRawEvents;
                for(SensorServer::SensorData::iterator sdi = mySensorData.begin();
                    sdi != mySensorData.end(); ++sdi) {
                    if (sali->second->getGridSize()[0] == 0 &&
                        sali->second->getGridSize()[1] == 0)
                    {
                        // TODO: Raw Events for multiple Ports
                        sali->second->createRawEvents(myRawEvents,
                                               ssli->first, // thePortId
                                               sdi->first,  // theControllerId
                                               sdi->second  // theBitMask
                                               );
                    } else {
                        sali->second->createCookedEvents(myRawEvents,
                                               ssli->first, // thePortId
                                               sdi->first,  // theControllerId
                                               sdi->second  // theBitMask
                                               );
                    }
                }
                for (unsigned int i = 0; i < myRawEvents.size(); ++i) {
                    asl::Vector2f myPosition((float)myRawEvents[i][0], (float)myRawEvents[i][1]);
                    unsigned int myCount = 1;
                    // interpolate neighboring events
                    if (_myInterpolateFlag) {
                        for (unsigned int j = 0; j < myRawEvents.size(); ++j) {
                            if (i == j) {
                                continue;
                            }
                            asl::Vector2i myDelta = asl::difference(myRawEvents[j], myRawEvents[i]);
                            if (magnitude(myDelta) <= 1) {
                                myPosition += asl::Vector2f((float)myRawEvents[j][0], (float)myRawEvents[j][1]);
                                myCount++;
                            }
                        }
                        if (myCount > 1) {
                            myPosition = product(myPosition, 1.0f / float(myCount));
                            AC_TRACE << "DSADriver: interpolated distance " << myPosition << "," << myCount;
                        }
                    }

                    y60::EventPtr myEvent(new y60::TouchEvent(sali->second->getName(), myPosition, sali->second->getGridSize(), float(myCount)));
                    myEvents.push_back(myEvent);
                }
            }
        }
        return myEvents;
    }
}
