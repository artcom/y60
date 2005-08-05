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
//    $RCSfile: SensorArray.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.5 $
//       $Date: 2005/02/03 16:59:51 $
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
SensorArray::createEvents(std::vector<asl::Vector2i> & theEventList,
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


