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

#ifndef _IRENDERER_EXTENSION_INCLUDED
#define _IRENDERER_EXTENSION_INCLUDED

#include "y60_jslib_settings.h"

#include <asl/base/Ptr.h>
#include <y60/input/Event.h>

namespace jslib {
    class AbstractRenderWindow;
}

namespace y60 {

class Renderer;

class Y60_JSLIB_DECL IRendererExtension {
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
