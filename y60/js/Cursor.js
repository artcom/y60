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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $RCSfile: Cursor.js,v $
//   $Author: valentin $
//   $Revision: 1.1 $
//   $Date: 2004/10/04 10:39:39 $
//
//
//=============================================================================

function setCursor(theImageString, theSize) {
    var myCursorDoc  = Node.createDocument();
    myCursorDoc.parse("<CursorDesc data='" + theImageString + "' size='" + theSize + "' hotSize='[0,0]' />");
    var myCursorNode = myCursorDoc.firstChild;
    window.createCursor(myCursorNode);

}

function resetCursor() {
    window.resetCursor();
}

//      Paint instructions:
//      the Cursorimage must have dimensions dividable by 8
//
//      Paint the image as follows:
//      'X' = black
//      ' ' = transparent
//      '.' = white

var myArrowImage = "X                               " +
                   "XX                              " +
                   "X.X                             " +
                   "X..X                            " +
                   "X...X                           " +
                   "X....X                          " +
                   "X.....X                         " +
                   "X......X                        " +
                   "X.......X                       " +
                   "X........X                      " +
                   "X.....XXXXX                     " +
                   "X..X..X                         " +
                   "X.X X..X                        " +
                   "XX  X..X                        " +
                   "X    X..X                       " +
                   "     X..X                       " +
                   "      X..X                      " +
                   "      X..X                      " +
                   "       XX                       " +
                   "                                " +
                   "                                " +
                   "                                " +
                   "                                " +
                   "                                " +
                   "                                " +
                   "                                " +
                   "                                " +
                   "                                " +
                   "                                " +
                   "                                " +
                   "                                " +
                   "                                ";