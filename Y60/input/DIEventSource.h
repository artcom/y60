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
//   $RCSfile: DIEventSource.h,v $
//   $Author: janbo $
//   $Revision: 1.4 $
//   $Date: 2004/10/22 10:48:01 $
//
//
//=============================================================================
#ifndef _Y60_INPUT_DIEVENTSOURCE_INCLUDED_
#define _Y60_INPUT_DIEVENTSOURCE_INCLUDED_

#ifdef WIN32
#include "Event.h"
#include "IEventSource.h"

#include <asl/Ptr.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>

namespace y60 {

    class DIEventSource : public IEventSource {
        public:
            DIEventSource();
			~DIEventSource();
            virtual void init();
            virtual std::vector<EventPtr> poll();

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
}
#endif
#endif
