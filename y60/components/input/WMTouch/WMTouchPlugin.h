

#include <y60/jslib/IRendererExtension.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/input/IEventSource.h>
#include <y60/input/GenericEventSourceFilter.h>

namespace y60 {

class WMTouchPlugin : public asl::PlugInBase,
                   public y60::IEventSource,
                   public jslib::IScriptablePlugin,
                   public y60::IRendererExtension,
                   public y60::GenericEventSourceFilter
{

private:
    EventPtrList _queuedEvents;
    asl::Vector2f _calibrationPointBottomLeft;
    asl::Vector2f _calibrationPointTopRight;
    bool _isRegistered;
    static HHOOK _msgHook;
    dom::DocumentPtr            _myEventSchemaDocument;
    asl::Ptr<dom::ValueFactory> _myEventValueFactory;
    std::set<DWORD> _downCursors;
    HWND findSDLWindow();
    void onTouch(HWND hWnd, WPARAM wParam, LPARAM lParam);
    
public:

    WMTouchPlugin(asl::DLHandle myDLHandle);
    ~WMTouchPlugin();

// IScriptablePlugin

    const char * ClassName() {
        return "WMTouch";
    }

    JSFunctionSpec *StaticFunctions();
    JSPropertySpec *StaticProperties();
    
    virtual void onUpdateSettings(dom::NodePtr theSettings) {}
// IRendererExtension
    virtual void onStartup(jslib::AbstractRenderWindow * theWindow) {
        HWND hWnd = findSDLWindow();
        setup(hWnd);
    };

    virtual bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow) { return true; };

    virtual void handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {};
    virtual void onFrame(jslib::AbstractRenderWindow * theWindow , double t) {};

    virtual void onPreRender(jslib::AbstractRenderWindow * theRenderer) {};
    virtual void onPostRender(jslib::AbstractRenderWindow * theRenderer) {};

// IEventSource

    void init() { }
    dom::NodePtr WMTouchPlugin::getEventSchema();
    EventPtrList poll();

	void setCalibrationPoint (const unsigned int theCalibrationPointCode, const asl::Vector2f theWMTouchPosition);

private:

    static 
    LRESULT WINAPI GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
    void setup(HWND theWindow);
};

};

