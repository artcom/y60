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

#ifndef _Y60_POWERMATE_INPUTEXTENSION_INCLUDED_
#define _Y60_POWERMATE_INPUTEXTENSION_INCLUDED_

#include <asl/base/Ptr.h>
#include <asl/base/PlugInBase.h>

#include <y60/input/Event.h>
#include <y60/input/IEventSource.h>

#include <vector>
#include <linux/input.h>

#define NUM_EVENT_DEVICES 16
#define BUFFER_SIZE       32

namespace y60 {

    class PowermateInputExtension :
        public asl::PlugInBase,
        public y60::IEventSource
    {
        public:
            PowermateInputExtension(asl::DLHandle theDLHandle);
		        ~PowermateInputExtension();
            virtual void init();
            virtual y60::EventPtrList poll();

            void setLED(float thePercent, int theFileDescriptor);
            void setLED(unsigned char theLevel, int theFileDescriptor);
            void pulseLED(int theStaticBrightness, int thePulseSpeed, int thePulseTable, int thePulseAsleep,
                          int thePulseAwake, int theFileDescriptor);
            void turnLEDOff(int theFileDescriptor);

        private:

            void findPowermates();
            int openPowermate(const char *dev);
            void processEvent(struct input_event *ev, int theID, EventPtrList & theEventList);
            std::vector<int> _myFileDescriptorList;
            unsigned char _myLEDLevel;
    };
    typedef asl::Ptr<PowermateInputExtension, dom::ThreadingModel> PowermateInputExtensionPtr;

}
#endif // _Y60_POWERMATE_INPUT_INCLUDED_
