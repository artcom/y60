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
//    $RCSfile: SensorArray.h,v $
//     $Author: ulrich $
//   $Revision: 1.4 $
//       $Date: 2005/02/03 16:59:51 $
//
// Dieters Sensor Array...
//
//=============================================================================

#ifndef _SENSOR_ARRAY_INCLUDED
#define _SENSOR_ARRAY_INCLUDED

#include <asl/Ptr.h>
#include <asl/Vector234.h>

#include <string>
#include <map>
#include <vector>


class SensorArray {
public:
    static unsigned makeKey(unsigned thePortId,
                            unsigned theControllerId,
                            unsigned theBitNumber);

    SensorArray (const std::string & theName,
                 const asl::Vector2i & theGridSize);

    const std::string & getName() const {
        return _myName;
    }

    const asl::Vector2i & getGridSize() const {
        return _myGridSize;
    }

    void addSensor(unsigned thePortId, unsigned theControllerId, unsigned theBitNumber,
                   const asl::Vector2i & theCoordinate);

    void createEvents(std::vector<asl::Vector2i> & theEventList,
                      unsigned thePortId, unsigned theControllerId, unsigned theBitMask);

private:
    std::string   _myName;
    asl::Vector2i _myGridSize;

    typedef std::map<unsigned, asl::Vector2i> SensorMap;
    SensorMap     _mySensorMap;
};

typedef asl::Ptr<SensorArray> SensorArrayPtr;

#endif
