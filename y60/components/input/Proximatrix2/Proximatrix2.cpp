
#include <asl/base/Auto.h>
#include <asl/base/ThreadLock.h>

#include "proximatrix2toucheventxsd.h"

#include <asl/dom/Nodes.h>
#include <asl/base/Time.h>

#include <asl/net/UDPConnection.h>
#include <asl/net/UDPSocket.h>
#include <asl/net/Socket.h>

#include <y60/base/DataTypes.h>
#include <y60/base/SettingsParser.h>
#include <y60/input/IEventSource.h>
#include <y60/input/GenericEvent.h>
#include <y60/input/GenericEventSourceFilter.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>
#include <y60/jsbase/JSNode.h>

#define DB(x) //x

#define UDP_HDR 8
#define PROXY_HDR 14*4+32
#define TOUCHSIZE 64
//#define TOUCHSIZE 4*5*3 TODO: there's something wrong
#define FRAMESIZE 4256

namespace y60 {

using namespace asl;
using namespace dom;
using namespace inet;
using namespace y60;

struct touch_info {
    int x;
    int y;
    int pressure;
    int id;
    double lastAppeared;
};

class Proximatrix2 : public PlugInBase,
                   public y60::IEventSource,
                   public jslib::IScriptablePlugin,
                   public GenericEventSourceFilter
{

private:
    asl::Ptr<dom::ValueFactory> _myEventValueFactory;
    asl::Vector2f _myTouchArea;
    inet::UDPSocket *_mySocket;
    unsigned int _myPort;
    NodePtr _myEventSchemaDocument;
    std::map<int, touch_info> _myIdMap;
    unsigned long long _myTouchUpTimeout;

public:
    Proximatrix2(DLHandle myDLHandle)
        : PlugInBase(myDLHandle),
          GenericEventSourceFilter(),
          _myEventValueFactory(new ValueFactory()),
          _myTouchArea(-1.0, -1.0),
          _myEventSchemaDocument(new Document(y60::ourproximatrix2toucheventxsd)),          
          _myPort(10000),
          _myIdMap(),
          _myTouchUpTimeout(100)
    {
        registerStandardTypes(*_myEventValueFactory);
        registerSomTypes(*_myEventValueFactory);
    }

    ~Proximatrix2()
    { }

// IScriptablePlugin
    const char * ClassName() {
        return "Proximatrix2";
    }
   
    NodePtr 
    getEventSchema() {
        return _myEventSchemaDocument;
    }

    virtual void onUpdateSettings(dom::NodePtr theSettings) {
        // dom::NodePtr mySettings = getTUIOSettings(theSettings);
        // int myFilterFlag = 0;
        // getConfigSetting( mySettings, "FilterMultipleMovePerCursor", myFilterFlag, 0);
        // _myFilterMultipleMovePerCursorFlag = (myFilterFlag == 1 ? true : false);
        // getConfigSetting( mySettings, "MaxCursorPositionsForAverage", _myMaxCursorPositionsInHistory, static_cast<unsigned int>(1));
        // int myMaxCursorCount = -1;
        // getConfigSetting( mySettings, "MaxCursorCount", myMaxCursorCount, -1);
        // setMaxCursorCount(myMaxCursorCount);
        // getConfigSetting( mySettings, "TouchArea", _myTouchArea, asl::Vector2f(-1.0,-1.0));
    }

// IEventSource

    void init() {
        AC_PRINT << "init";
        try {
            _mySocket = new inet::UDPSocket(INADDR_ANY, _myPort);

            // if(_mySocket->isValid()) {
            //     char trashBuffer[1024];
            //     size_t bytesReceived = sizeof(trashBuffer);
            //     _mySocket->receiveFrom(0, 0, trashBuffer, bytesReceived);
            // }
        } catch(SocketException &e) {
            AC_WARNING << "Error creating the UDP socket: " << e.where();
            _mySocket = 0;
        }
    }
   
    template<typename T>
    T getValue(unsigned char * input) {
        T value = 0;
        int multiplier = 1;
        for (size_t i=0; i<sizeof(T); i++) {
            value += ( (int)input[i] * multiplier);     
            multiplier *= 256;
        }

        return value;
    }

    GenericEventPtr createEvent(touch_info & info) {
        GenericEventPtr myEvent(new GenericEvent("onTouch", _myEventSchemaDocument, _myEventValueFactory));
        NodePtr myNode = myEvent->getNode();

        asl::Vector2f position(static_cast<float>(info.x), static_cast<float>(info.y));
        myNode->appendAttribute<int>("id", info.id);
        myNode->appendAttribute<Vector2f>("position", Vector2f(info.x, info.y));
        myNode->appendAttribute<Vector2f>("raw_position", Vector2f(info.x, info.y));
        myNode->appendAttribute<int>("pressure", info.pressure);
        
        return myEvent;
    }

    EventPtrList poll() {
        AC_TRACE << "Listening for Proximatrix2 messages on UDP port " << _myPort;
         
        unsigned int buffersize = UDP_HDR + PROXY_HDR + TOUCHSIZE + FRAMESIZE + 6600;
        unsigned char receiveBuffer[buffersize];

        EventPtrList events;
        
        if (_mySocket->isValid()) {
            while (_mySocket->peek(buffersize)) {
                
                size_t bytesReceived = sizeof(receiveBuffer);
                _mySocket->receiveFrom(0, 0, receiveBuffer, bytesReceived);
                
                // todo: put framedata into an image
            
                // print touches
                int TOUCH_SIZE = 16;
                int TOUCH_X = 0;
                int TOUCH_Y = 4;
                int TOUCH_PRESSURE = 8;
                int TOUCH_ID = 12;

                unsigned char * touches = receiveBuffer + UDP_HDR + PROXY_HDR + 4;

                for (unsigned int touchNo=0; touchNo<5; touchNo++) {
                    unsigned char * touch = touches + (touchNo * TOUCH_SIZE);
                    touch_info ti;
                    ti.x            = getValue<int>(touch+TOUCH_X);
                    ti.y            = getValue<int>(touch+TOUCH_Y);
                    ti.pressure     = getValue<int>(touch+TOUCH_PRESSURE);
                    ti.id           = getValue<int>(touch+TOUCH_ID);
                    ti.lastAppeared = asl::Time().millis();
                        
                    if (ti.id != -1) { 
                        AC_DEBUG << "   Touch: " << ti.id << " x: " << ti.x << " y: " << ti.y << " pressure: " << ti.pressure;
                        GenericEventPtr myEvent = createEvent(ti);
                        
                        if (_myIdMap.find(ti.id) == _myIdMap.end()) { 
                            myEvent->getNode()->appendAttribute<DOMString>("type", "down");
                        } else {
                            myEvent->getNode()->appendAttribute<DOMString>("type", "move");
                        }

                        events.push_back(myEvent);
                        _myIdMap[ti.id] = ti;
                    }
                }
            }
        }
       
        // trigger up events
        std::map<int, touch_info>::iterator it;
        for (it = _myIdMap.begin(); it != _myIdMap.end(); ++it) {
            int id = it->first;

            if ( asl::Time().millis() - it->second.lastAppeared > _myTouchUpTimeout) {
                GenericEventPtr myEvent = createEvent(it->second);
                myEvent->getNode()->appendAttribute("type", "up");
                events.push_back(myEvent);
                
                _myIdMap.erase(it);
            }
        }

        return events;
    }

// reused stuff

protected:

};

}

extern "C"
EXPORT asl::PlugInBase* Proximatrix2_instantiatePlugIn(asl::DLHandle myDLHandle) {
    AC_TRACE << "Instantiating Proxymatrix2 plugin";
    return new y60::Proximatrix2(myDLHandle);
}
