//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: textUtilities.cpp,v $
//   $Author: janbo $
//   $Revision: 1.3 $
//   $Date: 2004/10/18 16:22:56 $
//
//  Description: This class performs texture loading and management.
//
//============================================================================

#include "textUtilities.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

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
