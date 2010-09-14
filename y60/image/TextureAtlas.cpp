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
#include <asl/math/numeric_functions.h>
#include <netsrc/texture-atlas/TexturePacker.h>

using namespace asl;
using namespace std;

namespace y60 {
    TextureAtlas::TextureAtlas(const std::vector<std::string> & theNames, 
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
        {
            int atlasWidth;
            int atlasHeight;
            tp->packTextures(atlasWidth,atlasHeight,theForcePowerOfTwoFlag, thePixelBorderFlag);
            AC_TRACE << "creating texture atlas (" << atlasWidth << " x " << atlasHeight << ")";
            _masterRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(createRasterValue(y60::RGBA, atlasWidth, atlasHeight));
            _masterRaster->fillRect(0,0,atlasWidth,atlasHeight,asl::Vector4f(1,1,1,1));
        }
        
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
            int firstRowCol = thePixelBorderFlag ? -1 : 0;
            int lastRow = thePixelBorderFlag ? theBitmaps[i]->height()+1 : theBitmaps[i]->height();
            int lastCol = thePixelBorderFlag ? theBitmaps[i]->width()+1 : theBitmaps[i]->width();

            for (asl::AC_OFFSET_TYPE y = firstRowCol; y < lastRow; ++y) {
                for (asl::AC_OFFSET_TYPE x = firstRowCol; x < lastCol; ++x) {
                    asl::Vector4f myPixel = theBitmaps[i]->getPixel(clamp(x, static_cast<AC_OFFSET_TYPE>(0), static_cast<AC_OFFSET_TYPE>(theBitmaps[i]->width()-1)),
                                                                    clamp(y, static_cast<AC_OFFSET_TYPE>(0), static_cast<AC_OFFSET_TYPE>(theBitmaps[i]->height()-1)));
                    if (rotated) {
                        _masterRaster->setPixel(y+targetX,x+targetY,myPixel);
                    } else {
                        _masterRaster->setPixel(x+targetX,y+targetY,myPixel);
                    }
                }
            }
            AC_TRACE << "creating UV Translation for " << theNames[i] << " rotated:" << rotated;
            AC_TRACE << "        size: " << theBitmaps[i]->getSize();
            AC_TRACE << "    position: " << Vector2<AC_SIZE_TYPE>(targetX, targetY);
            _translations.insert(make_pair(theNames[i], createUVTranslation(_masterRaster->getSize(), theBitmaps[i]->getSize(), 
                           Vector2<AC_SIZE_TYPE>(targetX, targetY), rotated )));
        }
        TEXTURE_PACKER::releaseTexturePacker(tp);
    };

    asl::Matrix4f 
    TextureAtlas::createUVTranslation(const asl::Vector2<asl::AC_SIZE_TYPE> & theAtlasSize,
            const asl::Vector2<asl::AC_SIZE_TYPE> & theBitmapSize, 
            const asl::Vector2<asl::AC_SIZE_TYPE> & theBitmapPosition,
            bool theRotatedFlag) 
    {
        asl::Matrix4f myTranslation;
        asl::Vector2f atlasSize(theAtlasSize[0], theAtlasSize[1]);

        if (theRotatedFlag) {
            myTranslation.makeIdentity();
            myTranslation.rotateZ(0,-1); // rotate 90° counterclockwise
            myTranslation.scale(Vector3f(theBitmapSize[1]/atlasSize[0], theBitmapSize[0]/atlasSize[1], 1));
            myTranslation.translate(Vector3f(0,theBitmapSize[0]/atlasSize[1],0)); // correct pivot
        } else {
            myTranslation.makeScaling(Vector3f(theBitmapSize[0]/atlasSize[0], theBitmapSize[1]/atlasSize[1], 1));
        }
        myTranslation.translate(Vector3f(theBitmapPosition[0]/atlasSize[0], theBitmapPosition[1]/atlasSize[1], 0));
        return myTranslation;
    }

    bool 
    TextureAtlas::findTextureTranslation(const std::string & theTextureName, asl::Matrix4f & theTranslation) const {
       UVTranslations::const_iterator it = _translations.find(theTextureName);
       if (it == _translations.end()) {
           return false;
       }
       theTranslation = it->second;
       return true;
    }

}

