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
//    $RCSfile: DSADriver.cpp,v $
//     $Author: christian $
//   $Revision: 1.19 $
//       $Date: 2005/02/10 19:59:30 $
//
// Dieters Sensor Array...
//
//=============================================================================

#include "DSADriver.h"

#include <asl/Vector234.h>
#include <y60/TouchEvent.h>
#include <y60/DataTypes.h>

using namespace std;

#define DB(x) // x


DSADriver::DSADriver (asl::DLHandle theDLHandle) : PlugInBase(theDLHandle)
{
    _myInterpolateFlag = true;
}

void DSADriver::onUpdateSettings(dom::NodePtr theConfiguration) {
    const dom::NodePtr & myConfigNode = theConfiguration->childNode("DSADriver");
    if (!myConfigNode) {
        AC_ERROR << "DSADriver: No <DSADirver> element found in configuration" << endl;
        return;
    }

    const dom::NodePtr & myPortsNode = myConfigNode->childNode("Ports");
    if (myPortsNode) {
        for (unsigned i = 0; i < myPortsNode->childNodesLength("Port"); ++i) {
            const dom::NodePtr & myPortNode = myPortsNode->childNode("Port", i);
            unsigned myPortID = asl::as<int>(myPortNode->getAttribute("id")->nodeValue());
            unsigned myComPort = asl::as<int>(myPortNode->getAttribute("comport")->nodeValue());
            unsigned myBaudRate = asl::as<int>(myPortNode->getAttribute("baudrate")->nodeValue());

            DB(cerr << "Port " << myPortID << " on comport=" << myComPort << " @ " << myBaudRate << endl);
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

            const asl::Vector2i myGridSize = asl::as<asl::Vector2i>(myArrayNode->getAttribute("gridsize")->nodeValue());

            _myInterpolateFlag = true;
            if (myArrayNode->getAttribute("interpolate")) {
                _myInterpolateFlag = asl::as<bool>(myArrayNode->getAttribute("interpolate")->nodeValue());
            }
            DB(cerr << "DSADriver: interpolate=" << _myInterpolateFlag << endl);

            SensorArrayPtr mySensorArray(new SensorArray(myArrayName,myGridSize));
            _mySensorArray[myArrayID] = mySensorArray;

            /*
                * regular grid
                * [portID,controllerID,bitNumber]
                */
            std::string myData = (*myArrayNode)("#text").nodeValue();
            typedef std::vector< std::vector< asl::Vector3i > > VectorOfVectorOfVector3i;
            VectorOfVectorOfVector3i mySensorMapping = asl::as<VectorOfVectorOfVector3i>(myData);

            for (unsigned row = 0; row < myGridSize[1]; ++row) {
                for (unsigned col = 0; col < myGridSize[0]; ++col) {

                    unsigned myPortId, myControllerId, myBitNumber;
                    myPortId = mySensorMapping[row][col][0];
                    myControllerId = mySensorMapping[row][col][1];
                    myBitNumber = mySensorMapping[row][col][2];

                    SensorServerList::iterator it = _mySensorServers.find(myPortId);
                    if (it == _mySensorServers.end()) {
                        AC_WARNING << "DSADriver: No such port for sensor " << myPortId << "/" << myControllerId << "/" << myBitNumber << "; sensor ignored" << endl;
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

                sali->second->createEvents(myRawEvents,
                                           ssli->first, // thePortId
                                           sdi->first,  // theControllerId
                                           sdi->second  // theBitMask
                                           );
            }

            // interpolate neighboring events
            for (unsigned int i = 0; i < myRawEvents.size(); ++i) {

                asl::Vector2f myPosition((float)myRawEvents[i][0], (float)myRawEvents[i][1]);
                unsigned int myCount = 1;
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
                    DB(cerr << "DSADriver: interpolated distance " << myPosition << "," << myCount << endl);
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
