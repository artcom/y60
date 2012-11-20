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

#ifndef Y60_ASS_TRANSPORT_LAYER_INCLUDED
#define Y60_ASS_TRANSPORT_LAYER_INCLUDED

#include "y60_asscore_settings.h"

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

DEFINE_ENUM(DriverState, DriverStateEnum, Y60_ASSCORE_DECL);

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


class TransportLayer :  public asl::PosixThread {
    protected:

        // protected constructor (abstract class)
        TransportLayer(const char * theTransportName, const dom::NodePtr & theSettings);

    public:

        virtual ~TransportLayer();

        virtual bool settingsChanged(dom::NodePtr theSettings) = 0;

        void stopThread();

        // interface for retrieving frames and events
        void getEvents(std::vector<ASSEvent>& theDestination) {
            theDestination.clear();
            _myFrameQueueLock.lock();
            while(!_myFrameQueue.empty()) {
                theDestination.push_back(_myFrameQueue.front());
                _myFrameQueue.pop();
            }
            _myFrameQueueLock.unlock();
        }

        // command submission by clients
        void queueCommand( const char * theCommand );

        // exposed state
        DriverState getState() const;
        int getLastFrameNumber() const;
        int getLastChecksum() const;

        // controller configuration
        const asl::Vector2i & getGridSize() const;
        int getGridSpacing() const;
        int getFirmwareVersion() const;
        int getFirmwareStatus() const;
        int getControllerId() const;
        int getFirmwareMode() const;
        int getFramerate() const;

    protected:

        // interface to be implemented by concrete subclasses
        virtual void establishConnection() = 0;
        virtual void readData() = 0;
        virtual void writeData(const char* theData, size_t theSize) = 0;
        virtual void closeConnection() = 0;

        // interface provided for conrete subclasses
        void syncLost();
        void connectionLost();
        void setState(DriverState theState); // XXX: should be private

        void receivedData(char* theData, size_t theSize);

    private:

        // thread main function
        static void threadMain( asl::PosixThread & theThread );

        // command submit/response
        CommandResponse getCommandResponse();
        void sendCommand(const std::string & theCommand);

        // driving methods
        void poll();
        void synchronize();
        void parseStatusLine();
        void readSensorValues();
        void handleConfigurationCommand();

        // informational dumps
        void dumpConfiguration() const;
        void dumpStatistics() const;

        // parsing helpers
        void addLineToChecksum(std::deque<unsigned char>::const_iterator theLineIt,
                               std::deque<unsigned char>::const_iterator theEnd);

        size_t getBytesPerFrame();
        unsigned valuesPerLine() const;

        unsigned readStatusToken(std::deque<unsigned char>::iterator & theIt, const char theToken);
        unsigned getBytesPerStatusLine();

         // XXX: enum conversion
        const char * getFirmwareModeName(unsigned theId) const;

        // transport configuration
protected: // XXX: should not be
        std::string    _myTransportName;
private:

        // transport state
        DriverState     _myState;
        CommandState    _myConfigureState;
        double          _myLastCommandTime;
        asl::Time       _myLastDataTime;
        unsigned char*  _myFrameBuffer;
        asl::Unsigned16 _myChecksum;
        bool            _myFirstFrameFlag;

        bool            _myMagicTokenFlag;
        int             _myExpectedLine;

        // incoming data buffer
        std::deque<unsigned char> _myTmpBuffer;

        // queue transporting data from transport to driver
        std::queue<ASSEvent> _myFrameQueue;
        asl::ThreadLock      _myFrameQueueLock;
        std::queue<ASSEvent>::size_type _myEventQueueSize; // XXX: why?

        // queue transporting commands from driver to transport
        std::queue<std::string> _myCommandQueue;
        asl::ThreadLock         _myCommandQueueLock;

        // controller configuration
        int _myFirmwareVersion;
        int _myFirmwareStatus;
        asl::Vector2i _myGridSize;
        int _myGridSpacing;
        int _myControllerId;
        int _myFirmwareMode;
        int _myFramerate;
        int _myFrameNo;
        int _myCurrentMultiplex;
        int _myMultiplexMax;

        // communication statistics
        unsigned _mySyncLostCounter;
        unsigned _myDeviceLostCounter;
        unsigned _myReceivedFrames;
        unsigned _myChecksumErrorCounter;

        // latency test support
#ifdef TL_LATENCY_TEST
        asl::SerialDevice * _myLatencyTestPort;
#endif

};

typedef asl::Ptr<TransportLayer> TransportLayerPtr;

}

#endif // Y60_ASS_TRANSPORT_LAYER_INCLUDED
