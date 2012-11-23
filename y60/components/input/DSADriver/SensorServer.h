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
//    $RCSfile: SensorServer.h,v $
//     $Author: ulrich $
//   $Revision: 1.6 $
//       $Date: 2005/02/03 16:59:51 $
//
// Dieters Sensor Array...
//
//=============================================================================

#ifndef _SENSOR_SERVER_INCLUDED
#define _SENSOR_SERVER_INCLUDED

#include "y60_dsadriver_settings.h"

#include <asl/base/Ptr.h>
#include <asl/serial/SerialDevice.h>
#include <asl/base/Time.h>

#include <string>
#include <vector>

class SensorServer {
public:
    SensorServer();

    typedef std::vector< std::pair<unsigned,unsigned> > SensorData;
    void poll(SensorData & theSensorData);
    void openDevice(const std::string & theComPort, unsigned theBaudRate);
    std::string getStatus();
    bool isStatusOutDated();
    void calibrate(const std::string & theString);
    void reset();

    unsigned int         _myStatusInterval;

private:
    bool parseLine(const std::string & theLine, unsigned & theController, unsigned & theBitMask);
    void handleLines(std::string & theBuffer, SensorData & theSensorData);

    asl::Ptr<asl::SerialDevice> _myComPort;
    std::string                 _myFifo;
    std::string                 _mySensorStatus;
    asl::Time                   _myLastStatusTime;
};

typedef asl::Ptr<SensorServer> SensorServerPtr;

#endif
