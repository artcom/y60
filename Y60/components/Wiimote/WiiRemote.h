//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef Y60_WII_REMOTE_INCLUDED
#define Y60_WII_REMOTE_INCLUDED

#include "WiiEvent.h"
#include "Utils.h"

#include <asl/Ptr.h>
#include <asl/PosixThread.h>
#include <asl/ThreadLock.h>

#include <y60/DataTypes.h>
#include <y60/IEventSource.h>

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
