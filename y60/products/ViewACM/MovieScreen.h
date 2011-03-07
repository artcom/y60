//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.


#if !defined(_ac_video_MovieScreen_h_)
#define _ac_video_MovieScreen_h_

#include "ScreenRegion.h"

namespace video {

    class MovieBase;
    struct Image;
   
    class MovieScreen {
        public:
            enum MOVIESCREEN_TYPE {
                QUAD,
                SPHERE,
                CUBE
            };

            MovieScreen(MOVIESCREEN_TYPE screenType);
            ~MovieScreen();
            bool init();
            bool reset();
            
            void setPosition(float myPositionX, float myPositionY, float myPositionZ);
            void getPosition(double & myPositionX, double & myPositionY, double & myPositionZ);
            
            void scaleScreen(float scale);
            void scaleScreen(float scaleX, float scaleY);
            float getScale();
            void getScale(double & myScaleX, double & myScaleY);

            void rotateScreen(float rotx, float roty, float rotz);
            void getRotation(double & myRotationX, double & myRotationY, double & myRotationZ);
             
            
            bool updateToFrame(unsigned long nextFrame);
            bool updateToTime(double runningtime);
            bool update();
            void draw();
            
            void setStatistic(bool showStatistic);
            bool getStatistic();

            void  setWidth(float width) { _myWidth = width; }
            float getWidth() { return _myWidth; }
            
            void  setHeight(float height) { _myHeight = height; }
            float getHeight() { return _myHeight; }
            
            ScreenRegion getScreenRegion();
            void   setScreenRegion(ScreenRegion& theScreenRegion);

            
            MovieScreen * clone() const;

            void setCurrentFrame(const Image * myImage);

         private:
            bool bindTexture();

            MOVIESCREEN_TYPE _myScreenType;
            
            const Image *      _myCurrentFrameImage;
            
            float            _myPosition[3];
            float            _myRotation[3];
            float            _myWidth;
            float            _myHeight;
            float            _myScaleX;
            float            _myScaleY;
            mutable unsigned int     _myTexName;
            bool             _isTextureDefined;
            ScreenRegion     _myTextureCoordinates;
            bool             _myFrameIsPowerOf2;
    };

} // end namespace

#endif

