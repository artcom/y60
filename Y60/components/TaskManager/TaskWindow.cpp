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
//   $RCSfile: TaskWindow.cpp,v $
//   $Author: valentin $
//   $Revision: 1.14 $
//   $Date: 2005/03/08 10:20:39 $
//
//
//=============================================================================

#include "TaskWindow.h"

#include <winuser.h>

#if defined(_MSC_VER)
#    pragma warning(push,1)
#endif
#include <paintlib/win/plwinbmp.h>
#include <paintlib/plpngenc.h>
#include <paintlib/Filter/plfilterfliprgb.h>
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#include <asl/base/Exception.h>

using namespace std;

#define DB(x) // x

namespace y60 {

    TaskWindow::TaskWindow() : _myHandle(0) {
    }

    TaskWindow::TaskWindow(HWND theHandle) : _myHandle(theHandle) {
    }

    TaskWindow::TaskWindow(std::string theWindowTitle) {
        _myHandle = FindWindow(0, theWindowTitle.c_str());
        if (!_myHandle) {
            throw asl::Exception(string("Could not find window: ") + theWindowTitle, PLUS_FILE_LINE);
        }

    }

    TaskWindow::~TaskWindow() {
    }

    string
    TaskWindow::getName() const {
        char myWindowName[1024];
        GetWindowText(_myHandle, myWindowName, 1024);
        return string(myWindowName);
    }

    void
    TaskWindow::setVisible(bool theShowFlag) {
        if (theShowFlag) {
            ShowWindow(_myHandle, SW_SHOW);
        } else {
            ShowWindow(_myHandle, SW_HIDE);
        }
    }

    bool
    TaskWindow::isVisible() const {
        return 0 != IsWindowVisible(_myHandle);
    }

    void
    TaskWindow::setAlwaysOnTop() {
        SetWindowPos(_myHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW);
    }

    void
    TaskWindow::validateRect() {
        ValidateRect(_myHandle, 0);
    }

    void
    TaskWindow::hideDecoration() {
        SetWindowLong(_myHandle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    }

    void
    TaskWindow::enable() {
        EnableWindow(_myHandle, true);
    }

    void
    TaskWindow::disable() {
        EnableWindow(_myHandle, false);
    }

    void
    TaskWindow::close() {
        SendMessage(_myHandle, WM_CLOSE, 0, 0);
    }

    void
    TaskWindow::destroy() {
        SendMessage(_myHandle, WM_DESTROY, 0, 0);
    }

    void
    TaskWindow::activate() {
        SetForegroundWindow(_myHandle);
    }

    void
    TaskWindow::capture(std::string theFilename) {
        DB(cerr << "capture: " << getWindowName(_myHandle) << " to " << theFilename << endl;)
        activate();
        RECT myWindowRectangle;
        GetWindowRect(_myHandle, &myWindowRectangle);
        unsigned myWindowSizeX = myWindowRectangle.right  - myWindowRectangle.left;
        unsigned myWindowSizeY = myWindowRectangle.bottom - myWindowRectangle.top;

        HDC myWindowDC = GetWindowDC(_myHandle);
        HDC myMemoryDC = CreateCompatibleDC(myWindowDC);
        HBITMAP myBitmapHandle = CreateCompatibleBitmap(myWindowDC, myWindowSizeX, myWindowSizeY);

        if (!SelectObject(myMemoryDC, myBitmapHandle)) {
            throw asl::Exception("Could not select bitmap into memory device context", PLUS_FILE_LINE);
        }

        BitBlt(myMemoryDC, 0, 0, myWindowSizeX, myWindowSizeY, myWindowDC, 0, 0, SRCCOPY);
        PLWinBmp myPlBitmap;
        myPlBitmap.CreateFromHBitmap(myBitmapHandle);
        myPlBitmap.ApplyFilter(PLFilterFlipRGB());
        PLPNGEncoder myEncoder;
        myEncoder.MakeFileFromBmp(theFilename.c_str(), &myPlBitmap);
	    DeleteDC(myMemoryDC);
	    DeleteObject(myBitmapHandle);
	    ReleaseDC(_myHandle, myWindowDC);
    }

    void
    TaskWindow::resize(int theX, int theY) {
        if (isVisible()) {
            SetWindowPos(_myHandle, HWND_TOP, 0, 0, theX, theY, SWP_NOMOVE);
        }
    }

    void
    TaskWindow::setPosition(const asl::Vector2i & thePosition) {
        RECT myWindowRectangle;
        GetWindowRect(_myHandle, &myWindowRectangle);
        MoveWindow(_myHandle, thePosition[0], thePosition[1],
                   myWindowRectangle.right - myWindowRectangle.left,
                   myWindowRectangle.bottom - myWindowRectangle.top, TRUE);
    }

    asl::Vector2i
    TaskWindow::getPosition() const {
        WINDOWPLACEMENT myPlacement;
        myPlacement.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(_myHandle, &myPlacement);
        return asl::Vector2i(myPlacement.rcNormalPosition.left, myPlacement.rcNormalPosition.top);
    }

    void
    TaskWindow::maximize() {
        ShowWindow(_myHandle, SW_SHOWMAXIMIZED);
    }

    void
    TaskWindow::minimize() {
        if (isVisible()) {
            ShowWindow(_myHandle, SW_SHOWMINIMIZED);
        }
    }

    void
    TaskWindow::restore() {
        ShowWindow(_myHandle, SW_RESTORE);
    }

    void
    TaskWindow::setAlpha(float theAlpha) {
        SetWindowLong(_myHandle, GWL_EXSTYLE,
                      GetWindowLong(_myHandle, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(_myHandle, 0, static_cast<BYTE>(theAlpha * 255.0f), LWA_ALPHA);
    }

    void
    TaskWindow::fade(float theTime) {
        AnimateWindow(_myHandle, DWORD(theTime * 1000), AW_BLEND | AW_HIDE);
    }
}
