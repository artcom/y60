//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_WIIMOTE_DRIVER_INCLUDED_
#define _Y60_WIIMOTE_DRIVER_INCLUDED_

#include <asl/Ptr.h>
#include <asl/PlugInBase.h>
#include <asl/Vector234.h>
#include <y60/IEventSource.h>

#include "Win32mote.h"
#include <vector>

namespace y60 {

     
    
    template<class IOHANDLE>
    class WiimoteDriver //:
        //public asl::PlugInBase
        //public y60::IEventSource
    { 
        public:
        
        WiimoteDriver() //asl::DLHandle theDLHandle) {
            {
                _myIOHandles = IOHANDLE::discover();
            }  
        
        ~WiimoteDriver() {
            for (int i = 0; i < _myIOHandles.size(); ++i) {
                setLEDs(i, 0,0,0,0);
                setRumble(i, false);
                _myIOHandles[i].close();
            }
        }
        //virtual void init();
               
        
        virtual y60::EventPtrList poll() {

            y60::EventPtrList myEvents;
            for (int i = 0; i < _myIOHandles.size(); ++i) {
                asl::Vector3i m = _myIOHandles[i].getLastMotionData();
                //cerr << (*_myIOHandles[i].createEvent(i, "motiondata", m)->getNode()) << endl;
                myEvents.push_back( _myIOHandles[i].createEvent(i, "motiondata", m) );
                    
                std::vector<Button> myButtonList = _myIOHandles[i].getButtons();
                for( unsigned button=0; button < myButtonList.size(); ++button) {
                    Button myButton = myButtonList[button];
                    if( myButton.hasChanged() ) {
                        myEvents.push_back( _myIOHandles[i].createEvent(i, "button", myButton.getName(), myButton.pressed()));
                    }
                }
                //cout << "wiimote index " << i << " motion " << (int)m.x << " "
                //     << (int)m.y << " " << (int)m.z << endl;

                
            }

            return myEvents;
        }


        
        void setLEDs(int theIndex, bool led1, bool led2, bool led3, bool led4) {
            char out = 0;
            
            if (led1) out |= 0x10;
            if (led2) out |= 0x20;
            if (led3) out |= 0x40;
            if (led4) out |= 0x80;
            
            char rpt[2] = { 0x11, out };
            _myIOHandles[theIndex].WriteOutputReport(rpt);
        }

        
        void setRumble(int theIndex, bool on) {
            char rpt[2] = { 0x13, on ? 0x01 : 0x00 };
            _myIOHandles[theIndex].WriteOutputReport(rpt);
        }

        void requestMotionData() {
            for (int i = 0; i < _myIOHandles.size(); ++i) {
                char calibration_report[] = { OUTPUT_READ_DATA, 0x00, 0x00, 0x00, 0x16, 0x00, 0x0F };
                _myIOHandles[i].WriteOutputReport(calibration_report);
            
                char set_motion_report[] = { OUTPUT_REPORT_SET_MOTION, 0x00, INPUT_REPORT_MOTION };
                _myIOHandles[i].WriteOutputReport(set_motion_report);
            }
        }

        unsigned getNumWiimotes() const { return _myIOHandles.size(); }
        std::vector<IOHANDLE> _myIOHandles;

    private:
     
    			
        
    };
    
    #ifdef WIN32
    typedef WiimoteDriver<Win32mote> Wiimote;
    
    #elif LINUX
    
    #endif

    typedef asl::Ptr<Wiimote, dom::ThreadingModel> WiimotePtr;
}
#endif // _Y60_WIIMOTE_DRIVER_INCLUDED_

