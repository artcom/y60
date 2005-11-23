//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "DInputExtension.h"

#include <asl/Logger.h>
#include <y60/AxisEvent.h>
#include <y60/ButtonEvent.h>

#include <iostream>

#include <SDL/SDL_syswm.h>
#include <SDL/SDL.h>

using namespace std;
using namespace y60;

#define DB(x) // x

DInputExtension::DInputExtension(asl::DLHandle theDLHandle)
    : asl::PlugInBase(theDLHandle), _myDI(0), _myBufferSize(10)
{}


DInputExtension::~DInputExtension() {
    _myDI->Release();
    for (unsigned i = 0; i < _myJoysticks.size(); ++i) {
        _myJoysticks[i]->Release();
    }
}

BOOL CALLBACK DInputExtension::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance,
                                    void* This)
{
    ((DInputExtension*)This)->addJoystick(pdidInstance);
    return DIENUM_CONTINUE;
}

void DInputExtension::init() {
    HRESULT hr;

    hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
                            IID_IDirectInput8, (void**)&_myDI, NULL );
    if (FAILED(hr)) {
        AC_WARNING << "DirectInput8Create() failed. Joysticks will not be available.";
        _myDI = 0;
        return;
    }

    hr = _myDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
                            EnumJoysticksCallback,
                            this, DIEDFL_ATTACHEDONLY);  // DIEFL_ALLDEVICES?
    if (FAILED(hr)) {
        AC_ERROR << "DirectInput8::EnumDevices() failed. Joysticks will not be available.";
        _myDI = 0;
        return;
    }

    // Make sure we got a joystick
    if(_myJoysticks.size() == 0) {
        AC_WARNING << "DInputExtension - No joysticks found.";
        return;
    }

    for (unsigned i = 0; i < _myJoysticks.size(); ++i) {
        LPDIRECTINPUTDEVICE8 curJoystick = _myJoysticks[i];

        // Setup buffering
        DIPROPDWORD  dipdw;
        HRESULT      hr;
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData     = _myBufferSize;

        hr = curJoystick->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
        if (FAILED(hr)) {
            printErrorState("SetProperty(DIPROP_BUFFERSIZE)", hr);
            continue;
        }

        hr = curJoystick->SetDataFormat(&c_dfDIJoystick2);
        if (FAILED(hr)) {
            AC_ERROR << "SetDataFormat() failed.";
            continue;
        }

        HWND mainHWND = findSDLWindow();

        hr = curJoystick->SetCooperativeLevel(mainHWND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
        if (FAILED(hr)) {
            AC_ERROR << "SetCooperativeLevel() failed.";
            continue;
        }

        hr = curJoystick->Acquire();
        if (FAILED(hr)) {
            AC_ERROR << "Acquire() failed.";
            continue;
        }
    }
}

void
DInputExtension::addJoystick(const DIDEVICEINSTANCE* pdidInstance) {
    HRESULT hr;
    LPDIRECTINPUTDEVICE8 curJoystick;

    hr = _myDI->CreateDevice( pdidInstance->guidInstance, &curJoystick, NULL );
    if (FAILED(hr)) {
        AC_WARNING << "Joystick " << pdidInstance->tszInstanceName
            << ", " << pdidInstance->tszProductName
            << " found but CreateDevice failed. Ignoring.";
    } else {
        _myJoysticks.push_back(curJoystick);
        AC_INFO << "Added ";
        if (!strcmp(pdidInstance->tszInstanceName, "?")) {
            AC_INFO << "Joystick " << _myJoysticks.size()-1;
        } else {
            AC_INFO << (char*)(pdidInstance->tszInstanceName);
        }
        AC_INFO << " as input source.";
    }
}

HWND
DInputExtension::findSDLWindow() {
    SDL_SysWMinfo myInfo;
    SDL_VERSION(&myInfo.version);
    int rc = SDL_GetWMInfo(&myInfo);
    if (rc != 1) {
        AC_WARNING << "Failed to locate main application window: " << SDL_GetError() << ". Disabling DirectInput.";
        // TODO: release interface.
        _myDI = 0;
    }
    return myInfo.window;
}

EventPtrList
DInputExtension::poll() {
    HRESULT hr;
    EventPtrList curEvents;

    for (unsigned i=0; i<_myJoysticks.size(); i++) {
        LPDIRECTINPUTDEVICE8 curJoystick = _myJoysticks[i];

        hr = curJoystick->Poll();
        if (FAILED(hr)) {
            // DInput is telling us that the input stream has been
            // interrupted. We aren't tracking any state between polls, so
            // we don't have any special reset that needs to be done. We
            // just re-acquire and try again.
            // TODO: Does this ever happen if we're using DISCL_BACKGROUND?
            hr = curJoystick->Acquire();
            while(hr == DIERR_INPUTLOST) {
                hr = curJoystick->Acquire();
            }
            // Fall through to next joystick & poll in the next call...
        } else {
            LPDIDEVICEOBJECTDATA myBufferedJEvent = new DIDEVICEOBJECTDATA[_myBufferSize];
            DWORD myNumberOfInputs = _myBufferSize;
            hr = curJoystick->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), myBufferedJEvent, &myNumberOfInputs, 0);

            if (SUCCEEDED(hr)) {
                AC_DEBUG << "DInputExtension::poll() - Number of inputs read: " << myNumberOfInputs;
                if (hr == DI_BUFFEROVERFLOW) {
                    AC_WARNING << "Buffer overflow";
                }
            } else {
                AC_WARNING << "GetDeviceState() failed.";
                continue;
            }

            for (unsigned j = 0; j < myNumberOfInputs; ++j) {
                DIDEVICEOBJECTDATA myJEvent = myBufferedJEvent[j];

                switch (myJEvent.dwOfs) {
                    case DIJOFS_X:
                        curEvents.push_back(EventPtr(new AxisEvent(i, 0, myJEvent.dwData - 32768)));
                        break;
                    case DIJOFS_Y:
                        curEvents.push_back(EventPtr(new AxisEvent(i, 1, myJEvent.dwData - 32768)));
                        break;
                    case DIJOFS_Z:
                        curEvents.push_back(EventPtr(new AxisEvent(i, 2, myJEvent.dwData - 32768)));
                        break;
                    case DIJOFS_RX:
                        curEvents.push_back(EventPtr(new AxisEvent(i, 3, myJEvent.dwData - 32768)));
                        break;
                    case DIJOFS_RY:
                        curEvents.push_back(EventPtr(new AxisEvent(i, 4, myJEvent.dwData - 32768)));
                        break;
                    case DIJOFS_RZ:
                        curEvents.push_back(EventPtr(new AxisEvent(i, 5, myJEvent.dwData - 32768)));
                        break;
                    case DIJOFS_SLIDER(0):
                        curEvents.push_back(EventPtr(new AxisEvent(i, 6, myJEvent.dwData - 32768)));
                        break;
                    case DIJOFS_SLIDER(1):
                        curEvents.push_back(EventPtr(new AxisEvent(i, 7, myJEvent.dwData - 32768)));
                        break;
                }

                for (unsigned myButtonIndex = 0; myButtonIndex < 32; ++myButtonIndex) {
                    if (myJEvent.dwOfs == DIJOFS_BUTTON(myButtonIndex)) {
                        Event::Type myType;
                        if (myJEvent.dwData > 127) {
                            myType = Event::Type::BUTTON_DOWN;
                        } else {
                            myType = Event::Type::BUTTON_UP;
                        }
                        curEvents.push_back(EventPtr(new ButtonEvent(myType, i, myButtonIndex)));
                    }
                }
            }

            delete [] myBufferedJEvent;
        }
    }
    return curEvents;
}

void
DInputExtension::printErrorState(const string & theCall, HRESULT hr) {
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
EXPORT asl::PlugInBase* Y60DInput_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new DInputExtension(myDLHandle);
}

