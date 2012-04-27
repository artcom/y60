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

#ifndef Y60_TEXTURE_INCLUDED
#define Y60_TEXTURE_INCLUDED

#include "y60_scene_settings.h"

#include "TextureTags.h"
#include <y60/base/CommonTags.h>
#include <y60/base/NodeValueNames.h>

#include <asl/dom/AttributePlug.h>
#include <asl/dom/Facade.h>


namespace y60 {

    class IResourceManager;

    /**
     * Exception
     */
    DEFINE_EXCEPTION(TextureException, asl::Exception);

    /**
     * @ingroup y60texture
     * Facade for a texture node in the dom.
     */
    class Y60_SCENE_DECL Texture :
        public dom::Facade,
        public IdTag::Plug,
        public NameTag::Plug,
        public TextureImageIdTag::Plug,
        public TexturePixelFormatTag::Plug,
        public TextureMipmapTag::Plug,
        public TextureAnisotropyTag::Plug,
        public TextureMatrixTag::Plug,
        public TextureColorBiasTag::Plug,
        public TextureColorScaleTag::Plug,
        public TextureWrapModeTag::Plug,
        public TextureMinFilterTag::Plug,
        public TextureMagFilterTag::Plug,
        public TextureImageIndexTag::Plug,
        public dom::FacadeAttributePlug<TextureTypeTag>,
        public dom::FacadeAttributePlug<TextureIdTag>,
        public dom::FacadeAttributePlug<TextureInternalFormatTag>,
        public dom::FacadeAttributePlug<TextureWidthTag>,
        public dom::FacadeAttributePlug<TextureHeightTag>,
        public dom::FacadeAttributePlug<TextureDepthTag>,
        public dom::FacadeAttributePlug<TextureNPOTMatrixTag>,
        public dom::FacadeAttributePlug<TextureParamChangedTag>,
        public dom::FacadeAttributePlug<LastActiveFrameTag>
    {
        public:
            Texture(dom::Node & theNode);
            IMPLEMENT_FACADE(Texture);

            virtual ~Texture();

            void unbind();
            void removeTextureFromResourceManager();

            /// Get OpenGL texture ID.
            unsigned getTextureId() const {
                return _myTextureId;
            }

            // PBO usage
            virtual bool usePixelBuffer() const;
            void setPixelBufferId(unsigned theId) {
                _myPixelBufferId = theId;
            }
            unsigned getPixelBufferId() const {
                return _myPixelBufferId;
            }

            /// Return OpenGL texture ID and upload image if necessary.
            unsigned ensureTextureId() {
                return get<TextureIdTag>();
            }

            unsigned applyTexture();
            void applyTextureParams();

            void preload();

            void refTexture();
            void unrefTexture();

            ImagePtr getImage() const;

            TextureType getType() const;
            TextureWrapMode getWrapMode() const;
            TextureSampleFilter getMinFilter() const;
            TextureSampleFilter getMagFilter() const;
            TextureInternalFormat getInternalEncoding() const;

            virtual void registerDependenciesRegistrators();

            /// Get texture mem usage.
            unsigned getTextureMemUsage() const {
                return _myTextureMemUsage;
            }
            void setTextureMemUsage(unsigned theAmount) {
                _myTextureMemUsage = theAmount;
            }
        protected:
            // Some violated our styleguide rule by using a protected member variable (_myResourceManager) here
            // and it promply fell on our feet. Please do not use protected member variables. A derived class
            // might accidently modify it, but you have a hard time to find that out. It is a bit like having
            // a global variable.
        private:
            IResourceManager * _myResourceManager;
            unsigned _myRefCount;
            unsigned _myTextureId;
            unsigned _myPixelBufferId;
            unsigned _myImageNodeVersion;
            unsigned _myTextureMemUsage;

            Texture();

            void registerDependenciesForImageTag();
            void registerDependenciesForTextureWidthUpdate();
            void registerDependenciesForTextureHeightUpdate();
            void registerDependenciesForInternalFormatUpdate();
            void updateDependenciesForInternalFormatUpdate();
            void registerDependenciesForTextureParamChanged();
            void registerDependenciesForTextureTypeUpdate();
            void registerDependenciesForTextureUpdate();

            void calculateInternalFormat();
            void calculateWidth();
            void calculateHeight();
            void calculateTextureType();
            void calculateImageTag();

            /// Get ResourceManager from Scene facade.
            void ensureResourceManager();

            Image & getImageFacade();
            const Image & getImageFacade() const;
    };

    typedef asl::Ptr<Texture, dom::ThreadingModel> TexturePtr;
    typedef asl::WeakPtr<Texture, dom::ThreadingModel> TextureWeakPtr;
}

#endif
