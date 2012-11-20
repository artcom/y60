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
*/
//
//   $RCSfile: I60Header.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:00 $
//
//
//=============================================================================

#ifndef _ac_y60_I60Header_h_
#define _ac_y60_I60Header_h_

#include "y60_image_settings.h"

namespace y60 {

    const unsigned long CURRENT_IMAGE_FORMAT_VERSION = 2; // including layers
    extern Y60_IMAGE_DECL const char * I60_MAGIC_NUMBER; // defined in Image.cpp = "a+c "

    struct I60Header {
        I60Header() : headersize(sizeof(I60Header)), version(CURRENT_IMAGE_FORMAT_VERSION) {
            magicnumber[0] = I60_MAGIC_NUMBER[0];
            magicnumber[1] = I60_MAGIC_NUMBER[1];
            magicnumber[2] = I60_MAGIC_NUMBER[2];
            magicnumber[3] = I60_MAGIC_NUMBER[3];
        }

        bool checkMagicNumber() {
            return (magicnumber[0] == I60_MAGIC_NUMBER[0] &&
                    magicnumber[1] == I60_MAGIC_NUMBER[1] &&
                    magicnumber[2] == I60_MAGIC_NUMBER[2] &&
                    magicnumber[3] == I60_MAGIC_NUMBER[3]);
        }
        bool checkVersion() {
            return (version == CURRENT_IMAGE_FORMAT_VERSION);
        }

        char magicnumber[4];     ///< Magic number is I60_MAGIC_NUMBER;
        unsigned headersize;     ///< Headersize in byte
        unsigned version;        ///< Version (current version is CURRENT_IMAGE_FORMAT_VERSION)
        unsigned encoding;       ///< encoding (See enum in CompressionType.h)
        unsigned width;          ///< original imagewidth (actual data size is always padded to power of two)
        unsigned height;         ///< original imageheight (actual data size is always padded to power of two)
        unsigned layercount;     ///< layer count
    };
}

#endif
