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

#ifndef _Y60_INPUT_DINPUTEXTENSION_INCLUDED_
#define _Y60_INPUT_DINPUTEXTENSION_INCLUDED_

#include <asl/base/Ptr.h>
#include <asl/base/PlugInBase.h>

#include <y60/base/DataTypes.h>
#include <y60/input/Event.h>
#include <y60/input/IEventSource.h>

#include <y60/jsbase/JSScriptablePlugin.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>

namespace y60 {

    class DInputExtension :
        public asl::PlugInBase,
        public jslib::IScriptablePlugin,
        public y60::IEventSource
    {
        public:
            DInputExtension(asl::DLHandle theDLHandle);
            ~DInputExtension();
            virtual void init();
            virtual y60::EventPtrList poll();

            const char * ClassName() {
                static const char * myClassName = "DInput";
                return myClassName;
            }

            void setBufferSize(unsigned theSize) {
                _myBufferSize = theSize;
            }

            unsigned getBufferSize() const {
                return _myBufferSize;
            }

            // IScriptablePlugin
            virtual void onGetProperty(const std::string & thePropertyName,
                               y60::PropertyValue & theReturnValue) const {}
            virtual void onSetProperty(const std::string & thePropertyName,
                               const y60::PropertyValue & thePropertyValue) {}
            virtual void onUpdateSettings(dom::NodePtr theSettings) {}

            JSFunctionSpec * Functions();

            void applyForce(unsigned theJoystickIndex, int theXMagnitude, int theYMagnitude);
            void stopForce(unsigned theJoystickIndex);
            void setAutoCentering(unsigned theJoystickIndex, bool theFlag);

        private:
            void initJoysticks(DWORD theFlags, DWORD theCoopLevel);

            static int CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance,
                    void* This);

            void createForceEffect(LPDIRECTINPUTDEVICE8 & theJoystick, LPDIRECTINPUTEFFECT & theEffect);

            void addJoystick(const DIDEVICEINSTANCE* pdidInstance);
            HWND findSDLWindow();

            void printErrorState(const std::string & theCall, HRESULT hr);

            LPDIRECTINPUT8 _myDI;
            std::vector<LPDIRECTINPUTDEVICE8> _myJoysticks;
            unsigned                          _myBufferSize;
            std::vector<LPDIRECTINPUTEFFECT>  _myForceEffects;
            bool                              _myForceFeedBackFlag;

    };
}
#endif
