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

#include "cursors/cursor_angle1.xbm"
#include "cursors/cursor_angle1_mask.xbm"
#include "cursors/cursor_angle2.xbm"
#include "cursors/cursor_angle2_mask.xbm"
#include "cursors/cursor_angle3.xbm"
#include "cursors/cursor_angle3_mask.xbm"
#include "cursors/cursor_angle4.xbm"
#include "cursors/cursor_angle4_mask.xbm"

namespace acgtk {
#define INIT_CURSOR(theName, theXBM) \
    theName = createCustomCursor(theXBM ## _bits, theXBM ## _mask_bits, \
                    theXBM ## _width, theXBM ## _height, \
                    theXBM ## _x_hot, theXBM ## _y_hot);

Gdk::Cursor CustomCursors::AC_ADD_POINT;
Gdk::Cursor CustomCursors::AC_EDIT_ANGLE;
Gdk::Cursor CustomCursors::AC_EDIT_ANGLE1;
Gdk::Cursor CustomCursors::AC_EDIT_ANGLE2;
Gdk::Cursor CustomCursors::AC_EDIT_ANGLE3;

void
CustomCursors::init() {
    INIT_CURSOR(AC_ADD_POINT, cursor_add_point);
    INIT_CURSOR(AC_EDIT_ANGLE, cursor_angle1);
    INIT_CURSOR(AC_EDIT_ANGLE1, cursor_angle2);
    INIT_CURSOR(AC_EDIT_ANGLE2, cursor_angle3);
    INIT_CURSOR(AC_EDIT_ANGLE3, cursor_angle4);
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
