//============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: TTYPort.h,v $
//     $Author: ulrich $
//   $Revision: 1.5 $
//
//=============================================================================

#ifndef ASL_TTY_PORT_INCLUDED
#define ASL_TTY_PORT_INCLUDED

#include "SerialDevice.h"


#include <string>

namespace asl {

class TTYPort : public SerialDevice {
    public:
       TTYPort(const std::string & theDeviceName);
        virtual ~TTYPort();

        void open(unsigned int theBaudRate, unsigned int theDataBits,
                  ParityMode theParityMode, unsigned int theStopBits,
                  bool theHWHandShakeFlag = false);
        void close();

        bool read(char * theBuffer, size_t & theSize);
        void write(const char * theBuffer, size_t theSize);
        unsigned peek();

        void setStatusLine(unsigned theStatusMask);
        unsigned getStatusLine();

    private:
        TTYPort();
        int convertBaudRate(unsigned int theBaudRate);
        int convertDataBits(unsigned int theDataBits);
        int convertParityMode(ParityMode theParity);
        int convertStopBits(unsigned int theStopBits);
        int convertHandShaking(bool theFlag);

        int _myPortHandle;

         
};

}

#endif // ASL_TTY_PORT_INCLUDED
