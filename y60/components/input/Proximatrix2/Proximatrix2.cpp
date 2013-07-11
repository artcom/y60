
#include <asl/base/Auto.h>
#include <asl/base/ThreadLock.h>

#include "proximatrix2toucheventxsd.h"

#include <asl/dom/Nodes.h>
#include <asl/base/Time.h>
#include <asl/math/numeric_functions.h>
#include <asl/math/linearAlgebra.h>

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
    unsigned long long lastAppeared;
};

typedef asl::Ptr<inet::UDPSocket> UDPSocketPtr;

class Proximatrix2 : public PlugInBase,
                   public y60::IEventSource,
                   public jslib::IScriptablePlugin,
                   public GenericEventSourceFilter
{

private:
    asl::Ptr<dom::ValueFactory> _myEventValueFactory;
    asl::Vector2f _myTouchArea;
    UDPSocketPtr _mySocket;
    NodePtr _myEventSchemaDocument;
    std::map<int, touch_info> _myIdMap;
    unsigned long long _myTouchUpTimeout;
    std::vector<asl::Vector2f> _myReferencePoints;
    std::vector<asl::Vector2f> _myCalibrationPoints;
    unsigned short _myPort; 

public:
    Proximatrix2(DLHandle myDLHandle)
        : PlugInBase(myDLHandle),
          GenericEventSourceFilter(),
          _myEventValueFactory(new ValueFactory()),
          _myTouchArea(-1.0, -1.0),
          _myEventSchemaDocument(new Document(y60::ourproximatrix2toucheventxsd)),          
          _myIdMap(),
          _myTouchUpTimeout(100),
          _myCalibrationPoints(),
          _myReferencePoints(),
          _myPort(10000)
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
        AC_PRINT << "update settings : " << theSettings;
        _myCalibrationPoints.clear();
        _myReferencePoints.clear();

        int myFilterFlag = 0;
        asl::Vector2f screen0, screen1, screen2;
        getConfigSetting( theSettings, "Screen0", screen0, asl::Vector2f(0.0f, 0.0f));
        getConfigSetting( theSettings, "Screen1", screen1, asl::Vector2f(1.0f, 0.0f));
        getConfigSetting( theSettings, "Screen2", screen2, asl::Vector2f(0.0f, 1.0f));
        
        _myReferencePoints.push_back(screen0); 
        _myReferencePoints.push_back(screen1); 
        _myReferencePoints.push_back(screen2); 

        asl::Vector2f touch0, touch1, touch2;
        getConfigSetting( theSettings, "Touch0", touch0, asl::Vector2f(0.0f, 0.0f));
        getConfigSetting( theSettings, "Touch1", touch1, asl::Vector2f(1.0f, 0.0f));
        getConfigSetting( theSettings, "Touch2", touch2, asl::Vector2f(0.0f, 1.0f));
        
        _myCalibrationPoints.push_back(touch0); 
        _myCalibrationPoints.push_back(touch1); 
        _myCalibrationPoints.push_back(touch2); 
    
        // network receiver port
        int myPort;
        getConfigSetting( theSettings, "Port", myPort, 10000);
        _myPort = static_cast<unsigned short>(myPort);
        ensureSocket();
    }

// IEventSource

    void ensureSocket() {
        if (!_mySocket || _mySocket->getLocalPort() != _myPort) {
            
            try {
                _mySocket = UDPSocketPtr(new inet::UDPSocket(INADDR_ANY, _myPort));

                // if(_mySocket->isValid()) {
                //     char trashBuffer[1024];
                //     size_t bytesReceived = sizeof(trashBuffer);
                //     _mySocket->receiveFrom(0, 0, trashBuffer, bytesReceived);
                // }
            } catch(SocketException &e) {
                AC_WARNING << "Error creating the UDP socket: " << e.where();
                _mySocket = UDPSocketPtr(0);
            }
        }
    }

    void init() {
        ensureSocket();
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

    asl::Vector2f projection(asl::Vector2f v1, asl::Vector2f v2) {
        asl::Vector2f v3 = normalized(v2);
        return dot(v1, v3) * v3;    
    }

    asl::Vector2f transformPosition(float x, float y) {
        if (_myCalibrationPoints.size() < 3 || _myReferencePoints.size() < 3) {
            AC_ERROR << "calibration incomplete";
            return asl::Vector2f(0.0f, 0.0f);
        }
        
        asl::Vector2f result;
        asl::Vector2f input = asl::Vector2f(x,y);

        asl::Vector2f base1 = _myReferencePoints[1] - _myReferencePoints[0];
        asl::Vector2f base2 = _myReferencePoints[2] - _myReferencePoints[0];
    
        asl::Vector2f cal1 = _myCalibrationPoints[1] - _myCalibrationPoints[0];
        asl::Vector2f cal2 = _myCalibrationPoints[2] - _myCalibrationPoints[0];
   
        asl::Vector2f p1 = projection(input - _myCalibrationPoints[0], cal1);  
        asl::Vector2f p2 = projection(input - _myCalibrationPoints[0], cal2);  
   
        p1 = p1 / magnitude(cal1);
        p2 = p2 / magnitude(cal2);

        float l1 = magnitude(p1);
        float l2 = magnitude(p2);
            
        result = (l1 * base1 + l2 * base2) + _myReferencePoints[0]; 

        return result;
    }

    GenericEventPtr createEvent(touch_info & info) {
        GenericEventPtr myEvent(new GenericEvent("onProximatrix2Event", _myEventSchemaDocument, _myEventValueFactory));
        NodePtr myNode = myEvent->getNode();

        myNode->appendAttribute<int>("id", info.id);

        asl::Vector2f position = transformPosition(static_cast<float>(info.x), static_cast<float>(info.y));
        myNode->appendAttribute<Vector2f>("position", position);
        myNode->appendAttribute<Vector2f>("raw_position", Vector2f(static_cast<float>(info.x), static_cast<float>(info.y)));
        myNode->appendAttribute<int>("pressure", info.pressure);
        
        return myEvent;
    }

    EventPtrList poll() {
        AC_TRACE << "Listening for Proximatrix2 messages on UDP port " << _myPort;
         
        const unsigned int buffersize = UDP_HDR + PROXY_HDR + TOUCHSIZE + FRAMESIZE + 6600;
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
                            myEvent->getNode()->appendAttribute<DOMString>("type", "add");
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
                myEvent->getNode()->appendAttribute("type", "remove");
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
