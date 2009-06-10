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

#include "DInputFFBExtension.h"

#include <asl/base/Logger.h>
#include <y60/input/AxisEvent.h>
#include <y60/input/ButtonEvent.h>

#include <iostream>

#include <SDL/SDL_syswm.h>
#include <SDL/SDL.h>

#include <IFC/ImmCompoundEffect.h>

using namespace std;
using namespace y60;

#define DB(x) // x

DInputFFBExtension::DInputFFBExtension(asl::DLHandle theDLHandle)
    : asl::PlugInBase(theDLHandle), _myDevice(0), _myBufferSize(10)
{
    IFC_SET_ERROR_HANDLING(IFC_OUTPUT_ERR_TO_DEBUG);
}


DInputFFBExtension::~DInputFFBExtension() {
    //  Delete the project and device.
    if (_myProject) {
        _myProject->Close();
        delete _myProject;
        _myProject = 0;
    }

    if (_myDevice) {
        delete _myDevice;
        _myDevice = 0;
    }
}

void DInputFFBExtension::init() {
    HWND mainHWND = findSDLWindow();


    //  Create the device pointer.  This makes the proper DX or Immersion TouchSense API
    //  calls and returns a pointer to either a CImmDXDevice or a CImmMouse.  
    //  Note:  CreateDevice() will look for a mouse first, then a joystick, returning a 
    //  pointer to the first device it finds.
    _myDevice = CImmDevice::CreateDevice(getDLHandle(), mainHWND);
    if (!_myDevice) {
        AC_ERROR << "Initialization of FFB device failed.";
    } else {
        //  Use GetDeviceType() to determine which type of device you have.  This will be
        //  usefull when making calls that are dependent on the device type, such as
        //  GetJoyState() (later on in the program).
        _myDeviceType = _myDevice->GetDeviceType();
        if (_myDeviceType == IMM_DEVICETYPE_DIRECTINPUT) {
            // Read product name
            DIDEVICEINSTANCE didi;
            didi.dwSize = sizeof(DIDEVICEINSTANCE);
            ((LPDIRECTINPUTDEVICE2)(_myDevice->GetDevice()))->GetDeviceInfo(&didi);
            AC_PRINT << "Found FFB device '" << didi.tszProductName << "'.";

            //######## Setup ranges [-100,100]
            DIPROPRANGE diprg; 
 
            diprg.diph.dwSize       = sizeof(diprg); 
            diprg.diph.dwHeaderSize = sizeof(diprg.diph); 
            diprg.diph.dwObj        = DIJOFS_X; 
            diprg.diph.dwHow        = DIPH_BYOFFSET; 
            diprg.lMin              = -1000; 
            diprg.lMax              = 1000; 
 
            HRESULT hResult;
            hResult = ((LPDIRECTINPUTDEVICE2)(_myDevice->GetDevice()))->SetProperty(DIPROP_RANGE, &diprg.diph); 
            if (FAILED(hResult)) {
                AC_ERROR << "Setting X range failed.";
                _myDevice = 0;
                return;
            }

            diprg.diph.dwObj        = DIJOFS_Y; 

            hResult = ((LPDIRECTINPUTDEVICE2)(_myDevice->GetDevice()))->SetProperty(DIPROP_RANGE, &diprg.diph); 
            if (FAILED(hResult)) {
                AC_ERROR << "Setting Y range failed.";
                _myDevice = 0;
                return;
            }
/*
            //TODO: this does not work. why? [jb]
            //########  Setup buffering
            DIPROPDWORD  dipdw;
            dipdw.diph.dwSize = sizeof(DIPROPDWORD);
            dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
            dipdw.diph.dwObj = 0;
            dipdw.diph.dwHow = DIPH_DEVICE;
            dipdw.dwData     = _myBufferSize;
    
            hResult = ((LPDIRECTINPUTDEVICE8)(_myDevice->GetDevice()))->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
            if (FAILED(hResult)) {
                printErrorState("SetProperty(DIPROP_BUFFERSIZE)", hResult);
                return;
            }
    
            hResult = ((LPDIRECTINPUTDEVICE8)(_myDevice->GetDevice()))->SetDataFormat(&c_dfDIJoystick2);
            if (FAILED(hResult)) {
                AC_ERROR << "SetDataFormat() failed.";
                return;
            }            
*/
            //  Create a project and open the ifr file that contains the effects for this app
            _myProject = new CImmProject();
#if 0
            if (!_myProject->OpenFile("vroom.ifr", _myDevice)) {
                AC_ERROR << "Could not load effect file '" << "vroom.ifr" << "'.";
                return;
            }   

            LoadEngineParams();

            playEffectByName("Engine");
            playEffectByName("Ignition");
#else
            if (!_myProject->OpenFile("HelloIFC.ifr", _myDevice)) {
                AC_ERROR << "Could not load effect file '" << "HelloIFC.ifr" << "'.";
                return;
            }

            AC_PRINT << _myImmPeriodic2.InitializeFromProject(*_myProject, "UpDown");
	        AC_PRINT << _myImmPeriodic2.Start(); // Play "UpDown" periodic

            //_myProject->Start("MyEffect");
            _myCompoundEffect = _myProject->CreateEffect("MyEffect",_myDevice); // Play "MyEffect" (a compound effect)
            _myCompoundEffect->Start();

#endif
        } else {
            AC_ERROR << "Unknown device found.";
            _myDevice = 0;
            return;
        }
    }
    
}

void
DInputFFBExtension::LoadEngineParams() {
    //  GetEffect will return a compound effect.  In this case, the compound effect only
    //  contains a single effect.  We will store some of the effect's parameters
    //  for later use.

    //  Note that the project will free this effect when it is closed.
    _myCompoundEffect = _myProject->GetEffect("Engine");
    if (!_myCompoundEffect) {
        AC_ERROR << "LoadEffect failed.";
        return;
    }

    IMM_EFFECT myEff;
    _myCompoundEffect->GetContainedEffect((long)0)->GetParameters(myEff);

    LPIMM_PERIODIC buffer = (LPIMM_PERIODIC)myEff.lpvTypeSpecificParams;
    _myImmPeriodic.dwMagnitude = buffer->dwMagnitude; 
    _myImmPeriodic.lOffset     = buffer->lOffset; 
    _myImmPeriodic.dwPhase     = buffer->dwPhase; 
    _myImmPeriodic.dwPeriod    = buffer->dwPeriod; 

    //m_dwMagMultiplier        = _myImmPeriodic.dwMagnitude/20;

    _myCompoundEffect = 0;
}


void
DInputFFBExtension::playEffectByName(const string & theName) {
    //  If this effect pointer is already in use, get rid of it.
    if (_myCompoundEffect) {
        _myCompoundEffect->Stop();
    }

    //  Create an effect with the proper name from the project.  Note that it
    //  is safe to overwrite this pointer because the effect is created within
    //  the project.  When the project is closed, it will free all of its
    //  effects.
    if (_myProject) {
        _myCompoundEffect = _myProject->GetEffect(theName.c_str());
    } else {
        return;
    }

    if (!_myCompoundEffect) {
        AC_ERROR << "LoadEffect failed.";
        return;
    }

    //  Start the effect
    if (!_myCompoundEffect->Start()) {
        AC_ERROR << "StartEffect failed.";
        return;
    }
}

BOOL DInputFFBExtension::getJoyState() {
    HRESULT     hRes;

    if (_myDevice) {
        hRes = ((LPDIRECTINPUTDEVICE2)(_myDevice->GetDevice()))->Poll();
        if(FAILED(hRes)) {
            printErrorState("Poll()", hRes);
            return FALSE;// we did not read anything, return no motion
        }

        hRes = ((LPDIRECTINPUTDEVICE2)(_myDevice->GetDevice()))->GetDeviceState(sizeof(DIJOYSTATE), &_myDIEvent);
        if(FAILED(hRes)) {
            printErrorState("GetDeviceState()", hRes);
            return FALSE;// we did not read anything, return no motion
        }

        _myX = _myDIEvent.lX;
        _myY = _myDIEvent.lY;
    } else {
        return FALSE;
    }


    AC_PRINT << "Y = " << _myY << ", X = " << _myX;
    return TRUE;
}

EventPtrList
DInputFFBExtension::poll() {
    HRESULT hr;
    EventPtrList curEvents;
    
    if (_myDevice) {
        hr = ((LPDIRECTINPUTDEVICE2)(_myDevice->GetDevice()))->Poll();
        if(FAILED(hr)) {
            printErrorState("Poll()", hr);
            return curEvents;
        }

        hr = ((LPDIRECTINPUTDEVICE2)(_myDevice->GetDevice()))->GetDeviceState(sizeof(DIJOYSTATE), &_myDIEvent);
        if(FAILED(hr)) {
            printErrorState("GetDeviceState()", hr);
            return curEvents;// we did not read anything, return no motion
        }

        // TODO: try using DInputs mechanism to buffer like in DInputExtension [jb]
        if (_myX != _myDIEvent.lX) {
            _myX = _myDIEvent.lX;
            curEvents.push_back(EventPtr(new AxisEvent(0, 0, _myX)));
        }
        if (_myY != _myDIEvent.lY) {
            _myY = _myDIEvent.lY;
            curEvents.push_back(EventPtr(new AxisEvent(0, 1, _myY)));
        }
#if 1        
		_myImmPeriodic.dwMagnitude = 20;
		if (_myCompoundEffect)
			((CImmPeriodic*)(_myCompoundEffect->GetContainedEffect((long)0)))->ChangeParameters(_myImmPeriodic.dwMagnitude);

		//While the Y position varies from 0 - 100 period ranges from 50ms - 3 ms
		_myImmPeriodic.dwPeriod = ((100 - _myY) * 470) + 3000;

		//  Use ChangeParameters() to modify the type specific parameters of the effect
		//  dwPeriod is in microseconds
		//  device resolution is in miliseconds
		if (_myCompoundEffect)
			((CImmPeriodic*)(_myCompoundEffect->GetContainedEffect(long(0))))->ChangeParameters(
						IMM_EFFECT_DONT_CHANGE,
						_myImmPeriodic.dwPeriod/1000);
#else
        for (int i = 0, max = GetNumberOfContainedEffects();
             i < max; i++)
        {
            CImmEffect* myEffect = _myCompoundEffect->GetContainedEffect(long(1));
            DWORD myGain;
            myEffect->GetGain( myGain );
            myEffect->ChangeGain( myGain + 10 );
        }
#endif
    }

    return curEvents;
}    
    
HWND
DInputFFBExtension::findSDLWindow() {
    SDL_SysWMinfo myInfo;
    SDL_VERSION(&myInfo.version);
    int rc = SDL_GetWMInfo(&myInfo);
    if (rc != 1) {
        AC_WARNING << "Failed to locate main application window: " << SDL_GetError() << ". Disabling DirectInput.";
        // TODO: release interface.
        _myDevice = 0;
    }
    return myInfo.window;
}

void
DInputFFBExtension::printErrorState(const string & theCall, HRESULT hr) {
    AC_ERROR << theCall << " failed, reason:";
    switch (hr) {
        case DIERR_INPUTLOST:
            AC_ERROR << "Access to the input device has been lost. It must be reacquired.";
            break;
        case DIERR_INVALIDPARAM:
            AC_ERROR << "An invalid parameter was passed to the returning function, ";
            AC_ERROR << "or the object was not in a state that permitted the function to be called. ";
            AC_ERROR << "This value is equal to the E_INVALIDARG standard Component Object Model (COM) return value.";
            break;
        case DIERR_NOTACQUIRED:
            AC_ERROR << "The operation cannot be performed unless the device is acquired.";
            break;
        case DIERR_NOTBUFFERED:
            AC_ERROR << "The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering.";
            break;
        case DIERR_NOTINITIALIZED:
            AC_ERROR << "The object has not been initialized.";
            break;
        case DIERR_OBJECTNOTFOUND:
            AC_ERROR << "The requested object does not exist.";
            break;
        case DIERR_UNSUPPORTED:
            AC_ERROR << "The function called is not supported at this time.";
            AC_ERROR << "This value is equal to the E_NOTIMPL standard COM return value.";
            break;
    }
}

extern "C"
EXPORT asl::PlugInBase* DInputFFB_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new DInputFFBExtension(myDLHandle);
}

