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
