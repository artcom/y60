
#include <TuioClient.h>
#include <TuioListener.h>

#include <asl/base/Auto.h>
#include <asl/base/ThreadLock.h>

#include <asl/dom/Nodes.h>

#include <y60/base/DataTypes.h>
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
    typedef std::vector<CursorEvent> CursorEventList;


public:

    TUIOPlugin(DLHandle myDLHandle)
        : PlugInBase(myDLHandle),
          GenericEventSourceFilter(),
          _myEventSchemaDocument(new Document(y60::ourtuioeventxsd)),
          _myEventValueFactory(new ValueFactory())
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
        myFilterFlag = getSetting( mySettings, "FilterMultipleMovePerCursor", myFilterFlag);
        _myFilterMultipleMovePerCursorFlag = (myFilterFlag == 1 ? true : false);
        _myMaxCursorPositionsInHistory = getSetting( mySettings, "MaxCursorPositionsForAverage", _myMaxCursorPositionsInHistory);
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
        if(_myUndeliveredCursors.size() > 0) {
            EventPtrList myEvents;

            CursorEventList::iterator it;
            for(it = _myUndeliveredCursors.begin(); it != _myUndeliveredCursors.end(); ++it) {
                CursorEvent & myEvent = *it;
                myEvents.push_back(convertCursorEvent(myEvent.first, myEvent.second));
                delete myEvent.second;
                myEvent.second = 0;
            }

            _myUndeliveredCursors.clear();
            
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
    }

    virtual void updateTuioObject(TuioObject *tobj) {
    }

    virtual void removeTuioObject(TuioObject *tobj) {
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

    virtual void refresh(TuioTime ftime) {
    }

// implementation of js interface

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

    GenericEventPtr convertCursorEvent(const char *myEventType, TuioCursor *myCursor) {
        GenericEventPtr myEvent(new GenericEvent("onTuioEvent", _myEventSchemaDocument, _myEventValueFactory));
        NodePtr myNode = myEvent->getNode();

        myNode->appendAttribute<DOMString>("type", myEventType);

        myNode->appendAttribute<int>("id", myCursor->getSessionID());

        TuioTime myStartTime = myCursor->getStartTime();
        myNode->appendAttribute<double>("start_time", myStartTime.getSeconds() + (myStartTime.getMicroseconds() / 1000000.0));

        TuioTime myValueTime = myCursor->getTuioTime();
        myNode->appendAttribute<double>("value_time", myValueTime.getSeconds() + (myValueTime.getMicroseconds() / 1000000.0));

        Vector2f myPosition = Vector2f(myCursor->getX(), myCursor->getY());
        if (myCursor->getX() < 0 || myCursor->getX() > 1 || myCursor->getY() < 0 || myCursor->getY() > 1) {
            AC_WARNING << "illegal coordinates " << myCursor->getX() << " " << myCursor->getY();
            myPosition[0] = asl::clamp(myCursor->getX(),0.0f,1.0f);
            myPosition[1] = asl::clamp(myCursor->getY(),0.0f,1.0f);
            AC_WARNING << "fixed coordinates " << myPosition;
        }
        myPosition = calculateAveragePosition(myCursor->getSessionID(), myPosition);
        myNode->appendAttribute<Vector2f>("position", myPosition);
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

    std::vector<TuioClient*> _myClients;
};



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
