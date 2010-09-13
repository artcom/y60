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
#include "TextureAtlas.h"
#include "PixelEncoding.h"

#include <asl/base/Exception.h>
#include <netsrc/texture-atlas/TexturePacker.h>

using namespace asl;
using namespace std;

namespace y60 {
    asl::Ptr<TextureAtlas> 
    TextureAtlas::generate(const std::vector<std::string> & theNames, 
                           const Subtextures & theBitmaps,
                           bool thePixelBorderFlag, bool theForcePowerOfTwoFlag)
    {
        if (theNames.size() != theBitmaps.size()) {
            throw Exception("mismatched array sizes");
        }
        TEXTURE_PACKER::TexturePacker *tp = TEXTURE_PACKER::createTexturePacker();
        //
        // Next inform the system how many textures you want to pack.
        //
        tp->setTextureCount(theNames.size());
        //
        // Now, add each texture's width and height in sequence 1-10
        //
        for (Subtextures::const_iterator it = theBitmaps.begin(); it != theBitmaps.end(); ++it) { 
            tp->addTexture((*it)->width(),(*it)->height());
        }
        //
        // Next you pack them .
        //
        int atlasWidth;
        int atlasHeight;
        tp->packTextures(atlasWidth,atlasHeight,theForcePowerOfTwoFlag,thePixelBorderFlag);
        
        // create master texture
        TextureAtlasPtr myAtlas(new TextureAtlas(atlasWidth, atlasHeight));

        AC_TRACE << "creating texture atlas (" << atlasWidth << " x " << atlasHeight << ")";
        // blit subtextures
        for (asl::AC_SIZE_TYPE i = 0; i < theBitmaps.size(); ++i) {
            int targetX;
            int targetY;
            int targetWidth;
            int targetHeight;

            bool rotated = tp->getTextureLocation(i, targetX, targetY, targetWidth, targetHeight);
            //TODO: supported rotated bitmaps
            AC_TRACE << "blitting bitmap '" << theNames[i] << "' (" << theBitmaps[i]->width() << " x " << theBitmaps[i]->height() << ")";
            AC_TRACE << "   into (" << targetX << ", " << targetY << " )";
            AC_TRACE << "   new size: " << targetWidth << " x " << targetHeight << "(rotated: " << rotated << ")";

            //dom::ValuePtr myRasterValue = dynamic_cast_Ptr<dom::ValueBase>(theBitmaps[i]);
            for (AC_SIZE_TYPE y = 0; y < theBitmaps[i]->height(); ++y) {
                for (AC_SIZE_TYPE x = 0; x < theBitmaps[i]->width(); ++x) {
                    asl::Vector4f myPixel = theBitmaps[i]->getPixel(x,y);
                    if (rotated) {
                        myAtlas->_masterRaster->setPixel(y+targetX,x+targetY,myPixel);
                    } else {
                        myAtlas->_masterRaster->setPixel(x+targetX,y+targetY,myPixel);
                    }
                }
            }
        }
        TEXTURE_PACKER::releaseTexturePacker(tp);
        return myAtlas;
    };

    TextureAtlas::TextureAtlas(asl::AC_SIZE_TYPE width, asl::AC_SIZE_TYPE height) 
    {
        _masterRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(createRasterValue(y60::RGBA, width, height));
        _masterRaster->fillRect(0,0,width,height,asl::Vector4f(1,1,1,1));
    };
}

