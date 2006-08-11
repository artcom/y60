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

#ifndef AC_Y60_SPLASH_MANAGER_INCLUDED
#define AC_Y60_SPLASH_MANAGER_INCLUDED

#include <vector>
#include <GL/gl.h>

class SplashManager {
    public:
        SplashManager();
        virtual ~SplashManager();

        void update();
        void draw();
        void addSplash(float theXCoord, float theYCoord);
        
    private:

        SplashManager(SplashManager & otherSplashManager);
        SplashManager & operator=(SplashManager & otherSplashManager);

        struct SplashRepresentation {
            SplashRepresentation():
                _myXCoord(0),
                _myYCoord(0),
                _myIntensity(1.0)
            {
            }

            SplashRepresentation(float theXCoord, float theYCoord):
                _myXCoord(theXCoord),
                _myYCoord(theYCoord),
                _myIntensity(1.0)
            {
            }

            void draw(){
                glPushAttrib(GL_CURRENT_BIT);
                glColor3f(0.0, 0.0, 1.0);
                glBegin(GL_POLYGON);
                glVertex2f( _myXCoord - (0.02 / _myIntensity), _myYCoord);
                glVertex2f( _myXCoord, _myYCoord - (0.02 / _myIntensity));
                glVertex2f( _myXCoord + (0.02 / _myIntensity), _myYCoord);
                glVertex2f( _myXCoord, _myYCoord + (0.02 / _myIntensity));
                glEnd();
                glPopAttrib();
            }
                
            float _myXCoord;
            float _myYCoord;
            float _myIntensity;
        };
            
        typedef std::vector<SplashRepresentation*> SplashList;
        typedef std::vector<SplashRepresentation*>::iterator SplashListIterator;

        SplashList _currentSplashes;


};

#endif // AC_Y60_SPLASH_MANAGER_INCLUDED
