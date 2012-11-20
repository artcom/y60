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
//   $RCSfile: SliceViewer.js,v $
//   $Author: danielk $
//   $Revision: 1.97 $
//   $Date: 2005/04/29 13:49:59 $
//
//=============================================================================

//own header
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
