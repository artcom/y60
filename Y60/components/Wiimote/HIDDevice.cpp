// Wiim API ©2006 Eric B.
// http://digitalretrograde.com/projects/wiim/

// May be used and modified freely as long as this message is left intact

#include "HIDDevice.h"
#include <iostream>

using namespace std;

HIDDevice::HIDDevice(void) :
	m_event_object(NULL)
{
}

HIDDevice::~HIDDevice(void)
{
}

void HIDDevice::PrepareForOverlappedTransfer(HIDDevice & device, PSP_DEVICE_INTERFACE_DETAIL_DATA & detailData)
{
	// Get a handle to the device for the overlapped ReadFiles.
	device.m_read_handle = CreateFile (detailData->DevicePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
                                     (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	// Get an event object for the overlapped structure.
	if (device.m_event_object == 0)
	{
      string empty = _T("");
      device.m_event_object = CreateEvent (NULL, TRUE, TRUE, empty.c_str());
      device.m_hid_overlapped.hEvent = device.m_event_object;
      device.m_hid_overlapped.Offset = 0;
      device.m_hid_overlapped.OffsetHigh = 0;
	}
}

void HIDDevice::GetDeviceCapabilities(HIDDevice & device)
{
	//Get the Capabilities structure for the device.
	PHIDP_PREPARSED_DATA PreparsedData;

	HidD_GetPreparsedData(device.m_device_handle, &PreparsedData);
	HidP_GetCaps(PreparsedData, &device.m_capabilities);
	HidD_FreePreparsedData(PreparsedData);
}

void HIDDevice::WriteOutputReport(char out_bytes[], unsigned theNumBytes) {
    char	output_report[256];
    DWORD	bytes_written = 0;
    ULONG	result;

    memset(output_report, 0, 256);
    memcpy(output_report, out_bytes, theNumBytes);

    if (m_write_handle != INVALID_HANDLE_VALUE) {
        result = WriteFile(m_write_handle, output_report, m_capabilities.OutputReportByteLength,
                           &bytes_written, NULL);
    }

    //cout << " write done " << endl;
    if (!result) {
        // If we can't write to the device, we're not really connected
        throw asl::Exception("Failed to send outgoing report", PLUS_FILE_LINE);
    }
}


#if 0
void
HIDDevice::sendOutputReport( const unsigned char * theData, unsigned long theAddress, unsigned char theSize) {
	unsigned char cmd[22];
	//unsigned long addr = CFSwapInt32HostToBig(address);
	unsigned long addr = theAddress;

	int i;
	for(i=0 ; i < theSize ; i++) {
		cmd[i+6] = theData[i];
	}
	for(; i < 16; i++) {
		cmd[i+6]= 0;
	}
	
	cmd[0] = 0x16;
	cmd[1] = (addr >> 24) & 0xFF;
	cmd[2] = (addr >> 16) & 0xFF;
	cmd[3] = (addr >>  8) & 0xFF;
	cmd[4] = (addr >>  0) & 0xFF;
	cmd[5] = theSize;
	
	// and of course the vibration flag, as usual
	//if (isVibrationEnabled)	cmd[1] |= 0x01;
	
	data = cmd;
	
    // XXX send  data!
	//return [self sendCommand:cmd length:22];

}
#endif


//void HIDDevice::RegisterForDeviceNotifications(HIDDevice & device)
//{
//
//	// Request to receive messages when a device is attached or removed.
//	// Also see WM_DEVICECHANGE in BEGIN_MESSAGE_MAP(CUsbhidiocDlg, CDialog).
//
//	DEV_BROADCAST_DEVICEINTERFACE DevBroadcastDeviceInterface;
//	HDEVNOTIFY DeviceNotificationHandle;
//
//	DevBroadcastDeviceInterface.dbcc_size = sizeof(DevBroadcastDeviceInterface);
//	DevBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
//	DevBroadcastDeviceInterface.dbcc_classguid = HidGuid;
//
//	DeviceNotificationHandle =
//		RegisterDeviceNotification(m_hWnd, &DevBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);
//}


