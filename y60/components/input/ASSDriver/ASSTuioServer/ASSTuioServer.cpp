
#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSNode.h>

#include <y60/components/input/ASSDriver/ASSCore/ASSDriver.h>

#include <TuioServer.h>

namespace y60 {

using namespace TUIO;

class ASSTuioServer :
    public y60::ASSDriver,
    public asl::PlugInBase
{
public:
    ASSTuioServer (asl::DLHandle theDLHandle)
        : asl::PlugInBase(theDLHandle),
          _myServer(0)
    { }

    virtual ~ASSTuioServer()
    { }

    const char * ClassName() {
        static const char * myClassName = "ASSTuioServer";
        return myClassName;
    }

    void beginFrame() {
        if(_myServer) {
            _myServer->initFrame(TuioTime::getSessionTime());
        }
    }

    void finishFrame() {
        if(_myServer) {
            _myServer->commitFrame();
        }
    }

    void createEvent(int theID, const std::string & theType,
            const asl::Vector2f & theRawPosition, const asl::Vector3f & thePosition3D,
            const asl::Box2f & theROI, float intensity, const ASSEvent & theEvent) {
        if(_myServer) {
            float myX = (theRawPosition[0] / theEvent.size[0]);
            float myY = (theRawPosition[1] / theEvent.size[1]);
            if(theType == "add") {
                TuioCursor *myCursor = _myServer->addTuioCursor(myX, myY);
                _myCursors[theID] = myCursor;
            } else if(theType == "move") {
                _myServer->updateTuioCursor(_myCursors[theID], myX, myY);
            } else if(theType == "remove") {
                TuioCursor *myCursor = _myCursors[theID];
                _myCursors.erase(theID);
                _myServer->removeTuioCursor(myCursor);
            }
        }
    }

    void createTransportLayerEvent(const std::string & theType) {
    }

    void sendUDP(std::string theHost, asl::Unsigned16 thePort) {
        AC_INFO << "Will send udp to " << theHost << " on port " << thePort;

        if(_myServer) {
            throw asl::Exception("TUIO server has already been configured");
        }

        _myServer = new TuioServer(theHost.c_str(), thePort);

    }

    void poll() {
        processInput();
    }

    JSFunctionSpec * StaticFunctions();

private:
    TuioServer* _myServer;
    std::map<int,TuioCursor*> _myCursors;
};

static ASSTuioServer* ourTuioServerInstance = 0;

static ASSTuioServer*
GetInstance() {
    if(ourTuioServerInstance) {
        return ourTuioServerInstance;
    } else {
        throw asl::Exception("Calling TUIO static function before plugin initialization.");
    }
}

static JSBool
SendUDP(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ASSTuioServer* myPlugin = GetInstance();

        std::string myHost("localhost");
        asl::Unsigned16 myPort = 3333;

        if(argc > 0) {
            if(!jslib::convertFrom(cx, argv[0], myHost)) {
                JS_ReportError(cx, "ASSTuioServer::sendUDP - expected hostname as first argument");
                return JS_FALSE;
            }
            if(argc > 1) {
                if(!jslib::convertFrom(cx, argv[1], myPort)) {
                    JS_ReportError(cx, "ASSTuioServer::sendUDP - expected port as second argument");
                }
            }
        }

        myPlugin->sendUDP(myHost, myPort);
    } HANDLE_CPP_EXCEPTION;

    return JS_TRUE;
}

static JSBool
Poll(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ASSTuioServer* myPlugin = GetInstance();

        myPlugin->poll();
    } HANDLE_CPP_EXCEPTION;

    return JS_TRUE;
}

static JSBool
OnUpdateSettings(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        ASSTuioServer* myPlugin = GetInstance();

        if (argc != 1) {
            JS_ReportError(cx, "JSScriptablePlugin::onUpdateSettings(): Wrong number of arguments. One (ConfigNode) expected");
            return JS_FALSE;
        }

        dom::NodePtr myConfigNode;
        if (JSVAL_IS_VOID(argv[0]) || !jslib::convertFrom(cx, argv[0], myConfigNode)) {
            JS_ReportError(cx, "JSScriptablePlugin::onUpdateSettings(): Argument #1 must be a node");
            return JS_FALSE;
        }

        myPlugin->onUpdateSettings(myConfigNode);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;

    return JS_TRUE;
}


JSFunctionSpec * 
ASSTuioServer::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {"sendUDP", SendUDP, 0},
        {"poll", Poll, 0},
        {"onUpdateSettings", OnUpdateSettings, 1},
        {0}
    };
    return myFunctions;
}

} // end of namespace y60

extern "C"
EXPORT asl::PlugInBase * ASSTuioServer_instantiatePlugIn(asl::DLHandle myDLHandle) {
    if(y60::ourTuioServerInstance) {
        AC_FATAL << "Plugging TUIO server more than once";
        return 0;
    } else {
        AC_TRACE << "Instantiating TUIO server plugin";
        y60::ourTuioServerInstance = new y60::ASSTuioServer(myDLHandle);
        return y60::ourTuioServerInstance;
    }
}
