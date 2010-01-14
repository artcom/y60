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

/*! @addtogroup aslserial */
/* @{ */

/*! Unix implementation of a serial port.
 * @see SerialDevice
 */
class TTYPort : public SerialDevice {
    public:
       TTYPort(const std::string & theDeviceName);
        virtual ~TTYPort();

        void open(unsigned int theBaudRate, unsigned int theDataBits,
                  ParityMode theParityMode, unsigned int theStopBits,
                  bool theHWHandShakeFlag,
                  int theMinBytesPerRead, int theTimeout);
        void close();

        bool read(char * theBuffer, size_t & theSize);
        void write(const char * theBuffer, size_t theSize);
        void flush();

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

/* @} */

}

#endif // ASL_TTY_PORT_INCLUDED
