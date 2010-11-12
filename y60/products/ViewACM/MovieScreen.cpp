//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or dupli:mcated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: MovieScreen.cpp,v $
//
//     $Author: valentin $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
// (CVS log at the bottom of this file)
//
//
//=============================================================================

//#define  GL_GLEXT_PROTOTYPES
//#include <GL/gl.h>
#include <y60/glutil/GLUtils.h>


#include "MovieScreen.h"
#include "MovieBase.h"

//#include <net/glut.h>

#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>


using namespace std;  // automatically added!




#define NVIDIA_OpenGL


namespace video {

    MovieScreen::MovieScreen(MOVIESCREEN_TYPE screenType) : _myScreenType(screenType),
        _myTexName(0),
        _myCurrentFrameImage(0),
        _myHeight(1.0),
        _myWidth(1.0),
        _isTextureDefined(false),
        _myFrameIsPowerOf2(true)
    {
        reset();
    }

    MovieScreen::~MovieScreen() {
        if (_myTexName) {
            glDeleteTextures(1, &_myTexName);
            _myTexName = 0;
        }
    }

    bool 
    MovieScreen::init() {
        return true;
    }
    
    bool
    MovieScreen::reset() {
       _myTextureCoordinates.reset();
       setPosition(0.0f, 0.0f, 0.0f);
       scaleScreen(1.0);
       rotateScreen(0.0f, 0.0f, 0.0f);

       return true;
    }
    
    void  
    MovieScreen::setStatistic(bool showStatistic) {
        //if (_myMovie) {
        //    _myMovie-> setStatistic(showStatistic);
        //}
    }
    
    bool  
    MovieScreen::getStatistic() {
        //if (_myMovie) {
        //    return _myMovie->getStatistic();
        //}
        return false;
    }
 
    void 
    MovieScreen::setPosition(float myPositionX, float myPositionY, float myPositionZ) {
        _myPosition[0] = myPositionX;
        _myPosition[1] = myPositionY;
        _myPosition[2] = myPositionZ;
    }
    
    void 
    MovieScreen::getPosition(double & myPositionX, 
                             double & myPositionY, 
                             double & myPositionZ) 
    {
        myPositionX = _myPosition[0];
        myPositionY = _myPosition[1];
        myPositionZ = _myPosition[2];
        
    }
    
    ScreenRegion 
    MovieScreen::getScreenRegion() {
        return _myTextureCoordinates;
    }
    
    void
    MovieScreen::setScreenRegion(ScreenRegion& theScreenRegion) {
        _myTextureCoordinates = theScreenRegion;
    }

    void  
    MovieScreen::scaleScreen(float scale) {
        _myScaleX = scale;
        _myScaleY = scale;
    }

    void MovieScreen::scaleScreen(float scaleX, float scaleY) {
        _myScaleX = scaleX;
        _myScaleY = scaleY;
    }

    float 
    MovieScreen::getScale() {
        return _myScaleX;
    }

    void 
    MovieScreen::getScale(double & myScaleX, double & myScaleY) {
        myScaleX = _myScaleX;
        myScaleY = _myScaleY;
    }

    void  
    MovieScreen::rotateScreen(float rotx, float roty, float rotz) {
        _myRotation[0] = rotx;
        _myRotation[1] = roty;
        _myRotation[2] = rotz;
    }
    
    void 
    MovieScreen::setCurrentFrame(const Image * myImage){
        _myCurrentFrameImage  =  myImage;
        unsigned char *  myTmp = _myCurrentFrameImage->_myCurrentFrameData;
        _myTextureCoordinates.reset();
        _myTextureCoordinates.rotate( myImage->getRotation());
        _myTextureCoordinates.stretch(myImage->getWidthStretch(), 
                                      myImage->getHeightStretch());
    }
        
    void 
    MovieScreen::getRotation(double & myRotationX, 
                              double & myRotationY, 
                              double & myRotationZ) 
    {
        myRotationX = _myRotation[0];
        myRotationY = _myRotation[1];
        myRotationZ = _myRotation[2];
     }
    
    bool 
    MovieScreen::update() {
        if (_myTexName == 0) {
            // first time called!!
            glGenTextures(1, &_myTexName);  
        }
        return bindTexture();
    }
    
    bool 
    MovieScreen::updateToTime(double runningTime) {
        
        // update your movies
            return update();
    }

    bool
    MovieScreen::updateToFrame(unsigned long nextFrame) {
            return update();
    }
    
    bool
    MovieScreen::bindTexture() {
        unsigned long theGLTextureType = GL_TEXTURE_2D;

        if ( ((!asl::powerOfTwo(_myCurrentFrameImage->_myFrameWidth)) || 
                (!asl::powerOfTwo(_myCurrentFrameImage->_myFrameHeight) ))  && 
                (_myCurrentFrameImage->_myDataType != video::Image::OPENGL_COMPRESSED)) 
        {
            _myFrameIsPowerOf2 = false;
#ifdef GL_TEXTURE_RECTANGLE_NV           
            theGLTextureType = GL_TEXTURE_RECTANGLE_NV;
#endif            
        } else {
            _myFrameIsPowerOf2 = true;
        }
        
        glBindTexture(theGLTextureType, _myTexName);
        
        glTexParameteri(theGLTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(theGLTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(theGLTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(theGLTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP);

           
        if (_myCurrentFrameImage->_myDataType == video::Image::OPENGL_COMPRESSED) {
#ifdef GL_ARB_texture_compression

            unsigned int theTextureWidth;
            unsigned int theTextureHeight;
            unsigned int theTextureNumBytes;
            
            _myCurrentFrameImage->getTextureInfo(theTextureWidth, theTextureHeight, theTextureNumBytes);
            //unsigned char * myData = 0;
            //myData = (unsigned char*) realloc(myData,_myCurrentFrameImage->_myFrameWidth * _myCurrentFrameImage->_myFrameHeight * 4);
            if (!_isTextureDefined) {
                glCompressedTexImage2DARB(GL_TEXTURE_2D,
                        0,
                        _myCurrentFrameImage->_myGLCompressionFormat,
                        _myCurrentFrameImage->_myFrameWidth, 
                        _myCurrentFrameImage->_myFrameHeight,
                        0,
                        _myCurrentFrameImage->_myTextureDataSize,
                        _myCurrentFrameImage->_myCurrentFrameData);
                _isTextureDefined = true;
            } else {
                glCompressedTexSubImage2DARB(GL_TEXTURE_2D,
                        0, 0, 0, 
                        theTextureWidth, theTextureHeight,
                        _myCurrentFrameImage->_myGLCompressionFormat,
                        theTextureNumBytes,
                        _myCurrentFrameImage->_myCurrentFrameData);
            }
            
#elif
            
            return false;
#endif
        } else {

            GLenum myFormat;           
            if (_myCurrentFrameImage->_myDataType == video::Image::FRAME_GRABBER) {
                myFormat= (_myCurrentFrameImage->_myComponents== 3) ? (GL_BGR):(GL_BGRA);
            } else {
                myFormat= (_myCurrentFrameImage->_myComponents== 3) ? (GL_RGB):(GL_RGBA);                
            }

            glTexImage2D(theGLTextureType, 0,
                    (_myCurrentFrameImage->_myComponents== 3) ? (GL_RGB):(GL_RGBA),
                    _myCurrentFrameImage->_myFrameWidth,
                    _myCurrentFrameImage->_myFrameHeight, 
                    0,
                    myFormat,
                    GL_UNSIGNED_BYTE,
                    _myCurrentFrameImage->_myCurrentFrameData);
            
            if ( !_myFrameIsPowerOf2 ) {
                _myTextureCoordinates.set(ScreenRegion::TOP_RIGHT, _myCurrentFrameImage->_myFrameWidth, 0);
                _myTextureCoordinates.set(ScreenRegion::TOP_LEFT, 0, 0);
                _myTextureCoordinates.set(ScreenRegion::BOTTOM_RIGHT, _myCurrentFrameImage->_myFrameWidth, 
                                          _myCurrentFrameImage->_myFrameHeight);
                _myTextureCoordinates.set(ScreenRegion::BOTTOM_LEFT, 0,  _myCurrentFrameImage->_myFrameHeight);
            }
        }
        return true;
    }

    void  
    MovieScreen::draw(){
        
        glPushMatrix();
        if (!_myFrameIsPowerOf2) {
#ifdef GL_TEXTURE_RECTANGLE_NV   
            glEnable(GL_TEXTURE_RECTANGLE_NV);
#endif  
            glDisable(GL_TEXTURE_2D);
        } else {
            glEnable(GL_TEXTURE_2D);
#ifdef GL_TEXTURE_RECTANGLE_NV   
            glDisable(GL_TEXTURE_RECTANGLE_NV);
#endif  
        }

 
        glRotatef(_myRotation[0], 0.0f, 1.0f, 0.0f);
        glRotatef(_myRotation[1], 1.0f, 0.0f, 0.0f);
        glRotatef(_myRotation[2], 0.0f, 0.0f, 1.0f);
        glScalef(_myScaleX, _myScaleY, 0.0f);
        
        
        // we can have more than one screens, so we must bind the correct texture again!!
#ifdef GL_TEXTURE_RECTANGLE_NV   
        glBindTexture(_myFrameIsPowerOf2 ? (GL_TEXTURE_2D):(GL_TEXTURE_RECTANGLE_NV), _myTexName);
#else
        glBindTexture(GL_TEXTURE_2D, _myTexName);
#endif        

         // draw your projection plane
        switch (_myScreenType) {
            case QUAD:
                glBegin(GL_QUADS);
                glTexCoord2f(_myTextureCoordinates.get(ScreenRegion::BOTTOM_LEFT)[0], _myTextureCoordinates.get(ScreenRegion::BOTTOM_LEFT)[1] );
                glVertex3f (_myPosition[0]-(_myWidth/2.0), 
                            _myPosition[1]-(_myHeight/2.0),
                            0.0);
                glTexCoord2f(_myTextureCoordinates.get(ScreenRegion::BOTTOM_RIGHT)[0], _myTextureCoordinates.get(ScreenRegion::BOTTOM_RIGHT)[1]);
                glVertex3f (_myPosition[0]+(_myWidth/2.0), 
                            _myPosition[1]-(_myHeight/2.0),
                            0.0);

                glTexCoord2f(_myTextureCoordinates.get(ScreenRegion::TOP_RIGHT)[0], _myTextureCoordinates.get(ScreenRegion::TOP_RIGHT)[1]);
                glVertex3f (_myPosition[0]+(_myWidth/2.0), 
                            _myPosition[1]+(_myHeight/2.0),
                            0.0);

                glTexCoord2f(_myTextureCoordinates.get(ScreenRegion::TOP_LEFT)[0], _myTextureCoordinates.get(ScreenRegion::TOP_LEFT)[1]);
                glVertex3f (_myPosition[0]-(_myWidth/2.0), 
                            _myPosition[1]+(_myHeight/2.0),
                            0.0);
                glEnd();
                break;
            case SPHERE:
                break;
            case CUBE:
                break;
          };

        glPopMatrix();        
     }



    MovieScreen * 
    MovieScreen::clone() const {
        MovieScreen * myNewMovieScreen = new MovieScreen(_myScreenType);
        myNewMovieScreen->_myCurrentFrameImage = 0; //) = *_myCurrentFrameImage;
        myNewMovieScreen->_myPosition[0] = _myPosition[0];
        myNewMovieScreen->_myPosition[1] = _myPosition[1];
        myNewMovieScreen->_myPosition[2] = _myPosition[2];
        
        myNewMovieScreen->_myRotation[0] = _myRotation[0];
        myNewMovieScreen->_myRotation[1] = _myRotation[1];
        myNewMovieScreen->_myRotation[2] = _myRotation[2];
         
        myNewMovieScreen->_myScaleX = _myScaleX;
        myNewMovieScreen->_myScaleY = _myScaleY;
        myNewMovieScreen->_myTextureCoordinates = _myTextureCoordinates;

        return myNewMovieScreen;
    }
}


