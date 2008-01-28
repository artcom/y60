//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "TextureManager.h"
#include "Texture.h"

#include <y60/ImageLoader.h>
#include <y60/Movie.h>
#include <y60/Capture.h>
#include <y60/NodeNames.h>
#include <y60/NodeValueNames.h>

#include <dom/Nodes.h>
#include <asl/string_functions.h>
#include <asl/file_functions.h>
#include <asl/os_functions.h>
#include <asl/Dashboard.h>

#define DB(x) // x

using namespace std;
using namespace asl;


namespace y60 {

    TextureManager::TextureManager() :
        _myResourceManager(0), _myResourceManagerCount(0),
        _myMaxTextureSize(TextureManager::getTextureSizeLimit()),
        _myMemoryResourceManager(new MemoryResourceManager())
    {
        _myResourceManager = _myMemoryResourceManager.getNativePtr();
    }

    TextureManager::~TextureManager() {
    }

    asl::WeakPtr<ITextureManager>
    TextureManager::getSelf() const {
        return _mySelf;
    }

    void
    TextureManager::setTextureList(dom::NodePtr theTextureListNode) {
        _myTextureList = theTextureListNode;
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
        unsigned myTextureCount = _myTextureList->childNodesLength();
        for (unsigned i = 0; i < myTextureCount; ++i) {
            dom::NodePtr myTextureNode = _myTextureList->childNode(i);
            if (myTextureNode->nodeType() == dom::Node::ELEMENT_NODE) {
                TexturePtr myTexture = myTextureNode->getFacade<Texture>();
                unbindTexture(&(*myTexture));
            }
        }
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

        // First time load, or source has changed
        if (theMovie->reloadRequired()) {
            MAKE_SCOPE_TIMER(TextureManager_loadMovieFrame_movieLoad);
            try {
                theMovie->load(AppPackageManager::get().getPtr()->getSearchPath());
            } catch (asl::Exception & ex) {
                ex.appendWhat(string("while loading movie src='") +
                        theMovie->get<ImageSourceTag>() + "', "+
                        "searching in '"+AppPackageManager::get().getPtr()->getSearchPath()+"'");
                throw ex;
            }
        }

        {
            MAKE_SCOPE_TIMER(TextureManager_loadMovieFrame_movieReadFrame);
            if (theCurrentTime == -1) {
                theMovie->readFrame();
            } else {
                theMovie->readFrame(theCurrentTime);
            }
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
        AC_INFO << StackTrace();
        return;
        unsigned myTextureCount = _myTextureList->childNodesLength();
        for (unsigned i = 0; i < myTextureCount; ++i) {
            dom::NodePtr myTextureNode = _myTextureList->childNode(i);
            if (myTextureNode->nodeType() == dom::Node::ELEMENT_NODE) {
                myTextureNode->getFacade<Texture>()->preload();
            }
        }
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
        return TexturePtr(0);
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
                _myResourceManager = _myMemoryResourceManager.getNativePtr();
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
