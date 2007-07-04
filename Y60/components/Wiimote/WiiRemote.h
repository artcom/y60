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

#include <queue>

namespace y60 {

// Wii infos:
// http://www.wiibrew.org/
// http://www.wiili.org

// TODO: rename and refactor mercilessly
const static char OUT_SET_LEDS           = 0x11;
const static char OUT_DATA_REPORT_MODE   = 0x12;
const static char OUT_IR_CMAERA_ENABLE   = 0x13;
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
        WiiRemote(PosixThread::WorkFunc theThreadFunction, unsigned theId);
        virtual ~WiiRemote();

        void setEventQueue( asl::Ptr<std::queue<WiiEvent> > theQueue,
                            asl::Ptr<asl::ThreadLock> theLock);


        asl::Vector3f getLastMotionData() const { return _myLastMotion; }

        Button getButton(std::string label);
        std::vector<Button> getButtons() const { return _myButtons; }
        
        void startThread();
        void stopThread();
        
        static void printStatus(const unsigned char * theBuffer);
        int getControllerID() const { return _myControllerId; }

        virtual std::string getDeviceName() const = 0;
        virtual void send(unsigned char theOutputReport[], unsigned theNumBytes) = 0;
        void sendOutputReport(unsigned char theOutputReport[], unsigned theNumBytes);


        void setRumble( bool theFlag);
        bool isRumbling() const;

        void setLED(int theIndex, bool theFlag);
        void setLEDs(bool theLED0, bool theLED1, bool theLED2, bool theLED3 );
        bool isLedOn(int i) const;

        void setContinousReportFlag( bool theFlag );
        bool getContinousReportFlag() const;

        void writeMemoryOrRegister(uint32_t theAddress, unsigned char * theData,
                                   unsigned theNumBytes, bool theWriteRegisterFlag);

        // TODO: implement memory/ register read

    protected:
        virtual void closeDevice() = 0;

        void setLEDState();
        void addContinousReportBit( unsigned char * theOutputReport );

        void dispatchInputReport(const unsigned char * theBuffer, int theOffset);
        
        void handleButtonEvents( const unsigned char * theInputReport );
        void handleMotionEvents( const unsigned char * theInputReport );
        void handleIREvents( const unsigned char * theInputReport );

        void createEvent( int theID, const std::string & theButtonName, bool thePressedState);
        void createEvent( int theID, asl::Vector3f & theMotionData);
        void createEvent( int theID, const asl::Vector2i theIRData[4],
                          const asl::Vector2f & theNormalizedScreenCoordinates, const float & theAngle );
    
        bool getListeningFlag() const;
        asl::Ptr< std::queue<WiiEvent> > _myEventQueue;
        asl::Ptr< asl::ThreadLock > _myLock;

        std::vector<Button> setButtons(int code);
        std::vector<Button> _myButtons;

        asl::Vector3f       _myLastMotion;
        asl::Vector3f       _myZeroPoint;
        asl::Vector3f       _myOneGPoint;
        
        bool            _myInputListening;

        int _myLeftPoint;
        asl::Vector3f _myLowPassedOrientation;
        int _myOrientation;

        int             _myControllerId;

        bool _myRumbleFlag;
        bool _myLEDState[4];
        bool _myContinousReportFlag;

    private:
        WiiRemote();
};

typedef asl::Ptr<WiiRemote> WiiRemotePtr;

}

#endif // Y60_WII_REMOTE_INCLUDED
