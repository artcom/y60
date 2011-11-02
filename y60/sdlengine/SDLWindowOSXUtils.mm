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
// Description:
// Some Helper functions for OSX Window handling. The file is compiled
// as ObjC++ to be able to use Objective-C System Calls in a C++ environment.
// TODO: Improve Exception Handling
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#import <AppKit/AppKit.h>
#include "SDLWindowOSXUtils.h"

std::string setWindowPositionOSX(int theX, int theY) {
    @try {
        NSWindow * window = [[NSApp windows] lastObject];

        int screenHeight = [[NSScreen mainScreen] frame].size.height;
        
        NSPoint p;
        p.x = theX;
        
        // position starts on the lower left corner so it has to be translated up by the screenheight 
        p.y = screenHeight - theY;
        
        if (window != NULL) {
            [window setFrameTopLeftPoint:p];
        }
    } 
    @catch(NSException * e) {
        std::string myExceptionMessage = std::string("Error trying to set NSWindow setFrameTopLeftPosition: ") +
            std::string([[e name] cStringUsingEncoding: NSASCIIStringEncoding]) + std::string(" : ") +
            std::string([[e reason] cStringUsingEncoding: NSASCIIStringEncoding] ); 
        return myExceptionMessage;
    }
        
    return std::string();
}

std::string setShowTaskBarOSX(bool theFlag) {
    @try {
        if (theFlag) {
            [NSApp setPresentationOptions: NSApplicationPresentationDefault];
        } else {
            [NSApp setPresentationOptions: NSApplicationPresentationAutoHideDock|NSApplicationPresentationAutoHideMenuBar];
        }
    } 
    @catch(NSException * e) {
        std::string myExceptionMessage = std::string("Error trying to set NSApplicationPresentationMode: ") +
            std::string([[e name] cStringUsingEncoding: NSASCIIStringEncoding]) + std::string(" : ") +
            std::string([[e reason] cStringUsingEncoding: NSASCIIStringEncoding] ); 
        return myExceptionMessage;
    }
    
    return std::string();
}

std::string getScreenSizeOSX(int &theWidth, int &theHeight) {
    @try {
        theWidth  = [[NSScreen mainScreen] frame].size.width;
        theHeight = [[NSScreen mainScreen] frame].size.height;
    } 
    @catch(NSException * e) {
        std::string myExceptionMessage = std::string("Error trying to set NSWindow setFrameTopLeftPosition: ") +
            std::string([[e name] cStringUsingEncoding: NSASCIIStringEncoding]) + std::string(" : ") +
            std::string([[e reason] cStringUsingEncoding: NSASCIIStringEncoding] ); 
        return myExceptionMessage;
    }
        
    return std::string();
}

