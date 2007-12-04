//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_TextureManager_h_
#define _ac_y60_TextureManager_h_

#include <y60/IResourceManager.h>
#include <y60/ITextureManager.h>
#include <asl/Exception.h>
#include <asl/PackageManager.h>
#include <dom/typedefs.h>
#include "ResourceManager.h"
#include "MemoryResourceManager.h"

class PLAnyPicDecoder;

namespace y60 {

    class Texture;
    typedef asl::Ptr<Texture, dom::ThreadingModel> TexturePtr;

    class Movie;
    class Capture;
    class ResourceManager;

    DEFINE_EXCEPTION(TextureManagerException, asl::Exception);

    /**
     * @ingroup y60scene
     * Performs texture loading and management
     */
    class TextureManager : public ITextureManager {
        public:
            static asl::Ptr<TextureManager> create();

            virtual ~TextureManager();

            void setSelf(const asl::Ptr<ITextureManager> & theSelf);
            asl::WeakPtr<ITextureManager> getSelf() const;
            void setTextureList(dom::NodePtr theTexturesListNode);

            void reloadTextures();
            void loadMovieFrame(asl::Ptr<Movie, dom::ThreadingModel> theMovie,
                                double theCurrentTime = -1);
            void loadCaptureFrame(asl::Ptr<Capture, dom::ThreadingModel> theCapture);
            TexturePtr getTexture(const std::string & theTextureId) const;
            TexturePtr findTexture(const std::string & theTextureId) const;

            // Delegates to virtual methods of ResourceManager
            virtual int getMaxTextureSize(int theDimensions) const; 
            //void updateTextureData(const TexturePtr & theTexture);
            //void setTexturePriority(const TexturePtr & theTexture, float thePriority);           

            void unbindTexture(Texture * theTexture);
            void unbindTextures();
            void validateGLContext(bool theFlag);

            /**
             * Registers theResourceManager with the TextureManager. One Renderer
             * should register its ResourceManager only once and should deregister
             * when it does not want to render the Scene anymore
             * @warn do not call this directly. Use Scene::registerResourceManager instead.
             * @param theResourceManager ResourceManager to register.
             * @return number of registered ResourceManagers
             */
            int registerResourceManager(ResourceManager* theResourceManager); 
            ResourceManager * getResourceManager() { return _myResourceManager; }
            const ResourceManager * getResourceManager() const { return _myResourceManager; }

        protected:
            dom::NodePtr      _myTextureList;
            ResourceManager * _myResourceManager;
            int               _myResourceManagerCount;

        private:
            TextureManager();
            asl::WeakPtr<ITextureManager> _mySelf;
            unsigned _myMaxTextureSize;
            MemoryResourceManagerPtr _myMemoryResourceManager;
    };

    typedef asl::Ptr<TextureManager> TextureManagerPtr;
}

#endif // _ac_y60_TextureManager_h_
