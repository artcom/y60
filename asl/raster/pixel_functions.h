/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: pixel_functions.h,v $
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
#ifndef _ASL_RASTER_PIXEL_FUNCTIONS_H_INCLUDED_
#define _ASL_RASTER_PIXEL_FUNCTIONS_H_INCLUDED_

#include <functional>

namespace asl {
/*
template <class TO, class FROM>
TO auto_cast(const FROM & mySrc) {
    return mySrc;
}
*/

template <class TO>
TO auto_cast(int mySrc) {
    return mySrc;
}
template <class TO>
TO auto_cast(unsigned int mySrc) {
    return mySrc;
}
template <class TO>
TO auto_cast(long mySrc) {
    return mySrc;
}
template <class TO>
TO auto_cast(unsigned long mySrc) {
    return mySrc;
}
template <class TO>
TO auto_cast(char mySrc) {
    return mySrc;
}
template <class TO>
TO auto_cast(unsigned char mySrc) {
    return mySrc;
}
template <class TO>
TO auto_cast(float mySrc) {
    return mySrc;
}
template <class TO>
TO auto_cast(double mySrc) {
    return mySrc;
}

template <class TO>
TO auto_cast(DXT1 mySrc) {
    throw NotSupportedException(JUST_FILE_LINE); 
    return mySrc;
}
template <class TO>
TO auto_cast(DXT1a mySrc) {
    throw NotSupportedException(JUST_FILE_LINE); 
    return mySrc;
}
template <class TO>
TO auto_cast(DXT3 mySrc) {
    throw NotSupportedException(JUST_FILE_LINE); 
    return mySrc;
}

template <class TO>
TO auto_cast(DXT5 mySrc) {
    throw NotSupportedException(JUST_FILE_LINE); 
    return mySrc;
}




template <class TO, class FROM_T>
TO auto_cast(const RGB_t<FROM_T> & mySrc) {
    TO myResult;
    setRedValue(myResult, getRedValue(mySrc));
    setGreenValue(myResult, getGreenValue(mySrc));
    setBlueValue(myResult, getBlueValue(mySrc));
    return myResult;
}

template <class TO, class FROM_T>
TO auto_cast(const BGR_t<FROM_T> & mySrc) {
    TO myResult;
    setRedValue(myResult, getRedValue(mySrc));
    setGreenValue(myResult, getGreenValue(mySrc));
    setBlueValue(myResult, getBlueValue(mySrc));
    return myResult;
}

template <class TO, class FROM_T>
TO auto_cast(const RGBA_t<FROM_T> & mySrc) {
    TO myResult;
    setRedValue(myResult, getRedValue(mySrc));
    setGreenValue(myResult, getGreenValue(mySrc));
    setBlueValue(myResult, getBlueValue(mySrc));
    setAlphaValue(myResult, getAlphaValue(mySrc));
    return myResult;
}

template <class TO, class FROM_T>
TO auto_cast(const ABGR_t<FROM_T> & mySrc) {
    TO myResult;
    setRedValue(myResult, getRedValue(mySrc));
    setGreenValue(myResult, getGreenValue(mySrc));
    setBlueValue(myResult, getBlueValue(mySrc));
    setAlphaValue(myResult, getAlphaValue(mySrc));
    return myResult;
}

template <class TO, class FROM_T>
TO auto_cast(const BGRA_t<FROM_T> & mySrc) {
    TO myResult;
    setRedValue(myResult, getRedValue(mySrc));
    setGreenValue(myResult, getGreenValue(mySrc));
    setBlueValue(myResult, getBlueValue(mySrc));
    setAlphaValue(myResult, getAlphaValue(mySrc));
    return myResult;
}

template <class TO, class FROM_T>
TO auto_cast(const gray<FROM_T> & mySrc) {
    TO myResult;
    setGrayValue(myResult, getGrayValue(mySrc));
    return myResult;
}

template <class PIXEL, class VALUE>
PIXEL expand(VALUE v) {
    PIXEL p;
    typename PixelTraits<PIXEL>::iterator myBegin = begin(p);
    typename PixelTraits<PIXEL>::iterator myEnd = end(p);
    while (myBegin != myEnd) {
        *myBegin++ = v;
    }
    return p;
}


template <class SRC, class DEST>
struct invert : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	    return DEST(src ^ SRC(-1));
    }
};

template <class SRC, class DEST, long long OFFSET>
struct offset : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	    return DEST(src + SRC(OFFSET));
    }
};

template <class COMPONENT, class PIXEL>
COMPONENT channel_sum(const PIXEL & p, const COMPONENT & c) {
    COMPONENT result(c);
    for (int i = 0;i<p.size();i++) {
        result += p[i];
    }
    return result;
}

template <class PIXEL, class COMPONENT>
COMPONENT channel_min(const PIXEL & p, const COMPONENT & c) {
    COMPONENT result(c);
    for (int i = 0;i<p.size();i++) {
        if (p[i]<c) 
            result = p[i];
    }
    return result;
}
template <class PIXEL, class COMPONENT>
COMPONENT channel_max(const PIXEL & p, const COMPONENT & c) {
    COMPONENT result(c);
    for (int i = 0;i<p.size();i++) {
        if (p[i]>c) 
            result = p[i];
    }
    return result;
}

template <class PIXEL>
PIXEL pixel_abs(const PIXEL & p) {
    PIXEL result;
    for (int i = 0;i<p.size();i++) {
        result[i] = abs(p[i]);
    }
    return result;
}

template <class PIXEL>
PIXEL pixel_max(const PIXEL & p1,const PIXEL & p2) {
    PIXEL result;
    for (int i = 0;i<p1.size();i++) {
        result[i] = p1[i]>p2[i] ? p1[i] : p2[i];
    }
    return result;
}
template <class PIXEL>
PIXEL pixel_min(const PIXEL & p1,const PIXEL & p2) {
    PIXEL result;
    for (int i = 0;i<p1.size();i++) {
        result[i] = p1[i]<p2[i] ? p1[i] : p2[i];
    }
    return result;
}

inline
int pixel_abs(int p) {
    return p > 0 ? p : -p;
}

inline
float pixel_abs(float p) {
    return ::fabs(p);
}

template <class PIXEL>
bool all_non_zero(const PIXEL & p) {
    PIXEL result;
    for (int i = 0;i<p.size();i++) {
        if (p[i] == 0) return 0;
    }
    return 1;
}

inline
bool all_non_zero(int p) {
    return p!=0;
}
inline
bool all_non_zero(float p) {
    return p!=0;
}
inline
bool all_non_zero(unsigned char p) {
    return p!=0;
}
inline
bool all_non_zero(char p) {
    return p!=0;
}

template <class SRC, class DEST>
struct make_rgb : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	    DEST result;
	    result.set(src.get(RED()));
	    result.set(src.get(GREEN()));
	    result.set(src.get(BLUE()));

        return result;
    }
};

template <class SRC, class DEST>
struct make_hsv_from_rgb : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	    DEST result;
         
	    int red = src.get(RED())();
	    int green = src.get(GREEN())();
	    int blue = src.get(BLUE())();

        float h, s, v;
        int min, max;
        int delta;

        h = 0.0;

        if (red > green)
        {
          if (red > blue)
            max = red;
          else
            max = blue;

          if (green < blue)
            min = green;
          else
            min = blue;
        }
        else
        {
          if (green > blue)
            max = green;
          else
            max = blue;

          if (red < blue)
            min = red;
          else
            min = blue;
        }

        v = max;

        if (max != 0)
            s = ((max - min) * 255) / (float) max;
        else
            s = 0;

        if (s == 0)
            h = 0;
        else
        {
          delta = max - min;
          if (red == max)
            h = (green - blue) / (float) delta;
          else if (green == max)
            h = 2 + (blue - red) / (float) delta;
          else if (blue == max)
            h = 4 + (red - green) / (float) delta;
          h *= 42.5;

          if (h < 0)
            h += 255;
          if (h > 255)
            h -= 255;
        }

        result.set(HUE(h));
        result.set(SATURATION(s));
        result.set(GRAY(v));
        return result;
    }
};
template <class SRC, class DEST>
struct imake_hsv_from_rgb : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	    DEST result;
         
	    int red = src.get(RED())();
	    int green = src.get(GREEN())();
	    int blue = src.get(BLUE())();

        int h, s, v;
        int min, max;
        int delta;

        h = 0.0;

        if (red > green)
        {
          if (red > blue)
            max = red;
          else
            max = blue;

          if (green < blue)
            min = green;
          else
            min = blue;
        }
        else
        {
          if (green > blue)
            max = green;
          else
            max = blue;

          if (red < blue)
            min = red;
          else
            min = blue;
        }

        v = max;

        if (max != 0)
            s = ((max - min) * 255) / max;
        else
            s = 0;

        if (s == 0)
            h = 0;
        else
        {
          delta = max - min;
          if (red == max)
            h = (green - blue) * 42 / delta;
          else if (green == max)
            h = (2 + (blue - red)) * 42 / delta;
          else if (blue == max)
            h = (4 + (red - green)) * 42 / delta;
          //h *= 42.5;

          if (h < 0)
            h += 255;
          if (h > 255)
            h -= 255;
        }

        result.set(HUE(h));
        result.set(SATURATION(s));
        result.set(GRAY(v));
        return result;
    }
};
template <class SRC, class DEST>
struct make_rgb_from_hsv : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	    DEST result;
         
	    int h = src.get(HUE())();
	    int s = src.get(SATURATION())();
	    int v = src.get(GRAY())();

        float hue, saturation, value;
        float f, p, q, t;

        if (s == 0)
        {
            h = v;
            s = v;
            v = v;
        }
        else
        {
            hue = h * 6.0 / 255.0;
            saturation = s / 255.0;
            value = v / 255.0;

            f = hue - (int) hue;
            p = value * (1.0 - saturation);
            q = value * (1.0 - (saturation * f));
            t = value * (1.0 - (saturation * (1.0 - f)));

            switch ((int) hue)
	        {
	            case 0:
                  h = value * 255;
                  s = t * 255;
                  v = p * 255;
                  break;
                case 1:
                  h = q * 255;
                  s = value * 255;
                  v = p * 255;
                  break;
                case 2:
                  h = p * 255;
                  s = value * 255;
                  v = t * 255;
                  break;
                case 3:
                  h = p * 255;
                  s = q * 255;
                  v = value * 255;
                  break;
                case 4:
                  h = t * 255;
                  s = p * 255;
                  v = value * 255;
                  break;
                case 5:
                  h = value * 255;
                  s = p * 255;
                  v = q * 255;
                  break;
            }
        }
        result.set(RED(h));
        result.set(GREEN(s));
        result.set(BLUE(v));
        return result;
    }
};

template <class SRC, class DEST>
struct make_rgba : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	DEST result;
/*
	set(result, get(src,RED()));
	set(result, get(src,GREEN()));
	set(result, get(src,BLUE()));
	set(result, get(src,ALPHA()));

*/
	result.set(src.get(RED()));
	result.set(src.get(GREEN()));
	result.set(src.get(BLUE()));
	result.set(src.get(ALPHA()));
	return result;
    }
};
template <class SRC, class DEST>
struct make_gray_alpha : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	DEST result;
/*
	set(result, get(src,GRAY()));
	set(result, get(src,ALPHA()));
*/
	result.set(src.get(GRAY()));
	result.set(src.get(ALPHA()));
	return result;
    }
};

template <class SRC, class DEST>
struct make_gray : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	DEST result;
//	set(result, get(src,GRAY()));
	result.set(src.get(GRAY()));
	return result;
    }
};

template <class SRC, class DEST>
struct read_red : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC& src) {
	    DEST result;
	    result=src.get(RED())();
	    return result;
    }
};
template <class SRC, class DEST>
struct read_green : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	    DEST result;
	    result=src.get(GREEN())();
	    return result;
    }
};
template <class SRC, class DEST>
struct read_blue : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	    DEST result;
	    result=src.get(BLUE())();
	    return result;
    }
};
template <class SRC, class DEST>
struct read_luminance : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	    DEST result;
	    result=(src.get(RED())()+src.get(GREEN())()+src.get(BLUE())())/3;
	    return result;
    }
};

template <class SRC1, class SRC2, class DEST>
struct copy_alpha : public std::binary_function<SRC1,SRC2,DEST> {
    DEST operator()(const SRC1 & base, const SRC2 & component) {
	    DEST result(base);
	    result.set(component.get(ALPHA()));
	    return result;
    }
};

} // namespace asl

#endif
