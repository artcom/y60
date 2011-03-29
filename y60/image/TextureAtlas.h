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
#ifndef _ac_y60_TextureAtlas_h_
#define _ac_y60_TextureAtlas_h_

#include "y60_image_settings.h"

#include <vector>

#include <asl/dom/Value.h>
#include <asl/math/Matrix4.h>
#include <asl/base/Path.h>
#include <asl/zip/PackageManager.h>

namespace y60 {
    /**
    * @ingroup y60image
    * Texture atlas creation and use.
    */
    class Y60_IMAGE_DECL TextureAtlas {
        public:
            typedef std::map<std::string, dom::ResizeableRasterPtr> Subtextures;

            TextureAtlas(const Subtextures & theBitmaps,
                         bool thePixelBorderFlag = true, bool theForcePowerOfTwoFlag = false);

            TextureAtlas(const asl::Path & theFilename, asl::Ptr<asl::PackageManager> thePackageManager = asl::Ptr<asl::PackageManager>());

            const dom::ResizeableRasterPtr getRaster() const;

            bool findTextureTranslation(const std::string & theTextureName, asl::Matrix4f & theTranslation) const;
            bool findTextureSize(const std::string & theTextureName, asl::Vector2<asl::AC_SIZE_TYPE> & theSize) const;
            bool containsTexture(const std::string & theTextureName) const;
            asl::AC_SIZE_TYPE getTextureCount() const { return _translations.size(); };
            void saveToFile(const asl::Path & theFilename) const;
            const asl::Path & getRasterPath() const { return _masterRasterPath; }; 

        private:
            struct Subtexture {
                Subtexture(const asl::Matrix4f & theMatrix, const asl::Vector2<asl::AC_SIZE_TYPE> & theSize) :
                    matrix(theMatrix), size(theSize) {}
                asl::Matrix4f matrix;
                asl::Vector2<asl::AC_SIZE_TYPE> size;
            };

            typedef std::map<std::string, Subtexture> UVTranslations;
            TextureAtlas();
            TextureAtlas(const TextureAtlas &);
            TextureAtlas operator=(const TextureAtlas &);

            friend class TextureAtlasUVTest; // for tests 
            static asl::Matrix4f createUVTranslation(const asl::Vector2<asl::AC_SIZE_TYPE> & theAtlasSize,
                    const asl::Vector2<asl::AC_SIZE_TYPE> & theBitmapSize, const asl::Vector2<asl::AC_SIZE_TYPE> & theBitmapPosition,
                    bool theRotatedFlag);

            asl::Path _masterRasterPath;
            mutable dom::ResizeableRasterPtr _masterRaster; // lazy loading
            UVTranslations _translations;
    };
    typedef asl::Ptr<TextureAtlas> TextureAtlasPtr; 
}

#endif
