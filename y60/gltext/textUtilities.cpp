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

//own header
#include "textUtilities.h"


#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>

#include <iostream>

using namespace std;

namespace y60 {

    void glDrawString(const char * myText, const BitFont & font) {
        // Save current modes
        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

        // Set new mode
        glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
        glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (const unsigned char * b = (unsigned char *) myText; *b != '\0'; b++) {
            if (*b < font.first || *b >= font.first + font.num_chars) {
                continue;
            }

            const BitChar * ch = font.ch[*b - font.first];

            if (ch) {
                // finally draw
                glBitmap(ch->width, ch->height, ch->xorig, ch->yorig,
                         ch->advance, 0, ch->bitmap);
            }
        }

        glPopClientAttrib();
    }}
