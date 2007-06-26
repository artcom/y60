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
#include <asl/ThreadLock.h>
#include <y60/IEventSource.h>
#include <y60/IScriptablePlugin.h>

#include "Win32mote.h"
#include <vector>
#include <queue>

namespace y60 {

     
    template<class IOHANDLE>
    class WiimoteDriver :
        public asl::PlugInBase,
        public jslib::IScriptablePlugin,
        public y60::IEventSource
    { 
        public:
        
        WiimoteDriver(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle),
                                                   y60::IEventSource(),
                                                   _myEventQueue( new std::queue<y60::GenericEventPtr>() ),
                                                   _myLock( new asl::ThreadLock() )
            {
                _myIOHandles = IOHANDLE::discover();
                for (unsigned i = 0; i < _myIOHandles.size(); ++i) {
                    _myIOHandles[i]->setEventQueue( _myEventQueue, _myLock );
                    _myIOHandles[i]->startListening();

                    // XXX set LEDs to controller id (binary) ;-)
                    setLEDs(i, i+1 & (1<<0) ,i+1 & (1<<1),i+1 & (1<<2),i+1 & (1<<3));
                }
                //requestMotionData();
                //requestButtonData();
                requestInfraredData();
            }  
        
        ~WiimoteDriver() {
            for (int i = 0; i < _myIOHandles.size(); ++i) {
                setLEDs(i, 0,0,0,0);
                setRumble(i, false);
            }
        }
               
        


        virtual y60::EventPtrList poll() {

            y60::EventPtrList myEvents;

            _myLock->lock();

            while ( ! _myEventQueue->empty() ) {
                myEvents.push_back( _myEventQueue->front() );
                _myEventQueue->pop();
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
            _myIOHandles[theIndex]->WriteOutputReport(rpt);
        }

        
        void setRumble(int theIndex, bool on) {
            char rpt[2] = { 0x13, on ? 0x01 : 0x00 };
            _myIOHandles[theIndex]->WriteOutputReport(rpt);
        }

        void requestButtonData() {
            for (int i = 0; i < _myIOHandles.size(); ++i) {
                char set_motion_report[] = { OUTPUT_REPORT_SET , 0x00, INPUT_REPORT_BUTTONS };
                _myIOHandles[i]->WriteOutputReport(set_motion_report);
            }
        
        }

        void requestInfraredData() {
            for (int i = 0; i < _myIOHandles.size(); ++i) {
                char set_ir_report[] = {  0x13, 0x04 };
                char set_ir_report2[] = { 0x1a, 0x04 };
                _myIOHandles[i]->WriteOutputReport(set_ir_report);
                asl::msleep(10);
                _myIOHandles[i]->WriteOutputReport(set_ir_report2);
                asl::msleep(10);

                // Write 0x08 to register 0xb00030
                //16 MM FF FF FF SS DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD
                char write_register[] = {0x16, 0x04,
                                         0xb0, 0x00, 0x30, // address
                                         0x01,             // size
                                         0x01 };           // data
                _myIOHandles[i]->WriteOutputReport( write_register );
                asl::msleep(10);
                //Write Sensitivity Block 1 to registers at 0xb00000
                //Write Sensitivity Block 2 to registers at 0xb0001a 
                char sensivity_setting_block1[] = {0x16, 0x04,
                                                   0xb0, 0x00, 0x00, // address
                                                   0x09,             // size
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0xC0 }; // data
                _myIOHandles[i]->WriteOutputReport(sensivity_setting_block1);
                asl::msleep(10);
                
                char sensivity_setting_block2[] = {0x16, 0x04,
                                                   0xb0, 0x00, 0x1a,  // address
                                                   0x02,               // size
                                                   0x40, 0x00 };      // data
                _myIOHandles[i]->WriteOutputReport(sensivity_setting_block2);
                asl::msleep(10);
                
                 char write_register2[] = {0x16, 0x04,
                                         0xb0, 0x00, 0x30, // address
                                         0x01,             // size
                                         0x08 };           // data
                _myIOHandles[i]->WriteOutputReport( write_register2 );
                asl::msleep(10);
                char ir_mode_setting[] = { 0x16, 0x04,
                                            0xb0, 0x00, 0x33, // adress
                                            0x01,             // size
                                            0x03};            // data (extended mode ... for now)
                _myIOHandles[i]->WriteOutputReport(ir_mode_setting);


                
                char set_motion_report[] = { OUTPUT_REPORT_SET, 0x00, 0x33 };
                _myIOHandles[i]->WriteOutputReport(set_motion_report);
            }
        
        }
        
        void requestMotionData() {
            for (int i = 0; i < _myIOHandles.size(); ++i) {
                char calibration_report[] = { OUTPUT_READ_DATA, 0x00, 0x00, 0x00, 0x16, 0x00, 0x0F };
                _myIOHandles[i]->WriteOutputReport(calibration_report);
            
                char set_motion_report[] = { OUTPUT_REPORT_SET, 0x00, INPUT_REPORT_MOTION };
                _myIOHandles[i]->WriteOutputReport(set_motion_report);
            }
        }

        unsigned getNumWiimotes() const { return _myIOHandles.size(); }
        std::vector<asl::Ptr<IOHANDLE, dom::ThreadingModel> > _myIOHandles;

        void onSetProperty(const std::string & thePropertyName,
                           const PropertyValue & thePropertyValue)
        {
        }
        void onGetProperty(const std::string & thePropertyName,
                           PropertyValue & theReturnValue) const
        {
        }
        void onUpdateSettings(dom::NodePtr theSettings) {
        }

        JSFunctionSpec * Functions() {
            static JSFunctionSpec myFunctions[] = {
                {"setRumble", SetRumble, 2},
                {"setLEDs", SetLEDs, 5},
                {0}
            };
            return myFunctions;
        }

        static JSBool SetRumble(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
        static JSBool SetLEDs(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    
        const char * ClassName() {
            static const char * myClassName = "Wiimote";
            return myClassName;
        }
    private:
     
        asl::Ptr<std::queue<y60::GenericEventPtr> > _myEventQueue;
        asl::Ptr<asl::ThreadLock> _myLock;
        
    };
    
#ifdef WIN32
    typedef WiimoteDriver<Win32mote> Wiimote;

#elif LINUX

#endif

    typedef asl::Ptr<Wiimote, dom::ThreadingModel> WiimotePtr;
}
#endif // _Y60_WIIMOTE_DRIVER_INCLUDED_

