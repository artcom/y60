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
*/

#include "DInputExtension.h"

#include <asl/base/Logger.h>
#include <asl/base/Assure.h>

#include <y60/jsbase/Documentation.h>
#include <y60/jsbase/JScppUtils.h>

#include <y60/input/AxisEvent.h>
#include <y60/input/ButtonEvent.h>

#include <iostream>

#include <SDL/SDL_syswm.h>
#include <SDL/SDL.h>

using namespace std;
using namespace y60;
using namespace jslib;

#define DB(x) // x
namespace y60 {

    DInputExtension::DInputExtension(asl::DLHandle theDLHandle)
        : asl::PlugInBase(theDLHandle), _myDI(0), _myBufferSize(10), _myForceFeedBackFlag(false)
    {}


    DInputExtension::~DInputExtension() {
        if (_myDI) {
            _myDI->Release();
        }
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
        DWORD myStyle = DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK;
        DWORD myCoopLevel = DISCL_EXCLUSIVE | DISCL_FOREGROUND;
        initJoysticks(myStyle, myCoopLevel);
        _myForceFeedBackFlag = _myJoysticks.size() >0;

        if (!_myForceFeedBackFlag) {
            // try again for non forcefeedbacks
            myStyle = DIEDFL_ATTACHEDONLY;
            myCoopLevel = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
            initJoysticks(myStyle, myCoopLevel);
        } else {
            _myForceEffects.resize(_myJoysticks.size());
        }
        // Make sure we got a joystick
        if(_myJoysticks.size() == 0) {
            AC_WARNING << "DInputExtension - No DInput device found.";
            return;
        }

    }

    void
    DInputExtension::initJoysticks(DWORD theFlags, DWORD theCoopLevel) {
        HRESULT hr;

        hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
                                IID_IDirectInput8, (void**)&_myDI, NULL );
        if (FAILED(hr)) {
            AC_WARNING << "DirectInput8Create() failed. DInput devices will not be available.";
            _myDI = 0;
            return;
        }

        hr = _myDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
                                EnumJoysticksCallback,
                                this, theFlags);
        if (FAILED(hr)) {
            AC_ERROR << "DirectInput8::EnumDevices() failed. Joystick will not be available.";
            _myDI = 0;
            return;
        }

        hr = _myDI->EnumDevices(DI8DEVTYPE_DRIVING,
                                EnumJoysticksCallback,
                                this, DIEDFL_ATTACHEDONLY);
        if (FAILED(hr)) {
            AC_ERROR << "DirectInput8::EnumDevices() failed. Driving device will not be available.";
            _myDI = 0;
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

            hr = curJoystick->SetDataFormat(&c_dfDIJoystick);
            //hr = curJoystick->SetDataFormat(&c_dfDIJoystick2);
            if (FAILED(hr)) {
                AC_ERROR << "SetDataFormat() failed.";
                continue;
            }

            HWND mainHWND = findSDLWindow();

            hr = curJoystick->SetCooperativeLevel(mainHWND, theCoopLevel );
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
                                myType = Event::BUTTON_DOWN;
                            } else {
                                myType = Event::BUTTON_UP;
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
    DInputExtension::createForceEffect(LPDIRECTINPUTDEVICE8 & theJoystick, LPDIRECTINPUTEFFECT & theEffect) {
            HRESULT hr;
            DIEFFECT diEffect;               // parameters for created effect
            ZeroMemory( &diEffect, sizeof( diEffect ) );

            DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
            LONG     lDirection[2] = { 18000, 0 };

            DICONSTANTFORCE diConstantForce;

            diConstantForce.lMagnitude =0;//DI_FFNOMINALMAX;   // Full force

            diEffect.dwSize          = sizeof(DIEFFECT);
            diEffect.dwFlags         = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
            diEffect.dwDuration      = INFINITE;//(DWORD)(10.5 * DI_SECONDS);
            diEffect.dwSamplePeriod  = 0;                 // = default
            diEffect.dwGain          = DI_FFNOMINALMAX;   // No scaling
            diEffect.dwTriggerButton = DIEB_NOTRIGGER;    // Not a button response
            diEffect.dwTriggerRepeatInterval = 0;         // Not applicable
            diEffect.cAxes                   = 2;
            diEffect.rgdwAxes                = dwAxes;
            diEffect.rglDirection            = lDirection;
            diEffect.lpEnvelope              = NULL;
            diEffect.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
            diEffect.lpvTypeSpecificParams   = &diConstantForce;

            hr = theJoystick->CreateEffect(GUID_ConstantForce,
                                       &diEffect,
                                       &theEffect,
                                       NULL);
            if (FAILED(hr)) {
                AC_ERROR << "createForceEffect() failed.";
            }
    }

    void
    DInputExtension::printErrorState(const string & theCall, HRESULT hr) {
        AC_ERROR << theCall << " failed, reason (" << hr << "):";
        switch (hr) {
            case DIERR_INPUTLOST:
                {AC_ERROR << "Access to the input device has been lost. It must be reacquired.";}
                break;
            case DIERR_INVALIDPARAM:
                {AC_ERROR << "An invalid parameter was passed to the returning function, ";}
                {AC_ERROR << "or the object was not in a state that permitted the function to be called. ";}
                {AC_ERROR << "This value is equal to the E_INVALIDARG standard Component Object Model (COM) return value.";}
                break;
            case DIERR_NOTACQUIRED:
                {AC_ERROR << "The operation cannot be performed unless the device is acquired.";}
                break;
            case DIERR_NOTBUFFERED:
                {AC_ERROR << "The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering.";}
                break;
            case DIERR_NOTINITIALIZED:
                {AC_ERROR << "The object has not been initialized.";}
                break;
            case DIERR_OBJECTNOTFOUND:
                {AC_ERROR << "The requested object does not exist.";}
                break;
            case DIERR_UNSUPPORTED:
                {AC_ERROR << "The function called is not supported at this time.";}
                {AC_ERROR << "This value is equal to the E_NOTIMPL standard COM return value.";}
                break;
        }
    }
    void
    DInputExtension::applyForce(unsigned theJoystickIndex, int theXMagnitude, int theYMagnitude) {
        if (!_myForceFeedBackFlag) {
            AC_INFO << "DInputExtension::setAutoCentering: calling a forcefeedback method on a non forcefeedback joystick, sorry!";
            return;
        }
        if (theJoystickIndex > _myJoysticks.size()-1) {
            AC_WARNING << "DInputExtension::setAutoCentering, index: '" << theJoystickIndex << "' is out of joysticks range: [0.." << _myJoysticks.size()-1 << "], sorry!";
            return;
        }

        ASSURE(_myJoysticks.size());
        if (!_myForceEffects[theJoystickIndex]) {
            createForceEffect(_myJoysticks[theJoystickIndex], _myForceEffects[theJoystickIndex]);
        }

        HRESULT hr;

        LONG rglDirection[2] = { 0, 0 };

        DICONSTANTFORCE cf;
        rglDirection[0] = theXMagnitude;
        rglDirection[1] = theYMagnitude;

        cf.lMagnitude = ( DWORD )sqrt( ( double )theXMagnitude * ( double )theXMagnitude +
                                       ( double )theYMagnitude * ( double )theYMagnitude );

        DIEFFECT eff;
        ZeroMemory( &eff, sizeof( eff ) );
        eff.dwSize = sizeof( DIEFFECT );
        eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        eff.cAxes = 2;
        eff.rglDirection = rglDirection;
        eff.lpEnvelope = 0;
        eff.cbTypeSpecificParams = sizeof( DICONSTANTFORCE );
        eff.lpvTypeSpecificParams = &cf;
        eff.dwStartDelay = 0;

        hr = _myJoysticks[0]->Acquire();
        if (FAILED(hr)) {
            AC_ERROR << "Acquire() failed.";
        }

        // Now set the new parameters and start the effect immediately.
        hr =  _myForceEffects[theJoystickIndex]->SetParameters( &eff, DIEP_DIRECTION |
                                         DIEP_TYPESPECIFICPARAMS |
                                         DIEP_START );

    }

    void
    DInputExtension::setAutoCentering(unsigned theJoystickIndex, bool theFlag) {
        if (!_myForceFeedBackFlag) {
            AC_INFO << "DInputExtension::setAutoCentering: calling a forcefeedback method on a non forcefeedback joystick, sorry!";
            return;
        }
        if (theJoystickIndex > _myJoysticks.size()-1) {
            AC_WARNING << "DInputExtension::setAutoCentering, index: '" << theJoystickIndex << "' is out of joysticks range: [0.." << _myJoysticks.size()-1 << "], sorry!";
            return;
        }

        HRESULT hr;

        hr = _myJoysticks[theJoystickIndex]->Acquire();
        if (FAILED(hr)) {
            AC_ERROR << "Acquire() failed.";
        }
        DIPROPDWORD dipdw2;
        dipdw2.diph.dwSize = sizeof( DIPROPDWORD );
        dipdw2.diph.dwHeaderSize = sizeof( DIPROPHEADER );
        dipdw2.diph.dwObj = 0;
        dipdw2.diph.dwHow = DIPH_DEVICE;
        dipdw2.dwData = theFlag ? DIPROPAUTOCENTER_ON:DIPROPAUTOCENTER_OFF;
        hr = _myJoysticks[theJoystickIndex]->SetProperty( DIPROP_AUTOCENTER, &dipdw2.diph );
        if( FAILED( hr) ) {
            AC_WARNING << "DInputExtension::setAutoCentering: disable autocentering failed";
        }

    }
    void
    DInputExtension::stopForce(unsigned theJoystickIndex) {
        if (!_myForceFeedBackFlag) {
            AC_INFO << "DInputExtension::setAutoCentering: calling a forcefeedback method on a non forcefeedback joystick, sorry!";
            return;
        }
        if (theJoystickIndex > _myJoysticks.size()-1) {
            AC_WARNING << "DInputExtension::setAutoCentering, index: '" << theJoystickIndex << "' is out of joysticks range: [0.." << _myJoysticks.size()-1 << "], sorry!";
            return;
        }
        if (_myForceEffects[theJoystickIndex]) {
            _myForceEffects[theJoystickIndex]->Stop();
        }

    }

    static JSBool
    ApplyForce(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;

        if (argc != 3) {
            JS_ReportError(cx, "DInputExtension::ApplyForce(): not enough arguments, must be 3");
            return JS_FALSE;
        }

        unsigned myJoystickIndex = 0;
        if (!convertFrom(cx, argv[0], myJoystickIndex)) {
            JS_ReportError(cx, "DInputExtension::ApplyForce(): argument #1 must be a unsigned int");
            return JS_FALSE;
        }
        float myForceX = 0.0;
        if (!convertFrom(cx, argv[1], myForceX)) {
            JS_ReportError(cx, "DInputExtension::ApplyForce(): argument #2 must be a float");
            return JS_FALSE;
        }
        float myForceY = 0.0;
        if (!convertFrom(cx, argv[2], myForceY)) {
            JS_ReportError(cx, "DInputExtension::ApplyForce(): argument #3 must be a float");
            return JS_FALSE;
        }

        asl::Ptr<DInputExtension> myNative(getNativeAs<DInputExtension>(cx, obj));
        if (myNative) {
            myNative->applyForce(myJoystickIndex, int(myForceX * DI_FFNOMINALMAX), int(myForceY * DI_FFNOMINALMAX));
        } else {
            assert(myNative);
        }
        return JS_TRUE;
    }

    static JSBool
    StopForce(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;

        if (argc != 1) {
            JS_ReportError(cx, "DInputExtension::StopForce(): not enough arguments, must be 1");
            return JS_FALSE;
        }

        unsigned myJoystickIndex = 0;
        if (!convertFrom(cx, argv[0], myJoystickIndex)) {
            JS_ReportError(cx, "DInputExtension::StopForce(): argument #1 must be a unsigned int");
            return JS_FALSE;
        }

        asl::Ptr<DInputExtension> myNative(getNativeAs<DInputExtension>(cx, obj));
        if (myNative) {
            myNative->stopForce(myJoystickIndex);
        } else {
            assert(myNative);
        }
        return JS_TRUE;
    }
    static JSBool
    SetAutoCentering(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;
        if (argc != 2) {
            JS_ReportError(cx, "DInputExtension::SetAutoCentering(): not enough arguments, must be 2");
            return JS_FALSE;
        }

        unsigned myJoystickIndex = 0;
        if (!convertFrom(cx, argv[0], myJoystickIndex)) {
            JS_ReportError(cx, "DInputExtension::SetAutoCentering(): argument #1 must be a unsigned int");
            return JS_FALSE;
        }

        bool myAutoCenteringMode = false;
        if (!convertFrom(cx, argv[1], myAutoCenteringMode)) {
            JS_ReportError(cx, "DInputExtension::SetAutoCentering(): argument #2 must be a boolean");
            return JS_FALSE;
        }

        asl::Ptr<DInputExtension> myNative(getNativeAs<DInputExtension>(cx, obj));
        if (myNative) {
            myNative->setAutoCentering(myJoystickIndex, myAutoCenteringMode);
        } else {
            assert(myNative);
        }
        return JS_TRUE;
    }

    JSFunctionSpec *
    DInputExtension::Functions() {
        static JSFunctionSpec myFunctions[] = {
            {"applyForce", ApplyForce, 0},
            {"stopForce", StopForce, 0},
            {"setAutoCentering", SetAutoCentering, 0},
            {0}
        };
        return myFunctions;
    }
} // end of namespace y60

extern "C"
EXPORT asl::PlugInBase* DInput_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new DInputExtension(myDLHandle);
}

