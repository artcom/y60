//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "TextureManager.h"

#include <y60/Image.h>
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

const unsigned DEFAULT_TEXTURE_SIZE_LIMIT = 0; // ask gl for max texture size

namespace y60 {

    TextureManager::TextureManager() :
        _myResourceManager(0), _myResourceManagerCount(0),
        _myMaxTextureSize(DEFAULT_TEXTURE_SIZE_LIMIT),
        _myMemoryResourceManager(new MemoryResourceManager())
    {
        string myLimitString;
        if (get_environment_var(Y60_TEXTURE_SIZE_LIMIT_ENV, myLimitString)) {
            if (!fromString(myLimitString, _myMaxTextureSize)) {
                AC_WARNING << "Invalid "<<Y60_TEXTURE_SIZE_LIMIT_ENV<<" environment variable, using default value = "<<DEFAULT_TEXTURE_SIZE_LIMIT<<endl;
            }
        }
        _myResourceManager = _myMemoryResourceManager.getNativePtr();
    }

    TextureManager::~TextureManager() {
    }

    asl::WeakPtr<ITextureManager>
    TextureManager::getSelf() const {
        return _mySelf;
    }

    void
    TextureManager::setImageList(dom::NodePtr theImageListNode) {
        _myImageList = theImageListNode;
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
        unsigned myImageCount = _myImageList->childNodesLength();
        for (unsigned i = 0; i < myImageCount; ++i) {
            dom::NodePtr myImageNode = _myImageList->childNode(i);
            if (myImageNode->nodeType() == dom::Node::ELEMENT_NODE) {
                ImagePtr myImage = myImageNode->getFacade<Image>();
                unbindTexture(&(*myImage));
            }
        }
    }

    int
    TextureManager::getMaxTextureSize(int theDimensions) const {
        if (_myMaxTextureSize == 0) {
            return _myResourceManager->getMaxTextureSize(theDimensions);
        } else {
            return _myMaxTextureSize;
        }
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
        unsigned myImageCount = _myImageList->childNodesLength();
        for (unsigned i = 0; i < myImageCount; ++i) {
            dom::NodePtr myImageNode = _myImageList->childNode(i);
            if (myImageNode->nodeType() == dom::Node::ELEMENT_NODE) {
                myImageNode->getFacade<Image>()->triggerUpload();
            }
        }
    }

    ImagePtr
    TextureManager::getImage(const std::string & theImageId) const{
        ImagePtr myImage = findImage(theImageId);
        if (!myImage) {
            throw TextureManagerException(std::string("Request for image '") + theImageId +
                                          "' failed.", PLUS_FILE_LINE);
        }
        return myImage;
    }

    ImagePtr
    TextureManager::findImage(const std::string & theImageId) const {

        dom::NodePtr myImageNode = _myImageList->getElementById(theImageId);
        if (myImageNode) {
             return myImageNode->getFacade<Image>();
        } else {
             return ImagePtr(0);
        }
    }

    void TextureManager::updateImageData(ImagePtr theImage) {
        // Delegate to my ResourceManager
        _myResourceManager->updateTextureData(theImage);
    }

    void TextureManager::setPriority(Image * theImage, float thePriority) {
        // Delegate to my ResourceManager
        _myResourceManager->setTexturePriority(theImage, thePriority);
    }

    void TextureManager::unbindTexture(Image * theImage) {
        // Delegate to my ResourceManager
        //AC_TRACE << "Unbinding Texture " << theImage->get<TextureIdTag>();
        theImage->removeTextureFromResourceManager();
        //_myResourceManager->unbindTexture(theImage);
    }

    asl::Ptr<TextureManager> TextureManager::create() {
        asl::Ptr<TextureManager> myInstance = asl::Ptr<TextureManager>(new TextureManager());
        myInstance->setSelf(myInstance);
        return myInstance;
    }

    unsigned TextureManager::setupImage(ImagePtr theImage) {
        return _myResourceManager->setupTexture(theImage);
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
