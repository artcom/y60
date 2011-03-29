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
#include <asl/base/file_functions.h>
#include <asl/dom/Nodes.h>
#include <asl/math/numeric_functions.h>
#include <netsrc/texture-atlas/TexturePacker.h>

#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>

#include "PixelEncoding.h"
#include "ImageLoader.h"

using namespace asl;
using namespace std;

namespace y60 {
    TextureAtlas::TextureAtlas(const Subtextures & theBitmaps,
                 bool thePixelBorderFlag, bool theForcePowerOfTwoFlag)
    {
        TEXTURE_PACKER::TexturePacker *tp = TEXTURE_PACKER::createTexturePacker();
        //
        // Next inform the system how many textures you want to pack.
        //
        tp->setTextureCount(theBitmaps.size());
        //
        // Now, add each texture's width and height in sequence 1-10
        //
        for (Subtextures::const_iterator it = theBitmaps.begin(); it != theBitmaps.end(); ++it) { 
            tp->addTexture((it->second)->width(),(it->second)->height());
        }
        //
        // Next you pack them .
        //
        {
            int atlasWidth;
            int atlasHeight;
            tp->packTextures(atlasWidth,atlasHeight,theForcePowerOfTwoFlag, thePixelBorderFlag);
            if (atlasWidth > 8000 || atlasHeight > 8000) {
                AC_WARNING << "There might be problems with images larger than 8kx8k. The dimensions resulting from packTextures are " << atlasWidth << "x" << atlasHeight << ". Reduce the number of textures in map.";
            }
            AC_TRACE << "creating texture atlas (" << atlasWidth << " x " << atlasHeight << ")";
            _masterRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(createRasterValue(y60::RGBA, atlasWidth, atlasHeight));
            _masterRaster->fillRect(0,0,atlasWidth,atlasHeight,asl::Vector4f(1,1,1,1));
        }
        
        // blit subtextures
        int index = 0;
        for (Subtextures::const_iterator it=theBitmaps.begin(); it != theBitmaps.end(); ++it) {
            int targetX;
            int targetY;
            int targetWidth;
            int targetHeight;
            
            const dom::ResizeableRasterPtr & curBitmap = it->second;

            bool rotated = tp->getTextureLocation(index++, targetX, targetY, targetWidth, targetHeight);
            //TODO: supported rotated bitmaps
            AC_TRACE << "blitting bitmap '" << it->first << "' (" << curBitmap->width() << " x " << curBitmap->height() << ")";
            AC_TRACE << "   into (" << targetX << ", " << targetY << " )";
            AC_TRACE << "   new size: " << targetWidth << " x " << targetHeight << "(rotated: " << rotated << ")";

            //dom::ValuePtr myRasterValue = dynamic_cast_Ptr<dom::ValueBase>(theBitmaps[i]);
            int firstRowCol = thePixelBorderFlag ? -1 : 0;
            int lastRow = thePixelBorderFlag ? curBitmap->height()+1 : curBitmap->height();
            int lastCol = thePixelBorderFlag ? curBitmap->width()+1 : curBitmap->width();
            bool fillAlpha = ! curBitmap->hasAlpha();

            for (asl::AC_OFFSET_TYPE y = firstRowCol; y < lastRow; ++y) {
                for (asl::AC_OFFSET_TYPE x = firstRowCol; x < lastCol; ++x) {
                    asl::Vector4f myPixel = curBitmap->getPixel(clamp(x, static_cast<AC_OFFSET_TYPE>(0), static_cast<AC_OFFSET_TYPE>(curBitmap->width()-1)),
                                                                clamp(y, static_cast<AC_OFFSET_TYPE>(0), static_cast<AC_OFFSET_TYPE>(curBitmap->height()-1)));
                    if (fillAlpha) {
                        myPixel[3] = 1.0f;
                    }
                    if (rotated) {
                        _masterRaster->setPixel(y+targetX,targetY+lastCol+firstRowCol-1-x, myPixel);
                    } else {
                        _masterRaster->setPixel(x+targetX,y+targetY,myPixel);
                    }
                }
            }
            AC_TRACE << "creating UV Translation for " << it->first << " rotated:" << rotated;
            AC_TRACE << "        size: " << curBitmap->getSize();
            AC_TRACE << "    position: " << Vector2<AC_SIZE_TYPE>(targetX, targetY);
            asl::Matrix4f transMatrix = createUVTranslation(_masterRaster->getSize(), curBitmap->getSize(), 
                           Vector2<AC_SIZE_TYPE>(targetX, targetY), rotated);
            _translations.insert(make_pair(it->first, Subtexture(transMatrix, curBitmap->getSize())));
        }
        TEXTURE_PACKER::releaseTexturePacker(tp);
    };

    const dom::ResizeableRasterPtr 
    TextureAtlas::getRaster() const {
        if (!_masterRaster) {
            // load the raster
            ImageLoader imageLoader(_masterRasterPath.toLocale());
            _masterRaster = imageLoader.getRaster();
        }
        return _masterRaster;
    }

    asl::Matrix4f 
    TextureAtlas::createUVTranslation(const asl::Vector2<asl::AC_SIZE_TYPE> & theAtlasSize,
            const asl::Vector2<asl::AC_SIZE_TYPE> & theBitmapSize, 
            const asl::Vector2<asl::AC_SIZE_TYPE> & theBitmapPosition,
            bool theRotatedFlag) 
    {
        asl::Matrix4f myTranslation;
        asl::Vector2f atlasSize(static_cast<float>(theAtlasSize[0]), static_cast<float>(theAtlasSize[1]));

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
    TextureAtlas::findTextureSize(const std::string & theTextureName, asl::Vector2<AC_SIZE_TYPE> & theSize) const {
       UVTranslations::const_iterator it = _translations.find(theTextureName);
       if (it == _translations.end()) {
           AC_WARNING << "Texture " << theTextureName << " not found in atlas";
           return false;
       }
       theSize = it->second.size;
       return true;
    }

    bool 
    TextureAtlas::findTextureTranslation(const std::string & theTextureName, asl::Matrix4f & theTranslation) const {
       UVTranslations::const_iterator it = _translations.find(theTextureName);
       if (it == _translations.end()) {
           AC_WARNING << "Texture " << theTextureName << " not found in atlas";
           return false;
       }
       theTranslation = it->second.matrix;
       return true;
    }

    bool 
    TextureAtlas::containsTexture(const std::string & theTextureName) const {
        return _translations.find(theTextureName) != _translations.end();
    }

    TextureAtlas::TextureAtlas(const asl::Path & theFilename, PackageManagerPtr thePackageManager) {
        dom::Document doc;
        asl::Ptr<ReadableStreamHandle> mySource;
        asl::Ptr<ReadableFile> fileSource;

        if (thePackageManager) {
            mySource = thePackageManager->readStream(theFilename.toLocale());
        }
        if (!mySource && fileExists(theFilename.toLocale())) {
            mySource = asl::Ptr<AlwaysOpenReadableFileHandle>(new AlwaysOpenReadableFileHandle(theFilename.toLocale()));
        }
        if (!mySource) {
            if (thePackageManager) {
                throw Exception(std::string("atlas definition '") + theFilename.toLocale()+ "' not found in " +
                        thePackageManager->getSearchPath(), PLUS_FILE_LINE);
            } else {
                throw Exception(std::string("atlas definition '") + theFilename.toLocale() + "' not found in " +
                        "current directory.", PLUS_FILE_LINE);
            }
        }
        std::string myXMLFile;
        mySource->getStream().readString(myXMLFile, mySource->getStream().size(), 0);
        doc.parse(myXMLFile);
        
        std::string atlasDirectory = getDirectoryPart(theFilename.toUTF8());
        _masterRasterPath = Path(atlasDirectory+ doc.childNode("TextureAtlas")->getAttributeString("src"), UTF8);
        // load the translation table
        for (AC_SIZE_TYPE i = 0; i < doc.childNode("TextureAtlas")->childNodesLength("Subtexture"); ++i) {
            const dom::NodePtr subTextureNode =  doc.childNode("TextureAtlas")->childNode("Subtexture",i);
            _translations.insert(make_pair(subTextureNode->getAttributeString("name"),
                        Subtexture(subTextureNode->getAttributeValue<asl::Matrix4f>("matrix"), 
                                   subTextureNode->getAttributeValue<asl::Vector2<AC_SIZE_TYPE> >("size"))));
        }
    }

    void
    TextureAtlas::saveToFile(const asl::Path & theFilename) const {
        // save the bitmap
        Path bitmapPath(removeExtension(theFilename.toUTF8())+".png", UTF8);
        PLAnyBmp myBmp;
        PLPixelFormat pf;
        mapPixelEncodingToFormat(y60::RGBA, pf);

        myBmp.Create( getRaster()->width(), getRaster()->height(), pf,
                const_cast<unsigned char*>(getRaster()->pixels().begin()), getRaster()->width() * 4);
        PLPNGEncoder myEncoder;
        myEncoder.MakeFileFromBmp(bitmapPath.toLocale().c_str(), &myBmp);
        // save the translation table
        dom::Document doc;
        dom::NodePtr root = doc.appendChild(dom::Element("TextureAtlas"));
        root->appendAttribute("src", getFilenamePart(bitmapPath.toUTF8()));
        UVTranslations::const_iterator it = _translations.begin();
        for (; it != _translations.end(); ++it) {
            dom::NodePtr subtexture = root->appendChild(dom::Element("Subtexture"));
            subtexture->appendAttribute("name", it->first);
            subtexture->appendAttribute("matrix", it->second.matrix);
            subtexture->appendAttribute("size", it->second.size);
        };
        std::ofstream myFile(theFilename.toLocale().c_str(), std::ios_base::trunc | std::ios_base::out);
        if (!myFile) {
            throw Exception(std::string("Can not open file '") + theFilename.toLocale() + "' for writing", PLUS_FILE_LINE);
        }
        doc.print(myFile, "", true);
        if (!myFile) {
            throw Exception(std::string("Could not write text file '") + theFilename.toLocale() + "'", PLUS_FILE_LINE);
        }
    }

}

