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

#include "SensorArray.h"

#include <iostream>
using namespace std;


unsigned
SensorArray::makeKey(unsigned thePortId,
                     unsigned theControllerId,
                     unsigned theBitNumber)
{
    return (((thePortId & 0xff) << 16) | ((theControllerId & 0xff) << 8) | (theBitNumber & 0xff));
}


SensorArray :: SensorArray(const std::string & theName,
                           const asl::Vector2i & theGridSize) :
    _myName(theName), _myGridSize(theGridSize)
{
}


void
SensorArray::addSensor(unsigned thePortId, unsigned theControllerId, unsigned theBitNumber,
                       const asl::Vector2i & theCoordinate)
{
    unsigned key = makeKey(thePortId, theControllerId, theBitNumber);
    _mySensorMap[key] = theCoordinate;
    //cerr << "addSensor port=" << thePortId << " ctrl=" << theControllerId << " bit=" << theBitNumber << " pos=" << theCoordinate << endl;
}


void
SensorArray::createCookedEvents(std::vector<asl::Vector2i> & theEventList,
                          unsigned thePortId, unsigned theControllerId, unsigned theBitMask)
{
    for (unsigned bit = 0; bit < 8; ++bit) {

        if (theBitMask & (1 << bit)) {
            unsigned key = makeKey(thePortId, theControllerId, bit);
            SensorMap::iterator iter = _mySensorMap.find(key);
            if (iter != _mySensorMap.end()) {
                theEventList.push_back(iter->second);
            }
        }
    }
}
void
SensorArray::createRawEvents(std::vector<asl::Vector2i> & theEventList,
                          unsigned thePortId, unsigned theControllerId, unsigned theBitMask)
{
    for (unsigned bit = 0; bit < 8; ++bit) {

        if (theBitMask & (1 << bit)) {
             theEventList.push_back(asl::Vector2i(theControllerId, 1 << bit));
        }
    }
}
