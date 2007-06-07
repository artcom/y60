//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: SDLSplashScreen.cpp,v $
//   $Author: oliver $
//   $Revision: 11918 $
//   $Date: 2007/06/07 10:45:40 $
//
//
//  Description: shows an image during application-start and -restart using SDL
//
//
//=============================================================================

#include "SDLSplashScreen.h"

#include <SDL/SDL_getenv.h>
#include <sstream>

SDLSplashScreen * SDLSplashScreen::_instance = 0;

SDLSplashScreen * SDLSplashScreen::getInstance() {
    if (_instance == 0)
        _instance = new SDLSplashScreen();
    return _instance;   
}

SDLSplashScreen::~SDLSplashScreen() {
    disable();   
}

SDLSplashScreen::SDLSplashScreen()
:_myEnabled(false), _myShown(false)
{}

bool
SDLSplashScreen::isEnabled() {
    return _myEnabled;
}

void
SDLSplashScreen::enable(const std::string& theBMPFilePath, int theXPos, int theYPos) {    
    std::stringstream myEnvText;
    
    myEnvText << "SDL_VIDEO_WINDOW_POS=" << theXPos << "," << theYPos;
    putenv(myEnvText.str().c_str());
    
    _myImage = SDL_LoadBMP(theBMPFilePath.c_str());
    
    if(_myImage!=NULL) 
        _myEnabled = true;
}

void
SDLSplashScreen::disable() {
    if (_myShown)
        hide();
        
    if (_myEnabled)
        SDL_FreeSurface(_myImage);
        
    _myEnabled = false;
}

void
SDLSplashScreen::show() {
    if (!_myEnabled) return;
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_ShowCursor(SDL_DISABLE);
    _myScreen = SDL_SetVideoMode(_myImage->w, _myImage->h, 0, SDL_NOFRAME);
    
    SDL_BlitSurface(_myImage, NULL, _myScreen, NULL);
    SDL_UpdateRect(_myScreen, 0, 0, 0, 0);
    
    _myShown = true;
}

void
SDLSplashScreen::redraw() {
    if (!_myShown) return;
    SDL_UpdateRect(_myScreen, 0, 0, 0, 0);
}

void
SDLSplashScreen::hide() {
    if (!_myEnabled) return;
    
    SDL_FreeSurface(_myScreen);
    SDL_Quit();
    
    _myShown = false;
}