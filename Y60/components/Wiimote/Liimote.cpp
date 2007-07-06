//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Liimote.h"

#include <asl/Logger.h>

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
