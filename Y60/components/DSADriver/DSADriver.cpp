//=============================================================================
// Copyright (C) 2004-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
// Dieters Sensor Array...
//
//=============================================================================

#include "DSADriver.h"

#include <asl/Vector234.h>
#include <y60/TouchEvent.h>
#include <y60/DataTypes.h>

using namespace std;


DSADriver::DSADriver (asl::DLHandle theDLHandle) : PlugInBase(theDLHandle)
{
    _myInterpolateFlag = false;
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
            unsigned myComPort = asl::as<int>(myPortNode->getAttribute("comport")->nodeValue());
            unsigned myBaudRate = asl::as<int>(myPortNode->getAttribute("baudrate")->nodeValue());

            AC_DEBUG << "DSADriver: Port=" << myPortID << " on comport=" << myComPort << " @ " << myBaudRate;
            try {
                SensorServerPtr mySensorServer(new SensorServer(myComPort, myBaudRate));
                _mySensorServers[myPortID] = mySensorServer;
            } catch (asl::SerialPortException & e) {
                AC_WARNING << "DSADriver: Could not open com port in " << __FILE__ << ", "
                     << __LINE__ << ". Message was " << e.what();
            }
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
            if (myGridSize[1] && myGridSize[2]) {
                std::string myData = (*myArrayNode)("#text").nodeValue();
                typedef std::vector< std::vector< asl::Vector3i > > VectorOfVectorOfVector3i;
                VectorOfVectorOfVector3i mySensorMapping = asl::as<VectorOfVectorOfVector3i>(myData);

                for (unsigned row = 0; row < myGridSize[1]; ++row) {
                    for (unsigned col = 0; col < myGridSize[0]; ++col) {

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
                        }
                        else {
                            mySensorArray->addSensor(myPortId,
                                    myControllerId,
                                    myBitNumber,
                                    asl::Vector2i(col,row));
                        }
                    }
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

            // interpolate neighboring events
            for (unsigned int i = 0; i < myRawEvents.size(); ++i) {

                asl::Vector2f myPosition((float)myRawEvents[i][0], (float)myRawEvents[i][1]);
                unsigned int myCount = 1;
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


extern "C"
EXPORT asl::PlugInBase* DSADriver_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new DSADriver(myDLHandle);
}
