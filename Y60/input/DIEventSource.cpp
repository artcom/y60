//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: DIEventSource.cpp,v $
//   $Author: janbo $
//   $Revision: 1.5 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

#include "DIEventSource.h"
#include "AxisEvent.h"
#include "ButtonEvent.h"

#include <iostream>

#include <SDL/SDL_syswm.h>

using namespace std;

#define DB(x) // x;

namespace y60 {

    DIEventSource::DIEventSource()
        : _myDI(0), _myBufferSize(10)
    {}


    DIEventSource::~DIEventSource() {
        _myDI->Release();
        for (unsigned i = 0; i < _myJoysticks.size(); ++i) {
            _myJoysticks[i]->Release();
        }
    }

    BOOL CALLBACK DIEventSource::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance,
                                        void* This)
    {
        ((DIEventSource*)This)->addJoystick(pdidInstance);
        return DIENUM_CONTINUE;
    }

    void DIEventSource::init() {
        HRESULT hr;

        hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
                                IID_IDirectInput8, (void**)&_myDI, NULL );
        if (FAILED(hr)) {
            cerr << "DIEventSource warning: DirectInput8Create() failed. Joysticks will not be available." << endl;
            _myDI = 0;
            return;
        }

        hr = _myDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
                                EnumJoysticksCallback,
                                this, DIEDFL_ATTACHEDONLY);  // DIEFL_ALLDEVICES?
        if (FAILED(hr)) {
            cerr << "DIEventSource - DirectInput8::EnumDevices() failed. Joysticks will not be available." << endl;
            _myDI = 0;
            return;
        }

        // Make sure we got a joystick
        if(_myJoysticks.size() == 0) {
            DB(cerr << "### WARNING: DIEventSource - No joysticks found." << endl;)
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
                cerr << "### WARNING: DIEventSource - SetDataFormat() failed." << endl;
                continue;
            }

            HWND mainHWND = findSDLWindow();

            hr = curJoystick->SetCooperativeLevel(mainHWND, DISCL_EXCLUSIVE | DISCL_FOREGROUND );
            if (FAILED(hr)) {
                cerr << "### WARNING: DIEventSource - SetCooperativeLevel() failed." << endl;
                continue;
            }

            hr = curJoystick->Acquire();
            if (FAILED(hr)) {
                cerr << "### WARNING: DIEventSource - Acquire() failed." << endl;
                continue;
            }
        }
    }

    void
    DIEventSource::addJoystick(const DIDEVICEINSTANCE* pdidInstance) {
        HRESULT hr;
        LPDIRECTINPUTDEVICE8 curJoystick;

        hr = _myDI->CreateDevice( pdidInstance->guidInstance, &curJoystick, NULL );
        if (FAILED(hr)) {
            cerr << "DIEventSource warning: joystick " << pdidInstance->tszInstanceName
                << ", " << pdidInstance->tszProductName
                << " found but CreateDevice failed. Ignoring." << endl;
        } else {
            _myJoysticks.push_back(curJoystick);
            cerr << "DIEventSource: added ";
            if (!strcmp(pdidInstance->tszInstanceName, "?")) {
                cerr << "Joystick " << _myJoysticks.size()-1;
            } else {
                cerr << (char*)(pdidInstance->tszInstanceName);
            }
            cerr << " as input source." << endl;
        }
    }

    HWND
    DIEventSource::findSDLWindow() {
        SDL_SysWMinfo myInfo;
        SDL_VERSION(&myInfo.version);
        int rc = SDL_GetWMInfo(&myInfo);
        if (!rc) {
            cerr << "DIEventSource warning: failed to locate main application window. "
                << "Disabling DirectInput." << endl;
            // TODO: release interface.
            _myDI = 0;
        }
        return myInfo.window;
    }

    vector<EventPtr>
    DIEventSource::poll() {
        HRESULT hr;
        vector<EventPtr> curEvents;

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
                    DB(cerr << "DIEventSource::poll() - Number of inputs read: " << myNumberOfInputs << endl;)
                    if (hr == DI_BUFFEROVERFLOW) {
                        DB(cerr << "DIEventSource::poll() - WARNING: Buffer overflow" << endl;)
                    }
                } else {
                    cerr << "### WARNING: DDIEventSource::poll() - GetDeviceState() failed." << endl;
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
    DIEventSource::printErrorState(const string & theCall, HRESULT hr) {
        cerr << "### WARNING: DIEventSource - " << theCall << " failed, reason:" << endl;
        switch (hr) {
            case DIERR_INPUTLOST:
                cerr << "Access to the input device has been lost. It must be reacquired." << endl;
                break;
            case DIERR_INVALIDPARAM:
                cerr << "An invalid parameter was passed to the returning function, " << endl;
                cerr << "or the object was not in a state that permitted the function to be called. " << endl;
                cerr << "This value is equal to the E_INVALIDARG standard Component Object Model (COM) return value." << endl;
                break;
            case DIERR_NOTACQUIRED:
                cerr << "The operation cannot be performed unless the device is acquired." << endl;
                break;
            case DIERR_NOTBUFFERED:
                cerr << "The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering." << endl;
                break;
            case DIERR_NOTINITIALIZED:
                cerr << "The object has not been initialized." << endl;
                break;
            case DIERR_OBJECTNOTFOUND:
                cerr << "The requested object does not exist." << endl;
                break;
            case DIERR_UNSUPPORTED:
                cerr << "The function called is not supported at this time." << endl;
                cerr << "This value is equal to the E_NOTIMPL standard COM return value." << endl;
                break;
        }
    }
/*
    void
    DIEventSource::handleAxis(int myJoystick, int myAxis, int curValue, int lastValue,
            vector<EventPtr>& curEvents)
    {
        if (curValue != lastValue) {
            curEvents.push_back(EventPtr (new AxisEvent(myJoystick, myAxis, curValue-32768)));
        }
    }
*/
}

