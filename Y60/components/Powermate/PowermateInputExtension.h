//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_POWERMATE_INPUTEXTENSION_INCLUDED_
#define _Y60_POWERMATE_INPUTEXTENSION_INCLUDED_

#include <asl/Ptr.h>
#include <asl/PlugInBase.h>

#include <y60/Event.h>
#include <y60/IEventSource.h>

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
