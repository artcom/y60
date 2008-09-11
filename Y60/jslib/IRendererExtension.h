#ifndef _IRENDERER_EXTENSION_INCLUDED
#define _IRENDERER_EXTENSION_INCLUDED

#include <asl/base/Ptr.h>
#include <y60/input/Event.h>

namespace jslib {
    class AbstractRenderWindow;
}

namespace y60 {

class Renderer;

class IRendererExtension {
    public:
        IRendererExtension(const std::string & theExtensionName) :
            _myExtensionName(theExtensionName)
        {};
        virtual ~IRendererExtension() {};
        virtual void onStartup(jslib::AbstractRenderWindow * theWindow) = 0;
        virtual bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow) = 0;

        virtual void handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent) = 0;
        virtual void onFrame(jslib::AbstractRenderWindow * theWindow , double t) = 0;

        virtual void onPreRender(jslib::AbstractRenderWindow * theRenderer) = 0;
        virtual void onPostRender(jslib::AbstractRenderWindow * theRenderer) = 0;

        const std::string & getName() const {
            return _myExtensionName;
        }
        void setName(const std::string & theName) {
            _myExtensionName = theName; 
        }
    private:
        IRendererExtension();
        
        std::string _myExtensionName;
};
typedef asl::Ptr<IRendererExtension> IRendererExtensionPtr;

}

#endif
