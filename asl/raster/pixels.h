/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2004, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: pixels.h,v $
//
//   $Revision: 1.16 $
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
#ifndef _ASL_RASTER_PIXELS_H_INCLUDED_
#define _ASL_RASTER_PIXELS_H_INCLUDED_

#include "contain.h"
#include <asl/begin_end.h>
#include <functional>
#include <asl/Exception.h>

namespace asl {

    DEFINE_EXCEPTION(NotSupportedException, Exception);

template <class T>
struct PixelTraits {
    typedef T * iterator;
};

template <class THECOLOR, class VALUE, class DERIVED>
struct Pixel {
    typedef VALUE value_type;
    typedef Pixel<THECOLOR,VALUE,DERIVED> base_type;
    typedef DERIVED data_type;
    typedef VALUE * iterator;
    typedef const VALUE * const_iterator;

    Pixel() {}
    Pixel(const base_type & otherPixel) {
        v = otherPixel.get();
    }
    const base_type & operator=(const base_type & otherPixel) {
        v = otherPixel.get();
        return *this;
    }
    const base_type & operator=(const DERIVED & otherPixel) {
        v = otherPixel.get();
        return *this;
    }
    Pixel(const VALUE & initValue) : v(initValue) {}
    const base_type & operator=(const VALUE & newValue) {
        v = newValue; return *this;
    }
    base_type & operator+=(const DERIVED & x) {
        v += x.get(); return *this;
    }
    base_type & operator-=(const DERIVED & x) {
        v -= x.get(); return *this;
    }
    base_type & operator*=(const DERIVED & x) {
        v *= x.get(); return *this;
    }
    base_type & operator/=(const DERIVED & x) {
        v /= x.get(); return *this;
    }
    base_type & operator&=(const DERIVED & x) {
        v &= x.get(); return *this;
    }
    base_type & operator|=(const DERIVED & x) {
        v |= x.get(); return *this;
    }
    base_type & operator^=(const DERIVED & x) {
        v ^= x.get(); return *this;
    }
    base_type & operator>>=(const DERIVED & x) {
        v >>= x.get(); return *this;
    }
    base_type & operator<<=(const DERIVED & x) {
        v <<= x.get(); return *this;
    }

    DERIVED operator+(const DERIVED & y) const {
        return get() + y.get();
    }
    DERIVED operator-(const DERIVED & y) const {
        return get() - y.get();
    }
    DERIVED operator*(const DERIVED & y) const {
        return get() * y.get();
    }
    DERIVED operator/(const DERIVED & y) const {
        return get() / y.get();
    }
    DERIVED operator&(const DERIVED & y) const {
        return get() & y.get();
    }
    DERIVED operator|(const DERIVED & y) const {
        return get() | y.get();
    }
    DERIVED operator^(const DERIVED & y) const {
        return get() ^ y.get();
    }
    DERIVED operator>>(const DERIVED & y) const {
        return get() >> y.get();
    }
    DERIVED operator<<(const DERIVED & y) const {
        return get() << y.get();
    }

    template <class NUMBER>
    DERIVED operator+(NUMBER y) const {
        return get() + y;
    }
    template <class NUMBER>
    DERIVED operator-(NUMBER y) const {
        return get() - y;
    }
    template <class NUMBER>
    DERIVED operator*(NUMBER y) const {
        return VALUE(get() * y);
    }
    template <class NUMBER>
    DERIVED operator/(NUMBER y) const {
        return get() / y;
    }

    const VALUE & get() const {
        return v;
    }
    const VALUE & set(const VALUE & newValue) {
        return v = newValue;
    }
    bool operator==(const DERIVED& y ) const {
        return get() == y.get();
    }
    bool operator!=(const DERIVED& y ) const {
        return get() != y.get();
    }
    VALUE * begin() {
        return asl::begin(v);
    }
    VALUE * end() {
        return asl::end(v);
    }
    const VALUE * begin() const {
        return asl::begin(v);
    }
    const VALUE * end() const {
        return asl::end(v);
    }
    int bytesRequired(int x, int y, int z = 1) const {
        return sizeof(VALUE) * x * y * z;
    }
  #if 0
    // for legacy compatibility:
    VALUE operator()()  const { return v;}
    int size() const {return 1;}

    base_type & get(const data_type&) {return *this;}
    const base_type & get(const data_type&) const {return *this;}
    void set(const data_type & x) {v = x.get();}
#endif
private:
    VALUE v;
};

template <class THECOLOR, class VALUE, class DERIVED>
struct PixelTraits<Pixel<THECOLOR, VALUE, DERIVED> > {
    typedef typename Pixel<THECOLOR, VALUE, DERIVED>::iterator iterator;
};


template <class VALUE,class THECOLOR, class PIXEL>
inline std::ostream& operator<<(std::ostream& o, const Pixel<THECOLOR,VALUE,PIXEL>& s)
{
    o << s.get();
    return o;
}

template <class THECOLOR, class PIXEL>
inline std::ostream& operator<<(std::ostream& o, const Pixel<THECOLOR,char,PIXEL>& s)
{
    o << static_cast<int>(s.get());
    return o;
}

template <class THECOLOR, class PIXEL>
inline std::ostream& operator<<(std::ostream& o, const Pixel<THECOLOR,unsigned char,PIXEL>& s)
{
    o << static_cast<int>(s.get());
    return o;
}

template <class VALUE,class THECOLOR, class PIXEL>
inline std::istream & operator>>(std::istream & is, Pixel<THECOLOR,VALUE,PIXEL> & s)
{
    VALUE v;
    if (is >> v) {
        s.set(v);
    } else {
        is.setstate(std::ios::failbit);
    }
    return is;
}

template <class THECOLOR, class PIXEL>
inline std::istream & operator>>(std::istream & is, Pixel<THECOLOR,char,PIXEL> & s)
{
    int v;
    if (is >> v && int(char(v)) == v) {
        s.set(v);
    } else {
        is.setstate(std::ios::failbit);
    }
    return is;
}

template <class THECOLOR, class PIXEL>
inline std::istream&  operator>>(std::istream & is, Pixel<THECOLOR,unsigned char,PIXEL> & s)
{
    int v;
    if (is >> v && int((unsigned char)(v)) == v) {
        s.set(v);
    } else {
        is.setstate(std::ios::failbit);
    }
    return is;
}

struct ColorRed {};
struct ColorGreen {};
struct ColorBlue {};
struct ColorGray {};
struct ColorAlpha {};
struct ColorSaturation {};
struct ColorHue {};

template <class VALUE>
struct red : public Pixel<ColorRed,VALUE, red<VALUE> > {
    red() {}
    red(const VALUE & initValue) : Pixel<ColorRed,VALUE,red<VALUE> >::base_type(initValue) {}
    red(const typename Pixel<ColorRed, VALUE, red<VALUE> >::base_type & initValue) :
            Pixel<ColorRed,VALUE,red<VALUE> >::base_type(initValue) {}
};

template <template <class> class PIXEL, class VALUE>
VALUE getRedValue(const PIXEL<VALUE> & thePixel) {
    return thePixel.get(red<VALUE>()).get();
}
template <template <class> class PIXEL, class VALUE, class SRC_VALUE>
void setRedValue(PIXEL<VALUE> & thePixel, SRC_VALUE theValue) {
    thePixel.get(red<VALUE>()).set(theValue);
}

template <class VALUE>
struct green : public Pixel<ColorGreen,VALUE, green<VALUE> > {
    green() {}
    green(const VALUE & initValue) : Pixel<ColorGreen,VALUE,green<VALUE> >::base_type(initValue) {}
    green(const typename Pixel<ColorGreen, VALUE, green<VALUE> >::base_type & initValue) :
            Pixel<ColorGreen,VALUE,green<VALUE> >::base_type(initValue) {}
};
template <template <class> class PIXEL, class VALUE>
VALUE getGreenValue(const PIXEL<VALUE> & thePixel) {
    return thePixel.get(green<VALUE>()).get();
}
template <template <class> class PIXEL, class VALUE, class SRC_VALUE>
void setGreenValue(PIXEL<VALUE> & thePixel, SRC_VALUE theValue) {
    thePixel.get(green<VALUE>()).set(theValue);
}


template <class VALUE>
struct blue : public Pixel<ColorBlue,VALUE, blue<VALUE> > {
    blue() {}
    blue(const VALUE & initValue) : Pixel<ColorBlue, VALUE, blue<VALUE> >::base_type(initValue) {}
    blue(const typename Pixel<ColorBlue, VALUE, blue<VALUE> >::base_type & initValue) :
            Pixel<ColorBlue, VALUE, blue<VALUE> >::base_type(initValue) {}
};
template <template <class> class PIXEL, class VALUE>
VALUE getBlueValue(const PIXEL<VALUE> & thePixel) {
    return thePixel.get(blue<VALUE>()).get();
}
template <template <class> class PIXEL, class VALUE, class SRC_VALUE>
void setBlueValue(PIXEL<VALUE> & thePixel, SRC_VALUE theValue) {
    thePixel.get(blue<VALUE>()).set(theValue);
}

template <class VALUE>
struct alpha : public Pixel<ColorAlpha,VALUE, alpha<VALUE> > {
    alpha() {}
    alpha(const VALUE & initValue) : Pixel<ColorAlpha, VALUE, alpha<VALUE> >::base_type(initValue) {}
    alpha(const typename Pixel<ColorAlpha, VALUE, alpha<VALUE> >::base_type & initValue) :
                Pixel<ColorAlpha, VALUE, alpha<VALUE> >::base_type(initValue) {}
};
template <template <class> class PIXEL, class VALUE>
VALUE getAlphaValue(const PIXEL<VALUE> & thePixel) {
    return thePixel.get(alpha<VALUE>()).get();
}
template <template <class> class PIXEL, class VALUE, class SRC_VALUE>
void setAlphaValue(PIXEL<VALUE> & thePixel, SRC_VALUE theValue) {
    thePixel.get(alpha<VALUE>()).set(theValue);
}

template <class VALUE>
struct gray : public Pixel<ColorGray,VALUE, gray<VALUE> > {
    gray() {}
    gray(const VALUE & initValue) : Pixel<ColorGray, VALUE, gray<VALUE> >::base_type(initValue) {}
    gray(const typename Pixel<ColorGray, VALUE, gray<VALUE> >::base_type & initValue) :
                Pixel<ColorGray, VALUE, gray<VALUE> >::base_type(initValue) {}
};
template <template <class> class PIXEL, class VALUE>
VALUE getGrayValue(const PIXEL<VALUE> & thePixel) {
    return thePixel.get(gray<VALUE>()).get();
}
template <template <class> class PIXEL, class VALUE, class SRC_VALUE>
void setGrayValue(PIXEL<VALUE> & thePixel, SRC_VALUE theValue) {
    thePixel.get(gray<VALUE>()).set(theValue);
}

template <class VALUE>
struct saturation : public Pixel<ColorSaturation,VALUE, saturation<VALUE> > {
    saturation() {}
    saturation(const VALUE & initValue) : Pixel<ColorSaturation, VALUE, saturation<VALUE> >::base_type(initValue) {}
    saturation(const typename Pixel<ColorSaturation, VALUE, saturation<VALUE> >::base_type
            & initValue) : Pixel<ColorSaturation, VALUE, saturation<VALUE> >::base_type(initValue) {}
};

template <class VALUE>
struct hue : public Pixel<ColorHue,VALUE, hue<VALUE> > {
    hue() {}
    hue(const VALUE & initValue) : Pixel<ColorHue, VALUE, hue<VALUE> >::base_type(initValue) {}
    hue(const typename Pixel<ColorHue, VALUE, hue<VALUE> >::base_type & initValue) :
                Pixel<ColorHue, VALUE, hue<VALUE> >::base_type(initValue) {}
};

template <class T>
struct RGB_t : public Triple<red<T>, green<T>, blue<T>, RGB_t<T> > {
    typedef RGB_t my_type;
    typedef Triple<red<T>, green<T>, blue<T>, RGB_t<T> > data_type;

    typedef T value_type;
    typedef T sequence_type[3];
    typedef T * iterator;
    typedef const T * const_iterator;

    T& operator[](int i) {return reinterpret_cast<T*>(this)[i];}
    const T& operator[](int i) const {return reinterpret_cast<const T*>(this)[i];};
    static int size() { return Triple<red<T>, green<T>, blue<T>, RGB_t<T> >::SIZE;}
    T * begin() {
        return &(*this)[0];
    }
    T * end() {
        return &(*this)[3];
    }
    const T * begin() const {
        return &(*this)[0];
    }
    const T * end() const {
        return &(*this)[3];
    }

    RGB_t() {}
    RGB_t(const RGB_t& x) : data_type(x()) {}
    RGB_t(const data_type& x) : data_type(x) {}
    RGB_t(red<T> r, green<T> g, blue<T> b) : data_type(r, g, b) {}
    template <class V>
    RGB_t(red<V> r, green<V> g, blue<V> b) : data_type(r.get(), g.get(), b.get()) {}
    RGB_t(T r, T g, T b) : data_type(r, g, b) {}
    RGB_t(T x) : data_type(x, x, x) {}
    data_type operator()()  const { return *this;}
};

template <class T>
struct PixelTraits<RGB_t<T> > {
    typedef T * iterator;
};

template <class T>
struct BGR_t : public Triple<blue<T>, green<T>, red<T>, BGR_t<T> > {
    typedef BGR_t my_type;
    typedef Triple<blue<T>, green<T>, red<T>, BGR_t<T> > data_type;

    typedef T value_type;
    typedef T sequence_type[3];
    typedef T * iterator;
    typedef const T * const_iterator;

    T& operator[](int i) {return reinterpret_cast<T*>(this)[i];}
    const T& operator[](int i) const {return reinterpret_cast<const T*>(this)[i];};
    static int size() { return Triple<blue<T>, green<T>, red<T>, BGR_t<T> >::SIZE;}
    T * begin() {
        return &(*this)[0];
    }
    T * end() {
        return &(*this)[3];
    }
    const T * begin() const {
        return &(*this)[0];
    }
    const T * end() const {
        return &(*this)[3];
    }

    BGR_t() {}
    BGR_t(const BGR_t& x) : data_type(x()) {}
    BGR_t(const data_type& x) : data_type(x) {}
    BGR_t(red<T> r, green<T> g, blue<T> b) : data_type(b, g, r) {}
    BGR_t(blue<T> b, green<T> g, red<T> r) : data_type(b, g, r) {}
    template <class V>
    BGR_t(red<V> r, green<V> g, blue<V> b) : data_type(b.get(), g.get(), r.get()) {}
    template <class V>
    BGR_t(blue<V> r, green<V> g, red<V> b) : data_type(b.get(), g.get(), r.get()) {}
    BGR_t(T b, T g, T r) : data_type(b, g, r) {}
    BGR_t(T x) : data_type(x, x, x) {}
    data_type operator()()  const { return *this;}
};

template <class T>
struct HSV_t : public Triple<hue<T>, saturation<T>, gray<T>, HSV_t<T> >{
    typedef HSV_t my_type;
    typedef Triple<hue<T>, saturation<T>, gray<T>, HSV_t<T> > data_type;

    typedef T value_type;
    typedef T sequence_type[3];
    typedef T * iterator;
    typedef const T * const_iterator;

    T& operator[](int i) {return reinterpret_cast<T*>(this)[i];}
    const T& operator[](int i) const {return reinterpret_cast<const T*>(this)[i];};
    static int size() { return Triple<hue<T>, saturation<T>, gray<T>, HSV_t<T> >::SIZE;}
    T * begin() {
        return &(*this)[0];
    }
    T * end() {
        return &(*this)[3];
    }
    const T * begin() const {
        return &(*this)[0];
    }
    const T * end() const {
        return &(*this)[3];
    }
    HSV_t() {}
    HSV_t(const HSV_t& x) : data_type(x()) {}
    HSV_t(const data_type& x) : data_type(x) {}
    HSV_t(hue<T> h, saturation<T> s, gray<T> v) : data_type(h, s, v) {}
    template <class V>
    HSV_t(hue<V> h, saturation<V> s, gray<V> v) : data_type(h.get(), s.get(), v.get()) {}
    HSV_t(T h, T s, T v) : data_type(h, s, v) {}
    HSV_t(T x) : data_type(x, x, x) {}
    data_type operator()()  const { return *this;}
};

template <class T>
struct RGBA_t : public Quad<red<T>, green<T>, blue<T>, alpha<T>, RGBA_t<T> > {
    typedef RGBA_t my_type;
    typedef Quad<red<T>, green<T>, blue<T>, alpha<T>, RGBA_t<T> > data_type;

    typedef T value_type;
    typedef T sequence_type[4];
    typedef T * iterator;
    typedef const T * const_iterator;

    T& operator[](int i) {return reinterpret_cast<T*>(this)[i];}
    const T& operator[](int i) const {return reinterpret_cast<const T*>(this)[i];};
    static int size() { return Quad<red<T>, green<T>, blue<T>, alpha<T>, RGBA_t<T> >::SIZE;}
    T * begin() {
        return &(*this)[0];
    }
    T * end() {
        return &(*this)[4];
    }
    const T * begin() const {
        return &(*this)[0];
    }
    const T * end() const {
        return &(*this)[4];
    }

    RGBA_t() {}
    RGBA_t(const RGBA_t& x) : data_type(x()) {}
    RGBA_t(const data_type& x) : data_type(x) {}
    RGBA_t(red<T> r, green<T> g, blue<T> b, alpha<T> a) : data_type(r, g, b, a) {}
    template <class V>
    RGBA_t(red<V> r, green<V> g, blue<V> b, alpha<T> a) : data_type(r.get(), g.get(), b.get(), a.get()) {}
    RGBA_t(T r, T g, T b, T a) : data_type(r, g, b, a) {}
    data_type operator()()  const { return *this;}
};

template <class T>
struct ABGR_t : public Quad<alpha<T>, blue<T>, green<T>, red<T>, ABGR_t<T> > {
    typedef ABGR_t my_type;
    typedef Quad<alpha<T>, blue<T>, green<T>, red<T>, ABGR_t<T> > data_type;

    typedef T value_type;
    typedef T sequence_type[4];
    typedef T * iterator;
    typedef const T * const_iterator;

    T& operator[](int i) {return reinterpret_cast<T*>(this)[i];}
    const T& operator[](int i) const {return reinterpret_cast<const T*>(this)[i];};
    static int size() { return Quad<alpha<T>, blue<T>, green<T>, red<T>, ABGR_t<T> >::SIZE;}
    T * begin() {
        return &(*this)[0];
    }
    T * end() {
        return &(*this)[4];
    }
    const T * begin() const {
        return &(*this)[0];
    }
    const T * end() const {
        return &(*this)[4];
    }

    ABGR_t() {}
    ABGR_t(const ABGR_t& x) : data_type(x()) {}
    ABGR_t(const data_type& x) : data_type(x) {}
    ABGR_t(alpha<T> a, blue<T> b, green<T> g, red<T> r ) : data_type(a, b, g, r) {}
    template <class V>
    ABGR_t(alpha<V> a, blue<V> b, green<V> g, red<V> r) : data_type(a.get(), b.get(), g.get(), r.get()) {}
    ABGR_t(T a, T b, T g, T r) : data_type(a, b, g, r) {}
    data_type operator()()  const { return *this;}
};

template <class T>
struct BGRA_t : public Quad<blue<T>, green<T>, red<T>, alpha<T>, BGRA_t<T> > {
    typedef BGRA_t my_type;
    typedef Quad<blue<T>, green<T>, red<T>, alpha<T>, BGRA_t<T> > data_type;

    typedef T value_type;
    typedef T sequence_type[4];
    typedef T * iterator;
    typedef const T * const_iterator;

    T& operator[](int i) {return reinterpret_cast<T*>(this)[i];}
    const T& operator[](int i) const {return reinterpret_cast<const T*>(this)[i];};
    static int size() { return Quad<blue<T>, green<T>, red<T>, alpha<T>, BGRA_t<T> >::SIZE;}
    T * begin() {
        return &(*this)[0];
    }
    T * end() {
        return &(*this)[4];
    }
    const T * begin() const {
        return &(*this)[0];
    }
    const T * end() const {
        return &(*this)[4];
    }

    BGRA_t() {}
    BGRA_t(const BGRA_t& x) : data_type(x()) {}
    BGRA_t(const data_type& x) : data_type(x) {}
    BGRA_t(blue<T> b, green<T> g, red<T> r, alpha<T> a) : data_type(b, g, r, a) {}
    template <class V>
    BGRA_t(blue<V> b, green<V> g, red<V> r, alpha<V> a) : data_type(b.get(), g.get(), r.get(), a.get()) {}
    BGRA_t(T b, T g, T r, T a) : data_type(b, g, r, a) {}
    data_type operator()()  const { return *this;}
};

struct DXT1 {
    unsigned long long value; // 8 bytes

    int bytesRequired(int x, int y, int z = 1) const {
        return x * y * z / 2;
    }
    inline
    DXT1 operator+(DXT1 v) const {
        DXT1 myResult;
        myResult.value = value + v.value;
        return myResult;
    }
    inline
    DXT1 operator-(DXT1 v) const {
        DXT1 myResult;
        myResult.value = value - v.value;
        return myResult;
    }
    inline
    bool operator==(DXT1 v) const {
        return (value == v.value);
    }
};

struct DXT1a {
    unsigned long long value; // 8 bytes

    int bytesRequired(int x, int y, int z = 1) const {
        return x * y * z / 2;
    }
    inline
    DXT1a operator+(DXT1a v) const {
        DXT1a myResult;
        myResult.value = value + v.value;
        return myResult;
    }
    inline
    DXT1a operator-(DXT1a v) const {
        DXT1a myResult;
        myResult.value = value - v.value;
        return myResult;
    }
    inline
    bool operator==(DXT1a v) const {
        return (value == v.value);
    }
};
struct DXT3 {
    unsigned long long value1; // 8 bytes
    unsigned long long value2; // 8 bytes

    int bytesRequired(int x, int y, int z = 1) const {
        return x * y * z ;
    }
    inline
    DXT3 operator+(DXT3 v) const {
        DXT3 myResult;
        myResult.value1 = value1 + v.value1;
        myResult.value2 = value2 + v.value2;
        return myResult;
    }
    inline
    DXT3 operator-(DXT3 v) const {
        DXT3 myResult;
        myResult.value1 = value1 - v.value1;
        myResult.value2 = value2 - v.value2;
        return myResult;
    }
    inline
    bool operator==(DXT3 v) const {
        return (value1 == v.value1 && value2 == v.value2);
    }
};

struct DXT5 {
    unsigned long long value1; // 8 bytes
    unsigned long long value2; // 8 bytes

    int bytesRequired(int x, int y, int z = 1) const {
        return x * y * z ;
    }
    inline
    DXT5 operator+(DXT5 v) const {
        DXT5 myResult;
        myResult.value1 = value1 + v.value1;
        myResult.value2 = value2 + v.value2;
        return myResult;
    }
    inline
    DXT5 operator-(DXT5 v) const {
        DXT5 myResult;
        myResult.value1 = value1 - v.value1;
        myResult.value2 = value2 - v.value2;
        return myResult;
    }
    inline
    bool operator==(DXT5 v) const {
        return (value1 == v.value1 && value2 == v.value2);
    }
};


template <class T>
struct PackedRaster;

template <>
struct PackedRaster<DXT1> {
	enum {Factor = 16};
};

template <>
struct PackedRaster<DXT1a> {
	enum {Factor = 16};
};

template <>
struct PackedRaster<DXT3> {
	enum {Factor = 16};
};

template <>
struct PackedRaster<DXT5> {
	enum {Factor = 16};
};

inline std::ostream& operator<<(std::ostream& o, const DXT1 & s) {
    return o << s.value;
}
inline std::istream & operator>>(std::istream & is, DXT1 & s) {
    return is >> s.value;
}

inline std::ostream& operator<<(std::ostream& o, const DXT1a & s) {
    return o << s.value;
}
inline std::istream & operator>>(std::istream & is, DXT1a & s) {
    return is >> s.value;
}

inline std::ostream& operator<<(std::ostream& o, const DXT3 & s) {
    return o << s.value1 << s.value2;
}
inline std::istream & operator>>(std::istream & is, DXT3 & s) {
    return is >> s.value1 >> s.value2;
}

inline std::ostream& operator<<(std::ostream& o, const DXT5 & s) {
    return o << s.value1 << s.value2;
}
inline std::istream & operator>>(std::istream & is, DXT5 & s) {
    return is >> s.value1 >> s.value2;
}

#if 0
template <class SRC, class DEST>
struct route : public std::unary_function<SRC, DEST> {
    DEST operator()(const SRC & src) {
	DEST result;
	result.set(result_type( src.get(src)() ));
	return result;
    }
};

template <class SRC, class DEST, class CONV1, class CONV2, class CONV3>
struct output3 {
    typedef SRC argument_type;
    typedef DEST result_type;
    typedef typename CONV1::argument_type CONV1_argument_type;
    typedef typename CONV2::argument_type CONV2_argument_type;
    typedef typename CONV3::argument_type CONV3_argument_type;
    result_type operator()(const argument_type& src) {
	    result_type result;
	    result.set(CONV1()(src.get(CONV1_argument_type())));
	    result.set(CONV2()(src.get(CONV2_argument_type())));
	    result.set(CONV3()(src.get(CONV3_argument_type())));
	    return result;
    }
};

template <class SRC, class DEST, class CONV1, class CONV2, class CONV3, class CONV4>
struct output4 {
    typedef SRC argument_type;
    typedef DEST result_type;
    typedef typename CONV1::argument_type CONV1_argument_type;
    typedef typename CONV2::argument_type CONV2_argument_type;
    typedef typename CONV3::argument_type CONV3_argument_type;
    typedef typename CONV4::argument_type CONV4_argument_type;
    result_type operator()(const argument_type& src) {
	    result_type result;
	    result.set(CONV1()(src.get(CONV1_argument_type())));
	    result.set(CONV2()(src.get(CONV2_argument_type())));
	    result.set(CONV3()(src.get(CONV3_argument_type())));
	    result.set(CONV4()(src.get(CONV4_argument_type())));
	    return result;
    }
};
#endif

} // namespace asl
#endif
