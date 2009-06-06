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

#include "WiiScanner.h"
#include "Utils.h"

#include <asl/base/Logger.h>

#ifdef LINUX
#   include "Liimote.h"

#   include <bluetooth/bluetooth.h>
#   include <bluetooth/hci.h>
#   include <bluetooth/hci_lib.h>
#   include <bluetooth/l2cap.h>
#   include <errno.h>

#elif WIN32

extern "C" {
#   include "hidsdi.h"
#   include "setupapi.h"
}
#   include "Win32mote.h"
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

const int MAX_CONSECUTIVE_ERRORS( 5 );
#endif


WiiScanner::WiiScanner() :
    PosixThread( scan ),
    _isScanning( true )
{
    init();
    fork();
}

WiiScanner::~WiiScanner() {
    _isScanning = false;
    AC_PRINT << "Waiting for scanner thread shutdown. This may take a couple of seconds ...";
    join();
#ifdef LINUX
    if (_mySocket != -1) {
        hci_close_dev( _mySocket );
        _mySocket = -1;
    }
#endif
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
        for (unsigned i = 0; i < theLostWiiIds.size(); ++i) {
            _myLostConnectionIdQueue.push( theLostWiiIds[i] );
        }
        while ( ! _myNewWiiQueue.empty() ) {
            theNewWiis.push_back( _myNewWiiQueue.front() );
            _myNewWiiQueue.pop();
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

void
WiiScanner::init() {
#ifdef LINUX

    // TODO: handle more than one BT interface
    _myDeviceId = hci_get_route( NULL );
    if (_myDeviceId == -1) {
        throw WiiException(string("No bluetooth interface found") +
                strerror( errno ), PLUS_FILE_LINE);
    }
    _mySocket = hci_open_dev( _myDeviceId );
    if ( _mySocket == -1) {
        throw WiiException(string("Failed to open bluetooth device: ") +
                strerror( errno ), PLUS_FILE_LINE);
    }

#endif
}

// TODO: clean up this mess
void
WiiScanner::collectNewWiiControllers() {
#ifdef LINUX
    inquiry_info * myDeviceList(0);
    try {
        int myTimeout = 2; // * 1.28s
        int myDeviceCount = hci_inquiry( _myDeviceId, myTimeout, MAX_BT_INQUIRY, 0,
                & myDeviceList, IREQ_CACHE_FLUSH);

        if (myDeviceCount == -1) {
            // Sometimes the inquiry fails with "device or resource busy". This probably
            // happens when the system hci daemon performs a inquiry. So we handle this 
            // gracefully.
            // This seems to happen more often with low batteries
            if (_myConsecutiveInquiryFailures < MAX_CONSECUTIVE_ERRORS) {
                AC_WARNING << "Failed to inquire bluetooth devices. Retrying...";
                _myConsecutiveInquiryFailures += 1;
                sleep( 2 ); // wait a while
            } else {
                throw WiiException(string("Failed to inquire bluetooth devices.") +
                        "Bailing out.",
                        PLUS_FILE_LINE);
            }
        } else {
            _myConsecutiveInquiryFailures = 0;
        }

        if (myDeviceCount <= 0) {
            // clean up
            if (myDeviceList) {
                free( myDeviceList );
            }
            return;
        }


        char myNameBuffer[WIIMOTE_NAME_LENGTH];
        char myAddressString[19];
        vector<WiiRemotePtr> myNewWiis;
        for (int i = 0; i < myDeviceCount; ++i) {
            myTimeout = 5000; // in ms (?)
            if (hci_remote_name( _mySocket, & myDeviceList[i].bdaddr, WIIMOTE_NAME_LENGTH,
                        myNameBuffer, myTimeout) < 0)
            {
                if (errno == ETIMEDOUT || errno == EIO) {
                    // This seems to happen more often with low batteries
                    if (_myConsecutiveTimeouts < MAX_CONSECUTIVE_ERRORS) {
                        if (errno == ETIMEDOUT ) {
                            AC_WARNING << "Failed to read bluetooth device name. Got timeout. Retrying...";
                        } else if (errno == EIO) {
                            AC_WARNING << "Failed to read bluetooth device name. Got IO error. Retrying...";
                        }
                        _myConsecutiveTimeouts += 1;
                    } else {
                        throw WiiException(string("Failed to read bluetooth device name. Got ") +
                                as_string(MAX_CONSECUTIVE_ERRORS) + " consecutive timeouts." +
                                "Bailing out.",
                                PLUS_FILE_LINE);
                    }
                } else {
                    throw WiiException(string("Failed to read bluetooth device name: ") +
                            strerror( errno ), PLUS_FILE_LINE);
                }
            } else {
                _myConsecutiveTimeouts = 0;

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
                set<string>::iterator myIt = _myKnownWiiIds.find( myBTAddress );
                if (myIt == _myKnownWiiIds.end() ) {
                    LiimotePtr myWii( new Liimote(& myDeviceList[i].bdaddr ) );
                    myNewWiis.push_back( myWii );
                    _myKnownWiiIds.insert( myBTAddress );
                }
            }
        }

        _myLock.lock();
        for (unsigned i = 0; i < myNewWiis.size(); ++i) {
            _myNewWiiQueue.push( myNewWiis[i] );
        }
        while ( ! _myLostConnectionIdQueue.empty() ) {
            _myKnownWiiIds.erase( _myLostConnectionIdQueue.front() );
            _myLostConnectionIdQueue.pop();
        }
        _myLock.unlock();

    } catch (...) {
        // clean up
        if (myDeviceList) {
            free( myDeviceList );
        }
        throw;
    }
    if (myDeviceList) {
        free( myDeviceList );
    }

#elif WIN32

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
       
        if( !Result ){
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
            // --------
//        PHIDP_PREPARSED_DATA PreparsedData;
//        HIDP_CAPS       _myCapabilities;
//	    HidD_GetPreparsedData(DeviceHandle, &PreparsedData);
//	    HidP_GetCaps(PreparsedData, &_myCapabilities);
//	    HidD_FreePreparsedData(PreparsedData);
//    
//        //AC_PRINT << "winscanner " << detailData->DevicePath;
//        HANDLE myWriteHandle = CreateFile(detailData->DevicePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 
//                                    (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);
//        DWORD myRetVal = GetLastError();
//        if (myRetVal) {
//            LPVOID myMessageBuffer;
//            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
//                          FORMAT_MESSAGE_IGNORE_INSERTS, NULL, myRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//                          (LPTSTR) & myMessageBuffer, 0, NULL);
//            string myErrorString;
//            myErrorString.append((LPTSTR) myMessageBuffer);
//            LocalFree(myMessageBuffer);
//            SetLastError(0);
//            throw WiiException(myErrorString, PLUS_FILE_LINE);
//        }
//        if(myWriteHandle == INVALID_HANDLE_VALUE) {
//            //CloseHandle(myWriteHandle);
//            AC_PRINT << "invalid write handle";
//            break;   
//        }
        
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
#endif
}
    
} // end of namespace

