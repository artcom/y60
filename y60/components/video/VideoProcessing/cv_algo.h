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

#include <asl/raster/raster.h>

#include "y60_videoprocessing_settings.h"

namespace asl {

    template <class InputIterator,class OutputIterator,class Tin, class Tout>
        OutputIterator multi_chan_discriminate(InputIterator begin, InputIterator end, OutputIterator out,
                const Tin& cut_value, const Tout& below_value, const Tout& above_value)
        {
            InputIterator in = begin;
            if (cut_value.size()!=below_value.size()) {
                AC_ERROR << "chan_discriminate: channel numbers dont match";
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

    template <class InputIterator,class OutputIterator,class Tin, class Tout>
        OutputIterator chan_discriminate(InputIterator begin, InputIterator end, OutputIterator out,
                const Tin& zero_value, const Tin& cut_value,
                const Tout& below_value, const Tout& above_value)
        {
            InputIterator in = begin;
            while (in!=end) {
                if (channel_sum(*in, zero_value)<cut_value)
                    *out = below_value;
                else
                    *out = above_value;
                ++out;
                ++in;
            }
            return out;
        }

//    template <class T>
//        bool Pixel::operator<(const asl::gray<T> & first, const asl::gray<T> & second) const {
//            return first.get() < second.get();
//        }
//
//    template <class T>
//        bool Pixel::operator>(const asl::gray<T> & first, const asl::gray<T> & second) const {
//            return first.get() > second.get();
//        }

    template <class InputIterator,class OutputIterator,class Tin, class Tout>
        OutputIterator discriminate(InputIterator begin, InputIterator end, OutputIterator out,
                const Tin& cut_value, const Tout& below_value, const Tout& above_value)
        {
            InputIterator in = begin;
            while (in!=end) {
                if (cut_value.get() > (*in).get()) // TODO: implement operators in Pixel
                    *out++ = below_value;
                else
                    *out++ = above_value;
                ++in;
            }
            return out;
        }

    template <class InputIterator,class OutputIterator,class T>
        OutputIterator lowpass(unsigned int radius,InputIterator begin, InputIterator end, OutputIterator out, const T& init) {
            //    AC_ERROR << "lowpas(0):size = " << end-begin <<  endl;
            T value = init;
            InputIterator lead = begin;

            //    InputIterator first_out = out;
            unsigned int lead_count = 0;
            while (lead!=end && lead_count<radius) {
                value = value + auto_cast<T>(*lead);
                ++lead;
                ++lead_count;
            }
            //   cerr << "lowpas(1):lead_count = " << lead_count << ",lead = " << lead - begin << endl;
            unsigned int count=lead_count;
            while (lead_count--) {
                //       cerr << "lowpas(I):out = " << out-first_out << " count = " << count << ",lead = " << lead - begin << endl;
                *out++ = auto_cast<typename std::iterator_traits<OutputIterator>::value_type>(value/count);
                if (lead!=end) {
                    value = value + auto_cast<T>(*lead);
                    ++lead;
                    ++count;
                }
            }
            //   cerr << "lowpas(2):count = " << count << ",lead = " << lead - begin << endl;
            InputIterator trail = begin;
            if (count) --count;
            while (lead!=end) {
                value = value - auto_cast<T>(*trail++);
                //       cerr << "lowpas(X):out = " << out-first_out << " count = " << count << ",trail = " << trail - begin <<",lead = " << lead - begin << endl;
                *out++ = auto_cast<typename std::iterator_traits<OutputIterator>::value_type>(value/count);
                value = value + auto_cast<T>(*lead++);
            }
            //   cerr << "lowpas(3):count = " << count << ",trail = " << trail - begin <<",lead = " << lead - begin << endl;
            while (trail!=end && count>=radius) {
                //       cerr << "lowpas(T):out = " << out-first_out << " count = " << count-1 << ",trail = " << trail - begin <<",lead = " << lead - begin << endl;
                value = value - auto_cast<T>(*trail++);
                *out++ = auto_cast<typename std::iterator_traits<OutputIterator>::value_type>(value/count);
                count--;
            }
            return out;
        }

    template <class IN_RASTER, class OUT_RASTER, class SUM_T>
        void h_lowpass(int radius, const IN_RASTER& in, OUT_RASTER& out, const SUM_T& sumt) {
            if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
                for (int v = 0; v < in.vsize(); v++) {
                    lowpass(radius, in.hfind(0,v),in.hfind(in.hsize(),v), out.hfind(0,v),sumt);
                }
            } else {
                AC_ERROR << "h_lowpass(const matrix& in, matrix& out): bad out size";
            }
        }
    template <class IN_RASTER, class OUT_RASTER, class SUM_T>
        void v_lowpass(unsigned int radius, const IN_RASTER& in, OUT_RASTER& out, const SUM_T& sumt) {
            if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
                for (int h = 0; h < in.hsize(); h++) {
                    lowpass(radius, in.vfind(h,0),in.vfind(h,in.vsize()), out.vfind(h,0),sumt);
                }
            } else {
                AC_ERROR << "v_lowpass(const matrix& in, matrix& out): bad out size";
            }
        }

    template <class IN_RASTER, class OUT_RASTER, class DIV_T>
        void lowpass2d(unsigned int h_radius, unsigned int v_radius, const IN_RASTER& in, OUT_RASTER& out, const DIV_T& divt) {
            raster<typename IN_RASTER::value_type> tmp(in.hsize(),in.vsize());
            h_lowpass(h_radius, in, tmp,divt);
            v_lowpass(v_radius, tmp, out,divt);
        }

    template <class InputIterator,class OutputIterator,class T>
        OutputIterator gradient(int radius,InputIterator begin, InputIterator end, OutputIterator out, const T& init) {
            InputIterator lead = begin;
            InputIterator trail = begin;
            int lead_count = 0;
            T lead_value = init;
            while (lead!=end && lead_count<radius) {
                lead_value = auto_cast<T>(*lead++);
                ++lead_count;
            }

            //    InputIterator first_out = out;

            while (lead_count-- && lead!=end) {
                //      cerr << "gradient(I):out = " << out-first_out << " lead_count = " << lead_count << ",trail = " << trail - begin <<",lead = " << lead - begin << endl;
                lead_value = auto_cast<T>(*lead++);
                *out++ = lead_value - auto_cast<T>(*trail);
            }
            while (lead!=end) {
                //        cerr << "gradient(M):out = " << out-first_out << " lead_count = " << lead_count << ",trail = " << trail - begin <<",lead = " << lead - begin << endl;
                lead_value = auto_cast<T>(*lead++);
                *out++ =  lead_value - auto_cast<T>(*trail++);;
            }
            int trail_count = radius;
            while (trail!=end && trail_count--) {
                //         cerr << "gradient(E):out = " << out-first_out << " lead_count = " << lead_count << ",trail = " << trail - begin <<",lead = " << lead-1 - begin << endl;
                *out++ = lead_value - auto_cast<T>(*trail++);;
            }
            return out;
        }

    template <class InputIterator,class OutputIterator,class T>
        OutputIterator abs_gradient(int radius,InputIterator begin, InputIterator end, OutputIterator out, const T& init) {
            InputIterator lead = begin;
            InputIterator trail = begin;
            int lead_count = 0;
            T lead_value = init;
            while (lead!=end && lead_count<radius) {
                lead_value = auto_cast<T>(*lead++, init);
                ++lead_count;
            }

            while (lead_count-- && lead!=end) {
                lead_value = auto_cast<T>(*lead++);
                *out++ = auto_cast<typename std::iterator_traits<OutputIterator>::value_type>(pixel_abs(lead_value - auto_cast<T>(*trail)));
            }
            while (lead!=end) {
                lead_value = auto_cast<T>(*lead++, init);
                *out++ =  auto_cast<typename std::iterator_traits<OutputIterator>::value_type>(pixel_abs(lead_value - auto_cast<T>(*trail++)));
            }
            int trail_count = radius;
            while (trail!=end && trail_count--) {
                *out++ = auto_cast<typename std::iterator_traits<OutputIterator>::value_type>(pixel_abs(lead_value - auto_cast<T>(*trail++)));
            }
            return out;
        }

    template <class IN_RASTER, class OUT_RASTER, class SUM_T>
        void h_gradient(int radius, const IN_RASTER& in, OUT_RASTER& out, const SUM_T& sumt) {
            if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
                for (int v = 0; v < in.vsize(); v++) {
                    gradient(radius, in.hfind(0,v),in.hfind(in.hsize(),v), out.hfind(0,v),sumt);
                }
            } else {
                AC_ERROR << "h_gradient(const matrix& in, matrix& out): bad out size";
            }
        }
    template <class IN_RASTER, class OUT_RASTER, class SUM_T>
        void v_gradient(int radius, const IN_RASTER& in, OUT_RASTER& out, const SUM_T& sumt) {
            if ((out.hsize()==in.hsize()) && (out.vsize()==in.vsize())) {
                for (int h = 0; h < in.hsize(); h++) {
                    gradient(radius, in.vfind(h,0),in.vfind(h,in.vsize()), out.vfind(h,0),sumt);
                }
            } else {
                AC_ERROR << "v_gradient(const matrix& in, matrix& out): bad out size";
            }
        }

    //template <class PIXEL, class DIV_T>
    //void gradient(int radius, const const_submat<PIXEL>& in, const submat<PIXEL>& out, const DIV_T& divt) {
    template <class IN_RASTER, class OUT_RASTER, class DIV_T>
        void gradient(int radius, const IN_RASTER& in, OUT_RASTER& out, const DIV_T& divt) {
            raster<typename IN_RASTER::value_type> h_tmp(in.hsize(),in.vsize());
            raster<typename IN_RASTER::value_type> v_tmp(in.hsize(),in.vsize());

            h_gradient(radius, in, h_tmp,divt);
            v_gradient(radius, in, v_tmp,divt);

            paverage(h_tmp.begin(), h_tmp.end(), v_tmp.begin(), out.begin(), DIV_T(2));
        }

    template <class InputIterator,class OutputIterator,class T>
        OutputIterator paverage(InputIterator begin, InputIterator end, InputIterator begin2, OutputIterator out, const T& divisor) {
            while (begin!=end) {
                *out++ = auto_cast<typename std::iterator_traits<OutputIterator>::value_type>(((auto_cast<T>(*begin++) + auto_cast<T>(*begin2++))) / divisor);
            }
            return out;
        }

    template <class T, class SumT>
        struct channel_sum_function : public std::unary_function<T, SumT> {
            channel_sum_function() {}
            channel_sum_function(const SumT & Init_val) : init_val(Init_val) {}
            SumT operator()(const T& pixel) const {
                 return channel_sum(pixel, init_val)/T::size();
            }
            SumT init_val;
        };

}
