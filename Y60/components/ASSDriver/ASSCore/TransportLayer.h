//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef Y60_ASS_TRANSPORT_LAYER_INCLUDED
#define Y60_ASS_TRANSPORT_LAYER_INCLUDED

#include "ASSUtils.h"

#include <asl/base/Ptr.h>
#include <asl/base/PosixThread.h>
#include <asl/dom/Nodes.h>

#include <queue>

//#define TL_LATENCY_TEST
#ifdef TL_LATENCY_TEST
#   include <asl/SerialDevice.h>
#endif

namespace y60 {

enum DriverStateEnum {
    NOT_CONNECTED,
    SYNCHRONIZING,
    RUNNING,
    CONFIGURING,
    DriverStateEnum_MAX
};

enum CommandState {
    SEND_CONFIG_COMMANDS,
    WAIT_FOR_RESPONSE,
    WAIT_FOR_EXIT
};

enum CommandResponse {
    RESPONSE_NONE,
    RESPONSE_OK,
    RESPONSE_ERROR,
    RESPONSE_TIMEOUT
};

DEFINE_ENUM( DriverState, DriverStateEnum );

class ASSDriver;

class TransportLayer :  public asl::PosixThread {
    public:
        TransportLayer(const char * theTransportName, const dom::NodePtr & theSettings);
        virtual ~TransportLayer();

        void poll( /*RasterPtr theTargetRaster*/);
        //virtual void onUpdateSettings(dom::NodePtr theSettings) = 0;
        virtual bool settingsChanged(dom::NodePtr theSettings) = 0;


        void stopThread();

        void lockFrameQueue() {
            _myFrameQueueLock.lock();    
        }
        void unlockFrameQueue() {
            _myFrameQueueLock.unlock();    
        }
        std::queue<ASSEvent> & getFrameQueue() {
            return _myFrameQueue;
        }

        void queueCommand( const char * theCommand );

        DriverState getState() const;

        const asl::Vector2i & getGridSize() const;
        int getGridSpacing() const; // XXX
        int getFirmwareVersion() const;
        int getFirmwareStatus() const;
        int getControllerId() const;
        int getFirmwareMode() const;
        int getFramerate() const; // XXX
        int getLastFrameNumber() const;
        int getLastChecksum() const;


    protected:
        static void threadMain( asl::PosixThread & theThread );

        void dumpControllerStatus() const;
        CommandResponse getCommandResponse();
        void setState( DriverState theState );
        void sendCommand( const std::string & theCommand );
        void handleConfigurationCommand();

        unsigned readStatusToken( std::vector<unsigned char>::iterator & theIt, const char theToken );
        unsigned getBytesPerStatusLine();
        size_t getBytesPerFrame();
        void parseStatusLine(/*RasterPtr & theTargetRaster*/);
        void readSensorValues( /*RasterPtr theTargetRaster*/ );
        void synchronize();
        const char * getFirmwareModeName(unsigned theId) const;

        void addLineToChecksum(std::vector<unsigned char>::const_iterator theLineIt,
                               std::vector<unsigned char>::const_iterator theEnd );

        unsigned valuesPerLine() const;

        // establishConnection is not abstract, because it will get
        // called BEFORE the derived object is created
        virtual void establishConnection() {}

        virtual void readData() = 0;
        virtual void writeData(const char * theData, size_t theSize) = 0;
        virtual void closeConnection() = 0;

        std::string    _myTransportName;
        bool           _myRunningFlag;
        DriverState    _myState;
        std::vector<unsigned char> _myTmpBuffer;
        std::vector<unsigned char> _myReceiveBuffer;
        unsigned char * _myFrameBuffer;

        asl::Vector2i _myGridSize;
        int           _myGridSpacing;
        bool _myMagicTokenFlag;
        int _myExpectedLine;

        //dom::NodePtr _mySettings;
        //ASSDriver * _myDriver;

        std::queue<ASSEvent> _myFrameQueue;
        asl::ThreadLock _myFrameQueueLock;

        std::queue<std::string> _myCommandQueue; // TODO: use a queue?
        asl::ThreadLock _myCommandQueueLock;
        CommandState _myConfigureState;
        double       _myLastCommandTime;
        int          _myEventQueueSize;

        // Controller Status
        // TODO: expose to JavaScript
        int _myFirmwareVersion;
        int _myFirmwareStatus;
        int _myControllerId;
        int _myFirmwareMode;
        int _myFramerate;
        int _myFrameNo;
        int _myCurrentMultiplex;
        int _myMultiplexMax;
        asl::Unsigned16 _myChecksum;

        bool _myFirstFrameFlag;

        unsigned _mySyncLostCounter;
        unsigned _myDeviceLostCounter;
        unsigned _myReceivedFrames;    
        unsigned _myChecksumErrorCounter;
    private:
        //TransportLayer();

#ifdef TL_LATENCY_TEST
        asl::SerialDevice * _myLatencyTestPort;
#endif

};

typedef asl::Ptr<TransportLayer> TransportLayerPtr;

}

#endif // Y60_ASS_TRANSPORT_LAYER_INCLUDED
