/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2004, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: standard_pixel_types.h,v $
//
//   $Revision: 1.4 $
//
// Description: pixel container algorithms
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
// originally written by Pavel - Tue Jun 17 19:18:17 CEST 1997
// major refactoring during spare time 2003/2004
//
#ifndef _ASL_RASTER_STANDARD_PIXEL_TYPES_H_INCLUDED_
#define _ASL_RASTER_STANDARD_PIXEL_TYPES_H_INCLUDED_

#include "pixels.h"

#ifdef AC_BUILT_WITH_CMAKE
#include <half.h> // ILM OpenEXR 16Bit floating-point implementation
#else
#include <OpenEXR/half.h> // ILM OpenEXR 16Bit floating-point implementation
#endif

namespace asl {

typedef unsigned char pchar;

typedef RGB_t<pchar> RGB;
typedef BGR_t<pchar> BGR;
typedef HSV_t<pchar> HSV;

typedef RGBA_t<pchar> RGBA;
typedef ABGR_t<pchar> ABGR;
typedef BGRA_t<pchar> BGRA;

typedef gray<pchar> GRAY;
typedef gray<unsigned short> GRAY16;
typedef gray<signed short> GRAYS16;
typedef gray<unsigned> GRAY32;

typedef alpha<pchar> ALPHA;
typedef red<pchar> RED;
typedef green<pchar> GREEN;
typedef blue<pchar> BLUE;

typedef hue<pchar> HUE;
typedef saturation<pchar> SATURATION;
typedef gray<pchar> VALUE;

//typedef RGB_t<char> GRAYALPHA;

typedef RGBA_t<half> RGBA_HALF;
typedef RGB_t<half>  RGB_HALF;

typedef RGBA_t<float> RGBA_FLOAT;
typedef RGB_t<float>  RGB_FLOAT;


template <class VALUE>
VALUE getRedValue(const gray<VALUE> & thePixel) {
    return thePixel.get();
}

template <class T>
void setRedValue(gray<T> & thePixel, unsigned char theValue) {
    thePixel.set(theValue);
}

template <class VALUE>
VALUE getGreenValue(const gray<VALUE> & thePixel) {
    return thePixel.get();
}
template <class T>
void setGreenValue(gray<T> & thePixel, unsigned char theValue) {
    thePixel.set(theValue);
}

template <class VALUE>
VALUE getBlueValue(const gray<VALUE> & thePixel) {
    return thePixel.get();    
}
template <class T>
void setBlueValue(gray<T> & thePixel, unsigned char theValue) {
    thePixel.set(theValue);
}



inline
unsigned char getRedValue(const DXT1 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setRedValue(DXT1 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getRedValue(const DXT1a & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setRedValue(DXT1a & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getRedValue(const DXT3 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setRedValue(DXT3 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getRedValue(const DXT5 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setRedValue(DXT5 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
    
inline
unsigned char getGreenValue(const DXT1 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setGreenValue(DXT1 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getGreenValue(const DXT1a & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setGreenValue(DXT1a & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getGreenValue(const DXT3 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setGreenValue(DXT3 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getGreenValue(const DXT5 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setGreenValue(DXT5 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}    

    
inline
unsigned char getBlueValue(const DXT1 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setBlueValue(DXT1 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getBlueValue(const DXT1a & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setBlueValue(DXT1a & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getBlueValue(const DXT3 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setBlueValue(DXT3 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getBlueValue(const DXT5 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setBlueValue(DXT5 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}    


    
inline
unsigned char getAlphaValue(const DXT1 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setAlphaValue(DXT1 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getAlphaValue(const DXT1a & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setAlphaValue(DXT1a & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getAlphaValue(const DXT3 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setAlphaValue(DXT3 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}
inline
unsigned char getAlphaValue(const DXT5 & thePixel) {
    throw NotSupportedException(JUST_FILE_LINE);   
}
inline
void setAlphaValue(DXT5 & thePixel, unsigned char theValue) {
    throw NotSupportedException(JUST_FILE_LINE);
}    


} // namespace asl
#endif
