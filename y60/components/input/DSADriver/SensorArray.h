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

#include "y60_dsadriver_settings.h"

#include <asl/base/Ptr.h>
#include <asl/math/Vector234.h>

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

    void createCookedEvents(std::vector<asl::Vector2i> & theEventList,
                      unsigned thePortId, unsigned theControllerId, unsigned theBitMask);
    void createRawEvents(std::vector<asl::Vector2i> & theEventList,
                      unsigned thePortId, unsigned theControllerId, unsigned theBitMask);

private:
    std::string   _myName;
    asl::Vector2i _myGridSize;

    typedef std::map<unsigned, asl::Vector2i> SensorMap;
    SensorMap     _mySensorMap;
};

typedef asl::Ptr<SensorArray> SensorArrayPtr;

#endif
