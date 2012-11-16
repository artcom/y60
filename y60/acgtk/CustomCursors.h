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

#ifndef ACGTK_CUSTOM_CURSORS_INCLUDED
#define ACGTK_CUSTOM_CURSORS_INCLUDED

#include "y60_acgtk_settings.h"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4413 4244 4512)
#endif //defined(_MSC_VER)
#include <gdkmm.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <iostream>

using namespace std;

namespace acgtk {

class Y60_ACGTK_DECL CustomCursors {
    public:
        static void init();

        static Gdk::Cursor AC_ADD_POINT;
        static Gdk::Cursor AC_EDIT_ANGLE;
        static Gdk::Cursor AC_EDIT_ANGLE1;
        static Gdk::Cursor AC_EDIT_ANGLE2;
        static Gdk::Cursor AC_EDIT_ANGLE3;

    private:
};

Gdk::Cursor createCustomCursor(unsigned char * theBitmap, unsigned char * theMask,
        int theWidth, int theHeight, int theHotX, int theHotY);
} // end of namespace

#endif // ACGTK_CUSTOM_CURSORS_INCLUDED
