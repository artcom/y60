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
//    $RCSfile: palgo.h,v $
//
//   $Revision: 1.7 $
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
#ifndef _ASL_RASTER_PALGO_H_INCLUDED_
#define _ASL_RASTER_PALGO_H_INCLUDED_

#include <asl/Vector234.h>

#include <stddef.h>
#include <algorithm>
#include <functional>

#include <asl/Exception.h>

#include "standard_pixel_types.h"
#include "raster.h"
#include "piterator.h"
#include "pixel_functions.h"

namespace asl {

inline void char_fill(char *begin, char* end, char x) {
    memset(begin, x, end - begin);
}

inline void pfill(char *begin, char* end, char x) {
    char_fill(begin, end, x);
}

inline void char_copy(char *begin, char* end, char* dest)
{
    memcpy(dest, begin, end - begin);
}
inline void copy(char *begin, char* end, char* dest)
{
    char_copy(begin, end, dest);
}

template <class InputIterator, class OutputIterator>
OutputIterator pcopy(InputIterator first, InputIterator last,
                     OutputIterator result)
{
    while (first != last) {
        *result = *first;
        ++first;
        ++result;
    }
    return result;
}

template <class ForwardIterator, class T>
void pfill(ForwardIterator first, ForwardIterator last, const T& value) {
    while (first != last) {*first = value; ++first;}
}

// lazy input iterator based 1D-scale 
template <
    class InputIterator,
    class OutputIterator,
    class SrcDistance,
    class SrcT,
    class DestDistance,
    class DestT
>
OutputIterator scale1d(InputIterator src_begin, InputIterator src_end, 
                     OutputIterator dest_begin, OutputIterator dest_end, 
                     SrcDistance*, SrcT*, DestDistance*, DestT*)
    {
    SrcDistance src_size = src_end - src_begin;
    DestDistance dest_size = dest_end - dest_begin;
    fraction_iterator<InputIterator, SrcT, const SrcT &, SrcDistance> src(src_begin,src_size,dest_size);
    while (dest_begin != dest_end)
    {
        *dest_begin = *src;
        ++dest_begin;
        ++src;
    }
    return dest_begin;
}

template <class InputIterator, class OutputIterator>
OutputIterator scale1d(InputIterator src_begin, InputIterator src_end, 
                     OutputIterator dest_begin, OutputIterator dest_end)
{
    return scale1d(src_begin, src_end, dest_begin, dest_end,
        distance_type(src_begin), value_type(src_begin),
        distance_type(dest_begin), value_type(dest_end));
}

template <
    class hInputIterator,   class vInputIterator,
    class hOutputIterator,  class vOutputIterator,
    class hSrcDistance,     class vSrcDistance, class SrcT, 
    class hDestDistance,    class vDestDistance, class DestT
>
void scale2d(hInputIterator h_src_begin, 
            vInputIterator v_src_begin,
            hSrcDistance h_src_size, 
            vSrcDistance v_src_size, 
            hOutputIterator h_dest_begin,
            vOutputIterator v_dest_begin,
            hDestDistance h_dest_size,
            vDestDistance v_dest_size,  SrcT*, DestT*)
{
    fraction_iterator<hInputIterator, SrcT, const SrcT &, hSrcDistance> h_scaled_src_begin(h_src_begin, h_src_size, h_dest_size);
    fraction_iterator<vInputIterator, SrcT, const SrcT &, vSrcDistance> v_scaled_src_begin(v_src_begin, v_src_size, v_dest_size);
    rect_iterator<
        fraction_iterator<hInputIterator, SrcT, const SrcT &, hSrcDistance>,
        fraction_iterator<vInputIterator, SrcT, const SrcT &, vSrcDistance>,
        SrcT,
        const SrcT &
    > src_begin(h_scaled_src_begin, v_scaled_src_begin, h_dest_size);
    rect_iterator<
        fraction_iterator<hInputIterator, SrcT, const SrcT &, hSrcDistance>,
        fraction_iterator<vInputIterator, SrcT, const SrcT &, vSrcDistance>,
        SrcT,
        const SrcT &
    > src_end(src_begin + (h_dest_size * v_dest_size));
//#define RECT_ITER_OUT
#ifdef RECT_ITER_OUT
    rect_iterator<hOutputIterator, vOutputIterator, SrcT> dest_rect_begin(h_dest_begin, v_dest_begin, h_dest_size);
    rect_iterator<hOutputIterator, vOutputIterator, SrcT> dest_rect_end(dest_rect_begin + (h_dest_size * v_dest_size));

    scale1d(src_begin, src_end, dest_rect_begin, dest_rect_end);
#else
    scale1d(src_begin, src_end, h_dest_begin, h_dest_begin + (h_dest_size * v_dest_size));
#endif
}

template <
    class hInputIterator,class vInputIterator,
    class hSrcDistance, class vSrcDistance,  
    class hOutputIterator,  class vOutputIterator,
    class hDestDistance, class vDestDistance
>
void scale2d(hInputIterator h_src_begin, 
                         vInputIterator v_src_begin,
                         hSrcDistance h_src_size, 
                         vSrcDistance v_src_size, 
                         hOutputIterator h_dest_begin,
                         vOutputIterator v_dest_begin,
                         hDestDistance h_dest_size,
                         vDestDistance v_dest_size)
{
    scale2d(h_src_begin, v_src_begin, h_src_size, v_src_size, 
        h_dest_begin, v_dest_begin, h_dest_size, v_dest_size,
        value_type(h_src_begin), value_type(h_dest_begin));      
}

template <class Srcmat, class Destmat>
void scale2d(const Srcmat & src, Destmat & dest)
{
    scale2d(src.hbegin(), src.vbegin(), src.hsize(), src.vsize(),
           dest.hbegin(), dest.vbegin(), dest.hsize(), dest.vsize()); 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

template <class InputIterator, class T>
T paccumulate(InputIterator first, InputIterator last, T init) {
    while (first != last) {
        //	std::cerr << "paccumulate << " << *first << std::endl;
        //	init = init + *first++;
        init = init + auto_cast<T>(*first);
        ++first;
    }
    return init;
}

template <class Iterator, class T, class SumT>
struct average_function : public std::binary_function<Iterator, Iterator, T> {
    average_function() {}
    average_function(const T & Init_val) : init_val(Init_val) {}
    T operator()(const Iterator& begin, const Iterator& end) const {
        SumT result = auto_cast<SumT>(init_val);
        //	std::cerr << "average_function:" << std::endl;
        //T beginval = *begin;
        //T endval = *end;
        //	void * bp = (void*)&(*begin);
        //	void * ep = (void*)&(*end);
        //	std::cerr << "average_function: begin = " << beginval << " @ " << bp << std::endl;
        //	std::cerr << "average_function: end = " << endval << " @ " << ep << std::endl;

        int numvalues = end - begin;
        //	std::cerr << "average_function: numvalues = " << numvalues << std::endl;
        //	std::cerr << "average_function: initval = " << result << std::endl;
        result = paccumulate(begin, end, result) / numvalues;
        //	std::cerr << "average_function: result = " << result << std::endl;
        return auto_cast<T>(result);
    }
    T init_val;
};

template <
    class hInputIterator, class vInputIterator,
    class OutputIterator,
    class IntervalOperation,
    class hSubIterator, class vSubIterator,
    class hDistance, class vDistance,
    class T
>
OutputIterator transform_raster(hInputIterator hfirst, vInputIterator vfirst, 
                                hDistance hsize, vDistance vsize, 
                                OutputIterator result,
                                IntervalOperation IntervalOp,
                                const hSubIterator& hsub, const vSubIterator& vsub,
                                T)
{
#define DBTRANS
#ifdef DBTRANS
    std::cerr << "transform_matrix:" << std::endl;
    std::cerr << "hfirst = " << *hfirst << " @ " << (void*)&(*hfirst) << std::endl;
    std::cerr << "vfirst = " << *vfirst << " @ " << (void*)&(*vfirst) << std::endl;
    std::cerr << "hsize = " << hsize <<  std::endl;
    std::cerr << "vsize = " << vsize <<  std::endl;
    std::cerr << "result @ " << (void*)&(*result) << std::endl;
#endif
    vInputIterator vend = vfirst + vsize;
    vInputIterator vnext = vfirst + 1;
#ifdef DBTRANS
    std::cerr << "vfirst = " << *vfirst << " @ " << (void*)&(*vfirst) << std::endl;
    std::cerr << "vnext = " << *vnext << " @ " << (void*)&(*vnext) << std::endl;
    std::cerr << "vend = " << *vend << " @ " << (void*)&(*vend) << std::endl;
#endif
    while (vfirst!=vend) {
        hfirst = iseek(hfirst, vfirst);
        hInputIterator hend = hfirst + hsize;
        hInputIterator hnext = hfirst + 1;
        hInputIterator hvnext = iseek(hnext, vnext);
#ifdef DBTRANS
        std::cerr << "Start transform line,  hfirst = " << *hfirst << " @ " << (void*)&(*hfirst) << std::endl;
#endif
        while (hfirst!=hend) {

#ifdef DBTRANS
            std::cerr << "hfirst = " << *hfirst << " @ " << (void*)&(*hfirst) << std::endl;
            std::cerr << "hnext = " << *hnext << " @ " << (void*)&(*hnext) << std::endl;
            std::cerr << "hend = " << *hend << " @ " << (void*)&(*hend) << std::endl;
            std::cerr << "hvnext = " << *hvnext << " @ " << (void*)&(*hvnext) << std::endl;
#endif
            int hsample_size = iseek(hsub, hnext) - iseek(hsub, hfirst);
#ifdef DBTRANS
            std::cerr << "hsample_size = " << hsample_size  << std::endl;
#endif
            rect_iterator<hSubIterator,vSubIterator, T, T&, hDistance> ibegin(
                iseek(hsub, hfirst), iseek(vsub, hfirst), hsample_size);

            rect_iterator<hSubIterator,vSubIterator, T, T&, hDistance> iend(
                iseek(hsub, hvnext), iseek(vsub, hvnext), hsample_size );

            *result++ = IntervalOp(ibegin, iend);

            hfirst++;
            hnext++;
            hvnext++;
        }
        vfirst++;
        vnext++;
    }
    return result;
}

template <class InputIterator, class OutputIterator, class UnaryOperation>
OutputIterator transform_it(InputIterator first, InputIterator last,
                            OutputIterator result, UnaryOperation op)
{
    while (first != last) {
        *result++ = op(first++);
    }
    return result;
}


template <class hiterator, class viterator, class AverageFunction, class Distance,  class T, class OutputIterator>
void scale_down(hiterator hbegin, viterator vbegin,
                Distance hsize, Distance vsize,
                OutputIterator dest,
                Distance hdestsize, Distance vdestsize, 
                AverageFunction af, T x)
{
    typedef step_iterator<hiterator,T, T&,Distance> hscale_iterator;
    typedef step_iterator<viterator,T, T&,Distance> vscale_iterator;

    typedef fraction_iterator<hiterator,T, T&,Distance> fraction_hscale_iterator;
    typedef fraction_iterator<viterator,T, T&,Distance> fraction_vscale_iterator;

    if (hsize % hdestsize) {
        fraction_hscale_iterator h_outer_begin(hbegin, hsize, hdestsize);
        if (vsize % vdestsize) {
            fraction_vscale_iterator v_outer_begin(vbegin, vsize, vdestsize);
            transform_raster(h_outer_begin, v_outer_begin, hdestsize, vdestsize, dest, 
                af, 
                hbegin, vbegin, x );
        }
        else // ganzahliger v - faktor
        {
            vscale_iterator v_outer_begin(vbegin, vsize / vdestsize);
            transform_raster(h_outer_begin, v_outer_begin, hdestsize, vdestsize, dest, 
                af, 
                hbegin, vbegin, x );
        }
    }
    else {
        hscale_iterator h_outer_begin(hbegin, hsize / hdestsize);
        if (vsize % vdestsize) {
            fraction_vscale_iterator v_outer_begin(vbegin, vsize, vdestsize);
            transform_raster(h_outer_begin, v_outer_begin, hdestsize, vdestsize, dest, 
                af, 
                hbegin, vbegin, x );
        }
        else // ganzahliger v - faktor
        {
            vscale_iterator v_outer_begin(vbegin, vsize / vdestsize);
            transform_raster(h_outer_begin, v_outer_begin, hdestsize, vdestsize, dest, 
                af, 
                hbegin, vbegin, x );
        }

    }
}


template <class Mat, class SumT>
SumT rectsum(const Mat& src, int xstart, int ys, int xend, int yend, SumT& sum) {
    for (; ys != yend; ys++) {
        for (int xs = xstart; xs != xend; xs++) {
            sum= sum + auto_cast<SumT>(src(xs, ys));
        }
    }
    return sum;
} 

template <class Srcmat, class Destmat, class SumT>
void scale_down(const Srcmat& src, Destmat& dest, int xfactor, int yfactor, const SumT& initval)
{
    typedef typename Destmat::value_type dest_value_type;

    const int xsrcsize = src.xsize();
    const int xdestsize = dest.xsize();
    const int ns = xfactor * yfactor;

    for (int y = 0; y < dest.ysize();y++) {
        int ystart = y * yfactor;
        int yend = ystart + yfactor;
        for (int x = 0; x < xdestsize;x++) {
            const int xstart = x * xfactor;
            const int xend = xstart + xfactor;
            SumT sum(initval);
            rectsum(src, xstart, ystart, xend, yend, sum);
            dest(x, y) = auto_cast<dest_value_type>(sum/ns);
        }
    }    
}

DEFINE_EXCEPTION(SizeMismatchException, asl::Exception);

template <class Srcmat, class Destmat, class SumT>
void halfsize(const Srcmat& src, Destmat& dest, const SumT& initval)
{
    typedef typename Destmat::value_type dest_value_type;

    if (src.xsize() != dest.xsize() * 2 ||
        src.ysize() != dest.ysize() * 2)
    {
        throw asl::SizeMismatchException(JUST_FILE_LINE);
    }
    const int xsrcsize = src.xsize();
    const int xdestsize = dest.xsize();

    for (int y = 0; y < dest.ysize();y++) {
        int sy = y * 2;
        for (int x = 0; x < xdestsize;x++) {
            int sx = x * 2;
            SumT mySum = auto_cast<SumT>(src(sx,sy));
            mySum += auto_cast<SumT>(src(sx+1,sy));
            mySum += auto_cast<SumT>(src(sx,sy+1));
            mySum += auto_cast<SumT>(src(sx+1,sy+1));
            dest(x, y) = auto_cast<dest_value_type>(mySum/4);
        }
    }    
}
//
// Die folgende skalierroutine arbeitet ganzzahlig und ist geeignet
// fuer ganzzahlige skalierverhaeltnisse oder starkes nicht - ganzzahliges
// verkleinern ab Faktor 3
// 
template <class Srcmat, class Destmat, class SumT>
void scale_down(const Srcmat& src, Destmat& dest, const SumT& initval)
{
    typedef typename Destmat::value_type dest_value_type;

    if ((src.xsize()%dest.xsize() == 0) && 
        (src.ysize()%dest.ysize() == 0)) {
            scale_down(src, dest, src.xsize()/dest.xsize(), src.ysize()/dest.ysize(), initval);
        }
    else {

        const int xsrcsize = src.xsize();
        const int xdestsize = dest.xsize();

        for (int y = 0; y < dest.ysize();y++) {
            int ystart = y * src.ysize() / dest.ysize();
            int yend = (y+1) * src.ysize() / dest.ysize();
            for (int x = 0; x < xdestsize;x++) {
                const int xstart = x * xsrcsize / xdestsize;
                const int xend = (x+1) * xsrcsize / xdestsize;
                SumT sum(initval);
                rectsum(src, xstart, ystart, xend, yend, sum);
                int ns = (yend - ystart) * (xend - xstart);
                dest(x, y) = auto_cast<dest_value_type>(sum/ns);
            }
        }
    }
}

template <class T>
struct bilinear {
    T operator()(const T & minmin, const T & maxmin, const T & minmax, const T & maxmax,
        float xratio, float yratio)
    {
        //	T result = T(0); 
        T result = T(minmin * ((1.0 - xratio) * (1.0 - yratio)) +
                     maxmin * (       xratio  * (1.0 - yratio)) +
                     minmax * ((1.0 - xratio) *        yratio ) +
                     maxmax * (       xratio  *        yratio ));
        //	std::cerr << "bilinear(" << minmin << "," << maxmin << "," << minmax << "," << maxmax << ","
        //	     << xratio << "," <<yratio << ") = " << result << std::endl;
        return result;
    }
};

template <class T>
struct bicubic {
    T operator()(const T& minmin, const T& maxmin, const T& minmax, const T& maxmax,
        float xratio, float yratio)
    {
        return bilinear<T>(minmin, maxmin, minmax, maxmax, xratio * xratio, yratio * yratio);
    }
};

template <class Mat, class T>
struct offset_get_pixel{
    offset_get_pixel(float Xoffset = 0.5, float Yoffset = 0.5 ): xoffset(Xoffset), yoffset(Yoffset) {}
    float xoffset;
    float yoffset;
    T operator()(const Mat& src, float x, float y )
    {
        T result = auto_cast<T>(src(x+xoffset, y+yoffset));
        //	std::cerr << "offset_get_pixel(" << x << "," << y << ") = " << result << std::endl;
        return result;
    }
};

template <class Mat, class T>
struct default_get_pixel {
    T operator()(const Mat & src, int x, int y )
    {
        T result = auto_cast<T>(src(x, y));
        //	std::cerr << "default_get_pixel(" << x << "," << y << ") = " << result << std::endl;
        return result;
    }
};

template <class Mat, class T>
struct safe_get_pixel {
    safe_get_pixel(const T& Outvalue) : outvalue(Outvalue) {}
    T outvalue;
    T operator()(const Mat& src, float x, float y )
    {
        if ((x>=0) && (x<src.xsize()) && (y>=0) && (y<src.ysize()))
            return convert(src(x, y), T());
        else
            return outvalue;
    }
};

template <class Mat, class T>
struct mod_get_pixel {
    T operator()(const Mat& src, float x, float y )
    {
        if ((x>=0) && (x<src.xsize()) && (y>=0) && (y<src.ysize()))
            return convert(src(x, y), T());
        else
            return convert(src(mod(x, src.xsize()), mod(y,src.ysize() ), T()));
    }
};

template <class Srcmat, class Destmat, class SampleFunction, class AccessFunction>
void resample(const Srcmat& src, Destmat& dest, SampleFunction sample, AccessFunction getpixel)
{
    typedef typename Destmat::value_type dest_value_type;
    const int xdestsize = dest.xsize();

    float xfactor = ((float)src.xsize()-1)/((float)dest.xsize());
    float yfactor = ((float)src.ysize()-1)/((float)dest.ysize());

    for (int y = 0; y < dest.ysize();y++) {
        const float ysrcf = (float)y * yfactor;
        const int ysrc = static_cast<int>(ysrcf);
        const float yratio = ysrcf - (float)ysrc;
        for (int x = 0; x < xdestsize;x++) {
            const float xsrcf = (float)x * xfactor;
            const int xsrc = static_cast<int>(xsrcf);
            const float xratio = xsrcf - (float)xsrc;
            dest(x, y) = auto_cast<dest_value_type>(
                 sample(getpixel(src, xsrc,   ysrc),
                        getpixel(src, xsrc+1, ysrc), 
                        getpixel(src, xsrc,   ysrc+1),
                        getpixel(src, xsrc+1, ysrc+1),
                        xratio,
                        yratio));
        }
    }    
}


/**
* resample erzeugt ein bilinear gesampelte verkleinerte oder vergroessert Variante
* von src. Die linke und obere Kante der erzeugten raster liegen genau
* auf der linken bzw. oberen Kante der Ausgangsmatrix. Die rechte und untere Kante
* werden beim samplen in der Ausgangsmatrix niemals erreicht.
* Dadurch kann ein passendes Mosaic aus (gleichgrossen!) Patches zusammengesamplet werden,
* indem die letzte gueltige Pixelreihe (nicht end!) als erste gueltige Pixelreihe beim
* Samplen des Naechsten Ausgangspatches wiederholt wird.
*/
template <class Srcmat, class Destmat, class SumT>
void resample(const Srcmat& src, Destmat& dest, SumT) {
    resample(src, dest, bilinear<SumT>(), default_get_pixel<Srcmat, SumT>());
}

template <
    class hInputIterator, class vInputIterator,
    class OutputIterator,
    class IntervalOperation,
    class hSubIterator, class vSubIterator,
    class hDistance, class vDistance,
    class T
>
OutputIterator transform_matrix2(hInputIterator hfirst, const vInputIterator vfirst, 
                                 hDistance hsize, vDistance vsize, 
                                 OutputIterator result,
                                 IntervalOperation IntervalOp,
                                 const hSubIterator& hsub, const vSubIterator& vsub,
                                 T) 
{
    vInputIterator vend = vfirst + vsize;
    vInputIterator vnext = vfirst + 1;
    while (vfirst!=vend) {
        hfirst = iseek(hfirst, vfirst);
        hInputIterator hend = hfirst + hsize;
        hInputIterator hnext = hfirst + 1;
        hInputIterator hvnext = iseek(hnext, vnext);
        while (hfirst!=hend) {
            int hsample_size = iseek(hsub, hnext) - iseek(hsub, hfirst);
            rect_iterator<hSubIterator,vSubIterator, T>ibegin(
                iseek(hsub, hfirst), iseek(vsub, hfirst), hsample_size);

            rect_iterator<hSubIterator,vSubIterator, T>iend(
                iseek(hsub, hvnext), iseek(vsub, hvnext), hsample_size );

            *result++ = IntervalOp(ibegin, iend);

            hfirst++;
            hnext++;
            hvnext++;
        }
        vfirst++;
        vnext++;
    }
    return result;
}
//////////////////////////////////////////////////////////////////////////////////		  

template <class OutputIterator, class T>
void bresenham_fill(OutputIterator it, int stride, int dx, int dy, 
                    const T& fill_value)
{
    bresenham_iterator<OutputIterator, T, T&, int> begin(it, stride, dx, dy);
    bresenham_iterator<OutputIterator, T, T&, int> end(it + stride * dy + dx);

    while (begin!=end) {
        *begin = fill_value;
        ++begin;
    }
}

template <class pixelType> 
double color_abs (const pixelType& color )
{
    const int size = color.size ();
    double sum = 0.0;

    for (int i = 0; i<size; i++ )
    {
        sum += (double ) color[i] * (double ) color[i];
    }

    return (sqrt (sum ) );
}

template <class RESULT, class RASTER>
RESULT moment(int p, int q, const RASTER & image, RESULT init) {
    int x, y;
    RESULT result = init;
    for (int y = 0; y < image.vsize(); y++) {
        int yp = ipow(y,q);
        for (int x = 0; x < image.hsize(); x++) {
            int xp = ipow(x,p);
            result += (RESULT)xp * (RESULT)yp * (RESULT) image(x,y).get();
            //           cerr << DBG(x) << DBG(y) << DBG(xp) << DBG(yp) << DBG(image(x,y)) << DBG(result) << endl;
        }

    }     
    return result;
}

struct MomentResults {
    float l;
    float w;
    Vector2<float> center;
    float major_angle;
    Vector2<float> major_dir;
    float minor_angle;
    Vector2<float> minor_dir;
};

inline
asl::Vector2f
direction_vector( float theAngle ) {
    return Vector2f( cos( theAngle ), sin( theAngle ));
}

inline
asl::Vector2f
orthonormal_vector( const Vector2f & v) {
    return Vector2f( v[1], - v[0] );
}

template <class RASTER>
bool analyseMoments(const RASTER & mat, MomentResults & mom)
{
    //ptime mom_begin;
    float m00 = moment(0,0,mat,float(0));
    float m10 = moment(1,0,mat,float(0));
    float m01 = moment(0,1,mat,float(0));
    float m11 = moment(1,1,mat,float(0));
    float m20 = moment(2,0,mat,float(0));
    float m02 = moment(0,2,mat,float(0));
    //ptime momentset_time = ptime()-mom_begin;

    //cerr << DBG(m00) << DBG(m10) <<DBG(m01) <<DBG(m11) <<DBG(m20) <<DBG(m02) <<endl;

    float xc = (m10/m00);
    float yc = (m01/m00);

    float cm11 = m11/m00 - xc * yc;
    float cm20 = m20/m00 - xc * xc;
    float cm02 = m02/m00 - yc * yc;

    float major_axis_angle = 0.5 * atan( 2 * cm11 / ( cm20 - cm02));

    //cerr << DBG(xc) << DBG(yc) <<DBG(cm11) <<DBG(cm20) <<DBG(cm02) <<
    //        DBG(major_axis_angle) << endl;

    float a = cm20;
    float b = 2 * cm11;
    float c = cm02;

    mom.l = sqrt( ((a+c) + sqrt(b*b + (a-c)*(a-c)))/2);
    mom.w = sqrt( ((a+c) - sqrt(b*b + (a-c)*(a-c)))/2);

    mom.center = Vector2<float>(xc,yc);
    mom.major_angle = major_axis_angle;
    mom.major_dir = direction_vector(major_axis_angle);
    // TODO mom.minor_dir = mom.major_dir.ortho();
    // TODO mom.minor_angle = angle_of(mom.minor_dir);
    
    //cerr << DBG(a) << DBG(b) <<DBG(c) <<DBG(mom.l) <<DBG(mom.w) <<
     //       DBG(gcenter) << DBG(mom.major_dir) << DBG(mom.minor_dir) <<endl;

    return true;
}



} // namespace asl

#endif
