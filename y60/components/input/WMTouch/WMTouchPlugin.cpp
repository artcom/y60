
#define WINVER 0x0601 // assume Windows 7 

#include <asl/base/Auto.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/error_functions.h>

#include <asl/dom/Nodes.h>
#include <y60/jslib/IRendererExtension.h>

#include <y60/base/DataTypes.h>
#include <y60/base/SettingsParser.h>
#include <y60/input/IEventSource.h>
#include <y60/input/GenericEvent.h>
#include <y60/input/GenericEventSourceFilter.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>
#include <y60/jsbase/JSNode.h>

#include <SDL/SDL_syswm.h>
#include <SDL/SDL.h>

#define DB(x) //x


// some defines so we can look up Win7 functions at runtime
typedef BOOL (__stdcall *RegisterTouchWindow_M)(HWND, ULONG); 
typedef BOOL (__stdcall *UnregisterTouchWindow_M)(HWND); 
typedef BOOL (__stdcall *IsTouchWindow_M)(HWND,PULONG); 

namespace y60 {

RegisterTouchWindow_M RegisterTouchWindow; 
UnregisterTouchWindow_M UnregisterTouchWindow; 

using namespace asl;
using namespace dom;

class WMTouchPlugin : public PlugInBase,
                   public y60::IEventSource,
                   public jslib::IScriptablePlugin,
                   public y60::IRendererExtension,
                   public GenericEventSourceFilter
{

private:
    bool _isRegistered;

    HWND
    findSDLWindow() {
        SDL_SysWMinfo myInfo;
        SDL_VERSION(&myInfo.version);
        int rc = SDL_GetWMInfo(&myInfo);
        if (rc != 1) {
            AC_WARNING << "Failed to locate main application window: " << SDL_GetError() << ". Disabling WMTouch.";
        }
        return myInfo.window;
    }
    
public:

    WMTouchPlugin(DLHandle myDLHandle)
        : PlugInBase(myDLHandle),
          GenericEventSourceFilter(),
          _isRegistered(false),
          IRendererExtension("WMTouch")
    {
        AC_WARNING << "loading";
        HMODULE	hMod = LoadLibrary(_T("User32.dll"));
        RegisterTouchWindow = (RegisterTouchWindow_M) GetProcAddress(hMod, "RegisterTouchWindow");
        if (RegisterTouchWindow == 0) {
            AC_ERROR << "can't find Win7 RegisterTouchWindow - Windows 7 Touch Events not supported!";
            return;
        }
        UnregisterTouchWindow = (UnregisterTouchWindow_M) GetProcAddress(hMod, "UnregisterTouchWindow");
        
    }

    ~WMTouchPlugin()
    { 
        if (_isRegistered && UnregisterTouchWindow) {
            UnregisterTouchWindow(findSDLWindow());
        }
    }

// IScriptablePlugin

    const char * ClassName() {
        return "WMTouch";
    }

    JSFunctionSpec *StaticFunctions();
    JSPropertySpec *StaticProperties();
    
    virtual void onUpdateSettings(dom::NodePtr theSettings) {
    }
// IRendererExtension
    virtual void onStartup(jslib::AbstractRenderWindow * theWindow) {
        AC_WARNING << "onStartup";
        HWND hWnd = findSDLWindow();
        if (hWnd && RegisterTouchWindow) {
            BOOL bRet = RegisterTouchWindow(0L, 0L);
            if (bRet) {
                AC_WARNING << "RegisterTouchWindow: " << bRet;
                _isRegistered = true;
            } else {
                AC_WARNING << errorDescription(lastError()); 
            }
        }
    };
    virtual bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow) { return true; };

    virtual void handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {};
    virtual void onFrame(jslib::AbstractRenderWindow * theWindow , double t) {};

    virtual void onPreRender(jslib::AbstractRenderWindow * theRenderer) {};
    virtual void onPostRender(jslib::AbstractRenderWindow * theRenderer) {};

// IEventSource

    void init() {
    }

    EventPtrList poll() {
        return EventPtrList();
    }
    

// reused stuff

protected:

private:

};

// implementation of js interface

static WMTouchPlugin* ourWMTouchPluginInstance = 0;

static WMTouchPlugin*
GetInstance() {
    if(ourWMTouchPluginInstance) {
        return ourWMTouchPluginInstance;
    } else {
        throw Exception("Calling WMTouch static function before plugin initialization.");
    }
}

enum WMTouchPropertyNumbers {
    PROP_eventSchema = -100
};

JSFunctionSpec *
WMTouchPlugin::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        // {"listenToUDP", ListenToUDP, 0},
        {0}
    };
    return myFunctions;
}

static JSBool
GetStaticProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    WMTouchPlugin* myPlugin = GetInstance();

    int myID = JSVAL_TO_INT(id);
    switch(myID) {
    case PROP_eventSchema:
        *vp = jslib::as_jsval(cx, 0);
        break;
    default:
        JS_ReportError(cx, "WMTouch::getStaticProperty: index %d out of range", myID);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSPropertySpec *
WMTouchPlugin::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {"eventSchema", PROP_eventSchema, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY, GetStaticProperty, 0},
        {0}
    };
    return myProperties;
}

}


extern "C"
EXPORT asl::PlugInBase* WMTouch_instantiatePlugIn(asl::DLHandle myDLHandle) {
    if(y60::ourWMTouchPluginInstance) {
        AC_FATAL << "Plugging WMTouch more than once";
        return 0;
    } else {
        AC_TRACE << "Instantiating WMTouch plugin";
        y60::ourWMTouchPluginInstance = new y60::WMTouchPlugin(myDLHandle);
        return y60::ourWMTouchPluginInstance;
    }
}
