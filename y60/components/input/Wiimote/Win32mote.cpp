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

Win32mote::Win32mote(PSP_DEVICE_INTERFACE_DETAIL_DATA	& theDeviceData, HANDLE & theDeviceHandle) :
    WiiRemote(inputReportListener),
    _myEventObject(NULL)
{

    _myDevicePath = theDeviceData->DevicePath;
    _myDeviceHandle = theDeviceHandle;

    setId( _myDevicePath);
    // Register to receive device notifications.
    // RegisterForDeviceNotifications();

    GetDeviceCapabilities();
    _myWriteHandle = CreateFile(_myDevicePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
                                (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);
    if (_myWriteHandle == INVALID_HANDLE_VALUE) {
        //AC_PRINT << "win32 cstor invalid write handle";
        throw WiiException("write handle invalid" ,  PLUS_FILE_LINE);
    }

    PrepareForOverlappedTransfer();

    _isConnected = true;

    startThread();
}

Win32mote::~Win32mote() {
    stopThread();
}

void
Win32mote::inputReportListener(PosixThread & theThread)
{
    try {

        Win32mote & myDevice = dynamic_cast<Win32mote&>(theThread);

        AC_PRINT << myDevice._myControllerId;
        unsigned char myInputReport[RECV_BUFFER_SIZE];
        while (myDevice.getListeningFlag()) {
            ZeroMemory(myInputReport, RECV_BUFFER_SIZE);

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

            myDevice.dispatchInputReport(myInputReport, 0);
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

void
Win32mote::closeDevice() {
	CloseHandle(_myDeviceHandle);
	CloseHandle(_myReadHandle);
	CloseHandle(_myWriteHandle);
}


void
Win32mote::PrepareForOverlappedTransfer()
{
	// Get a handle to the device for the overlapped ReadFiles.
    _myReadHandle = CreateFile (_myDevicePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
                                (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    // Get an event object for the overlapped structure.
    if (_myEventObject == 0)
    {
        string empty = _T("");
        _myEventObject = CreateEvent (NULL, TRUE, TRUE, empty.c_str());
        _myHIDOverlap.hEvent = _myEventObject;
        _myHIDOverlap.Offset = 0;
        _myHIDOverlap.OffsetHigh = 0;
    }
}

void
Win32mote::GetDeviceCapabilities() {
	//Get the Capabilities structure for the device.
	PHIDP_PREPARSED_DATA PreparsedData;

	HidD_GetPreparsedData(_myDeviceHandle, &PreparsedData);
	HidP_GetCaps(PreparsedData, &_myCapabilities);
	HidD_FreePreparsedData(PreparsedData);
}

void
Win32mote::send(unsigned char out_bytes[], unsigned theNumBytes) {
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
        throw WiiException("Failed to send outgoing report", PLUS_FILE_LINE);
    }
}

#if 0
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
            Win32motePtr myDevice( new Win32mote( myDevices.size() ) );

            // TODO: move this stuff to Win32mote CTor
            myDevice->_myDevicePath = detailData->DevicePath;
            myDevice->_myDeviceHandle = DeviceHandle;

            // Register to receive device notifications.
            // RegisterForDeviceNotifications();

            GetDeviceCapabilities(*myDevice);

            myDevice->_myWriteHandle = CreateFile(detailData->DevicePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
                    (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);

            PrepareForOverlappedTransfer(*myDevice, detailData);

            myDevice->_isConnected = true;

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
#endif



} // end of namespace
