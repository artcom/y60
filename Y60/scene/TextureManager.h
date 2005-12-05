//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_TextureManager_h_
#define _ac_y60_TextureManager_h_

#include <y60/ITextureManager.h>
#include <asl/Exception.h>
#include <asl/PackageManager.h>
#include <dom/typedefs.h>
#include "ResourceManager.h"
#include "MemoryResourceManager.h"

class PLAnyPicDecoder;

namespace y60 {

    class Image;
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
            // TextureManager(const std::string & theTexturePath);
            virtual ~TextureManager();

            void setSelf(const asl::Ptr<ITextureManager> & theSelf);
            asl::WeakPtr<ITextureManager> getSelf() const;
            void setImageList(dom::NodePtr theImageListNode);

            void setupTextures();
            void loadMovieFrame(asl::Ptr<Movie, dom::ThreadingModel> theMovie,
                                double theCurrentTime = -1);
            void loadCaptureFrame(asl::Ptr<Capture, dom::ThreadingModel> theCapture);
            asl::Ptr<Image, dom::ThreadingModel> getImage(const std::string & theImageId) const;
            asl::Ptr<Image, dom::ThreadingModel> findImage(const std::string & theImageId) const;
            // Delegates to virtual methods of ResourceManager

            virtual int getMaxTextureSize(int theDimensions) const; 
            void updateImageData(asl::Ptr<Image, dom::ThreadingModel> theImage);
            void rebind(asl::Ptr<Image, dom::ThreadingModel> theImage);
            unsigned getMaxTextureUnits() const;
            void setPriority(Image * theImage, float thePriority);           
            void unbindTexture(Image * theImage);
            void unbindTextures();
            void update();
            asl::PackageManagerPtr getPackageManager() const {
                return _myPackageManager;
            }

            void setPackageManager(asl::PackageManagerPtr thePackageManager) {
                _myPackageManager = thePackageManager;
            }

            static asl::Ptr<TextureManager> create(); 
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
            virtual unsigned setupImage(asl::Ptr<Image, dom::ThreadingModel> theImage); 
            dom::NodePtr  _myImageList;
            ResourceManager * _myResourceManager;
            int _myResourceManagerCount;
        private:
            TextureManager();
            void uploadTexture(asl::Ptr<Image, dom::ThreadingModel> theImage);
            asl::WeakPtr<ITextureManager> _mySelf;
            asl::PackageManagerPtr _myPackageManager;
            unsigned _myMaxTextureSize;
            MemoryResourceManagerPtr _myMemoryResourceManager;
    };
    typedef asl::Ptr<TextureManager> TextureManagerPtr;
}
#endif // _ac_y60_TextureManager_h_

