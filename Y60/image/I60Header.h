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
//   $RCSfile: I60Header.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:00 $
//
//
//=============================================================================

#ifndef _ac_y60_I60Header_h_
#define _ac_y60_I60Header_h_

namespace y60 {

    const unsigned long CURRENT_IMAGE_FORMAT_VERSION = 2; // including layers
    extern const char * I60_MAGIC_NUMBER; // defined in Image.cpp = "a+c " 

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
