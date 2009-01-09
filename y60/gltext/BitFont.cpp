//=============================================================================
// Copyright (C) 2003 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: BitFont.cpp,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2004/09/29 08:50:54 $
//
//  Description: A simple renderer.
//
//=============================================================================

// own header
#include "BitFont.h"

#include <limits>

static inline void
insert(int x, int y, int & xmi, int & ymi, int & xma, int & yma) {
    xmi = x < xmi ? x : xmi;
    ymi = y < ymi ? y : ymi;
    xma = x > xma ? x : xma;
    yma = y > yma ? y : yma;
}

void
getStringSize(const BitFont & font, const char * text,
              int & x_ret, int & y_ret,
              int & w_ret, int & h_ret) {
    int xmi = std::numeric_limits<int>::max();
    int ymi = std::numeric_limits<int>::max();
    int xma = -std::numeric_limits<int>::max();
    int yma = -std::numeric_limits<int>::max();
    int base_x = 0;

    for (const char * b = text; *b != '\0'; b++) {
        if (*b < font.first || *b >= font.first + font.num_chars) {
            continue;
        }

        const BitChar * ch = font.ch[*b - font.first];

        if (ch) {
            insert(base_x - int(ch->xorig), -int(ch->yorig), xmi, ymi, xma, yma);
            insert(base_x - int(ch->xorig + ch->width), -int(ch->yorig + ch->height),
                   xmi, ymi, xma, yma);
            base_x += (int) ch->advance;
        }
    }
    x_ret = xmi; y_ret = ymi; w_ret = xma - xmi; h_ret = yma - ymi;
}

void
getFontSize(int & ascent, int & descent, const BitFont & font) {
    ascent = font.ascent;
    descent = font.descent;
}
