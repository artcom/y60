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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "TextureManager.h"

#include "Texture.h"

#include <y60/image/ImageLoader.h>
#include <y60/video/Movie.h>
#include <y60/video/Capture.h>
#include <y60/base/NodeNames.h>
#include <y60/base/NodeValueNames.h>

#include <asl/dom/Nodes.h>
#include <asl/base/string_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/os_functions.h>
#include <asl/base/Dashboard.h>

#define DB(x) // x

using namespace std;
using namespace asl;


namespace y60 {

    TextureManager::TextureManager() :
        _myResourceManager(0), _myResourceManagerCount(0),
        _myMaxTextureSize(TextureManager::getTextureSizeLimit()),
        _myMemoryResourceManager(new MemoryResourceManager())
    {
        _myResourceManager = _myMemoryResourceManager.get();
    }

    TextureManager::~TextureManager() {
    }

    asl::WeakPtr<ITextureManager>
    TextureManager::getSelf() const {
        return _mySelf;
    }

    void
    TextureManager::setTextureList(dom::NodePtr theTextureListNode) {
#if 0
        _myTextureList = theTextureListNode;
#else
        // speedup getElementByName
        _myTextureList = theTextureListNode->getRootNode()->self().lock();
#endif
    }

    void
    TextureManager::setSelf(const asl::Ptr<ITextureManager> & theSelf) {
        _mySelf = asl::WeakPtr<ITextureManager>(theSelf);
    }

    void
    TextureManager::validateGLContext(bool theFlag) {
        _myResourceManager->validateGLContext(theFlag);
    }

    void
    TextureManager::unbindTextures() {
        AC_DEBUG << "TextureManager::unbindTextures";
#if 0
        unsigned myTextureCount = _myTextureList->childNodesLength();
        for (unsigned i = 0; i < myTextureCount; ++i) {
            dom::NodePtr myTextureNode = _myTextureList->childNode(i);
            if (myTextureNode->nodeType() == dom::Node::ELEMENT_NODE) {
                TexturePtr myTexture = myTextureNode->getFacade<Texture>();
                unbindTexture(myTexture.get());
            }
        }
#else
        std::vector<TexturePtr> myTextures = _myTextureList->getAllFacades<Texture>(TEXTURE_NODE_NAME);
        for (unsigned i = 0; i < myTextures.size(); ++i) {
            unbindTexture(myTextures[i].get());
        }
#endif
    }

    int
    TextureManager::getMaxTextureSize(int theDimensions) const {
        if (_myMaxTextureSize == 0) {
            return _myResourceManager->getMaxTextureSize(theDimensions);
        }
        return _myMaxTextureSize;
    }

    void
    TextureManager::loadMovieFrame(MoviePtr theMovie, double theCurrentTime) {
        MAKE_SCOPE_TIMER(TextureManager_loadMovieFrame);
        AC_DEBUG << "loadMovieFrame: theCurrentTime = " << theCurrentTime;

        if (theCurrentTime == -1) {
            theMovie->readFrame();
        } else {
            theMovie->readFrame(theCurrentTime);
        }
    }

    void
    TextureManager::loadCaptureFrame(CapturePtr theCapture) {
        MAKE_SCOPE_TIMER(TextureManager_loadCaptureFrame);

        // First time load, or source has changed
        if (theCapture->reloadRequired()) {
            try {
                theCapture->load(AppPackageManager::get().getPtr()->getSearchPath());
            } catch (asl::Exception & ex) {
                ex.appendWhat(string("while loading movie src='") +
                        theCapture->get<ImageSourceTag>() + "', "+
                        "searching in '"+AppPackageManager::get().getPtr()->getSearchPath()+"'");
                throw ex;
            }
        }
        theCapture->readFrame();
    }

    void
    TextureManager::reloadTextures() {
        AC_DEBUG << "TextureManager::reloadTextures";
        AC_INFO << "TextureManager::reloadTextures preload() disabled";
        AC_DEBUG << StackTrace();
        return;
        //unsigned myTextureCount = _myTextureList->childNodesLength();
        //for (unsigned i = 0; i < myTextureCount; ++i) {
        //    dom::NodePtr myTextureNode = _myTextureList->childNode(i);
        //    if (myTextureNode->nodeType() == dom::Node::ELEMENT_NODE) {
        //        myTextureNode->getFacade<Texture>()->preload();
        //    }
        //}
    }

    TexturePtr
    TextureManager::getTexture(const std::string & theTextureId) const{
        TexturePtr myTexture = findTexture(theTextureId);
        if (!myTexture) {
            throw TextureManagerException(std::string("Request for texture '") + theTextureId + "' failed.", PLUS_FILE_LINE);
        }
        return myTexture;
    }

    TexturePtr
    TextureManager::findTexture(const std::string & theTextureId) const {

        dom::NodePtr myTextureNode = _myTextureList->getElementById(theTextureId);
        if (myTextureNode) {
             return myTextureNode->getFacade<Texture>();
        }
        return TexturePtr();
    }

    /*
    void TextureManager::updateTextureData(const TexturePtr & theTexture) {
        AC_PRINT << "XXX TextureManager::updateTextureData";
        _myResourceManager->updateTextureData(theTexture);
    }

    void TextureManager::setTexturePriority(const TexturePtr & theTexture, float thePriority) {
        AC_PRINT << "XXX TextureManager::setTexturePriority id=" << theTexture->get<IdTag>();
        _myResourceManager->setTexturePriority(theTexture, thePriority);
    }
    */

    void TextureManager::unbindTexture(Texture * theTexture) {
        AC_DEBUG << "TextureManager::unbindTexture id=" << theTexture->get<IdTag>();
        theTexture->removeTextureFromResourceManager();
        //_myResourceManager->unbindTexture(theTexture);
    }

    asl::Ptr<TextureManager> TextureManager::create() {
        asl::Ptr<TextureManager> myInstance = asl::Ptr<TextureManager>(new TextureManager());
        myInstance->setSelf(myInstance);
        return myInstance;
    }

    int TextureManager::registerResourceManager(ResourceManager* theResourceManager) {
        if (NULL == theResourceManager) {
            // Decrement count by one
            --_myResourceManagerCount;
            if (_myResourceManagerCount == 0) {
                // Remove the Textures from the resourcemanager
                unbindTextures();
                // XXX TODO: Copy vertices back into the Memory resource manager
                // Set the Resourcemanager back to the Memory implementation
                _myResourceManager = _myMemoryResourceManager.get();
            } else if (_myResourceManagerCount < 0) {
                throw TextureManagerException("No Resourcemanager to set to NULL", PLUS_FILE_LINE);
            }
            AC_TRACE << "ResourceManagerCount decremented to " << _myResourceManagerCount;
        } else {
            if (_myResourceManager != theResourceManager) {
                // XXX TODO: Copy vertices from _myResourceManager into theResourceManager
                _myResourceManager = theResourceManager;
            }
            ++_myResourceManagerCount;
            AC_TRACE << "ResourceManagerCount incremented to " << _myResourceManagerCount;
        }
        return _myResourceManagerCount;
    }

}
