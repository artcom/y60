
#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSNode.h>

#include <TuioServer.h>

namespace y60 {

using namespace TUIO;

class TuioServerPlugin :
    public asl::PlugInBase,
    public jslib::IScriptablePlugin
{
public:
    TuioServerPlugin (asl::DLHandle theDLHandle)
         : asl::PlugInBase(theDLHandle),
          _myServer(0)
    { }

    virtual ~TuioServerPlugin()
    { }

    const char * ClassName() {
        static const char * myClassName = "TUIOServer";
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

    void addCursor (const int theID, const float x, const float y) {
        TuioCursor *myCursor = _myServer->addTuioCursor(x, y);
        _myCursors[theID] = myCursor;
    }

    void updateCursor (const int theID, const float x, const float y) {
        _myServer->updateTuioCursor(_myCursors[theID], x, y);
    }

    void removeCursor (const int theID) {
        TuioCursor *myCursor = _myCursors[theID];
        _myCursors.erase(theID);
        _myServer->removeTuioCursor(myCursor);
    }

    void sendUDP(std::string theHost, asl::Unsigned16 thePort) {
        AC_INFO << "Will send udp to " << theHost << " on port " << thePort;

        if(_myServer) {
            throw asl::Exception("TUIO server has already been configured");
        }

        _myServer = new TuioServer(theHost.c_str(), thePort);

    }

    JSFunctionSpec * StaticFunctions();

private:
    TuioServer* _myServer;
    std::map<int,TuioCursor*> _myCursors;
};

static TuioServerPlugin* ourTuioServerInstance = 0;

static TuioServerPlugin*
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
        TuioServerPlugin* myPlugin = GetInstance();

        std::string myHost("localhost");
        asl::Unsigned16 myPort = 3333;

        if(argc > 0) {
            if(!jslib::convertFrom(cx, argv[0], myHost)) {
                JS_ReportError(cx, "TuioServerPlugin::sendUDP - expected hostname as first argument");
                return JS_FALSE;
            }
            if(argc > 1) {
                if(!jslib::convertFrom(cx, argv[1], myPort)) {
                    JS_ReportError(cx, "TuioServerPlugin::sendUDP - expected port as second argument");
                }
            }
        }

        myPlugin->sendUDP(myHost, myPort);
    } HANDLE_CPP_EXCEPTION;

    return JS_TRUE;
}

static JSBool
OnUpdateSettings(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        TuioServerPlugin* myPlugin = GetInstance();

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

static JSBool
AddCursor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        TuioServerPlugin* myPlugin = GetInstance();

        if (argc != 3) {
            JS_ReportError(cx, "TuioServerPlugin::addCursor - expected three arguments: cursorId, x, y. Argument count was " + argc);
            return JS_FALSE;
        }
        
        int myId;
        float myX; 
        float myY;
        if (!jslib::convertFrom(cx, argv[0], myId)) {
            JS_ReportError(cx, "TuioServerPlugin::addCursor - expected cursorId (int) as first argument");
            return JS_FALSE;
        }
        if (!jslib::convertFrom(cx, argv[1], myX)) {
            JS_ReportError(cx, "TuioServerPlugin::addCursor - expected x (float (0..1)) as second argument");
            return JS_FALSE;
        }
        if (!jslib::convertFrom(cx, argv[2], myY)) {
            JS_ReportError(cx, "TuioServerPlugin::addCursor - expected y (float (0..1)) as third argument");
            return JS_FALSE;
        }

        myPlugin->addCursor(myId, myX, myY);
    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}


static JSBool
UpdateCursor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        TuioServerPlugin* myPlugin = GetInstance();

        if (argc != 3) {
            JS_ReportError(cx, "TuioServerPlugin::updateCursor - expected three arguments: cursorId, x, y. Argument count was " + argc);
            return JS_FALSE;
        }
        
        int myId;
        float myX; 
        float myY;
        if (!jslib::convertFrom(cx, argv[0], myId)) {
            JS_ReportError(cx, "TuioServerPlugin::updateCursor - expected cursorId (int) as first argument");
            return JS_FALSE;
        }
        if (!jslib::convertFrom(cx, argv[1], myX)) {
            JS_ReportError(cx, "TuioServerPlugin::updateCursor - expected x (float (0..1)) as second argument");
            return JS_FALSE;
        }
        if (!jslib::convertFrom(cx, argv[2], myY)) {
            JS_ReportError(cx, "TuioServerPlugin::updateCursor - expected y (float (0..1)) as third argument");
            return JS_FALSE;
        }

        myPlugin->updateCursor(myId, myX, myY);
    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}

static JSBool
RemoveCursor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        TuioServerPlugin* myPlugin = GetInstance();

        if (argc != 1) {
            JS_ReportError(cx, "TuioServerPlugin::removeCursor - expected one arguments: cursorId. Argument count was " + argc);
            return JS_FALSE;
        }
        
        int myId;
        if (!jslib::convertFrom(cx, argv[0], myId)) {
            JS_ReportError(cx, "TuioServerPlugin::addCursor - expected cursorId (int) as first argument");
            return JS_FALSE;
        }

        myPlugin->removeCursor(myId);
    } HANDLE_CPP_EXCEPTION;
    return JS_TRUE;
}
        
static JSBool
BeginFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        TuioServerPlugin* myPlugin = GetInstance();
        myPlugin->beginFrame();
    } HANDLE_CPP_EXCEPTION;

    return JS_TRUE;
}

static JSBool
FinishFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        TuioServerPlugin* myPlugin = GetInstance();
        myPlugin->finishFrame();
    } HANDLE_CPP_EXCEPTION;

    return JS_TRUE;
}

JSFunctionSpec *
TuioServerPlugin::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {"sendUDP", SendUDP, 0},
        {"beginFrame", BeginFrame, 0},
        {"finishFrame", FinishFrame, 0},
        {"onUpdateSettings", OnUpdateSettings, 1},
        {"addCursor", AddCursor, 3},
        {"updateCursor", UpdateCursor, 3},
        {"removeCursor", RemoveCursor, 1},
        {0}
    };
    return myFunctions;
}

} // end of namespace y60

extern "C"
EXPORT asl::PlugInBase * TUIOServer_instantiatePlugIn(asl::DLHandle myDLHandle) {
    if(y60::ourTuioServerInstance) {
        AC_FATAL << "Plugging TUIO server more than once";
        return 0;
    } else {
        AC_TRACE << "Instantiating TUIO server plugin";
        y60::ourTuioServerInstance = new y60::TuioServerPlugin(myDLHandle);
        return y60::ourTuioServerInstance;
    }
}
