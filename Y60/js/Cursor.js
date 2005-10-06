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

var myArrowImage =      "X                               "+
                        "XX                              "+
                        "X.X                             "+
                        "X..X                            "+
                        "X...X                           "+
                        "X....X                          "+
                        "X.....X                         "+
                        "X......X                        "+
                        "X.......X                       "+
                        "X........X                      "+
                        "X.....XXXXX                     "+
                        "X..X..X                         "+
                        "X.X X..X                        "+
                        "XX  X..X                        "+
                        "X    X..X                       "+
                        "     X..X                       "+
                        "      X..X                      "+
                        "      X..X                      "+
                        "       XX                       "+
                        "                                "+
                        "                                "+
                        "                                "+
                        "                                "+
                        "                                "+
                        "                                "+
                        "                                "+
                        "                                "+
                        "                                "+
                        "                                "+
                        "                                "+
                        "                                "+
                        "                                ";