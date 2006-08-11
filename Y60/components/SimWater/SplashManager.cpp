//============================================================================
//
// Copyright (C) 2002-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "SplashManager.h"


SplashManager::SplashManager(){
};

SplashManager::~SplashManager(){
    for (SplashListIterator currentSplash = _currentSplashes.begin();
         currentSplash < _currentSplashes.end(); ++currentSplash )
    {
        delete *currentSplash;
    }

};


void
SplashManager::update() {
    SplashList oldSplashes = _currentSplashes;
    _currentSplashes.clear();

    for ( int i = 0; i < oldSplashes.size(); ++i) {
        if (oldSplashes[i]->_myIntensity >= 0.1) {
            oldSplashes[i]->_myIntensity = oldSplashes[i]->_myIntensity - 0.05;
            _currentSplashes.push_back(oldSplashes[i]);
        }
    }
}

void
SplashManager::draw() {
    for (SplashListIterator currentSplash = _currentSplashes.begin();
         currentSplash < _currentSplashes.end(); ++currentSplash )
    {
        (*currentSplash)->draw();
    }

}

void
SplashManager::addSplash(float theXCoord, float theYCoord) {
    _currentSplashes.push_back( new SplashRepresentation(theXCoord, theYCoord));
}
