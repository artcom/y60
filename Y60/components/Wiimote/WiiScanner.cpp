//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "WiiScanner.h"
#include "Utils.h"

#include <asl/Logger.h>

#ifdef LINUX
#include "Liimote.h" // XXX
#   include <bluetooth/bluetooth.h>
#   include <bluetooth/hci.h>
#   include <bluetooth/hci_lib.h>
#   include <bluetooth/l2cap.h>
#elif WIN32
// XXX
extern "C" {
#   include "hidsdi.h"
#   include "setupapi.h"
}
#include "Win32mote.h"
#endif


using namespace std;
using namespace asl;

namespace y60 {

#ifdef LINUX
const int MAX_BT_INQUIRY( 256 );
const int WIIMOTE_NAME_LENGTH( 32 );
const char * WIIMOTE_NAME("Nintendo RVL-CNT-01");

const uint8_t WIIMOTE_CLASS_0( 0x04 );
const uint8_t WIIMOTE_CLASS_1( 0x25 );
const uint8_t WIIMOTE_CLASS_2( 0x00 );
#endif


WiiScanner::WiiScanner() :
    PosixThread( scan ),
    _isScanning( true )
{
    fork();
}

WiiScanner::~WiiScanner() {
    _isScanning = false;
    join();
}

void
WiiScanner::poll(vector<WiiRemotePtr> & theNewWiis, const vector<string> & theLostWiiIds) {

    if ( theLostWiiIds.empty() ) {
        if (_myLock.nonblock_lock() == 0) {
            while ( ! _myNewWiiQueue.empty() ) {
                theNewWiis.push_back( _myNewWiiQueue.front() );
                _myNewWiiQueue.pop();
            }
            _myLock.unlock();
        }
    } else {
        _myLock.lock();
        while ( ! _myNewWiiQueue.empty() ) {
            theNewWiis.push_back( _myNewWiiQueue.front() );
            _myNewWiiQueue.pop();
        }
        for (unsigned i = 0; i < theLostWiiIds.size(); ++i) {
            _myLostConnectionIdQueue.push( theLostWiiIds[i] );
        }
        _myLock.unlock();
    }
}

void
WiiScanner::scan( asl::PosixThread & theThread ) {
    try {
        WiiScanner & mySelf = dynamic_cast<WiiScanner&>( theThread );

        while (mySelf._isScanning) {
            mySelf.collectNewWiiControllers();
        }
    } catch (const asl::Exception & ex) {
        AC_ERROR << "asl::Exception in wiimote scanner thread: " << ex;
        throw;
    } catch (const std::exception & ex) {
        AC_ERROR << "std::exception in wiimote scanner thread: " << ex.what();
        throw;
    } catch (...) {
        AC_ERROR << "Unknown exception in wiimote scanner thread";
        throw;
    }
}

#ifdef LINUX
// TODO: clean up this mess
void
WiiScanner::collectNewWiiControllers() {
    inquiry_info * myDeviceList(0);
    int mySocket(-1);
    try {

        // TODO: handle more than one BT interface
        int myDeviceId = hci_get_route( NULL );
        if (myDeviceId == -1) {
            throw WiiException("No bluetooth interface found", PLUS_FILE_LINE);
        }
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
            return;
        }

        mySocket = hci_open_dev( myDeviceId );
        if ( mySocket == -1) {
            throw WiiException("Failed to open bluetooth device.", PLUS_FILE_LINE);
        }

        char myNameBuffer[WIIMOTE_NAME_LENGTH];
        char myAddressString[19];
        vector<WiiRemotePtr> myNewWiis;
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
            char myBTAddressCStr[19];
            ba2str( & myDeviceList[i].bdaddr, myBTAddressCStr );
            string myBTAddress( myBTAddressCStr );
            set<string>::iterator myIt = _myKnownWiiIds.find( myBTAddress);
            if (myIt == _myKnownWiiIds.end() ) {
                LiimotePtr myWii( new Liimote(& myDeviceList[i].bdaddr ) );
                myNewWiis.push_back( myWii );
                _myKnownWiiIds.insert( myBTAddress );
            }
        }

        _myLock.lock();
        for (unsigned i = 0; i < myNewWiis.size(); ++i) {
            _myNewWiiQueue.push( myNewWiis[i] );
        }
        while ( ! _myLostConnectionIdQueue.empty() ) {
            AC_PRINT << "==== removing wii";
            _myKnownWiiIds.erase( _myLostConnectionIdQueue.front() );
            _myLostConnectionIdQueue.pop();
        }
        _myLock.unlock();

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
    if (myDeviceList) {
        free( myDeviceList );
    }
    if (mySocket != -1) {
        hci_close_dev( mySocket );
    }
}
#elif WIN32
void
WiiScanner::collectNewWiiControllers() {
    HANDLE WriteHandle = 0, DeviceHandle = 0;

    HANDLE hDevInfo;

    HIDD_ATTRIBUTES						Attributes;
    SP_DEVICE_INTERFACE_DATA			devInfoData;
    int									MemberIndex = 0;
    LONG								Result;	
    GUID								HidGuid;
    PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData;

    ULONG Required = 0;
    ULONG Length = 0;
    detailData = NULL;
    DeviceHandle = NULL;

    HidD_GetHidGuid(&HidGuid);	

    hDevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

    devInfoData.cbSize = sizeof(devInfoData);
    MemberIndex = 0;

    do {
        // Got any more devices?
        Result = SetupDiEnumDeviceInterfaces (hDevInfo, 0, &HidGuid, MemberIndex,	&devInfoData);

        if (Result == 0) {
            break;
        }
        // Call once to get the needed buffer length
        Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);
        detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // After allocating, call again to get data
        Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, Length, 
                                                 &Required, NULL);
        DeviceHandle = CreateFile(detailData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, 
                                  (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING,	0, NULL);

        Attributes.Size = sizeof(Attributes);
        
        Result = HidD_GetAttributes(DeviceHandle, &Attributes);
    
        if (Attributes.VendorID == WIIMOTE_VENDOR_ID && Attributes.ProductID == WIIMOTE_PRODUCT_ID) {
            vector<WiiRemotePtr> myNewWiis;
            set<string>::iterator myIt = _myKnownWiiIds.find( detailData->DevicePath);
            if (myIt == _myKnownWiiIds.end() ) {
                Win32motePtr myWii( new Win32mote(detailData, DeviceHandle) );
                myNewWiis.push_back( myWii );
                _myKnownWiiIds.insert( detailData->DevicePath );
            }
            
            _myLock.lock();
            for (unsigned i = 0; i < myNewWiis.size(); ++i) {
                _myNewWiiQueue.push( myNewWiis[i] );
            }
            while ( ! _myLostConnectionIdQueue.empty() ) {
                AC_PRINT << "==== removing wii";
                _myKnownWiiIds.erase( _myLostConnectionIdQueue.front() );
                _myLostConnectionIdQueue.pop();
            }
            _myLock.unlock();
        } else {
            CloseHandle(DeviceHandle);
        }
        
        free(detailData);
        MemberIndex = MemberIndex + 1;
        
    } while(true);
    SetupDiDestroyDeviceInfoList(hDevInfo);
}
    
#endif

} // end of namespace

