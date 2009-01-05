/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef MORE_ALGO_H
#define MORE_ALGO_H

#include <map.h>

#include <pix/matrix.h>
#include <pix/submat.h>
#include <asl/raster/palgo.h>
#include <pm/vec2.h>
#include <pm/box2.h>

#include <vector.h>
#include <pm/ptime.h>

#include <string>

#include "linalg.h"
#include "ellipse_fit.h"

template <class VALUE>
vec2<float> float_vec(const vec2<VALUE>& f) {
    return vec2<float>(f[0],f[1]);
}
template <class VALUE>
vec2<int> int_vec(const vec2<VALUE>& f) {
    return vec2<int>(static_cast<int>(f[0]),static_cast<int>(f[1]));
}

template <class T>
struct less_first : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x.first < y.first; }
};
template <class T>
struct greater_first : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x.first > y.first; }
};
template <class T>
struct less_second : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x.second < y.second; }
};
template <class T>
struct greater_second : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x.second > y.second; }
};

template <class T>
struct less_first_x : public binary_function<T, T, bool> {
    bool operator()(const T& a, const T& b) const { return a.first[0] < b.first[0]; }
};
template <class T>
struct greater_first_x : public binary_function<T, T, bool> {
    bool operator()(const T& a, const T& b) const { return a.first[0] > b.first[0]; }
};
template <class T>
struct less_first_y : public binary_function<T, T, bool> {
    bool operator()(const T& a, const T& b) const { return a.first[1] < b.first[1]; }
};
template <class T>
struct greater_first_y : public binary_function<T, T, bool> {
    bool operator()(const T& a, const T& b) const { return a.first[1] > b.first[1]; }
};

template <class SRC, class DEST>
struct cast : public unary_function<SRC,DEST> {
    result_type operator()(const argument_type& arg) {
	    return DEST(arg);
    }
};


template <class SRC_CONTAINER, class DEST_CONTAINER, class PIXEL, class CRITERION>
int seedfill2d(int x, int y, const SRC_CONTAINER& src, DEST_CONTAINER& dest,const PIXEL& p, CRITERION criterion) {
    int num_filled = 0;
    if ((x >= 0) && (y>=0) && (x<src.xsize()) && (y<src.ysize())) {
        if (dest(x,y)!=p) {
            if (criterion(src(x,y))) {
                dest(x,y) = p;
                num_filled = 1;
                num_filled += seedfill2d(x-1,y,src,dest,p,criterion);
                num_filled += seedfill2d(x,y-1,src,dest,p,criterion);
                num_filled += seedfill2d(x+1,y,src,dest,p,criterion);
                num_filled += seedfill2d(x,y+1,src,dest,p,criterion);
            }
        }
    }
    return num_filled;
}

template <class InputIterator,class OutputIterator,class T>
OutputIterator lowpass(int radius,InputIterator begin, InputIterator end, OutputIterator out, const T& init) {
//    cerr << "lowpas(0):size = " << end-begin <<  endl;
     T value = init;
    InputIterator lead = begin;

//    InputIterator first_out = out;
    int lead_count = 0;
    while (lead!=end && lead_count<radius) {
       value = value + convert(*lead, init);
        ++lead;
        ++lead_count;
    }
 //   cerr << "lowpas(1):lead_count = " << lead_count << ",lead = " << lead - begin << endl;
    int count=lead_count;
    while (lead_count--) {
 //       cerr << "lowpas(I):out = " << out-first_out << " count = " << count << ",lead = " << lead - begin << endl;
        *out++ = convert(value/count,*out);
        if (lead!=end) {
            value = value + convert(*lead, init);
            ++lead;
            ++count;
        }
    }
 //   cerr << "lowpas(2):count = " << count << ",lead = " << lead - begin << endl;
    InputIterator trail = begin;
    if (count) --count;
    while (lead!=end) {
        value = value - convert(*trail++, init);
 //       cerr << "lowpas(X):out = " << out-first_out << " count = " << count << ",trail = " << trail - begin <<",lead = " << lead - begin << endl;
        *out++ = convert(value/count,*out);
        value = value + convert(*lead++, init);
    }
 //   cerr << "lowpas(3):count = " << count << ",trail = " << trail - begin <<",lead = " << lead - begin << endl;
    while (trail!=end && count>=radius) {
 //       cerr << "lowpas(T):out = " << out-first_out << " count = " << count-1 << ",trail = " << trail - begin <<",lead = " << lead - begin << endl;
        value = value - convert(*trail++, init);        
        *out++ = convert(value/count,*out);
        count--;
    }
    return out;
}

template <class InputIterator,class OutputIterator,class T>
OutputIterator avrg_pass(InputIterator begin, InputIterator end, OutputIterator out, const T& dummy) {

    if (begin != end) {
        T trail_value = *begin >> 1;
        InputIterator lead = begin+1;
        InputIterator lend = end;
        T lead_value;      
        while (lead!=lend) {
            lead_value = *lead >> 1;
            ++lead;
            *out = trail_value + lead_value;
            trail_value = lead_value;
            ++out;
        }
        *out++ = trail_value + lead_value;
    }
}

template <class IN_RASTER, class OUT_RASTER, class DUMMY_T>
void h_avrg_pass(const IN_RASTER& in, OUT_RASTER& out, const DUMMY_T& dt) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int v = 0; v < in.vsize(); v++) {
            avrg_pass(in.hfind(0,v),in.hfind(in.hsize(),v), out.hfind(0,v),dt);
        }
    } else {
        cerr << "h_avrg_pass(const matrix& in, matrix& out): bad out size" << endl;
    }
}
template <class IN_RASTER, class OUT_RASTER, class DUMMY_T>
void v_avrg_pass(const IN_RASTER& in, OUT_RASTER& out, const DUMMY_T& dt) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int h = 0; h < in.hsize(); h++) {
            avrg_pass(in.vfind(h,0),in.vfind(h,in.vsize()), out.vfind(h,0),dt);
        }
    } else {
        cerr << "v_avrg_pass(const matrix& in, matrix& out): bad out size" << endl;
    }
}

template <class IN_RASTER, class OUT_RASTER, class DUMMY_T>
void avrg_pass_2d(const IN_RASTER& in, OUT_RASTER& out, const DUMMY_T& dt) {
//    ptime lp1start;
    h_avrg_pass(in, out,dt);
//    ptime lp2start;
    v_avrg_pass(out, out,dt);
#if 0       
    double h_lp_time = ptime() - lp2start;
    double v_lp_time = lp2start - lp1start;
    int hsize = in.hsize();  
    int vsize = in.vsize();  
    cerr << "Time to h_avrg_pass_2d " <<  DMS(h_lp_time) << " " << hsize*vsize/h_lp_time/1024/1024 << " Mpix/s" << endl;          
    cerr << "Time to v_avrg_pass_2d " <<  DMS(v_lp_time) << " " << hsize*vsize/v_lp_time/1024/1024 << " Mpix/s" << endl;          
#endif
}
    

template <class InputIterator,class OutputIterator,class T>
OutputIterator gradient(int radius,InputIterator begin, InputIterator end, OutputIterator out, const T& init) {
    T value = init;
    InputIterator lead = begin;
    InputIterator trail = begin;
    int lead_count = 0;
    T lead_value = init;
    while (lead!=end && lead_count<radius) {
        lead_value = convert(*lead++, init);
        ++lead_count;
    }

//    InputIterator first_out = out;

    while (lead_count-- && lead!=end) {
//      cerr << "gradient(I):out = " << out-first_out << " lead_count = " << lead_count << ",trail = " << trail - begin <<",lead = " << lead - begin << endl;
      lead_value = convert(*lead++, init);
        *out++ = lead_value - convert(*trail, init);
    }
    while (lead!=end) {
//        cerr << "gradient(M):out = " << out-first_out << " lead_count = " << lead_count << ",trail = " << trail - begin <<",lead = " << lead - begin << endl;
         lead_value = convert(*lead++, init);
        *out++ =  lead_value - convert(*trail++, init);;
    }
    int trail_count = radius;
    while (trail!=end && trail_count--) {
//         cerr << "gradient(E):out = " << out-first_out << " lead_count = " << lead_count << ",trail = " << trail - begin <<",lead = " << lead-1 - begin << endl;
         *out++ = lead_value - convert(*trail++, init);;
    }
    return out;
}

template <class InputIterator,class OutputIterator,class T>
OutputIterator abs_gradient(int radius,InputIterator begin, InputIterator end, OutputIterator out, const T& init) {
    T value = init;
    InputIterator lead = begin;
    InputIterator trail = begin;
    int lead_count = 0;
    T lead_value = init;
    while (lead!=end && lead_count<radius) {
        lead_value = convert(*lead++, init);
        ++lead_count;
    }

    while (lead_count-- && lead!=end) {
        lead_value = convert(*lead++, init);
        *out++ = convert(pixel_abs(lead_value - convert(*trail, init)),*out);
    }
    while (lead!=end) {
         lead_value = convert(*lead++, init);
        *out++ =  convert(pixel_abs(lead_value - convert(*trail++, init)),*out);
    }
    int trail_count = radius;
    while (trail!=end && trail_count--) {
         *out++ = convert(pixel_abs(lead_value - convert(*trail++, init)),*out);
    }
    return out;
}

template <class SRC, class DEST>
inline RGB_t<DEST> sconvert(RGB_t<SRC> src, RGB_t<DEST> dest) {
    return RGB_t<DEST>((char)src[0], (char)src[1], (char)src[2]);
}

template <class InputIterator,class OutputIterator,class T>
OutputIterator special_abs_gradient(int radius,
                                    InputIterator begin, InputIterator end, OutputIterator out, const T& init) {
    T value = init;
    InputIterator lead = begin;
    InputIterator trail = begin;
    int lead_count = 0;
    T lead_value = init;
    while (lead!=end && lead_count<radius) {
        lead_value = sconvert(*lead++, init);
        ++lead_count;
    }

    while (lead_count-- && lead!=end) {
        lead_value = convert(*lead++, init);
        *out++ = sconvert(pixel_abs(lead_value - sconvert(*trail, init)),*out);
    }
    while (lead!=end) {
         lead_value = convert(*lead++, init);
        *out++ =  sconvert(pixel_abs(lead_value - sconvert(*trail++, init)),*out);
    }
    int trail_count = radius;
    while (trail!=end && trail_count--) {
         *out++ = sconvert(pixel_abs(lead_value - sconvert(*trail++, init)),*out);
    }
    return out;
}

template <class InputIterator,class T>
T psum(InputIterator begin, InputIterator end, const T& init) {
  T result = init;
  for ( ; begin != end; ++begin)
    result = result + convert(*begin,init);
  return result;
}

template <class InputIterator,class T>
int count_if_greater(InputIterator begin, InputIterator end, const T& compare_value) {
  int count = 0;
  for ( ; begin != end; ++begin) {
    if (convert(*begin,compare_value) > compare_value) {
        ++count;
    }
  }
  return count;
}

template <class InputIterator,class T>
InputIterator find_if_greater(InputIterator begin, InputIterator end, const T& compare_value) {
  int count = 0;
  for ( ; begin != end; ++begin) {
    if (convert(*begin,compare_value) > compare_value) {
        return begin;
    }
  }
  return begin;
}
template <class InputIterator,class T>
InputIterator find_if_smaller(InputIterator begin, InputIterator end, const T& compare_value) {
  int count = 0;
  for ( ; begin != end; ++begin) {
    if (convert(*begin,compare_value) < compare_value) {
        return begin;
    }
  }
  return begin;
}

template <class InputIterator, class OutputIterator, class T>
void expand(int radius, InputIterator begin, InputIterator end, OutputIterator out, const T& region_value) {
    int out_count = 0;
    bool in_region = false;
    InputIterator first = begin;
    
    if (begin != end) 
        in_region = (*begin == region_value);
    else
        return;

    for ( ; begin != end; ++begin) {
        in_region = (convert(*begin,region_value) == region_value);
        if (in_region) out_count=radius+1;
        if (out_count) {
            *out = region_value;
            --out_count;
        } else {
            *out = *begin;
        }
        ++out;
    }
    // jetzt rueckwaerts 
    do {
        --begin;
        --out;
        in_region = (convert(*begin,region_value) == region_value);
        if (in_region) out_count=radius+1;
        if (out_count) {
            *out = region_value;
            --out_count;
        } else {
            *out = *begin;
        }
    } while(begin != first); 
}

template <class InputIterator, class OutputIterator, class T>
void erode(int radius, InputIterator begin, InputIterator end, OutputIterator out, const T& region_value,const T& eroded_value) {
    int out_count = 0;
    bool in_region = false;
    InputIterator first = begin;
    
    if (begin != end) 
        in_region = (*begin == region_value);
    else
        return;
    if (in_region) out_count=radius;

    for ( ; begin != end; ++begin) {
        if ((convert(*begin,region_value) == region_value) != in_region) {
            in_region = (convert(*begin,region_value) == region_value);
            if (in_region) out_count=radius;
        }
        if (out_count) {
            *out = eroded_value;
            --out_count;
        } else {
            *out = *begin;
        }
        ++out;
    }
    // jetzt rueckwaerts 
    do {
        --begin;
        --out;
        if ((convert(*begin,region_value) == region_value) != in_region) {
            in_region = (convert(*begin,region_value) == region_value);
            if (in_region) out_count=radius;
        }
        if (out_count) {
            *out = eroded_value;
            --out_count;
        } else {
            *out = *begin;
        }
    } while(begin != first);
}

//template <class PIXEL>
//void h_expand(int radius, const const_submat<PIXEL>& in, const submat<PIXEL>& out, const PIXEL& region_value) {
template <class IN_RASTER, class OUT_RASTER, class PIXEL>
void h_expand(int radius, const IN_RASTER& in, OUT_RASTER& out, const PIXEL& region_value) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int v = 0; v < in.vsize(); v++) {
            expand(radius, in.hfind(0,v),in.hfind(in.hsize(),v), out.hfind(0,v),region_value);
        }
    } else {
        cerr << "h_expand(const matrix& in, matrix& out): bad out size" << endl;
    }
}

//template <class PIXEL>
//void v_expand(int radius, const const_submat<PIXEL>& in, const submat<PIXEL>& out, const PIXEL& region_value) {
template <class IN_RASTER, class OUT_RASTER, class PIXEL>
void v_expand(int radius, const IN_RASTER& in, OUT_RASTER& out, const PIXEL& region_value) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int h = 0; h < in.hsize(); h++) {
            expand(radius, in.vfind(h,0),in.vfind(h,in.vsize()), out.vfind(h,0),region_value);
        }
    } else {
        cerr << "v_expand(const matrix& in, matrix& out): bad out size" << endl;
    }
}

//template <class PIXEL>
//void expand2d(int radius, const const_submat<PIXEL>& in, const submat<PIXEL>& out, const PIXEL& region_value) {
template <class IN_RASTER, class OUT_RASTER, class PIXEL>
void expand2d(int radius, const IN_RASTER& in, const OUT_RASTER& out, const PIXEL& region_value) {
    h_expand(radius, in, submat<PIXEL>(out),region_value);
    v_expand(radius, const_submat<PIXEL>(out), out,region_value);
}

//template <class PIXEL>
//void h_erode(int radius, const const_submat<PIXEL>& in, const submat<PIXEL>& out, const PIXEL& region_value, const PIXEL& eroded_value) {
template <class IN_RASTER, class OUT_RASTER, class PIXEL>
void h_erode(int radius, const IN_RASTER& in, OUT_RASTER& out, const PIXEL& region_value, const PIXEL& eroded_value) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int v = 0; v < in.vsize(); v++) {
            erode(radius, in.hfind(0,v),in.hfind(in.hsize(),v), out.hfind(0,v),region_value,eroded_value);
        }
    } else {
        cerr << "h_erode(const matrix& in, matrix& out): bad out size" << endl;
    }
}
//template <class PIXEL>
//void v_erode(int radius, const const_submat<PIXEL>& in, const submat<PIXEL>& out, const PIXEL& region_value, const PIXEL& eroded_value) {
template <class IN_RASTER, class OUT_RASTER, class PIXEL>
void v_erode(int radius, const IN_RASTER& in, OUT_RASTER& out, const PIXEL& region_value, const PIXEL& eroded_value) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int h = 0; h < in.hsize(); h++) {
            erode(radius, in.vfind(h,0),in.vfind(h,in.vsize()), out.vfind(h,0),region_value,eroded_value);
        }
    } else {
        cerr << "v_erode(const matrix& in, matrix& out): bad out size" << endl;
    }
}

//template <class PIXEL>
//void erode2d(int radius, 
//                const const_submat<PIXEL>& in,
//                const submat<PIXEL>& out, 
//                const PIXEL& region_value,const PIXEL& eroded_value) {
template <class IN_RASTER, class OUT_RASTER, class PIXEL>
void erode2d(int radius, 
                const IN_RASTER& in,
                OUT_RASTER& out, 
                const PIXEL& region_value,const PIXEL& eroded_value) {
//    h_erode(radius, in, submat<PIXEL>(out),region_value,eroded_value);
//    v_erode(radius, const_submat<PIXEL>(out), out,region_value,eroded_value);
    h_erode(radius, in, out,region_value,eroded_value);
    v_erode(radius, out, out,region_value,eroded_value);
}

template <class InputIterator,class T>
int count_regions(InputIterator begin, InputIterator end, const T& region_value) {
  int count = 0;
  bool in_region = false;
  if (begin != end) in_region = (*begin == region_value);
  for ( ; begin != end; ++begin) {
    if ((convert(*begin,region_value) == region_value) != in_region) {
        in_region = (convert(*begin,region_value) == region_value);
         if (!in_region) ++count;
    }
  }
  if (in_region) ++count;
  return count;
}

template <class InputIterator,class OutputIterator,class T>
int find_regions(InputIterator begin, InputIterator end, OutputIterator start_of_region, OutputIterator end_of_region, const T& region_value) {
  int count = 0;
  bool in_region = false;
  if (begin != end) in_region = (*begin == region_value);
  if (in_region) {
      *start_of_region++ = begin;
  }
  for ( ; begin != end; ++begin) {
    if ((convert(*begin,region_value) == region_value) != in_region) {
        in_region = (convert(*begin,region_value) == region_value);
        if (in_region) {
            *start_of_region++ = begin;
        } else {
            *end_of_region++ = begin;
            ++count;
        }
    }
  }
  if (in_region) {
    *end_of_region++ = end;
    ++count;
  }
  return count;
}

template <class InputIterator,class T>
int count_inner_regions(InputIterator begin, InputIterator end, const T& region_value) {
  int count = 0;
  while ((begin != end) && (convert(*begin,region_value) == region_value)) {
    begin++;
  }
  bool in_region = false;

  for ( ; begin != end; ++begin) {
    if ((convert(*begin,region_value) == region_value) != in_region) {
        in_region = (convert(*begin,region_value) == region_value);
        if (!in_region) ++count;
    }
  }
//  if (count && in_region) count--;
  return count;
}

template <class InputIterator,class OutputIterator,class T>
int find_inner_regions(InputIterator begin, InputIterator end, OutputIterator start_of_region, OutputIterator end_of_region,const T& region_value) {
  int count = 0;
  while ((begin != end) && (convert(*begin,region_value) == region_value)) {
    begin++;
  }
  bool in_region = false;

  for ( ; begin != end; ++begin) {
    if ((convert(*begin,region_value) == region_value) != in_region) {
        in_region = (convert(*begin,region_value) == region_value);
        if (in_region) {
            *start_of_region++ = begin;
        } else {
            *end_of_region++ = begin;
          ++count;
        }
    }
  }
//  if (count && in_region) count--;
  return count;
}


template <class InputIterator,class T>
InputIterator min_element(InputIterator begin, InputIterator end, const T& init) {
  InputIterator result = end;
  T min_val = init;
  for ( ; begin != end; ++begin) {
    if (convert(*begin,init) < min_val) {
        min_val = convert(*begin,init);
        result = begin;
    }
  }
  return result;
}
template <class InputIterator,class T>
InputIterator max_element(InputIterator begin, InputIterator end, const T& init) {
  InputIterator result = end;
  T max_val = init;
  for ( ; begin != end; ++begin) {
    if (convert(*begin,init) > max_val) {
        max_val = convert(*begin,init);
        result = begin;
    }
  }
  return result;
}
template <class InputIterator,class T>
T minimum(InputIterator begin, InputIterator end, const T& init) {
  T result = init;
  for ( ; begin != end; ++begin) {
    if (convert(*begin,init) < result) {
        result = convert(*begin,init);
    }
  }
  return result;
}
template <class InputIterator,class T>
T maximum(InputIterator begin, InputIterator end, const T& init) {
   T result = init;
   for ( ; begin != end; ++begin) {
    if (convert(*begin,init) > result) {
        result = convert(*begin,init);
    }
  }
 return result;
}

template <class InputIterator,class T>
T chan_minimum(InputIterator begin, InputIterator end, const T& init) {
    T result = init;
    for ( ; begin != end; ++begin) {
        result = pixel_min(convert(*begin,init),result);
    }
    return result;
}

template <class InputIterator,class T>
T chan_maximum(InputIterator begin, InputIterator end, const T& init) {
    T result = init;
    for ( ; begin != end; ++begin) {
        result = pixel_max(convert(*begin,init),result);
    }
    return result;
}


template <class InputIterator,class T>
T pcenter(InputIterator begin, InputIterator end, const T& init) {
  int count = 0;
  T result = init;
  T sum = init;
  for ( ; begin != end; ++begin) {
    T val = convert(*begin,init);
    sum += val;
    result += val * count++;
  }
  if (sum)
    return result/sum;
  else
    return 0;
}

template <class InputIterator,class OutputIterator,class T>
OutputIterator half_size(InputIterator begin, InputIterator end, OutputIterator out, const T& init) {
    InputIterator lead = begin;
    InputIterator trail = begin;
    if (lead != end) ++lead;
    while (lead!=end) {
        *out++ = (*lead++>>1) + (*trail++>>1);
        if (lead!=end) {
            ++lead;
            ++trail;
        }
    }
    return out;
}
template <class InputIterator,class OutputIterator,class T>
OutputIterator half_size(InputIterator begin1, InputIterator end1,
                         InputIterator begin2,
                         OutputIterator out, const T& init) {
    InputIterator lead1 = begin1;
    InputIterator trail1 = begin1;
    InputIterator lead2 = begin2;
    InputIterator trail2 = begin2;
    if (lead1 != end1) {
        ++lead1;
        ++lead2;
    }
    while (lead1!=end1) {
        *out++ = (*lead1++>>2) + (*trail1++>>2) + (*lead2++>>2) + (*trail2++>>2);
        if (lead1!=end1) {
            ++lead1;
            ++trail1;
            ++lead2;
            ++trail2;
        }
    }
    return out;
}

//template <class PIXEL>
//void half_size(const const_submat<PIXEL>& in, const submat<PIXEL>& out) {
template <class IN_RASTER, class OUT_RASTER>
void half_size(const IN_RASTER& in, OUT_RASTER& out) {
    if ((out.hsize()==in.hsize()/2) && (out.vsize()==in.vsize()/2)) {
        for (int v = 0; v*2+1 < in.vsize(); v++) {
            half_size(in.hfind(0,v*2),in.hfind(in.hsize(),v*2),
                      in.hfind(0,v*2+1),
                      out.hfind(0,v),typename OUT_RASTER::value_type());
        }
    } else {
        cerr << "half_size(const matrix& in, matrix& out): bad out size" << endl;
    }
}
template <class InputIterator,class OutputIterator,class T>
OutputIterator double_size(InputIterator begin, InputIterator end, OutputIterator out, const T& init) {
    InputIterator in = begin;
    while (in!=end) {
        *out++ = *in;
        *out++ = *in++;
    }
    return out;
}

template <class InputIterator,class OutputIterator,class T>
OutputIterator replace_if_smaller(InputIterator begin, InputIterator end, OutputIterator out, const T& cut_value, const T& replace_value) {
    InputIterator in = begin;
    while (in!=end) {
        if (*in<cut_value)
            *out++ = replace_value;
        else
            *out++ = *in;
        ++in;
    }
    return out;
}
template <class InputIterator,class OutputIterator,class T>
OutputIterator replace_if_not_equal(InputIterator begin, InputIterator end, OutputIterator out, const T& value, const T& replace_value) {
    InputIterator in = begin;
    while (in!=end) {
        if (*in!=value)
            *out++ = replace_value;
        else
            *out++ = *in;
        ++in;
    }
    return out;
}
template <class InputIterator,class OutputIterator,class T>
OutputIterator replace_if_equal(InputIterator begin, InputIterator end, OutputIterator out, const T& value, const T& replace_value) {
    InputIterator in = begin;
    while (in!=end) {
        if (*in==value)
            *out++ = replace_value;
        else
            *out++ = *in;
        ++in;
    }
    return out;
}
template <class InputIterator,class MaskInputIterator,class OutputIterator,class Tin, class Tout>
OutputIterator mask(InputIterator begin, InputIterator end, MaskInputIterator begin_mask,
                            OutputIterator out,
                            const Tin& use_input_if_value, const Tout& zero_value) {
    InputIterator in = begin;
    MaskInputIterator in_mask = begin_mask;
    while (in!=end) {
        if (*in_mask == use_input_if_value)
            *out++ = *in;
        else
            *out++ = zero_value;
        ++in;
        ++in_mask;
    }
    return out;
}

template <class InputIterator,class OutputIterator,class Tin, class Tout>
OutputIterator discriminate(InputIterator begin, InputIterator end, OutputIterator out, const Tin& cut_value, const Tout& below_value, const Tout& above_value) {
    InputIterator in = begin;
    while (in!=end) {
        if (cut_value > *in)
            *out++ = below_value;
        else
            *out++ = above_value;
        ++in;
    }
    return out;
}
template <class RASTER,class INPIXEL, class OUTPIXEL>
matrix<OUTPIXEL> discriminated_mat(const RASTER& m,
                                const INPIXEL& cut_value,
                                const OUTPIXEL& below_eq_value, 
                                const OUTPIXEL& above_value) 
{
    matrix<OUTPIXEL> result(m.hsize(),m.vsize());
    discriminate(m.begin(),m.end(),result.begin(),cut_value,below_eq_value,above_value);
    return result;
}

template <class InputIterator,class OutputIterator,class Tin, class Tout>
OutputIterator chan_discriminate(InputIterator begin, InputIterator end, OutputIterator out, const Tin& cut_value, const Tout& below_value, const Tout& above_value) {
    InputIterator in = begin;
    if (cut_value.size()!=below_value.size()) {
        cerr << "chan_discriminate: channel numbers dont match"<< endl;
        return out;
    }
    while (in!=end) {
        for (int c = 0;c<cut_value.size();c++) {
            if ((*in)[c]<cut_value[c])
                (*out)[c] = below_value[c];
            else
                (*out)[c] = above_value[c];
        }
        ++out;
        ++in;
    }
    return out;
}

template <class RASTER,class INPIXEL, class OUTPIXEL>
matrix<OUTPIXEL> chan_discriminated_mat(const RASTER& m,
                                const INPIXEL& cut_value,
                                const OUTPIXEL& below_value, 
                                const OUTPIXEL& above_value) 
{
    matrix<OUTPIXEL> result(m.hsize(),m.vsize());
    chan_discriminate(m.begin(),m.end(),result.begin(),cut_value,below_value,above_value);
    return result;
}


template <class InputIterator,class OutputIterator,class T>
OutputIterator pmult(InputIterator begin, InputIterator end, InputIterator begin2, OutputIterator out, const T& scale_div) {
    while (begin!=end) {
         *out++ = (convert(*begin++,scale_div) * convert(*begin2++,scale_div)) / scale_div ;
    }
    return out;
}
template <class InputIterator,class OutputIterator,class T>
OutputIterator pdiv(InputIterator begin, InputIterator end, InputIterator begin2, OutputIterator out, const T& scale_fac) {
    while (begin!=end) {
         *out++ = (convert(*begin++,scale_fac) / convert(*begin2++,scale_fac)) * scale_fac ;
    }
    return out;
}
template <class InputIterator,class OutputIterator,class T>
OutputIterator psubtract(InputIterator begin, InputIterator end, InputIterator begin2, OutputIterator out, const T& offset) {
    while (begin!=end) {
         *out++ = (convert(*begin++,offset) - convert(*begin2++,offset)) + offset ;
    }
    return out;
}
template <class InputIterator,class OutputIterator,class T>
OutputIterator padd(InputIterator begin, InputIterator end, InputIterator begin2, OutputIterator out, const T& offset) {
    while (begin!=end) {
         *out++ = (convert(*begin++,offset) + convert(*begin2++,offset)) + offset;
    }
    return out;
}
template <class InputIterator,class OutputIterator,class T>
OutputIterator paverage(InputIterator begin, InputIterator end, InputIterator begin2, OutputIterator out, const T& divisor) {
    while (begin!=end) {
         *out++ = convert(((convert(*begin++,divisor) + convert(*begin2++,divisor))) / divisor,*out);
    }
    return out;
}
template <class InputIterator,class OutputIterator,class T>
OutputIterator abs_diff(InputIterator begin, InputIterator end, InputIterator begin2, OutputIterator out, const T& arith_type) {
    while (begin!=end) {
         *out++ = convert((pixel_abs(convert(*begin++,arith_type) - convert(*begin2++,arith_type))),*out);
    }
    return out;
}
template <class InputIterator,class OutputIterator,class T>
OutputIterator squared_diff(InputIterator begin, InputIterator end, InputIterator begin2, OutputIterator out, const T& scale_down) {

//cerr << "Start squared_diff, sizeof T =" << sizeof(T) << endl;
//cerr << DBG(scale_down[0]) << DBG(scale_down[1]) << DBG(scale_down[2]) << DBG(scale_down) << endl;
    while (begin!=end) {
         T delta = convert(*begin,scale_down) - convert(*begin2,scale_down);
         T result = delta * delta / scale_down;
         //cerr << DBG(*begin) << DBG(*begin2) << DBG(delta) << DBG(result) << DBG(scale_down) << endl;
         ++begin;
         ++begin2;
         *out++ = convert(result,*out);
    }
    return out;
}

template <class InputIterator,class OutputIterator,class IN_VALUE, class OUT_VALUE>
OutputIterator normalize(InputIterator begin, InputIterator end, OutputIterator out, 
                            const IN_VALUE& max_in, const OUT_VALUE& max_out) {
    if (max_in != 0) {
        while (begin!=end) {
             *out++ = *begin++ * max_out / max_in;
        }
    } else {
        while (begin!=end) {
             *out++ = *begin++;
        }
    }
    return out;
}
template <class RASTER,class OUT_VALUE>
matrix<OUT_VALUE> normalized(const RASTER& m, const OUT_VALUE& max_out)
{
    matrix<OUT_VALUE> result(m.hsize(),m.vsize());
    typename RASTER::value_type max_in = maximum(m.begin(),m.end(),typename RASTER::value_type(0));
    normalize(m.begin(),m.end(),result.begin(),max_in,max_out);
    return result;
}

template <class InputIterator,class OutputIterator,class IN_VALUE, class OUT_VALUE,class ARITH_TYPE>
OutputIterator normalize(InputIterator begin, InputIterator end, OutputIterator out, 
                            const IN_VALUE& max_in, const OUT_VALUE& max_out,
                            const ARITH_TYPE &arith) {
    
    if (all_non_zero(max_in)) {
        while (begin!=end) {
            *out++ = convert(convert(*begin++,arith) * convert(max_out,arith) / convert(max_in,arith),*out);
        }
    } else {
        while (begin!=end) {
            *out++ = convert(*begin++,*out);
        }
    }
    return out;
}
template <class RASTER,class OUT_VALUE,class ARITH_TYPE>
matrix<OUT_VALUE> normalized(const RASTER& m, const OUT_VALUE& max_out,const ARITH_TYPE &arith)
{
    matrix<OUT_VALUE> result(m.hsize(),m.vsize());
    typename RASTER::value_type max_in = maximum(m.begin(),m.end(),typename RASTER::value_type(0));
    
    normalize(m.begin(),m.end(),result.begin(),max_in,max_out,arith);
    return result;
}

template <class InputIterator,class OutputIterator,class IN_VALUE, class OUT_VALUE,class ARITH_TYPE>
OutputIterator chan_normalize(InputIterator begin, InputIterator end, OutputIterator out, 
                            const IN_VALUE& max_in, const OUT_VALUE& max_out,
                            const ARITH_TYPE &arith) {
    
   while (begin!=end) {
         *out++ = convert(convert(*begin++,arith) * convert(max_out,arith) / convert(max_in,arith),*out);
    }
    return out;
}
template <class RASTER,class OUT_VALUE,class ARITH_TYPE>
matrix<OUT_VALUE> chan_normalized(const RASTER& m, const OUT_VALUE& max_out,const ARITH_TYPE &arith)
{
    matrix<OUT_VALUE> result(m.hsize(),m.vsize());
    typename RASTER::value_type max_in = chan_maximum(m.begin(),m.end(),typename RASTER::value_type(0));
    
    cerr << DBG(max_in) <<endl;
    chan_normalize(m.begin(),m.end(),result.begin(),max_in,max_out,arith);
    return result;
}


template <class InputIterator,class OutputIterator,class T>
OutputIterator run_sum(InputIterator begin, InputIterator end, OutputIterator out, const T& init) {
    
    T rs = convert(*begin++,init);
    if (begin!=end)
        *out++ = rs;
    while (begin!=end) {
         rs = rs + convert(*begin++,rs);
         *out++ = rs;
    }
    return out;
}


template <class IN_RASTER, class OUT_RASTER, class SUM_T>
void h_run_sum(const IN_RASTER& in, OUT_RASTER& out, const SUM_T& init) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int v = 0; v < in.vsize(); v++) {
            run_sum(in.hfind(0,v),in.hfind(in.hsize(),v), out.hfind(0,v),init);
        }
    } else {
        cerr << "h_run_sum(const matrix& in, matrix& out): bad out size" << endl;
    }
}

template <class IN_RASTER, class OUT_RASTER, class SUM_T>
void v_run_sum(const IN_RASTER& in, OUT_RASTER& out, const SUM_T& init) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int h = 0; h < in.hsize(); h++) {
            run_sum(in.vfind(h,0),in.vfind(h,in.vsize()), out.vfind(h,0),init);
        }
    } else {
        cerr << "v_run_sum(const matrix& in, matrix& out): bad out size" << endl;
    }
}

template <class IN_RASTER, class OUT_RASTER, class SUM_T>
 void run_sum(const IN_RASTER& in, OUT_RASTER& out, const SUM_T& init) {
   h_run_sum(in, out ,init);
   v_run_sum(out, out ,init);
}

template <class IN_RASTER, class SUM_T>
SUM_T get_run_rect_sum(const IN_RASTER& run_sums, int h, int v, int hsize, int vsize,const SUM_T& init) {
    if ((hsize<=0) || (vsize<=0)) 
        return init;
    h = h - 1;
    v = v - 1;
    if ((h >= 0) && (v>=0)) {
        SUM_T topleft = run_sums(h,v);
        SUM_T left = run_sums(h,v+vsize);
        SUM_T top = run_sums(h+hsize,v);
        SUM_T all = run_sums(h+hsize,v+vsize);
        SUM_T result = all + topleft - left - top;
        return result;
    }
    if (h >= 0) {
        SUM_T topleft = init;
        SUM_T left = run_sums(h,v+vsize);
        SUM_T top = init;
        SUM_T all = run_sums(h+hsize,v+vsize);
        SUM_T result = all + topleft - left - top;
//        cerr << "(v  < 0):" <<DBG(h)<<DBG(v)<< DBG(result) << "="<< DBG(all)<< "+"<< DBG(topleft)<< "-"<< DBG(left)<< "-"<< DBG(top) << endl;
        return result;
    }
    if (v >= 0) {
        SUM_T topleft = init;
        SUM_T left = init;
        SUM_T top = run_sums(h+hsize,v);
        SUM_T all = run_sums(h+hsize,v+vsize);
        SUM_T result = all + topleft - left - top;
//        cerr << "(h  < 0):" <<DBG(h)<<DBG(v)<< DBG(result) << "="<< DBG(all)<< "+"<< DBG(topleft)<< "-"<< DBG(left)<< "-"<< DBG(top) << endl;
        return result;
    }
    // h,v < 0
    {
        SUM_T topleft = init;
        SUM_T left = init;
        SUM_T top = init;
        SUM_T all = run_sums(h+hsize,v+vsize);
        SUM_T result = all + topleft - left - top;
//        cerr << "(hv < 0):" <<DBG(h)<<DBG(v)<< DBG(result) << "="<< DBG(all)<< "+"<< DBG(topleft)<< "-"<< DBG(left)<< "-"<< DBG(top) << endl;
        return result;
    }
}

template <class IN_RASTER, class OUT_RASTER, class SUM_T>
void h_abs_gradient(int radius, const IN_RASTER& in, OUT_RASTER& out, const SUM_T& sumt) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int v = 0; v < in.vsize(); v++) {
            abs_gradient(radius, in.hfind(0,v),in.hfind(in.hsize(),v), out.hfind(0,v),sumt);
        }
    } else {
        cerr << "h_abs_gradient(const matrix& in, matrix& out): bad out size" << endl;
    }
}
template <class IN_RASTER, class OUT_RASTER, class SUM_T>
void v_abs_gradient(int radius, const IN_RASTER& in, OUT_RASTER& out, const SUM_T& sumt) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int h = 0; h < in.hsize(); h++) {
            abs_gradient(radius, in.vfind(h,0),in.vfind(h,in.vsize()), out.vfind(h,0),sumt);
        }
    } else {
        cerr << "v_abs_gradient(const matrix& in, matrix& out): bad out size" << endl;
    }
}

//template <class PIXEL, class DIV_T>
//void abs_gradient(int radius, const const_submat<PIXEL>& in, const submat<PIXEL>& out, const DIV_T& divt) {
template <class IN_RASTER, class OUT_RASTER, class DIV_T>
void abs_gradient(int radius, const IN_RASTER& in, OUT_RASTER& out, const DIV_T& divt) {
    matrix<typename IN_RASTER::value_type> h_tmp(in.hsize(),in.vsize());
    matrix<typename IN_RASTER::value_type> v_tmp(in.hsize(),in.vsize());
    
    h_abs_gradient(radius, in, h_tmp,divt);
    v_abs_gradient(radius, in, v_tmp,divt);
    
    paverage(h_tmp.begin(), h_tmp.end(), v_tmp.begin(), out.begin(), DIV_T(2));
}
template <class IN_RASTER, class OUT_RASTER, class SUM_T>
void h_gradient(int radius, const IN_RASTER& in, OUT_RASTER& out, const SUM_T& sumt) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int v = 0; v < in.vsize(); v++) {
            gradient(radius, in.hfind(0,v),in.hfind(in.hsize(),v), out.hfind(0,v),sumt);
        }
    } else {
        cerr << "h_gradient(const matrix& in, matrix& out): bad out size" << endl;
    }
}
template <class IN_RASTER, class OUT_RASTER, class SUM_T>
void v_gradient(int radius, const IN_RASTER& in, OUT_RASTER& out, const SUM_T& sumt) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int h = 0; h < in.hsize(); h++) {
            gradient(radius, in.vfind(h,0),in.vfind(h,in.vsize()), out.vfind(h,0),sumt);
        }
    } else {
        cerr << "v_gradient(const matrix& in, matrix& out): bad out size" << endl;
    }
}

//template <class PIXEL, class DIV_T>
//void gradient(int radius, const const_submat<PIXEL>& in, const submat<PIXEL>& out, const DIV_T& divt) {
template <class IN_RASTER, class OUT_RASTER, class DIV_T>
void gradient(int radius, const IN_RASTER& in, OUT_RASTER& out, const DIV_T& divt) {
    matrix<typename IN_RASTER::value_type> h_tmp(in.hsize(),in.vsize());
    matrix<typename IN_RASTER::value_type> v_tmp(in.hsize(),in.vsize());
    
    h_gradient(radius, in, h_tmp,divt);
    v_gradient(radius, in, v_tmp,divt);
    
    paverage(h_tmp.begin(), h_tmp.end(), v_tmp.begin(), out.begin(), DIV_T(2));
}

template <class IN_RASTER, class OUT_RASTER, class SUM_T>
void h_lowpass(int radius, const IN_RASTER& in, OUT_RASTER& out, const SUM_T& sumt) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int v = 0; v < in.vsize(); v++) {
            lowpass(radius, in.hfind(0,v),in.hfind(in.hsize(),v), out.hfind(0,v),sumt);
        }
    } else {
        cerr << "h_lowpass(const matrix& in, matrix& out): bad out size" << endl;
    }
}
template <class IN_RASTER, class OUT_RASTER, class SUM_T>
void v_lowpass(int radius, const IN_RASTER& in, OUT_RASTER& out, const SUM_T& sumt) {
    if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
        for (int h = 0; h < in.hsize(); h++) {
            lowpass(radius, in.vfind(h,0),in.vfind(h,in.vsize()), out.vfind(h,0),sumt);
        }
    } else {
        cerr << "v_lowpass(const matrix& in, matrix& out): bad out size" << endl;
    }
}

template <class IN_RASTER, class OUT_RASTER, class DIV_T>
void lowpass2d(int radius, const IN_RASTER& in, OUT_RASTER& out, const DIV_T& divt) {
    matrix<typename IN_RASTER::value_type> tmp(in.hsize(),in.vsize());
    h_lowpass(radius, in, tmp,divt);
    v_lowpass(radius, tmp, out,divt);
}
template <class IN_RASTER, class OUT_RASTER, class DIV_T>
void lowpass2d(int h_radius, int v_radius, const IN_RASTER& in, OUT_RASTER& out, const DIV_T& divt) {
    matrix<typename IN_RASTER::value_type> tmp(in.hsize(),in.vsize());
    h_lowpass(h_radius, in, tmp,divt);
    v_lowpass(v_radius, tmp, out,divt);
}

template <class number>
number ipow(number a, int x) {
    if (x==0) return 1;
    number result = a;
    while (--x) result*=a;
    return result;
}

template <class RESULT, class MATRIX>
RESULT moment(int p, int q, const MATRIX& image, RESULT init) {
    int x, y;
    RESULT result = init;
    for (int y = 0; y < image.vsize(); y++) {
        int yp = ipow(y,q);
        for (int x = 0; x < image.hsize(); x++) {
            int xp = ipow(x,p);
            result += (RESULT)xp * (RESULT)yp * (RESULT)image(x,y);
//           cerr << DBG(x) << DBG(y) << DBG(xp) << DBG(yp) << DBG(image(x,y)) << DBG(result) << endl;
        }
            
    }     
    return result;
}


template <class InputIterator, class VALUE>
vector<int> histogram(InputIterator begin,InputIterator end, int num_buckets, VALUE min, VALUE max) {
    vector<int> histo(num_buckets,0);
    VALUE range = max - min;
    while (begin!=end) {
        if ((*begin >= min) && (*begin <= max))
            histo[(*begin-min)*num_buckets/range]++;
        begin++;
    }
    return histo;
}
template <class InputIterator,class InputIterator2, class Delta_value>
Delta_value delta(InputIterator begin1, InputIterator end1, InputIterator2 begin2, Delta_value init) {
    Delta_value result = init;
    while (begin1!=end1) {
        result = result + pixel_abs(convert(*begin1++,init) - convert(*begin2++,init));
        
    }
    return result;
}

template <class RASTER1,class RASTER2, class CoordIterator, class Delta_value>
Delta_value delta(const RASTER1& r1, const RASTER2& r2,
                  CoordIterator begin,CoordIterator end, Delta_value init) {
    Delta_value result = init;
    while (begin!=end) {
        int h = (*begin)[0];
        int v = (*begin)[1];
        result = result + pixel_abs(convert(r1(h,v),init) - convert(r2(h,v),init));
        ++begin;
    }
    return result;
}
#if 0
template <class InputIterator,class InputIterator2>
float correlation(InputIterator begin1, InputIterator end1, InputIterator2 begin2) {
    float ab = 0;
    float aa = 0;
    float bb = 0;
    while (begin1!=end1) {
        float a = *begin1++;
        float b = *begin2++;
        ab += a*b;
        aa += a*a;
        bb += b*b; 
    }
    float corr = ab*ab / sqrt(aa * aa * bb * bb);
    return corr;
}
template <class RASTER1,class RASTER2, class CoordIterator>
float correlation(const RASTER1& r1, const RASTER2& r2,
                  CoordIterator begin,CoordIterator end) {
    float ab = 0;
    float aa = 0;
    float bb = 0;
    while (begin!=end) {
        int h = (*begin)[0];
        int v = (*begin)[1];
        float a = r1(h,v);
        float b = r2(h,v);
        ab += a*b;
        aa += a*a;
        bb += b*b; 
        ++begin;
    }
    float corr = ab*ab / sqrt(aa * aa * bb * bb);
    return corr;
}
#endif


template <class SCORE, class ITEM, class COMPARE = greater_first<pair<SCORE,ITEM> > >
struct hit_list {
    hit_list() : max_size(1), list(0) { }
    hit_list(int in_max_size) : max_size(in_max_size), list(0) { }
    void push(SCORE score, const ITEM& item) {
       if (list.size() < max_size || COMPARE()(make_pair(score,item), list.back())) {
            list.push_back(make_pair(score,item));
            sort(list.begin(),list.end(), COMPARE());
            if (list.size() > max_size) {
                list.pop_back();
            }
       }
   }
   hit_list(const hit_list& hl) {
        max_size = hl.get_max_size();
        list = hl.get_list();
   }
   int size() const {
        return list.size(); 
   }
   const pair<SCORE,ITEM>& operator[](int i) {
        return list[i];
   }
   const SCORE& score(int i) const {
        return list[i].first;
   }
   const ITEM& item(int i) const {
        return list[i].second;
   }
   void clear() {
        erase(list.begin(),list.end());
   }
   int get_max_size() const {
        return max_size;
   }
   const vector<pair<SCORE,ITEM> >& get_list() const {
        return list;
   }
protected:    
   int max_size;
   vector<pair<SCORE,ITEM> > list;
};

template <class RASTER>
vector<vec2<int> > get_extreme_coords(const RASTER& r, int num_best = 16) {
    
    hit_list<int,vec2<int>,less_first<pair<int,vec2<int> > > > small_n(num_best);
    hit_list<int,vec2<int>,greater_first<pair<int,vec2<int> > > > large_n(num_best);

    for (int v = 0; v < r.vsize();v++) {
        for (int h = 0; h < r.hsize();h++) {
            small_n.push(r(h,v),vec2<int>(h,v));    
            large_n.push(r(h,v),vec2<int>(h,v));    
        }
    }
    
    vector<vec2<int> > result(small_n.size()+large_n.size());
    for (int i=0;i<small_n.size();i++) {
        result[i] = small_n.item(i);
    }
    for (int i=0;i<large_n.size();i++) {
        result[i+small_n.size()] = large_n.item(i);
    }
    return result;
}

template <class RASTER>
int feature_richness(const RASTER& r) {
    matrix<typename RASTER::value_type> grad(r.hsize(),r.vsize());
//    debug_mat_global(r);
    abs_gradient(1,r,grad,int(0));
//    debug_mat_global(normalized(grad,(unsigned char)(255)));
    return psum(grad.begin(),grad.end(),int(0));
}
template <class RASTER>
int vertical_feature_richness(const RASTER& r) {
    matrix<typename RASTER::value_type> grad(r.hsize(),r.vsize());
//    debug_mat_global(r);
    v_abs_gradient(1,r,grad,int(0));
//    debug_mat_global(normalized(grad,(unsigned char)(255)));
    return psum(grad.begin(),grad.end(),int(0));
}

template <class RASTER>
int grad_feature_richness(const RASTER& r) {
    return psum(r.begin(),r.end(),int(0));
}


//
// match_matrix_sub_pixel - find inner_mat inside outer_mat with best match; position returned in h_best and v_best
//    if  htol and vtol are zero, the whole outer_mat is searched
//    if  htol or vtol are nonzero, h_best and v_best are considered as start positions,
//        and the search for inner_mat is performed within a region from {best-tol} to {best+tol}
//        including both
//        if the hints are invalid, the whole region is searched  


template <class outer_mat, class inner_mat>
bool match_matrix_sub_pixel(const outer_mat& outer,
                           const inner_mat& inner,
                           const vec2<int>& inner_pos,
                         const vec2<float>& inner_center, // relative to inner_pos
                           const vec2<int>& search_tol,
                                 vec2<int>& best_match_pos,
                               vec2<float>& match_center,
                                     float& quality,
                                        int num_best = 1) {
    

#if 0    
    cerr << DBG(outer.hsize()) << endl;
    cerr << DBG(outer.vsize()) << endl;
    cerr << DBG(inner.hsize()) << endl;
    cerr << DBG(inner.vsize()) << endl;
    
    cerr << DBG(inner_pos) << endl;
    cerr << DBG(inner_center) << endl;
    cerr << DBG(search_tol) << endl;
#endif
   
    int h_start = 0;
    int v_start = 0;
    int h_offs_end = outer.hsize() - inner.hsize();
    int v_offs_end = outer.vsize() - inner.vsize();
    int h_best = inner_pos[0];
    int v_best = inner_pos[1];
    int h_tol = search_tol[0];
    int v_tol = search_tol[1];
    
    if (h_tol && (h_best>=h_tol))
        h_start = h_best - h_tol;

    if (v_tol && (v_best>=v_tol))
        v_start = v_best - v_tol;
    
    if (h_tol && (h_best + h_tol + 1< h_offs_end))
        h_offs_end = h_best + h_tol + 1;
    
    if (v_tol && (v_best + v_tol + 1< v_offs_end))
        v_offs_end = v_best + v_tol + 1;

    // return if nothing to search
    if ((h_start >= h_offs_end) || (v_start>= v_offs_end)) {
        return 0;
    }
        
//    cerr << DBG(h_start)<< DBG(h_offs_end)<< DBG(v_start)<< DBG(v_offs_end)<<endl;
#define N_correl_match
#ifdef correl_match
    hit_list<float,vec2<int> > best_n(num_best);
#else    
    hit_list<int,vec2<int>,less_first<pair<int,vec2<int> > > > best_n(num_best);
#endif
ptime match_start;    
    int init_type(0);

    for (int h = h_start; h < h_offs_end; h++) {
        for (int v = v_start; v < v_offs_end; v++) {
            const_submat<typename outer_mat::value_type> sub_outer(outer,h,v,inner.hsize(),inner.vsize());
#ifdef correl_match
            float match_result  = correlation(inner.begin(),inner.end(),sub_outer.begin());
#else
            int match_result  = delta(inner.begin(),inner.end(),sub_outer.begin(),init_type);
#endif
            best_n.push(match_result,vec2<int>(h,v));
        }
    }
ptime match_time = ptime() - match_start;
//    cerr << DMS(match_time) << endl;

    vec2<int> mbs(num_best/2,num_best/2);
    box2<vec2<int> > match_bounds(best_n.item(0)-mbs,best_n.item(0)+mbs); 
    
    if (!best_n.size()) {
        cerr << "WARNING: match_matrix_sub_pixel: no search area, returning" << endl;
        return 0;
    } else {
        
    }
    
    typedef pair<vec2<int>,int> point_match;
    hit_list<int,point_match> rich_n(num_best);
    
    // compute sum of match
#ifdef correl_match
    float total_score = 0;
    hit_list<float,vec2<int> > best_inside(num_best);
#else
    int total_score = 0;
    hit_list<int,vec2<int>,less_first<pair<int,vec2<int> > > > best_inside(num_best);
#endif

    for (int i = 0;i < best_n.size();i++) {
        if (match_bounds.contains(best_n.item(i))) {
#ifdef correl_match
            float score = best_n.score(i);
            total_score += score;
#else
            int score = best_n.score(i);
            total_score += score * score;
#endif
            best_inside.push(best_n.score(i),best_n.item(i));

            const_submat<typename outer_mat::value_type> feature_ref(outer,
                                                                best_n.item(i)[0],
                                                                best_n.item(i)[1],
                                                                inner.hsize(),
                                                                inner.vsize());
            int richness = vertical_feature_richness(feature_ref);
            rich_n.push(richness,point_match(best_n.item(i),best_n.score(i)));
//            cerr << DBG(best_n.score(i)) << DBG(best_n.item(i)) << DBG(richness) << endl;
        }
    }
    if (!rich_n.size()) {
        cerr << "WARNING: match_matrix_sub_pixel: no rich features found" << endl;
        return 0;
    };
    if (!best_inside.size()) {
        cerr << "WARNING: match_matrix_sub_pixel: no best_inside features" << endl;
        return 0;
    };
    
    best_match_pos = rich_n.item(0).first;
//    best_match_pos = best_n.item(0);
//    best_match_pos = best_inside.item(0);
//    cerr << DBG(total_score) << DBG (best_match_pos) << endl;
    
    // compute weighted average
    float h_best_f = 0;
    float v_best_f = 0;
    float total_weight = 0;
//    box2<vec2<int> > best_bounds;
    for (int j = 0;j < best_inside.size();j++) {
        float weight;
        if (num_best!=1) {
            float score = best_inside.score(j);
#ifdef correl_match
            weight = score / total_score;
#else
            weight = ( total_score - score * score) / (total_score * (best_inside.size()-1.0));
#endif
        }
        else 
            weight = 1;
        total_weight += weight;
        h_best_f += static_cast<float>(best_inside.item(j)[0]) * weight;
        v_best_f += static_cast<float>(best_inside.item(j)[1]) * weight;
//        cerr << DBG(weight)<< DBG(h_best_f)<< DBG(v_best_f)<<endl;
//        best_bounds.extendBy(best_inside.item(j));
    }
//    cerr << DBG(total_weight)<<endl;
    
    vec2<float> center_offs = vec2<float>(h_best_f,v_best_f) - float_vec(best_match_pos);
    
    vec2<float> fix_center = vec2<float>(inner.hsize(),inner.vsize()) /2.0;
    vec2<float> new_center = inner_center + center_offs;
    match_center = ((7 * new_center + fix_center) / 8.0);
//    match_center = new_center;
    
    int num_val = inner.hsize()*inner.vsize();
#ifdef correl_match
    quality = total_score / 4;
#else
    float rel_score = sqrt(total_score)/(num_val*4.0);
    if (rel_score != 0)
        quality = best_inside.size() / rel_score;
    else
        quality = largest(float());
#endif    
//    cerr << DBG(best_match_pos) <<DBG(inner_center)<<DBG(inner_pos)<<DBG(center_offs)<<DBG(h_best_f)<< DBG(v_best_f)<<DBG(match_center) <<DBG(quality)<<endl;
    
//    cerr << DBG(best_bounds) << endl;
    
//    int h_scatter,v_scatter;
//    best_bounds.getSize(h_scatter,v_scatter);
    
    bool good =  (best_inside.size() >= num_best/2);
    if (!good) {
        cerr << "less than half of required inside a rect [-req/2..+req/2]" << endl;
    }
    return good;
}

//
// match_matrix - find inner_mat inside outer_mat with best match; position returned in h_best and v_best
//    if  htol and vtol are zero, the whole outer_mat is searched
//    if  htol or vtol are nonzero, h_best and v_best are considered as start positions,
//        and the search for inner_mat is performed within a region from {best-tol} to {best+tol}
//        including both
//        if the hints are invalid, the whole region is searched  

template <class outer_mat, class inner_mat>
bool pre_match_matrix(const outer_mat& outer,
                      const inner_mat& inner,
                      const vec2<int>& inner_pos,
                      const vec2<int>& search_tol,
                            vec2<int>& new_inner_pos,
                            vec2<int>& new_search_tol,
                        int num_best = 2) {
    
    int h_start = 0;
    int v_start = 0;
    int h_offs_end = outer.hsize() - inner.hsize();
    int v_offs_end = outer.vsize() - inner.vsize();
    
    int h_best = inner_pos[0];
    int v_best = inner_pos[1];
    int h_tol = search_tol[0];
    int v_tol = search_tol[1];

    float best_match = -1;
    int match_result;
    int init_type(0);
    
    if (h_tol && (h_best>=h_tol))
        h_start = h_best - h_tol;

    if (v_tol && (v_best>=v_tol))
        v_start = v_best - v_tol;
    
    if (h_tol && (h_best + h_tol + 1< h_offs_end))
        h_offs_end = h_best + h_tol + 1;
    
    if (v_tol && (v_best + v_tol + 1< v_offs_end))
        v_offs_end = v_best + v_tol + 1;
        
    vector<vec2<int> > extreme_points = get_extreme_coords(inner,12);

#ifdef correl_match
    hit_list<int,vec2<int> > best_n(num_best);
#else
    hit_list<int,vec2<int>,less_first<pair<int,vec2<int> > > > best_n(num_best);
#endif
    for (int h = h_start; h < h_offs_end; h++) {
        for (int v = v_start; v < v_offs_end; v++) {
            const_submat<typename outer_mat::value_type> sub_outer(outer,h,v,inner.hsize(),inner.vsize());
#ifdef correl_match
            float match_result  = correlation(inner,sub_outer,extreme_points.begin(),extreme_points.end());
#else
            int match_result  = delta(inner,sub_outer,extreme_points.begin(),extreme_points.end(),init_type);
#endif
            best_n.push(match_result,vec2<int>(h,v));
        }
    }
    
    box2<vec2<int> > best_bounds;
    for (int j = 0;j < best_n.size();j++) {
        best_bounds.extendBy(best_n.item(j));
    }
    new_inner_pos = best_bounds.getCenter();
    new_search_tol = best_bounds.getSize()+vec2<int>(1,1);
    
    return 1;
}
template <class outer_mat, class inner_mat>
bool fast_match_matrix_sub_pixel(const outer_mat& outer,
                           const inner_mat& inner,
                           const vec2<int>& inner_pos,
                         const vec2<float>& inner_center, // relative to inner_pos
                           const vec2<int>& search_tol,
                                 vec2<int>& best_match_pos,
                               vec2<float>& match_center,
                                     float& quality,
                                        int num_best = 1)
{
    vec2<int> new_inner_pos;
    vec2<int> new_search_tol;
    if (pre_match_matrix(outer,inner,inner_pos,search_tol, new_inner_pos, new_search_tol,num_best)) {
        new_search_tol = vec2<int>(min(new_search_tol[0],search_tol[0]/2),
                                   min(new_search_tol[1],search_tol[1]/2));
        bool good = match_matrix_sub_pixel(outer,
                                     inner,
                                     new_inner_pos,
                                     inner_center,
                                     new_search_tol,
                                     best_match_pos,
                                     match_center,
                                     quality,
                                     num_best);
        vec2<int> necessary_dist = new_inner_pos - best_match_pos;
//        cerr << DBG(search_tol) << DBG(new_search_tol)<< DBG(necessary_dist) << endl;
        return good;
    };
    quality = 0;
    cerr << "pre_match_matrix returned false" << endl;
    return 0;
}


template <class PIXEL, class ARITH_TYPE>
matrix<PIXEL> make_abs_diff_mat(const const_submat<PIXEL>& m1, const const_submat<PIXEL>& m2, const ARITH_TYPE &at) {
    int hsize = min(m1.hsize(),m2.hsize());
    int vsize = min(m1.vsize(),m2.vsize());

    const_submat<PIXEL> cm1(m1,0,0,hsize,vsize);
    const_submat<PIXEL> cm2(m2,0,0,hsize,vsize);

    matrix<PIXEL> result(hsize,vsize);
    absdiff(cm1.begin(),cm1.end(),cm2.begin(),result.begin(),at);
    return result;
}
template <class PIXEL, class SCALE_TYPE>
matrix<PIXEL> make_squared_diff_mat(const const_submat<PIXEL>& m1, const const_submat<PIXEL>& m2, const SCALE_TYPE &scale_down) {
//cerr << "Start make_squared_diff_mat, sizeof SCALE_TYPE =" << sizeof(SCALE_TYPE) << endl;
//cerr << DBG(scale_down[0]) << DBG(scale_down[1]) << DBG(scale_down[2]) << DBG(scale_down) << endl;
    int hsize = min(m1.hsize(),m2.hsize());
    int vsize = min(m1.vsize(),m2.vsize());

    const_submat<PIXEL> cm1(m1,0,0,hsize,vsize);
    const_submat<PIXEL> cm2(m2,0,0,hsize,vsize);

    matrix<PIXEL> result(hsize,vsize);
    squared_diff(cm1.begin(),cm1.end(),cm2.begin(),result.begin(),scale_down);
    return result;
}

struct moment_results {
    float l;
    float w;
    vec2<float> center;
    float major_angle;
    vec2<float> major_dir;
    float minor_angle;
    vec2<float> minor_dir;
};


template <class MATRIX>
bool analyze_moments(const MATRIX& mat, moment_results& mom)
{
    ptime mom_begin;
    float m00 = moment(0,0,mat,float(0));
    float m10 = moment(1,0,mat,float(0));
    float m01 = moment(0,1,mat,float(0));
    float m11 = moment(1,1,mat,float(0));
    float m20 = moment(2,0,mat,float(0));
    float m02 = moment(0,2,mat,float(0));
    ptime momentset_time = ptime()-mom_begin;

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

    mom.center = vec2<float>(xc,yc);
    mom.major_angle = major_axis_angle;
    mom.major_dir = direction_vector(major_axis_angle);
    mom.minor_dir = mom.major_dir.ortho();
    mom.minor_angle = angle_of(mom.minor_dir);
    
    //cerr << DBG(a) << DBG(b) <<DBG(c) <<DBG(mom.l) <<DBG(mom.w) <<
     //       DBG(gcenter) << DBG(mom.major_dir) << DBG(mom.minor_dir) <<endl;

    return 1;
}

template <class InputIterator>
InputIterator intersection(int h, int v, int hs, int vs, InputIterator begin, InputIterator end) {
    while (begin!=end) {
        if ( (abs(h - (*begin).first[0]) < hs) && (abs(v - (*begin).first[1]) < vs)) {
//            cerr << "intersection ok" << endl;
            return begin;
        }
        ++begin;
//        cerr << "no intersection" << endl;
    }
    return end;
}
template <class InputIterator>
bool intersect(vec2<int>&pos1, vec2<int>&pos2, vec2<int>& size) {
        return (abs(pos1[0] - pos2[0]) < size[0]) && (abs(pos1[1] - pos2[1]) < size[1]);
}
typedef pair<vec2<int>,int> point_int_pair;
typedef vector<point_int_pair> point_int_vector;


bool try_feature( const point_int_pair& new_feature,point_int_vector& features, int max_features, int hsize, int vsize) {
//cerr << "// try_feature entered " << DBG(features.size()) << endl;
    if (max_features<= 0) return false;
    if (features.size() == 0) {
        // insert first feature
        features.push_back(new_feature);
//        cerr << "// insert feature << " << new_feature << " because it is the first" << endl;
    } else {
        // fast small feature check
        if ((features.size()>=max_features) && (new_feature.second <= features.back().second)) 
            return false;

        // remove all intersecting features with lower priority
        point_int_vector::iterator which;
        while ((which = intersection(new_feature.first[0],new_feature.first[1],hsize,vsize, features.begin(), features.end())) 
                != features.end()) {
            // "which" contains largest intersecting element
//            cerr << "// 'which' contains largest intersecting element" << endl;
            if (new_feature.second > which->second) {
//                cerr << "feature " << *which << "erase because of intersection;" << endl;
                features.erase(which);
           } else {
                // new_feature intersects already found higher priority feature
//                cerr << "// new_feature intersects already found higher priority feature" << endl;
                return false;
            }
        }
        // new_feature does not intersect any feature element now
//        cerr << "// new_feature does not intersect any feature element now" << endl;
        if ((features.size()<max_features) || (new_feature.second > features.back().second)) {
            // new feature insertion
//            cerr << "// new feature insertion of " << DBG(new_feature) << "before insertion " << DBG(features.size()) << endl;
            features.push_back(new_feature);
            sort(features.begin(),features.end(),greater_second<point_int_pair>());
            // remove ueberzaehlige features
//            cerr << "// remove ueberzaehlige features" << endl;
           if (features.size()>max_features) {
//                cerr << "remove last feature" << DBG(features.back()) << DBG(features.size()) << endl;
                features.pop_back();
            }
        } else {
            // no reason to remember new feature
//             cerr << "// no reason to remember new feature" << endl;
           return false;
        }
    }
//cerr << "// try_feature ready, return true" << endl;
    return true;         
}

template <class RASTER>
point_int_vector 
find_features(const RASTER& sums, int hsize, int vsize, int  max_features) {
    int hlim = sums.hsize() - hsize;
    int vlim = sums.vsize() - vsize;
    cerr << "find_features " << DBG(hsize) << DBG(vsize)
                             << DBG(sums.hsize()) << DBG(sums.vsize())
                             << DBG(hlim) << DBG(vlim) << endl;
    assert(hlim>0);
    assert(vlim>0);
    point_int_vector features(0);
    for (int vp = 0; vp < vlim; vp++) {
        for (int hp = 0; hp < hlim; hp++) {
            BGR_t<int> weight = get_run_rect_sum(sums, hp, vp, hsize, vsize,BGR_t<int>(0,0,0));
            int totalw = channel_sum(weight,int(0));
//            cerr << "try " << DBG(hp) << DBG (vp) << DBG(totalw) << endl;

            try_feature(make_pair(vec2<int>(hp,vp),totalw),features, max_features, hsize, vsize); 
//                for (int f = 0;f<features.size();f++) {
//                    cerr << f << ") pos = " << features[f].first << "val = " << features[f].second << endl;
//                }
//            cerr << "try ok" << DBG(hp) << DBG(vp) << DBG(vlim) << DBG(hlim)  << endl;
        }
    }
//    cerr << endl << "end find_features" << endl;
    return features;
}    

template <class PIXEL, class SUM_T>
fill_test(vec2<int> pos, const submat<PIXEL>& testmap, float radius, SUM_T max) {
    for (int v = 0; v < testmap.vsize(); v++) {
        float vd = pos[1]-v;
        for (int h = 0; h < testmap.hsize(); h++) {
            float hd = pos[0]-h;
            float dist = sqrt(hd*hd + vd*vd);
            if (dist <=radius) {
                float faktor =  1 - (dist/radius);
                testmap(h,v) = convert(max * faktor, PIXEL());
//                cerr << DBG(h) << DBG(v) << DBG(dist) << DBG(testmap(h,v)) << endl;
            }
        }
    }
}

template <class PIXEL, class SUM_T>
make_feature_test(const submat<PIXEL>& testmap, float radius, PIXEL init,SUM_T max) {
    if (radius == 0) return;
    fill(testmap.begin(),testmap.end(),init);
    vec2<int> size(testmap.hsize(),testmap.vsize());
    vec2<int> center = size / 2;
    vec2<int> quarter = size /4;
    vec2<int> eye1 = center + vec2<int>(quarter[0],-quarter[1]);
    vec2<int> eye2 = center + vec2<int>(-quarter[0],-quarter[1]);
    vec2<int> nose = center;
    fill_test(eye1,testmap,radius,max);
    fill_test(eye2,testmap,radius,max);
    fill_test(nose,testmap,radius,max);
}

#endif
