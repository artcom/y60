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
//
//   $RCSfile: M60Header.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:04 $
//
//
//=============================================================================

#ifndef _ac_y60_M60Header_h_
#define _ac_y60_M60Header_h_

#include "y60_video_settings.h"

#include <asl/base/Stream.h>

namespace y60 {

    const unsigned long CURRENT_MOVIE_FORMAT_VERSION = 4;
    const char * M60_MAGIC_NUMBER = "m60 ";

    /**
     * @ingroup y60-video
     * Header for M60 Files.
     *
     */
    struct M60Header {

        // For reading movies
        M60Header(const asl::ReadableStream & theStream) {
            // TODO: This only works on windows and linux, for MacOS we need to swap some
            //       bytes around.
            theStream.readData(*this, 0);
        }

        // For writing movies
        M60Header() : headersize(sizeof(M60Header)), version(CURRENT_MOVIE_FORMAT_VERSION) {
            magicnumber[0] = M60_MAGIC_NUMBER[0];
            magicnumber[1] = M60_MAGIC_NUMBER[1];
            magicnumber[2] = M60_MAGIC_NUMBER[2];
            magicnumber[3] = M60_MAGIC_NUMBER[3];
        }

        bool checkMagicNumber() {
            return (magicnumber[0] == M60_MAGIC_NUMBER[0] &&
                    magicnumber[1] == M60_MAGIC_NUMBER[1] &&
                    magicnumber[2] == M60_MAGIC_NUMBER[2] &&
                    magicnumber[3] == M60_MAGIC_NUMBER[3]);
        }

        bool checkVersion() {
            return (version == CURRENT_MOVIE_FORMAT_VERSION);
        }

        bool checkHeadersize() {
            return (headersize == sizeof(M60Header));
        }

        char magicnumber[4];     ///< Magic number is M60_MAGIC_NUMBER;
        unsigned headersize;     ///< Headersize in byte
        unsigned version;        ///< Version (current version is CURRENT_MOVIE_FORMAT_VERSION)
        unsigned framecount;     ///< Number of Frames in this Movie
        float    fps;            ///< frames per second
        unsigned width;          ///< width of source image, e.g. 720 for PAL
        unsigned height;         ///< height of source image, e.g. 576 for PAL
        unsigned framewidth;     ///< power of two for i60 based movies
        unsigned frameheight;    ///< power of two for i60 based movies
        unsigned compression;    ///< @see M60DecoderEncoding
        unsigned pixelformat;    ///< @See PixelEncoding
    };
}

#endif
