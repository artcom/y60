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

#include <asl/Ptr.h>
#include <asl/SerialDevice.h>
#include <asl/Time.h>

#include <string>
#include <vector>

class SensorServer {
public:
    SensorServer(unsigned theComPort, unsigned theBaudRate);

	typedef std::vector< std::pair<unsigned,unsigned> > SensorData;
	void poll(SensorData & theSensorData);
    
private:
	void parseLine(const std::string & theLine, unsigned & theController, unsigned & theBitMask);	
	void handleLines(std::string & theBuffer, SensorData & theSensorData);

private:
    asl::Ptr<asl::SerialDevice> _myComPort;
    std::string                 _myFifo;
};

typedef asl::Ptr<SensorServer> SensorServerPtr;

#endif
