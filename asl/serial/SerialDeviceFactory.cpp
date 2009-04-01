/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "SerialDeviceFactory.h"

#include <asl/base/string_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/Logger.h>

#include <string>

#ifdef _WIN32
#   include "ComPort.h"
#else
#   include "TTYPort.h"
#endif

namespace asl {



#ifdef _WIN32 
bool getSerialDeviceNames(std::vector<std::string> & theSerialDevicesNames) {
    theSerialDevicesNames.resize(0);;

    //Iterate through all possible 255 COM ports and see if we can open them or fail in a way that indicates they exist 
    for (unsigned i=1; i<256; i++) {
        std::string myPort = std::string("\\\\.\\COM")+asl::as_string(i);
        bool exists = false;

        HANDLE hPort = ::CreateFile(myPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

        if (hPort == INVALID_HANDLE_VALUE) {
            DWORD dwError = GetLastError();
            //Check to see if the error was because some other app had the port open or a general failure
            if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE || dwError == ERROR_SHARING_VIOLATION || dwError == ERROR_SEM_TIMEOUT) {
                exists = true;
			}
		} else {
			//The port was opened successfully
			exists = true;
			CloseHandle(hPort);
		}
		if (exists) {
			theSerialDevicesNames.push_back(myPort);
			AC_DEBUG << "Found serial device #" << theSerialDevicesNames.size()-1 << " , name = "<<theSerialDevicesNames.back();
		}
	}
    return theSerialDevicesNames.size() != 0;
}

#else

void getSerialDeviceNames(const std::string theDevicePrefix, std::vector<std::string> & theSerialDevicesNames) {
    std::vector<std::string> myDeviceNames;
    if (listDirectory("/dev", myDeviceNames) ) {
        std::sort(myDeviceNames.begin(), myDeviceNames.end());
        for (unsigned i = 0; i <  myDeviceNames.size();++i) {
            if (myDeviceNames[i].find(theDevicePrefix) == 0) {
                theSerialDevicesNames.push_back(std::string("/dev/"+myDeviceNames[i]));
                AC_DEBUG << "Found serial device #" << theSerialDevicesNames.size()-1 << " , name = "<<theSerialDevicesNames.back();
            }
        }
    }
}

#ifdef OSX
bool getSerialDeviceNames(std::vector<std::string> & theSerialDevicesNames) {
    theSerialDevicesNames.resize(0);;
    getSerialDeviceNames("cu.",theSerialDevicesNames);
    return theSerialDevicesNames.size() != 0;
}
#endif

#ifdef LINUX
bool getSerialDeviceNames(std::vector<std::string> & theSerialDevicesNames) {
    theSerialDevicesNames.resize(0);
    getSerialDeviceNames("ttyS",theSerialDevicesNames);
    getSerialDeviceNames("ttyUSB",theSerialDevicesNames);
    getSerialDeviceNames("ttyACM",theSerialDevicesNames);
    return theSerialDevicesNames.size() != 0;
}
#endif
#endif

SerialDevice * 
getSerialDevice(unsigned int theIndex) {

    std::vector<std::string> myDevicesNames;
    if (!getSerialDeviceNames(myDevicesNames)) {;
        AC_WARNING << "No serial devices found.";
    }
    if (theIndex < myDevicesNames.size()) {
#ifdef WIN32
        return new ComPort(myDevicesNames[theIndex]);
#else
        return new TTYPort(myDevicesNames[theIndex]);
#endif
    } else {
        AC_ERROR << "getSerialDevice: index out of range, index = "<<theIndex<<", max ="<<myDevicesNames.size()-1;
        return 0;
    }

#ifdef WIN32
    // return new ComPort(std::string("\\\\.\\COM") + as_string(theIndex + 1));
#endif
#ifdef LINUX
    // XXX: need this ATM so the first is actually ttyS0, which is tested.
    // return new TTYPort(std::string("/dev/ttyS") + as_string(theIndex));
#endif
}

SerialDevice * getSerialDeviceByName(const std::string & theDevice) {
#ifdef _WIN32
    return new ComPort( theDevice );
#else
    return new TTYPort( theDevice );
#endif
}
}
