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
//   $RCSfile: TaskWindow.cpp,v $
//   $Author: valentin $
//   $Revision: 1.14 $
//   $Date: 2005/03/08 10:20:39 $
//
//
//=============================================================================

#include "TaskWindow.h"

#include <asl/base/Exception.h>
#include <paintlib/win/plwinbmp.h>
#include <paintlib/plpngenc.h>
#include <paintlib/Filter/plfilterfliprgb.h>
#include <winuser.h>

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
        return IsWindowVisible(_myHandle);
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
        SetLayeredWindowAttributes(_myHandle, 0, int(theAlpha * 255.0f), LWA_ALPHA);
    }

    void
    TaskWindow::fade(float theTime) {
        AnimateWindow(_myHandle, DWORD(theTime * 1000), AW_BLEND | AW_HIDE);
    }
}
