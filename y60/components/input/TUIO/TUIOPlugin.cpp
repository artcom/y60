
#include <TuioClient.h>
#include <TuioListener.h>

#include <asl/base/Auto.h>
#include <asl/base/ThreadLock.h>

#include <asl/dom/Nodes.h>

#include <y60/base/DataTypes.h>
#include <y60/base/SettingsParser.h>
#include <y60/input/IEventSource.h>
#include <y60/input/GenericEvent.h>
#include <y60/input/GenericEventSourceFilter.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>
#include <y60/jsbase/JSNode.h>

#include "tuioeventxsd.h"

#define DB(x) //x

namespace y60 {

using namespace asl;
using namespace dom;
using namespace TUIO;

class TUIOPlugin : public PlugInBase,
                   public y60::IEventSource,
                   public jslib::IScriptablePlugin,
                   public TuioListener,
                   public GenericEventSourceFilter
{

private:
    typedef std::pair<const char*, TuioCursor*> CursorEvent;
    typedef std::pair<const char*, TuioObject*> ObjectEvent;
    typedef std::vector<CursorEvent> CursorEventList;
    typedef std::vector<ObjectEvent> ObjectEventList;


public:

    TUIOPlugin(DLHandle myDLHandle)
        : PlugInBase(myDLHandle),
          GenericEventSourceFilter(),
          _myEventSchemaDocument(new Document(y60::ourtuioeventxsd)),
          _myEventValueFactory(new ValueFactory()),
          _myTouchArea(-1.0,-1.0)
    {
        registerStandardTypes(*_myEventValueFactory);
        registerSomTypes(*_myEventValueFactory);

        // add filter for deleting multiple update
        if (_myFilterMultipleMovePerCursorFlag) {
            addCursorFilter("update", "id");
        }
        // no default cursor smoothing
        _myMaxCursorPositionsInHistory = 1;
    }

    ~TUIOPlugin()
    { }

// IScriptablePlugin

    const char * ClassName() {
        return "TUIOClient";
    }

    JSFunctionSpec *StaticFunctions();
    JSPropertySpec *StaticProperties();
    
    virtual void onUpdateSettings(dom::NodePtr theSettings) {
        dom::NodePtr mySettings = getTUIOSettings(theSettings);
        int myFilterFlag = 0;
        getConfigSetting( mySettings, "FilterMultipleMovePerCursor", myFilterFlag, 0);
        _myFilterMultipleMovePerCursorFlag = (myFilterFlag == 1 ? true : false);
        getConfigSetting( mySettings, "MaxCursorPositionsForAverage", _myMaxCursorPositionsInHistory, static_cast<unsigned int>(1));
        int myMaxCursorCount = -1;
        getConfigSetting( mySettings, "MaxCursorCount", myMaxCursorCount, -1);
        setMaxCursorCount(myMaxCursorCount);
        getConfigSetting( mySettings, "TouchArea", _myTouchArea, asl::Vector2f(-1.0,-1.0));
    }

    dom::NodePtr
    getTUIOSettings(dom::NodePtr theSettings) {
        dom::NodePtr mySettings;
        if ( theSettings->nodeType() == dom::Node::DOCUMENT_NODE) {
            if (theSettings->childNode(0)->nodeName() == "settings") {
                mySettings = theSettings->childNode(0)->childNode("TUIO", 0);
            }
        } else if ( theSettings->nodeName() == "settings") {
            mySettings = theSettings->childNode("TUIO", 0);
        } else if ( theSettings->nodeName() == "TUIO" ) {
            mySettings = theSettings;
        }

        if ( ! mySettings ) {
            throw Exception(
                std::string("Could not find TUIO node in settings: ") +
                as_string( * theSettings), PLUS_FILE_LINE );
        }
        return mySettings;
    }

// IEventSource

    void init() {
    }

    EventPtrList poll() {
        AutoLocker<ThreadLock> l(_myDeliveryMutex);
        if(_myUndeliveredCursors.size() > 0 || _myUndeliveredObjects.size() > 0) {
            EventPtrList myEvents;
            if(_myUndeliveredCursors.size() > 0) {
                CursorEventList::iterator it;
                for(it = _myUndeliveredCursors.begin(); it != _myUndeliveredCursors.end(); ++it) {
                    CursorEvent & myEvent = *it;
                    int myCursorId = myEvent.second->getSessionID();
                    if (allow2SendCursor(myCursorId)) {
                        myEvents.push_back(convertCursorEvent(myEvent.first, myEvent.second));
                        if (strcmp (myEvent.first, "remove") == 0) {
                            removeFromCursorList(myCursorId);
                        }
                    }
                    delete myEvent.second;
                    myEvent.second = 0;
                }

                _myUndeliveredCursors.clear();
                
            }
            if(_myUndeliveredObjects.size() > 0) {
                ObjectEventList::iterator it;
                for(it = _myUndeliveredObjects.begin(); it != _myUndeliveredObjects.end(); ++it) {
                    ObjectEvent & myEvent = *it;
                    myEvents.push_back(convertObjectEvent(myEvent.first, myEvent.second));
                    delete myEvent.second;
                    myEvent.second = 0;
                }
                _myUndeliveredObjects.clear();
            }

            DB(AC_INFO << "unfiltered tuio events # " << myEvents.size());
            // logs event statistics for multiple events per cursor and type
            DB(analyzeEvents(myEvents, "id"));
            // do the event filter in base class GenericEventSourceFilter
            applyFilter(myEvents);
            DB(AC_INFO << "deliver tuio events # " << myEvents.size());
            DB(analyzeEvents(myEvents, "id"));
            
            clearCursorHistoryOnRemove(myEvents);

            return myEvents;
        } else {
            return EventPtrList();
        }
    }
    

// TuioListener

    virtual void addTuioObject(TuioObject *tobj) {
        handleObject(tobj, "add");
    }

    virtual void updateTuioObject(TuioObject *tobj) {
        handleObject(tobj, "update");
    }

    virtual void removeTuioObject(TuioObject *tobj) {
        handleObject(tobj, "remove");
    }

    virtual void addTuioCursor(TuioCursor *myCursor) {
        handleCursor(myCursor, "add");
    }

    virtual void updateTuioCursor(TuioCursor *myCursor) {
        handleCursor(myCursor, "update");
    }

    virtual void removeTuioCursor(TuioCursor *myCursor) {
        handleCursor(myCursor, "remove");
    }
    
#ifdef TUIO_API_VERSION_1_5
    // blobs are not used in y60
    virtual void addTuioBlob(TuioBlob *tblb) {}

    virtual void updateTuioBlob(TuioBlob *tblb) {}

    virtual void removeTuioBlob(TuioBlob *tblb) {}
#endif //TUIO_API_VERSION_1_5

    virtual void refresh(TuioTime ftime) {
    }

    void listenToUDP(Unsigned16 thePort) {
        AC_INFO << "Listening for TUIO messages on UDP port " << thePort;
        TuioClient *myClient = new TuioClient(thePort);
        myClient->connect();
        myClient->addTuioListener(this);
        _myClients.push_back(myClient);
    }

    NodePtr getEventSchema() {
        return _myEventSchemaDocument;
    }

// reused stuff

protected:

    void handleCursor(TuioCursor *myCursor, const char *myEventType) {
        AC_TRACE << "Handling cursor " << myEventType << " for "
                 << myCursor->getSessionID() << ":" << myCursor->getCursorID();

        AutoLocker<ThreadLock> l(_myDeliveryMutex);

        TuioCursor *myCursorCopy = new TuioCursor(myCursor);

        _myUndeliveredCursors.push_back(CursorEvent(myEventType, myCursorCopy));
    }

    void handleObject(TuioObject *myObject, const char *myEventType) {
        AC_TRACE << "Handling object " << myEventType << " for "
                 << myObject->getSessionID() << ":" << myObject->getSymbolID();

        AutoLocker<ThreadLock> l(_myDeliveryMutex);

        TuioObject *myObjectCopy = new TuioObject(myObject);

        _myUndeliveredObjects.push_back(ObjectEvent(myEventType, myObjectCopy));
    }

    void fillStandardTUIOInfo(NodePtr & theNode, TuioContainer* theTUIOContainer) {
        theNode->appendAttribute<int>("id", theTUIOContainer->getSessionID());

        TuioTime myStartTime = theTUIOContainer->getStartTime();
        theNode->appendAttribute<double>("start_time", myStartTime.getSeconds() + (myStartTime.getMicroseconds() / 1000000.0));

        TuioTime myValueTime = theTUIOContainer->getTuioTime();
        theNode->appendAttribute<double>("value_time", myValueTime.getSeconds() + (myValueTime.getMicroseconds() / 1000000.0));

        Vector2f myPosition = Vector2f(theTUIOContainer->getX(), theTUIOContainer->getY());
        if (theTUIOContainer->getX() < 0 || theTUIOContainer->getX() > 1 || 
            theTUIOContainer->getY() < 0 || theTUIOContainer->getY() > 1) {
            AC_WARNING << "illegal coordinates " << theTUIOContainer->getX() << " " << theTUIOContainer->getY();
            myPosition[0] = asl::clamp(theTUIOContainer->getX(),0.0f,1.0f);
            myPosition[1] = asl::clamp(theTUIOContainer->getY(),0.0f,1.0f);
            AC_WARNING << "fixed coordinates " << myPosition;
        }
        myPosition = calculateAveragePosition(theTUIOContainer->getSessionID(), myPosition);
        theNode->appendAttribute<Vector2f>("position", myPosition);
        theNode->appendAttribute<Vector2f>("toucharea", _myTouchArea);

    }

    GenericEventPtr convertObjectEvent(const char *theEventType, TuioObject *theObject) {
        GenericEventPtr myEvent(new GenericEvent("onTuioEvent", _myEventSchemaDocument, _myEventValueFactory));
        NodePtr myNode = myEvent->getNode();

        myNode->appendAttribute<DOMString>("tuiotype", "object");
        myNode->appendAttribute<DOMString>("type", theEventType);
        myNode->appendAttribute<int>("symbolid", theObject->getSymbolID());
        myNode->appendAttribute<double>("rotation", theObject->getAngle());
        myNode->appendAttribute<double>("rotationspeed", theObject->getRotationSpeed());
        myNode->appendAttribute<double>("rotationaccel", theObject->getRotationAccel());
        

        fillStandardTUIOInfo(myNode, theObject);
        return myEvent;
    }

    GenericEventPtr convertCursorEvent(const char *myEventType, TuioCursor *myCursor) {
        GenericEventPtr myEvent(new GenericEvent("onTuioEvent", _myEventSchemaDocument, _myEventValueFactory));
        NodePtr myNode = myEvent->getNode();

        myNode->appendAttribute<DOMString>("tuiotype", "cursor");
        myNode->appendAttribute<DOMString>("type", myEventType);

        fillStandardTUIOInfo(myNode, myCursor);

        myNode->appendAttribute<Vector2f>("velocity", Vector2f(myCursor->getXSpeed(), myCursor->getYSpeed()));
        myNode->appendAttribute<double>("speed", myCursor->getMotionSpeed());
        myNode->appendAttribute<double>("acceleration", myCursor->getMotionAccel());

        return myEvent;
    }

private:

    DocumentPtr                 _myEventSchemaDocument;
    asl::Ptr<dom::ValueFactory> _myEventValueFactory;

    ThreadLock      _myDeliveryMutex;
    CursorEventList _myUndeliveredCursors;
    ObjectEventList _myUndeliveredObjects;

    std::vector<TuioClient*> _myClients;
    asl::Vector2f _myTouchArea;
};

// implementation of js interface

static TUIOPlugin* ourTuioPluginInstance = 0;

static TUIOPlugin*
GetInstance() {
    if(ourTuioPluginInstance) {
        return ourTuioPluginInstance;
    } else {
        throw Exception("Calling TUIO static function before plugin initialization.");
    }
}

static JSBool
ListenToUDP(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    TUIOPlugin* myPlugin = GetInstance();

    Unsigned16 myPort = 3333;

    if(argc > 0) {
        if(!jslib::convertFrom(cx, argv[0], myPort)) {
            JS_ReportError(cx, "TUIO::listenToUDP - expected port as first argument");
            return JS_FALSE;
        }
    }

    myPlugin->listenToUDP(myPort);

    return JS_TRUE;
}

enum TUIOPropertyNumbers {
    PROP_eventSchema = -100
};

JSFunctionSpec *
TUIOPlugin::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {"listenToUDP", ListenToUDP, 0},
        {0}
    };
    return myFunctions;
}

static JSBool
GetStaticProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    TUIOPlugin* myPlugin = GetInstance();

    int myID = JSVAL_TO_INT(id);
    switch(myID) {
    case PROP_eventSchema:
        *vp = jslib::as_jsval(cx, myPlugin->getEventSchema());
        break;
    default:
        JS_ReportError(cx, "TUIO::getStaticProperty: index %d out of range", myID);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSPropertySpec *
TUIOPlugin::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {"eventSchema", PROP_eventSchema, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY, GetStaticProperty, 0},
        {0}
    };
    return myProperties;
}

}


extern "C"
EXPORT asl::PlugInBase* TUIOClient_instantiatePlugIn(asl::DLHandle myDLHandle) {
    if(y60::ourTuioPluginInstance) {
        AC_FATAL << "Plugging TUIO more than once";
        return 0;
    } else {
        AC_TRACE << "Instantiating TUIO plugin";
        y60::ourTuioPluginInstance = new y60::TUIOPlugin(myDLHandle);
        return y60::ourTuioPluginInstance;
    }
}
