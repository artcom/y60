//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: WMPPlayer.cpp,v $
//     $Author: thomas $
//   $Revision: 1.11 $
//       $Date: 2005/04/19 09:28:22 $
//
//  Embededd Windows Media Player
//
//=============================================================================

#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <winuser.h>

#include "WMPPlayer.h"
#include "EventListener.h"

#include <asl/Logger.h>
#include <asl/os_functions.h>
#include <asl/Time.h>
#include <y60/JSWrapper.impl>




using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) //x
#define DB2(x) //x

#ifndef JIF
#define JIF(x) if (FAILED(hr=(x))) {return hr;}
#endif

namespace jslib {   
    template class JSWrapper<y60::WMPPlayer, asl::Ptr<y60::WMPPlayer>,
             jslib::StaticAccessProtocol>;
}
namespace y60 {

    HWND getActiveWindow();

    WMPPlayer::WMPPlayer() {
        AC_DEBUG << "WMPPlayer::WMPPlayer() ";
		_myModule.Init(NULL, NULL, &LIBID_ATLLib);
        AC_DEBUG << "WMPPlayer::WMPPlayer() done.";
    }

    WMPPlayer::~WMPPlayer() {
    	_myModule.Term();
    }

    void
    WMPPlayer::setup(const string & theParentWindowTitle) {
        AC_DEBUG << "WMPPlayer::setup ";

        _myParentWindow = FindWindow(0, theParentWindowTitle.c_str());
        if (_myParentWindow == NULL) {
            throw asl::Exception(string("### ERROR: Could not get parent window") + theParentWindowTitle, PLUS_FILE_LINE);
		}

		//_myParentWindow = getActiveWindow();

        HRESULT hr = createWMP(_myParentWindow);
        AC_DEBUG << "WMPPlayer::setup done.";
    }

    bool
    WMPPlayer::load(const std::string & theUrl) {
        AC_INFO << "WMPPlayer::load " << theUrl;
        HRESULT hr;

		USES_CONVERSION;
        hr = _myWMPPlayer->put_URL(A2W(theUrl.c_str()));
        if (hr != S_OK) {
            return false;
        }
        bool myErrorFlag = false;

		double myTimeOut = 20; //secs
		Time myStartTime;
        //wait until WMP has loaded the movie attributes
    	while (true) {
        	MSG msg;
    	    GetMessage(&msg, 0, 0, 0);
    			TranslateMessage(&msg);
    			DispatchMessage(&msg);

    	    myErrorFlag = _myEventListener->gotError();
    	    if (_myEventListener->isLoaded() || myErrorFlag) {
    	        break;
    	    }
    		Time myCurrentTime;
			if (double(myCurrentTime) - double(myStartTime) > myTimeOut) {
					AC_WARNING << "time out while loading movie information";
					myErrorFlag = true;
					break;
			}
    	}
        if (myErrorFlag) {
    				AC_WARNING << "error while loading movie " << theUrl;
            return false;
        }
    	hr = _myWMPCore->get_currentMedia(&_myWMPMedia);
        hr = _myWMPControl->stop();

        return hr == S_OK;
    }

    void
    WMPPlayer::setLoopCount(unsigned theLoopCount) {
        HRESULT hr;
		CComBSTR myMode(_T("loop"));
		switch (theLoopCount) {
		    case 0:
        		hr = _myWMPSettings->setMode(myMode, VARIANT_TRUE);
        		hr = _myWMPSettings->put_playCount(1);
        	    break;
        	case 1:
        		hr = _myWMPSettings->setMode(myMode, VARIANT_FALSE);
        		hr = _myWMPSettings->put_playCount(1);
        		break;
            default: //looping n times
        		hr = _myWMPSettings->setMode(myMode, VARIANT_FALSE);
        		hr = _myWMPSettings->put_playCount(long(theLoopCount));
		}
    }

    unsigned
    WMPPlayer::getLoopCount() const {
        HRESULT hr;
		CComBSTR myMode(_T("loop"));
		VARIANT_BOOL myValue;
		hr = _myWMPSettings->getMode(myMode, &myValue);
		if (myValue == VARIANT_TRUE) {
		    return 0;
		} else {
		    long myCount = 0;
            hr = _myWMPSettings->get_playCount(&myCount);
            return unsigned(myCount);
		}
    }

    void
    WMPPlayer::setCanvasPosition(const Vector2i &thePosition) {
		RECT myRect;
        GetClientRect(_myWindow, &myRect);
        _myWindow.MoveWindow(thePosition[0], thePosition[1], myRect.right, myRect.bottom);
    }

    void
    WMPPlayer::setCanvasSize(const Vector2i &theSize) {
		RECT myRect;
		POINT myPoint;
        GetWindowRect(_myWindow, &myRect);
        myPoint.x = myRect.left;
        myPoint.y = myRect.top;
        ScreenToClient(_myParentWindow, &myPoint);
        _myWindow.MoveWindow(myPoint.x, myPoint.y, theSize[0], theSize[1]);
    }

    void
    WMPPlayer::setVisible(bool theFlag) {
        ShowWindow(_myWindow.m_hWnd, theFlag ? SW_SHOW:SW_HIDE);
    }

    bool
    WMPPlayer::isVisible() const {
        return IsWindowVisible(_myWindow.m_hWnd);
    }

    Vector2i
    WMPPlayer::getCanvasPosition() const {
		RECT myRect;
		POINT myPoint;
        GetWindowRect(_myWindow, &myRect);
        myPoint.x = myRect.left;
        myPoint.y = myRect.top;
        ScreenToClient(_myParentWindow, &myPoint);
        return Vector2i(myPoint.x, myPoint.y);
    }
    Vector2i
    WMPPlayer::getCanvasSize() const {
		RECT myRect;
        GetClientRect(_myWindow, &myRect);
        return Vector2i(myRect.right, myRect.bottom);
    }

    Vector2i
    WMPPlayer::getSize() const {
        if ( ! _myWMPMedia) {
            AC_WARNING << "no media available";
            return Vector2i(0,0);
        }
        long myWidth, myHeight;
		_myWMPMedia->get_imageSourceWidth(&myWidth);
		_myWMPMedia->get_imageSourceHeight(&myHeight);
		return Vector2i(int(myWidth), int(myHeight));
    }

    double
    WMPPlayer::getDuration() const {
        if ( ! _myWMPMedia) {
            AC_WARNING << "no media available";
            return 0;
        }
        double myDuration;
		_myWMPMedia->get_duration(&myDuration);
		return myDuration;
	}

    double
    WMPPlayer::getVolume() const {
        long myVolume = 0;
        HRESULT hr;
        hr = _myWMPSettings->get_volume(&myVolume);
        return float(myVolume) / 100.0;
    }
    
    void
    WMPPlayer::setVolume(double theVolume) {
        long myVolume = long(theVolume * 100);
        HRESULT hr;
        hr = _myWMPSettings->put_volume(myVolume);
    }
    
    void
    WMPPlayer::setCurrentPosition(double theStartPosition) {
        _myWMPControl->put_currentPosition(theStartPosition);
    }
    
    double
    WMPPlayer::getCurrentPosition() const {
        if ( ! _myWMPControl) {
            AC_WARNING << "no control available";
            return 0;
        }
        double myPosition;
        _myWMPControl->get_currentPosition(&myPosition);
        return myPosition;
    }    
    
    void
    WMPPlayer::play(double theStartPosition) {
        AC_INFO << "WMPPlayer::startMovie at position" << theStartPosition;
        setVisible(true);
        if (theStartPosition != DBL_MAX) {
            _myWMPControl->put_currentPosition(theStartPosition);
        }
        _myWMPControl->play();
    }

    void
    WMPPlayer::stop() {
        AC_INFO << "WMPPlayer::stopMovie";
        _myWMPControl->stop();
    }

    void
    WMPPlayer::pause() {
        AC_INFO << "WMPPlayer::pauseMovie";
        _myWMPControl->pause();
    }

    void
    WMPPlayer::setAlpha(float theAlpha) {
        AC_INFO << "WMPPlayer::setAlpha " << theAlpha;
        SetWindowLong(_myParentWindow, GWL_EXSTYLE,
                      GetWindowLong(_myParentWindow, GWL_EXSTYLE) | WS_EX_LAYERED);
        bool myErrorCode = SetLayeredWindowAttributes(_myParentWindow, 0, int(theAlpha * 255.0f), LWA_ALPHA);
        AC_DEBUG << "ErrorCode: " <<GetLastError();
    }

    string
    WMPPlayer::getPlayState() const {
        AC_INFO << "WMPPlayer::getPlayState";
        WMPPlayState myState;
        HRESULT hr = _myWMPCore->get_playState(&myState);
        switch (myState) {
            case wmppsStopped:
                return "stopped";
            case wmppsPaused:
                return "paused";
            case wmppsPlaying:
                return "playing";
            default:
                return "unknown";
        }
    }

	HRESULT
	WMPPlayer::destroyWMP() {
		_myWMPCore->close();
		_myWMPPlayer.Release();
		_myWindow.DestroyWindow();
		return S_OK;
	}

	HRESULT
	WMPPlayer::createWMP(HWND theParent) {
        AtlAxWinInit();
		CComPtr<IAxWinHostWindow>           spHost;
		HRESULT                             hr;
		RECT                                myRect;

        GetClientRect(theParent, &myRect);
		_myWindow.Create(theParent, myRect, NULL,
			             WS_CHILD | WS_DISABLED | WS_CLIPCHILDREN);

		if (NULL == _myWindow.m_hWnd) {
			return E_POINTER;
		}

		// query interfaces
		JIF(_myWindow.QueryHost(&spHost));
		JIF(spHost->CreateControl(CComBSTR(_T("{6BF52A52-394A-11d3-B153-00C04F79FAA6}")), _myWindow, 0));
	    JIF(_myWindow.QueryControl(&_myWMPPlayer));
	    JIF(_myWindow.QueryControl(&_myWMPPlayer2));
		JIF(_myWindow.QueryControl(&_myWMPCore));
		JIF(_myWindow.QueryControl(&_myWMPSettings));
		JIF(_myWMPCore->get_controls(&_myWMPControl));

		// settings
        JIF(_myWMPPlayer->put_uiMode(CComBSTR(_T("none"))));
        JIF(_myWMPPlayer2->put_stretchToFit(VARIANT_TRUE));

		JIF(_myWMPSettings->put_autoStart(VARIANT_TRUE)); // we really need it, do not ever set this to false

        JIF(setupEventListener());
        return S_OK;
	 }

     HRESULT
     WMPPlayer::setupEventListener() {
        HRESULT hr;
        CComPtr<IWMPEvents>            myWMPEventListener;
        CComPtr<IConnectionPointContainer>  myConnectionContainer;

       // start listening to events

        JIF(CComEventListener::CreateInstance(&_myEventListener));
        myWMPEventListener = _myEventListener;

        JIF(_myWMPPlayer->QueryInterface(&myConnectionContainer));

        // See if OCX supports the IWMPEvents interface
        JIF(myConnectionContainer->FindConnectionPoint(__uuidof(IWMPEvents), &_myConnectionPoint));
        JIF(_myConnectionPoint->Advise(myWMPEventListener, &_myAdviseCookie));

        _myEventListener->init();
        return S_OK;
    }

    HWND getActiveWindow() {
		PROCESS_INFORMATION myProcessInfo;
		myProcessInfo.dwThreadId = GetCurrentThreadId();

        GUITHREADINFO myGuiThreadInfo;
        myGuiThreadInfo.cbSize = sizeof(GUITHREADINFO);
        GetGUIThreadInfo(myProcessInfo.dwThreadId, &myGuiThreadInfo);

        HWND myActiveWindow = myGuiThreadInfo.hwndActive;
        if (myActiveWindow == NULL) {
            throw asl::Exception("### ERROR: Could not get an active window", PLUS_FILE_LINE);
        }
        return myActiveWindow;
	}
}
