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

using namespace std;

namespace y60 {

const int MAX_BT_INQUIRY( 256 );
const int WIIMOTE_NAME_LENGTH( 32 );
const char * WIIMOTE_NAME("Nintendo RVL-CNT-01");

const uint8_t WIIMOTE_CLASS_0( 0x04 );
const uint8_t WIIMOTE_CLASS_1( 0x25 );
const uint8_t WIIMOTE_CLASS_2( 0x00 );

const int BT_TRANS_DATA( 0xA0 );
const int BT_PARAM_INPUT( 0x01 );

const int CTL_PSM(17);
const int INT_PSM(19);


Liimote::Liimote(const inquiry_info & theDeviceInfo, unsigned theId, const char * theName) :
    WiiRemote( inputListener, theId ),
    _myName( theName ),
    _myCtlSocket( -1 ),
    _myIntSocket( -1 )
{
    bacpy( & _myBDAddress, & theDeviceInfo.bdaddr );
    for (int i = 0; i < 3; ++i) {
        _myBTClass[i] = theDeviceInfo.dev_class[i];
    }

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
}

Liimote::~Liimote() {
    stopThread();
}

std::string 
Liimote::getDeviceName() const { 
    return _myName; 
}

void 
Liimote::send(unsigned char out_bytes[], unsigned theNumBytes) {
    unsigned char buf[SEND_BUFFER_SIZE];

    buf[0] = 0x50 | 0x02;
    
    memcpy(buf+1, out_bytes, theNumBytes);

    // cout.setf( std::ios::hex, std::ios::basefield );
//     for(unsigned i=0; i<theNumBytes+1; ++i) {
//         cout << "0x" << (int)buf[i] << ", ";
//     }
//     cout.unsetf( std::ios::hex );

//     cout << endl;
    
    /*
    if (!(flags & SEND_RPT_NO_RUMBLE)) {
        buf[2] |= wiimote->led_rumble_state & 0x01;
    }
*/
    if (write(_myCtlSocket, buf, theNumBytes+1) != (ssize_t)(theNumBytes+1)) {
        throw WiiException("failed to send output report", PLUS_FILE_LINE);
    }
   /*
   else if (verify_handshake(wiimote)) {
        return -1;
    }
*/

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
Liimote::inputListener( asl::PosixThread & theThread ) {
    Liimote & myDevice = dynamic_cast<Liimote&>( theThread );

    pollfd myPollSet;
    myPollSet.fd = myDevice._myIntSocket;
    myPollSet.events = POLLIN | POLLPRI;
    myPollSet.revents = 0;

    unsigned char myBuffer[ RECV_BUFFER_SIZE ];

    while (myDevice.getListeningFlag()) {

        int myResult = poll( & myPollSet, 1, 100 );
        if ( myResult == 0) {
            // timeout
            continue;
        } else if ( myResult == -1) {
            throw WiiException(string("Error in poll(): ") + strerror( errno ), PLUS_FILE_LINE);
        }

        int myBytesReceived = read( myDevice._myIntSocket, myBuffer, RECV_BUFFER_SIZE );
        if (( myBytesReceived == -1 ) || ( myBytesReceived == 0)) {
            AC_ERROR << "Failed to read from interrupt socket. byte: " << int(myBuffer[0]);
            throw WiiException("Failed to read from interrupt socket", PLUS_FILE_LINE);
        } else {
            if (myBuffer[0] != ( BT_TRANS_DATA | BT_PARAM_INPUT )) {
                AC_ERROR << "Received invalid packet type.";
            }
            myDevice.dispatchInputReport(myBuffer, 1);
        }
    }
}

std::vector<WiiRemotePtr> 
Liimote::discover() {
    std::vector<WiiRemotePtr> myDevices;

    inquiry_info * myDeviceList(0);
    int mySocket(-1);
    try {

        // TODO: handle more than one BT interface
        int myDeviceId = hci_get_route( NULL );
        if (myDeviceId == -1) {
            throw WiiException("No bluetooth interface found", PLUS_FILE_LINE);
        }

        int myTimeout = 2;
        int myDeviceCount = hci_inquiry( myDeviceId, 2, MAX_BT_INQUIRY, 0,
                & myDeviceList, IREQ_CACHE_FLUSH);

        if (myDeviceCount == -1) {
            throw WiiException("Failed to inquire bluetooth devices", PLUS_FILE_LINE);
        }

        if (myDeviceCount == 0) {
            // clean up
            if (myDeviceList) {
                free( myDeviceList );
            }
            return myDevices;
        }

        mySocket = hci_open_dev( myDeviceId );
        if ( mySocket == -1) {
            throw WiiException("Failed to open bluetooth device.", PLUS_FILE_LINE);
        }

        char myNameBuffer[WIIMOTE_NAME_LENGTH];
        for (int i = 0; i < myDeviceCount; ++i) {
            if (hci_remote_name( mySocket, & myDeviceList[i].bdaddr, WIIMOTE_NAME_LENGTH,
                        myNameBuffer, 5000))
            {
                throw WiiException("Failed to read bluetooth device name.", PLUS_FILE_LINE);
            }
            if (myDeviceList[i].dev_class[0] != WIIMOTE_CLASS_0 ||
                myDeviceList[i].dev_class[1] != WIIMOTE_CLASS_1 || 
                myDeviceList[i].dev_class[2] != WIIMOTE_CLASS_2 ||
                strncmp( myNameBuffer, WIIMOTE_NAME, WIIMOTE_NAME_LENGTH))
            {
                continue;
            }
            cerr << "name: " << myNameBuffer << endl;
            LiimotePtr myWii( new Liimote(myDeviceList[i], myDevices.size(), myNameBuffer) );
            myDevices.push_back( myWii );
            

        }

    } catch (...) {
        // clean up
        if (myDeviceList) {
            free( myDeviceList );
        }
        if (mySocket != -1) {
            hci_close_dev( mySocket );
        }
        throw;
    }
    return myDevices;
}

} // end of namespace
