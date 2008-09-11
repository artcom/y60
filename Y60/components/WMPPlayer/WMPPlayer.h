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
//   $RCSfile: WMPPlayer.h,v $
//   $Author: thomas $
//   $Revision: 1.10 $
//   $Date: 2005/04/19 09:28:22 $
//
//  Description: A simple scene class.
//
//=============================================================================

#ifndef _ac_WMPPlayer_h_
#define _ac_WMPPlayer_h_

#include <wmsdk/wmp.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <atlctl.h>
#include <atlconv.h>

#include <asl/math/Vector234.h>
#include <asl/base/PlugInBase.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/MappedBlock.h>
#include <asl/base/Exception.h>

class EventListener;
namespace y60 {
    DEFINE_EXCEPTION(WindowsMediaPlayerException, asl::Exception);
    const std::string MIME_TYPE_WMV = "application/wmv";

    /**
     * @ingroup Y60componentsWMPDecoder
     *
     * @par requirements
     * win98 or later\n
     * windows media sdk\n
     *
     * Embedded Window Media Player
     */
    class WMPPlayer
    {
        public:
            WMPPlayer();
            virtual ~WMPPlayer();
            void WMPPlayer::setup(const std::string & theParentWindowTitle);

            bool load(const std::string & theUrl);

            void setCanvasPosition(const asl::Vector2i & thePosition);
            void setCanvasSize(const asl::Vector2i & theSize);
            void setVisible(bool theFlag);
            void setVolume(double theVolume);
            void setCurrentPosition(double theStartPosition);
            void setLoopCount(unsigned theLoopCount); //0 == indefinite, 1 or more

            void play(double theStartPosition = DBL_MAX);
            void stop();
            void pause();

            asl::Vector2i getSize() const;
            asl::Vector2i getCanvasPosition() const;
            asl::Vector2i getCanvasSize() const;
            double getDuration() const;
            double getVolume() const;
            double getCurrentPosition() const;
            bool isVisible() const;
            unsigned getLoopCount() const;
            std::string getPlayState() const;

        private:
        	HRESULT createWMP(HWND theRenderGirlWindow);
        	HRESULT destroyWMP();
            HRESULT setupEventListener();

            HWND                        _myParentWindow;
            CComModule                  _myModule;

		    CAxWindow                   _myWindow;
		    CComPtr<IWMPPlayer>         _myWMPPlayer;
			CComPtr<IWMPPlayer2>		_myWMPPlayer2;
			CComPtr<IWMPCore>           _myWMPCore;
			CComPtr<IWMPSettings>       _myWMPSettings;
			CComPtr<IWMPMedia>          _myWMPMedia;
			CComPtr<IWMPControls>       _myWMPControl;

            CComPtr<IConnectionPoint>   _myConnectionPoint;
            DWORD                       _myAdviseCookie;

            CComObject<EventListener>*  _myEventListener;

    };
    typedef asl::Ptr<WMPPlayer> WMPPlayerPtr;
}

#endif
