//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include <asl/Stream.h>

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
