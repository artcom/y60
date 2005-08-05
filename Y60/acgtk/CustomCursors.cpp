//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: SliceViewer.js,v $
//   $Author: danielk $
//   $Revision: 1.97 $
//   $Date: 2005/04/29 13:49:59 $
//
//=============================================================================

#include "CustomCursors.h"

#include "cursors/cursor_add_point.xbm"
#include "cursors/cursor_add_point_mask.xbm"

namespace acgtk {
#define INIT_CURSOR(theName, theXBM) \
    theName = createCustomCursor(theXBM ## _bits, theXBM ## _mask_bits, \
                    theXBM ## _width, theXBM ## _height, \
                    theXBM ## _x_hot, theXBM ## _y_hot);

Gdk::Cursor CustomCursors::AC_ADD_POINT;

void
CustomCursors::init() {
    INIT_CURSOR(AC_ADD_POINT, cursor_add_point);
}


Gdk::Cursor 
createCustomCursor( unsigned char * theBitmap, unsigned char * theMask,
        int theWidth, int theHeight, int theHotX, int theHotY)
{
    Glib::RefPtr<Gdk::Bitmap> myBitmap = Gdk::Bitmap::create(reinterpret_cast<char*>(theBitmap),
            theWidth, theHeight);
    Glib::RefPtr<Gdk::Bitmap> myMask = Gdk::Bitmap::create(reinterpret_cast<char*>(theMask),
            theWidth, theHeight);
    Gdk::Color myFGColor("black");
    Gdk::Color myBGColor("white");
    return Gdk::Cursor(myBitmap, myMask, myFGColor, myBGColor, theHotX, theHotY);
}

}
