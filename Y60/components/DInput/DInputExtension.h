//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_INPUT_DINPUTEXTENSION_INCLUDED_
#define _Y60_INPUT_DINPUTEXTENSION_INCLUDED_

#include <asl/base/Ptr.h>
#include <asl/PluginBase.h>

#include <y60/input/Event.h>
#include <y60/input/IEventSource.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>

class DInputExtension :
    public asl::PlugInBase,
    public y60::IEventSource
{
    public:
        DInputExtension(asl::DLHandle theDLHandle);
		~DInputExtension();
        virtual void init();
        virtual y60::EventPtrList poll();

        void setBufferSize(unsigned theSize) {
            _myBufferSize = theSize;
        }

        unsigned getBufferSize() const {
            return _myBufferSize;
        }
    private:			
		static int CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance,
                void* This);

        void addJoystick(const DIDEVICEINSTANCE* pdidInstance);
        HWND findSDLWindow();

        void printErrorState(const std::string & theCall, HRESULT hr);

        LPDIRECTINPUT8 _myDI;
        std::vector<LPDIRECTINPUTDEVICE8> _myJoysticks;
        unsigned                          _myBufferSize;

};
#endif
