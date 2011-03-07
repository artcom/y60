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


#if !defined(_ac_video_Image_h_)
#define _ac_video_Image_h_

#include "CompressionType.h"
//#include <Yr/Time.h>

#include <asl/math/numeric_functions.h>

#include <string>
#include <vector>
//#include <math.h>
namespace video {


struct Image {
        enum IMAGE_TYPE {
            OPENGL_COMPRESSED,
            RAW_DATA,
            FRAME_GRABBER
        };
        Image() : _myGLCompressionFormat(-1), _myFrameDataSize(0), 
                        _myFrameWidth(0), _myFrameHeight(0),
                       _myDataType(RAW_DATA), _myComponents(3),
                       _myHeightStretch(1), _myWidthStretch(1), _myRotation(0) {}
        unsigned char * _myCurrentFrameData;
        int             _myGLCompressionFormat;
        int             _myComponents;
        IMAGE_TYPE      _myDataType;
        int             _myFrameDataSize;
        int             _myTextureDataSize;
        int             _myFrameWidth;  // OpenGL-compatible Texture size (power of two)
        int             _myFrameHeight; // OpenGL-compatible Texture size (power of two)
        int             _myWidthAspect;  // real presentation size
        int             _myHeightAspect; // real presentation size
        static int calcTextureSize (int theImageSize) {
            double oldBase =  log (float(theImageSize))/log(float(2));
            double newBase = ceil(oldBase);
            return int(pow(2.0, newBase));
        }

        float getHeightStretch() const { return _myHeightStretch; }
        float getWidthStretch() const  { return _myWidthStretch; }
        double getRotation()     const  { return _myRotation;}  
        
        virtual void    getTextureInfo(unsigned int & theTextureWidth, 
                               unsigned int & theTextureHeight, 
                               unsigned int & theTextureNumBytes) const
        {
            theTextureWidth = _myFrameWidth;
            theTextureHeight = _myFrameHeight;
            theTextureNumBytes = _myFrameDataSize;
        }
        
    protected:
        float          _myHeightStretch;
        float          _myWidthStretch;
        double          _myRotation;


};

} // namespace end

#endif
