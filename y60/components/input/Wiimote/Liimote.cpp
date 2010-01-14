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

#include "Liimote.h"

#include <asl/base/Logger.h>

#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#include <errno.h>
#include <poll.h>

using namespace asl;
using namespace std;

namespace y60 {

const bool DUMP_OUTPUT_REPORTS( false );

const int BT_TRANS_DATA( 0xA0 );
const int BT_PARAM_INPUT( 0x01 );

const int CTL_PSM(17);
const int INT_PSM(19);


Liimote::Liimote(bdaddr_t * theBTAddress ) :
    WiiRemote( inputReportListener ),
    _myCtlSocket( -1 ),
    _myIntSocket( -1 )
{
    bacpy( & _myBDAddress, theBTAddress );
    char myAddressString[19];
    ba2str( & _myBDAddress, myAddressString);

    setId( myAddressString);

	sockaddr_l2 myCtrlRemoteAddr;
    sockaddr_l2 myIntRemoteAddr;

	memset( & myCtrlRemoteAddr, 0, sizeof( myCtrlRemoteAddr ));
	myCtrlRemoteAddr.l2_family = AF_BLUETOOTH;
	myCtrlRemoteAddr.l2_bdaddr = _myBDAddress;
	myCtrlRemoteAddr.l2_psm = htobs(CTL_PSM);

	memset( & myIntRemoteAddr, 0, sizeof( myIntRemoteAddr ));
	myIntRemoteAddr.l2_family = AF_BLUETOOTH;
	myIntRemoteAddr.l2_bdaddr = _myBDAddress;
	myIntRemoteAddr.l2_psm = htobs(INT_PSM);

    _myCtlSocket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if ( _myCtlSocket == -1) {
        throw WiiException("Error opening control socket.", PLUS_FILE_LINE);
    }
    _myIntSocket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if ( _myIntSocket == -1) {
        throw WiiException("Error opening interrupt socket.", PLUS_FILE_LINE);
    }

    if (connect(_myCtlSocket, (sockaddr *) & myCtrlRemoteAddr,
                sizeof( myCtrlRemoteAddr )) == -1)
    {
        throw WiiException(string("Failed to connect to control socket: ") + strerror( errno ),
                           PLUS_FILE_LINE);
    }
    if (connect(_myIntSocket, (sockaddr *) & myIntRemoteAddr,
                sizeof(myIntRemoteAddr)) == -1)
    {
        throw WiiException(string("Failed to connect to interrupt socket: ") + strerror( errno ),
                    PLUS_FILE_LINE);
    }

    _isConnected = true;

    startThread();
}

Liimote::~Liimote() {
    stopThread();
}

void
dumpOutputReport(unsigned char theOutputReport[], unsigned theNumBytes) {
    cout.setf( std::ios::hex, std::ios::basefield );
    for(unsigned i=0; i<theNumBytes+1; ++i) {
        cout << "0x" << (int)theOutputReport[i] << ", ";
    }
    cout.unsetf( std::ios::hex );
    cout << endl;
}

void
Liimote::send(unsigned char theOutputReport[], unsigned theNumBytes) {
    unsigned char myBuffer[SEND_BUFFER_SIZE];

    myBuffer[0] = 0x50 | 0x02;

    memcpy(myBuffer+1, theOutputReport, theNumBytes);

    if (DUMP_OUTPUT_REPORTS) {
        dumpOutputReport( myBuffer, theNumBytes + 1);
    }

    if (write(_myCtlSocket, myBuffer, theNumBytes + 1) != (ssize_t)(theNumBytes + 1)) {
        // TODO: handle HUP
        throw WiiException("failed to send output report", PLUS_FILE_LINE);
    }
    //else if (verify_handshake(wiimote)) {
    //    return -1;
    //}

}

void
Liimote::closeDevice() {
    if (close( _myCtlSocket ) ) {
        throw WiiException("Failed to close control socket.", PLUS_FILE_LINE);
    }
    if( close( _myIntSocket ) ) {
        throw WiiException("Failed to close interrupt socket.", PLUS_FILE_LINE);
    }
}

void
Liimote::inputReportListener( PosixThread & theThread ) {
    try {
        Liimote & myDevice = dynamic_cast<Liimote&>( theThread );

        pollfd myPollSet;
        myPollSet.fd = myDevice._myIntSocket;
        myPollSet.events = POLLIN | POLLPRI;

        unsigned char myBuffer[ RECV_BUFFER_SIZE ];

        while (myDevice.getListeningFlag()) {

            if ( myDevice.isConnected() ) {
                myPollSet.revents = 0;
                int myResult = poll( & myPollSet, 1, 100 );
                if ( myResult == 0) {
                    // timeout
                    continue;
                } else if ( myResult == -1) {
                    throw WiiException(string("Error in poll(): ") + strerror( errno ), PLUS_FILE_LINE);
                }

                if (myPollSet.revents & POLLERR && myPollSet.revents & POLLHUP) {
                    myDevice.disconnect();
                } else if (myPollSet.revents & POLLIN || myPollSet.revents & POLLPRI) {

                    int myBytesReceived = read( myDevice._myIntSocket, myBuffer, RECV_BUFFER_SIZE );
                    if (( myBytesReceived == -1 ) || ( myBytesReceived == 0)) {
                        throw WiiException(string("Failed to read from interrupt socket: ") +
                                strerror( errno ) , PLUS_FILE_LINE);
                    } else {
                        if (myBuffer[0] != ( BT_TRANS_DATA | BT_PARAM_INPUT )) {
                            AC_ERROR << "Received invalid packet type.";
                        }
                        myDevice.dispatchInputReport(myBuffer, 1);
                    }
                }
            } else {
                // waiting for shutdown
                msleep( 10 );
            }
        }
    } catch (const asl::Exception & ex) {
        AC_ERROR << "asl::Exception in wiimote listener thread: " << ex;
        throw;
    } catch (const std::exception & ex) {
        AC_ERROR << "std::exception in wiimote listener thread: " << ex.what();
        throw;
    } catch (...) {
        AC_ERROR << "Unknown exception in wiimote listener thread";
        throw;
    }
}

} // end of namespace
