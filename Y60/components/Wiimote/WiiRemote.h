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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef Y60_WII_REMOTE_INCLUDED
#define Y60_WII_REMOTE_INCLUDED

#include "WiiEvent.h"
#include "Utils.h"

#include <asl/base/Ptr.h>
#include <asl/base/PosixThread.h>
#include <asl/base/ThreadLock.h>

#include <y60/base/DataTypes.h>
#include <y60/input/IEventSource.h>

#include <queue>

namespace y60 {

// Wii infos:
// http://www.wiibrew.org/
// http://www.wiili.org

// TODO: rename and refactor mercilessly
const static char OUT_SET_LEDS           = 0x11;
const static char OUT_DATA_REPORT_MODE   = 0x12;
const static char OUT_IR_CMAERA_ENABLE   = 0x13;
const static char OUT_STATUS_REQUEST     = 0x15;
// ... more ...
const static char OUT_IR_CMAERA_ENABLE_2 = 0x1a;

const static char OUT_WRITE_DATA         = 0x16;
const static char OUT_READ_DATA          = 0x17;

const static char INPUT_REPORT_BUTTONS      = 0x30; // just the buttons
const static char INPUT_REPORT_MOTION       = 0x31; // buttons and accelerometer
const static char INPUT_REPORT_IR           = 0x33; // buttons, accelerometer and IR data

const static char IR_CAMERA_ENABLE_BIT      = 0x04; // XXX not an output report ... this is the payload for IR camera enable (one and two)

const static char INPUT_READ_DATA           = 0x21;
const static char INPUT_WRITE_DATA          = 0x22;

const static char INPUT_REPORT_STATUS  = 0x20;

static const int WIIMOTE_VENDOR_ID = 0x057E;
static const int WIIMOTE_PRODUCT_ID = 0x0306;

static const int RECV_BUFFER_SIZE( 23 );
static const int SEND_BUFFER_SIZE( 23 );

class WiiRemote :
        public asl::PosixThread
{
    public:
        WiiRemote(PosixThread::WorkFunc theThreadFunction );
        virtual ~WiiRemote();

        asl::Vector3f getLastMotionData() const { return _myLastMotion; }

        Button getButton(std::string label);
        std::vector<Button> getButtons() const { return _myButtons; }
        
        
        void setReportMode( WiiReportMode theReportMode );
        WiiReportMode getReportMode() const;
        
        const std::string & getControllerID() const { return _myControllerId; }

        void setRumble( bool theFlag);
        bool isRumbling() const;

        void setLED(int theIndex, bool theFlag);
        void setLEDs(bool theLED0, bool theLED1, bool theLED2, bool theLED3 );
        bool isLedOn(int i) const;

        void requestStatusReport();

        void setContinousReportFlag( bool theFlag );
        bool getContinousReportFlag() const;

        void writeMemoryOrRegister(asl::Unsigned32 theAddress, unsigned char * theData,
                                   unsigned theNumBytes, bool theWriteRegisterFlag);

        // TODO: implement memory/ register read

        bool isConnected() const;
        void disconnect();

        void pollEvents( y60::EventPtrList & theEventList, std::vector<std::string> & theLostWiiIds );

    protected:
        void setId( const char * theId);
        virtual void closeDevice() = 0;
        void startThread();
        void stopThread();

        void setLEDState();
        
        void handleButtonEvents( const unsigned char * theInputReport );
        void handleMotionEvents( const unsigned char * theInputReport );
        void handleIREvents( const unsigned char * theInputReport );

        void createEvent( const std::string & theButtonName, bool thePressedState);
        void createEvent( asl::Vector3f & theMotionData);
        void createEvent( const asl::Vector2i theIRData[4],
                          const asl::Vector2f & theNormalizedScreenCoordinates, const float & theAngle );
    
        void requestButtonData();
        void requestInfraredData();
        void requestMotionData();

        void dispatchInputReport(const unsigned char * theBuffer, int theOffset);
        void addContinousReportBit( unsigned char * theOutputReport );
        virtual void send(unsigned char theOutputReport[], unsigned theNumBytes) = 0;
        void sendOutputReport(unsigned char theOutputReport[], unsigned theNumBytes);

        bool getListeningFlag() const;

        std::vector<Button> setButtons(int code);

        std::vector<Button> _myButtons;

        asl::Vector3f       _myLastMotion;
        asl::Vector3f       _myZeroPoint;
        asl::Vector3f       _myOneGPoint;
        
        bool            _myInputListening;

        int _myLeftPoint;
        asl::Vector3f _myLowPassedOrientation;
        int _myOrientation;

        std::string             _myControllerId;

        bool _myRumbleFlag;
        bool _myLEDState[4];
        bool _myContinousReportFlag;

        bool _isConnected;

        WiiReportMode _myReportMode;

        asl::ThreadLock      _myLock;
        std::queue<WiiEvent> _myEventQueue;

        dom::NodePtr                    _myEventSchema;
        asl::Ptr<dom::ValueFactory>     _myValueFactory;
    private:
        WiiRemote();
};

typedef asl::Ptr<WiiRemote> WiiRemotePtr;

}

#endif // Y60_WII_REMOTE_INCLUDED
