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

#ifndef _ac_y60_TextureManager_h_
#define _ac_y60_TextureManager_h_

#include "y60_scene_settings.h"

#include <y60/base/IResourceManager.h>
#include <y60/image/ITextureManager.h>
#include <asl/base/Exception.h>
#include <asl/zip/PackageManager.h>
#include <y60/base/typedefs.h>
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
    class Y60_SCENE_DECL TextureManager : public ITextureManager {
        public:
            static asl::Ptr<TextureManager> create();

            virtual ~TextureManager();

            void setSelf(const asl::Ptr<ITextureManager> & theSelf);
            asl::WeakPtr<ITextureManager> getSelf() const;
            void setTextureList(dom::NodePtr theTexturesListNode);

            void loadMovieFrame(asl::Ptr<Movie, dom::ThreadingModel> theMovie,
                                double theCurrentTime = -1);
            void loadCaptureFrame(asl::Ptr<Capture, dom::ThreadingModel> theCapture);
            TexturePtr getTexture(const std::string & theTextureId) const;
            
            // Delegates to virtual methods of ResourceManager
            virtual int getMaxTextureSize(int theDimensions) const;

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

        private:
            TextureManager();
            TexturePtr findTexture(const std::string & theTextureId) const;
            dom::NodePtr      _myTextureList;
            ResourceManager * _myResourceManager;
            int               _myResourceManagerCount;
            asl::WeakPtr<ITextureManager> _mySelf;
            unsigned _myMaxTextureSize;
            MemoryResourceManagerPtr _myMemoryResourceManager;
    };

    typedef asl::Ptr<TextureManager> TextureManagerPtr;
}

#endif // _ac_y60_TextureManager_h_
