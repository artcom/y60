//============================================================================
//
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: SoundLightUSBDMX.cpp,v $
//     $Author: valentin $
//   $Revision: 1.3 $
//
//=============================================================================

#include "SoundLightUSBDMX.h"
#include "_DasHard.h"

#include <iostream>
#include <asl/Vector234.h>
#include <y60/JSVector.h>

using namespace std;


extern "C"
EXPORT asl::PlugInBase* SoundLightUSBDMX_instantiatePlugIn(asl::DLHandle theDLHandle) {
    return new SoundLightUSBDMX(theDLHandle);
}


const unsigned SoundLightUSBDMX::MAX_DMX_CHANNELS = 512;

SoundLightUSBDMX::SoundLightUSBDMX(asl::DLHandle theDLHandle) :
	asl::PlugInBase(theDLHandle),
	_myDllHandle(0),
	_myUsbCommand(0),
	_myUniverse(0)
{
	cerr << "* SoundLightUSBDMX opening device" << endl;

	_myDllHandle = LoadLibrary("DasHard.dll");
	if (!_myDllHandle) {
		throw asl::Exception(std::string("DasHard.dll not found"), PLUS_FILE_LINE);
	}
	_myUsbCommand = (DASHARDCOMMAND) ::GetProcAddress((HMODULE)_myDllHandle, "DasHardCommand");
	if (!_myUsbCommand) {
		throw asl::Exception(std::string("DasHardCommand symbol not found"), PLUS_FILE_LINE);
	}
	if (DMXCommand(DHC_OPEN,0,0) <= 0) {
		throw asl::Exception(std::string("Unable to open device"), PLUS_FILE_LINE);
	}

	_myUniverse = new unsigned char[MAX_DMX_CHANNELS];
	memset(_myUniverse, 0, sizeof(_myUniverse));
	DMXCommand(DHC_DMXOUT, MAX_DMX_CHANNELS, _myUniverse);
}

SoundLightUSBDMX::~SoundLightUSBDMX()
{
	DMXCommand(DHC_CLOSE,0,0);
	if (_myDllHandle) {
		cerr << "* SoundLightUSBDMX closing device" << endl;
		FreeLibrary(_myDllHandle);
		_myDllHandle = 0;
	}
	_myUsbCommand = 0;

	delete[] _myUniverse;
	_myUniverse = 0;
}


void
SoundLightUSBDMX::onUpdateSettings(dom::NodePtr theConfiguration)
{
}

void
SoundLightUSBDMX::onGetProperty(const std::string & thePropertyName,
								y60::PropertyValue & theReturnValue) const
{
}

void
SoundLightUSBDMX::onSetProperty(const std::string & thePropertyName,
								const y60::PropertyValue & thePropertyValue)
{
	if (thePropertyName == "channel") {
		asl::Vector2i myData = thePropertyValue.get<asl::Vector2i>();
		cerr << "chan " << myData[0] << "=" << myData[1] << endl;

		unsigned myChannel = myData[0];
		unsigned char myValue = (unsigned char)myData[1];
		if (myChannel >= MAX_DMX_CHANNELS) {
			throw asl::Exception(std::string("DMX channel out-of-bounds: ")+asl::as_string(myChannel)+"="+asl::as_string(myValue), PLUS_FILE_LINE);
		}

		_myUniverse[myChannel] = myValue;
		DMXCommand(DHC_DMXOUT, MAX_DMX_CHANNELS, _myUniverse);
	}
}

int
SoundLightUSBDMX::DMXCommand(int theCommand, int theParam, unsigned char* theData)
{
	return (_myUsbCommand ? (*_myUsbCommand)(theCommand, theParam, theData) : -1);
}
