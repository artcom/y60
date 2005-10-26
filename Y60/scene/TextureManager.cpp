//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
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
    TextureManager::unbindTextures() {
        AC_TRACE << "TextureManager::unbindTextures()" << endl;
        unsigned myImageCount = _myImageList->childNodesLength();
        for (unsigned i = 0; i < myImageCount; ++i) {
            dom::NodePtr myImageNode = _myImageList->childNode(i);
            if (myImageNode->nodeType() == dom::Node::ELEMENT_NODE) {
                ImagePtr myImage = myImageNode->getFacade<Image>();
                unbindTexture(&(*myImage));
            }
        }
    }

    void
    TextureManager::update() {
        unsigned myImageCount = _myImageList->childNodesLength();
        AC_TRACE << "TextureManager::update() on " << myImageCount << " images" << endl;
        for (unsigned i = 0; i < myImageCount; ++i) {
            dom::NodePtr myImageNode = _myImageList->childNode(i);
            if (myImageNode->nodeType() == dom::Node::ELEMENT_NODE) {
                ImagePtr myImage = myImageNode->getFacade<Image>();
                AC_TRACE << "TextureManager::update() checking image '" << myImage->get<NameTag>() << "'" << endl;
                myImage->setTextureManager(*this);

                bool reloadRequired = myImage->reloadRequired();
                if (reloadRequired) {
                    myImage->load(*_myPackageManager);
                }

                bool textureUploadRequired = myImage->textureUploadRequired() ||
                                             myImage->isImageNewerThanTexture();
                AC_DEBUG << myImage->get<NameTag>() << " reload=" << reloadRequired << " upload=" << textureUploadRequired;

                if (reloadRequired || textureUploadRequired) {
                    uploadTexture(myImage);
                } else {
                    AC_TRACE << "Texture not uploaded.";
                }
            }
        }
    }

    void
    TextureManager::uploadTexture(ImagePtr theImage) {
        MAKE_SCOPE_TIMER(TextureManager_uploadTexture);
        AC_TRACE << "TextureManager::uploadTexture('" << theImage->get<NameTag>() << "')";
        if (theImage->canReuseTexture()) {
            AC_TRACE << "Reusing texture, just uploading image data." << endl;
            MAKE_SCOPE_TIMER(TextureManager_updatingImageData);
            updateImageData(theImage);
        } else {
            AC_TRACE << "Replacing texture." << endl;
            MAKE_SCOPE_TIMER(TextureManager_replacingTexture);
            if (theImage->getGraphicsId()) {
                // In order to prevent a texture leak, we need to unbind
                // the texture before setupImage()
                unbindTexture(&*theImage);
            }
            setupImage(theImage);
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

        // First time load, or source has changed
        if (theMovie->reloadRequired()) {
            try {
                theMovie->load(_myPackageManager->getSearchPath());
            } catch (asl::Exception & ex) {
                ex.appendWhat(string("while loading movie src='") +
                        theMovie->get<ImageSourceTag>() + "', "+
                        "searching in '"+_myPackageManager->getSearchPath()+"'");
                throw ex;
            }
        }
        theMovie->readFrame(theCurrentTime);

        // load/subload texture
        ImagePtr myMovieImage = dynamic_cast_Ptr<Image>(theMovie);
        if (theMovie->getGraphicsId()) {
            if (theMovie->isImageNewerThanTexture()) {
                updateImageData(myMovieImage);
            }
        } else {
            setupImage(myMovieImage);
        }
    }

    void
    TextureManager::loadCaptureFrame(CapturePtr theCapture) {
        MAKE_SCOPE_TIMER(TextureManager_loadCaptureFrame);

        // First time load, or source has changed
        if (theCapture->reloadRequired()) {
            try {
                theCapture->load(_myPackageManager->getSearchPath());
            } catch (asl::Exception & ex) {
                ex.appendWhat(string("while loading movie src='") +
                        theCapture->get<ImageSourceTag>() + "', "+
                        "searching in '"+_myPackageManager->getSearchPath()+"'");
                throw ex;
            }
        }
        theCapture->readFrame();

        // load/subload texture
        ImagePtr myCaptureImage = dynamic_cast_Ptr<Image>(theCapture);
        if (theCapture->getGraphicsId()) {
            if (theCapture->isImageNewerThanTexture()) {
                updateImageData(myCaptureImage);
            }
        } else {
            setupImage(myCaptureImage);
        }
    }

    void
    TextureManager::setupTextures() {
        unsigned myImageCount = _myImageList->childNodesLength();
        for (unsigned i = 0; i < myImageCount; ++i) {
            dom::NodePtr myImageNode = _myImageList->childNode(i);
            if (myImageNode->nodeType() == dom::Node::ELEMENT_NODE) {
                setupImage(myImageNode->getFacade<Image>());
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

    void TextureManager::rebind(ImagePtr theImage) {
        // Delegate to my ResourceManager
        _myResourceManager->rebindTexture(theImage);
    }

    unsigned TextureManager::getMaxTextureUnits() const {
        // Delegate to my ResourceManager
        return _myResourceManager->getMaxTextureUnits();
    }

    void TextureManager::setPriority(Image * theImage, float thePriority) {
        // Delegate to my ResourceManager
        _myResourceManager->setTexturePriority(theImage, thePriority);
    }

    void TextureManager::unbindTexture(Image * theImage) {
        // Delegate to my ResourceManager
        AC_TRACE << "Unbinding Texture " << theImage->getGraphicsId();
        _myResourceManager->unbindTexture(theImage);
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
