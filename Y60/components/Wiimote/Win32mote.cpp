//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


#include "stdafx.h"
#include "assert.h"
#include "Win32mote.h"
#include "Utils.h"

extern "C" {
// This file is in the Windows DDK available from Microsoft.
//#include "hidsdi.h"
//#include "setupapi.h"
#include <dbt.h>
}

#include <wtypes.h>
#include <initguid.h>
#include <process.h>

#include <iostream>

using namespace asl;
using namespace std;

namespace y60 {

Win32mote::Win32mote() : //HIDDevice(),
                         WiiRemote(InputReportListener),
                         _myEventObject(NULL)
{
	 // Add all Wii buttons to the our internal list
    /*
    _myButtons.push_back(Button("1",		0x0002));
    _myButtons.push_back(Button("2",		0x0001));
    _myButtons.push_back(Button("A",		0x0008));
    _myButtons.push_back(Button("B",		0x0004));
    _myButtons.push_back(Button("+",		0x1000));
    _myButtons.push_back(Button("-",		0x0010));
    _myButtons.push_back(Button("Home",	0x0080));
    _myButtons.push_back(Button("Up",	0x0800));
    _myButtons.push_back(Button("Down",	0x0400));
    _myButtons.push_back(Button("Right", 0x0200));
    _myButtons.push_back(Button("Left",	0x0100));
            
    _myInputListening = false;
    */
    
    //_myListenerThreadId = -1;
    //_myListenerThread = NULL;
    
    
}

void
Win32mote::InputReportListener(PosixThread & theThread)
{
    Win32mote & myDevice = dynamic_cast<Win32mote&>(theThread);

    //AC_PRINT << myDevice._myControllerId;
    while (myDevice.getListeningFlag()) {
        unsigned char myInputReport[256];
        ZeroMemory(myInputReport, 256);

        DWORD result, bytes_read;

        // Issue a read request
        if (myDevice._myReadHandle != INVALID_HANDLE_VALUE) {
            result = ReadFile(myDevice._myReadHandle, 
                    myInputReport, 
                    myDevice._myCapabilities.InputReportByteLength, 
                    &bytes_read, 
                    (LPOVERLAPPED)&myDevice._myHIDOverlap);																						  
        }  

        // Wait for read to finish
        result = WaitForSingleObject(myDevice._myEventObject, 300);

        ResetEvent(myDevice._myEventObject);

        // If the wait didn't result in a sucessful read, try again
        if (result != WAIT_OBJECT_0) {
            continue;
        }

        if (INPUT_REPORT_BUTTONS == myInputReport[0]) {

            myDevice._myLock->lock();

            myDevice.handleButtonEvents( myInputReport );

            myDevice._myLock->unlock();
        } else if (INPUT_REPORT_MOTION == myInputReport[0]) {

            
            myDevice._myLock->lock();


            myDevice.handleButtonEvents( myInputReport );
            myDevice.handleMotionEvents( myInputReport );

            myDevice._myLock->unlock();
            

        } else if (0x21 == myInputReport[0]) {
            Vector3f zero_point(myInputReport[6] - 128.0f, myInputReport[7] - 128.0f, myInputReport[8] - 128.0f);
            myDevice._myZeroPoint = zero_point;
            Vector3f one_g_point(myInputReport[10] - 128.0f, myInputReport[11] - 128.0f, myInputReport[12] - 128.0f);
            myDevice._myOneGPoint = one_g_point;
            
        } else if (INPUT_REPORT_IR == myInputReport[0]) {
            //int myX1 = myInputReport
            //cout << myInputReport << endl;
            
            myDevice._myLock->lock();
            myDevice.handleIREvents( myInputReport );
            myDevice.handleButtonEvents( myInputReport );
            myDevice.handleMotionEvents( myInputReport );

           
            // for(unsigned i=0; i<myDevice._myEventVector.size(); ++i) {
//                 myDevice._myEventQueue->push(myDevice._myEventVector[i]);
//             }
//             myDevice._myEventVector.clear();
            myDevice._myLock->unlock();
        } else if( INPUT_REPORT_STATUS == myInputReport[0]) {
            AC_PRINT << "\n============= status report =============";
            double batteryLevel = (double)myInputReport[6];
            batteryLevel /= (double)0xC0;
            AC_PRINT << " Battery Level              : " << batteryLevel;
            std::string extension = (myInputReport[3] & 0x02) ? "yes" : "no";
            AC_PRINT << " Extension Controller       : " << extension;
            std::string speaker = (myInputReport[3] & 0x04) ? "yes" : "no";
            AC_PRINT << " Speaker enabled            : " << speaker;
            std::string reporting = (myInputReport[3] & 0x08) ? "yes" : "no";
            AC_PRINT << " Continuous reports enabled : " << reporting;
            std::string led0 = (myInputReport[3] & 0x10) ? "yes" : "no";
            AC_PRINT << " LED 1 enabled              : " << led0;
            std::string led1 = (myInputReport[3] & 0x20) ? "yes" : "no";
            AC_PRINT << " LED 2 enabled              : " << led1;
            std::string led2 = (myInputReport[3] & 0x40) ? "yes" : "no";
            AC_PRINT << " LED 3 enabled              : " << led2;
            std::string led3 = (myInputReport[3] & 0x80) ? "yes" : "no";
            AC_PRINT << " LED 4 enabled              : " << led3;
            AC_PRINT << "=========================================\n";
            
        } else {
            //AC_PRINT << "unknown report " << myDevice._myControllerId << "  " << ios::hex << myInputReport[0] << 	ios::dec;
      	}
    }

}

void
Win32mote::closeDevice() {
	//CloseHandle(_myListenerThread);
	CloseHandle(_myDeviceHandle);
	CloseHandle(_myReadHandle);
	CloseHandle(_myWriteHandle);
}


std::vector<WiiRemotePtr>
Win32mote::discover() {

    std::vector<WiiRemotePtr> myDevices;

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

        if (Result == 0)
            break;

        // Call once to get the needed buffer length
        Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);
        detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // After allocating, call again to get data
        Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, Length, 
                &Required, NULL);

        cout << "device '" << detailData->DevicePath << "'" << endl;
        DeviceHandle = CreateFile(detailData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, 
                (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING,	0, NULL);

        Attributes.Size = sizeof(Attributes);

        Result = HidD_GetAttributes(DeviceHandle, &Attributes);

        if (Attributes.VendorID == WIIMOTE_VENDOR_ID && Attributes.ProductID == WIIMOTE_PRODUCT_ID) {
            // If the vendor and product IDs match, we've found a wiimote 
            Win32motePtr myDevice( new Win32mote() );

            myDevice->_myDevicePath = detailData->DevicePath;
            myDevice->_myDeviceHandle = DeviceHandle;

            // Register to receive device notifications.
            // RegisterForDeviceNotifications();

            GetDeviceCapabilities(*myDevice);

            myDevice->_myWriteHandle = CreateFile(detailData->DevicePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 
                    (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);

            PrepareForOverlappedTransfer(*myDevice, detailData);

            myDevice->_myControllerId = myDevices.size();
            //WiiRemotePtr myPtr( myDevice );
            myDevices.push_back( myDevice );
        } else {
            CloseHandle(DeviceHandle);
        }

        free(detailData);

        MemberIndex = MemberIndex + 1;
    } while (true);

    SetupDiDestroyDeviceInfoList(hDevInfo);

    return myDevices;
}

void
Win32mote::PrepareForOverlappedTransfer(Win32mote & device,
                                        PSP_DEVICE_INTERFACE_DETAIL_DATA & detailData)
{
	// Get a handle to the device for the overlapped ReadFiles.
	device._myReadHandle = CreateFile (detailData->DevicePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
                                     (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	// Get an event object for the overlapped structure.
	if (device._myEventObject == 0)
	{
      string empty = _T("");
      device._myEventObject = CreateEvent (NULL, TRUE, TRUE, empty.c_str());
      device._myHIDOverlap.hEvent = device._myEventObject;
      device._myHIDOverlap.Offset = 0;
      device._myHIDOverlap.OffsetHigh = 0;
	}
}

void
Win32mote::GetDeviceCapabilities(Win32mote & device) {
	//Get the Capabilities structure for the device.
	PHIDP_PREPARSED_DATA PreparsedData;

	HidD_GetPreparsedData(device._myDeviceHandle, &PreparsedData);
	HidP_GetCaps(PreparsedData, &device._myCapabilities);
	HidD_FreePreparsedData(PreparsedData);
}

void 
Win32mote::sendOutputReport(unsigned char out_bytes[], unsigned theNumBytes) {
    char	output_report[256];
    DWORD	bytes_written = 0;
    ULONG	result;

    memset(output_report, 0, 256);
    memcpy(output_report, out_bytes, theNumBytes);

    if (_myWriteHandle != INVALID_HANDLE_VALUE) {
        result = WriteFile(_myWriteHandle, output_report, _myCapabilities.OutputReportByteLength,
                           &bytes_written, NULL);
    }

    //cout << " write done " << endl;
    if (!result) {
        // If we can't write to the device, we're not really connected
        throw asl::Exception("Failed to send outgoing report", PLUS_FILE_LINE);
    }
}




} // end of namespace
