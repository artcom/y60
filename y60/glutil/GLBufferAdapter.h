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

#ifndef _ac_render_GLBufferAdapter_h_
#define _ac_render_GLBufferAdapter_h_

#include "y60_glutil_settings.h"

#include <asl/base/Exception.h>
#include <asl/base/Block.h>

#include <y60/scene/Texture.h>
#include <y60/image/Image.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planydec.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include <boost/shared_ptr.hpp>
#include <asl/thread/threadpool.hpp>

namespace y60 {

    enum GLSourceBuffer {
        FRAME_BUFFER,
        DEPTH_BUFFER
    };

    DEFINE_EXCEPTION(GLBufferAdapterException, asl::Exception);
    class Y60_GLUTIL_DECL BufferAdapter {
        public:
            BufferAdapter();
            BufferAdapter(unsigned theWidth, unsigned theHeight, unsigned theComponents);
            virtual ~BufferAdapter();
            virtual void performAction(GLSourceBuffer theSourceBuffer);

            asl::Block & getBlock();
            unsigned getWidth() const {return _myWidth;};
            unsigned getHeight() const {return _myHeight;};
            unsigned getComponents() const {return _myComponents;};
            void setWidth(unsigned theWidth) {_myWidth = theWidth;};
            void setHeight(unsigned theHeight) {_myHeight = theHeight;};
            void setComponents(unsigned theComponents) {_myComponents = theComponents;};
            unsigned getBufferFormat(GLSourceBuffer theSourceBuffer) const;
        protected:
            void alloc(const unsigned myMemorySize);
        private:
            asl::Block _myData;
            unsigned _myWidth;
            unsigned _myHeight;
            unsigned _myComponents;
    };

/* // paintlib image formats
#define PL_FT_WINBMP  1
#define PL_FT_MACPICT 2
#define PL_FT_TARGA   3
#define PL_FT_TIFF    4
#define PL_FT_JPEG    5
#define PL_FT_PNG     6 // supported
#define PL_FT_EPSTIFF 7
#define PL_FT_WMF     8
#define PL_FT_EMF     9
#define PL_FT_PCX    10
#define PL_FT_PGM    11
#define PL_FT_GIF    12
#define PL_FT_PPM    13
#define PL_FT_PSD    14
#define PL_FT_SGI    15
*/

    class Y60_GLUTIL_DECL BufferToFile : public BufferAdapter {
        public:
            BufferToFile();
            BufferToFile(const std::string & theFilename, unsigned theFormat,
                         unsigned theWidth, unsigned theHeight, unsigned theComponents);
            virtual ~BufferToFile();
            virtual void performAction(GLSourceBuffer theSourceBuffer);
            unsigned getFormat() const {return _myFormat;};
            void setFormat(unsigned theFormat) {_myFormat = theFormat;};
            std::string getFilename() const {return _myFilename;};
            void setFilename(std::string theFilename) {_myFilename = theFilename;};
        private:
            void encodeBuffer(const std::string & thePath, unsigned theFormat, boost::shared_ptr<PLAnyBmp> & theBmp);
            std::string _myFilename;
            unsigned _myFormat;
            boost::threadpool::pool _myThreadPool;
    };

    class Y60_GLUTIL_DECL BufferToTexture : public BufferAdapter {
        public:
            BufferToTexture(TexturePtr theTexture, const asl::Vector2i & theOffset, bool theCopyToImageFlag = false);
            virtual ~BufferToTexture();
            virtual void performAction(GLSourceBuffer theSourceBuffer);

        private:
            TexturePtr _myTexture;
            asl::Vector2i _myOffset;
            bool _myCopyToImage;
    };
}

#endif // _ac_render_GLBufferAdapter_h_
