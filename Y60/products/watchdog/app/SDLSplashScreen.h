//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: SDLSplashScreen.h,v $
//   $Author: oliver $
//   $Revision: 11918 $
//   $Date: 2007/06/07 10:45:40 $
//
//
//  Description: shows an image during application-start and -restart using SDL
//
//
//=============================================================================

#ifndef ac_watchdog_SDLSplashScreen_h
#define ac_watchdog_SDLSplashScreen_h

#include <string>
#include <SDL/SDL.h>

class SDLSplashScreen {
    public:
        virtual     ~SDLSplashScreen();
        void        enable(const std::string& theBMPFilePath, int theXPos, int theYPos);
        void        disable();
        void        show();
        void        redraw();
        void        hide();
        bool        isEnabled();
        
        static SDLSplashScreen * getInstance();
        
    protected:
        SDLSplashScreen();
        
    private:
        template <class T>
        void setSurfacePixels(T * theBmpPixel, T ** thePixels, SDL_Surface * theSurface, int theHeight, int theWidth);
        
        static SDLSplashScreen * _instance;
        
        SDL_Surface* _myImage;
        SDL_Surface* _myScreen;
        
        bool         _myEnabled;
        bool         _myShown;
};

#endif