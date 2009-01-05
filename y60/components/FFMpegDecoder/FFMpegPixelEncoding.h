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

#ifndef _ac_y60_FFMpegPixelEncoding_h_
#define _ac_y60_FFMpegPixelEncoding_h_

#include <y60/image/PixelEncoding.h>
#include <string>

#ifdef OSX
    extern "C" {
#       include <libavcodec/avcodec.h>
#       include <libavformat/avformat.h>
    }
#   undef AV_NOPTS_VALUE
#   define AV_NOPTS_VALUE 0x8000000000000000LL
#else
#   if defined(_MSC_VER)
#       pragma warning(push,1)
#   endif
    extern "C" {
#       include <ffmpeg/avcodec.h>
#       include <ffmpeg/avformat.h>
    }
#   if defined(_MSC_VER)
#       pragma warning(pop)
#   endif
#endif


namespace y60 {

    inline PixelEncoding 
    getPixelEncoding(AVStream * theStream) {
#if LIBAVCODEC_BUILD >= 0x5100
        switch (theStream->codec->pix_fmt) {
#else
        switch (theStream->codec.pix_fmt) {
#endif
            case PIX_FMT_RGB24:
                return y60::RGB;
            case PIX_FMT_BGR24:
                return y60::BGR;
            case PIX_FMT_RGBA32:
                return y60::RGBA;
            case PIX_FMT_GRAY8:
                return y60::GRAY;
            default:
                return y60::RGB;
        }
    }

    inline std::string
    getPixelEncodingInfo(AVStream * theStream) {
#if LIBAVCODEC_BUILD >= 0x5100
        switch (theStream->codec->pix_fmt) {
#else
        switch (theStream->codec.pix_fmt) {
#endif
            case PIX_FMT_YUV420P:
                return "PIX_FMT_YUV420P: Planar YUV 4:2:0 (1 Cr & Cb sample per 2x2 Y samples).";
            case PIX_FMT_YUV422:
                return "PIX_FMT_YUV422: Packed pixel, Y0 Cb Y1 Cr.";
            case PIX_FMT_RGB24:
                return "PIX_FMT_RGB24: Packed pixel, 3 bytes per pixel, RGBRGB...";
            case PIX_FMT_BGR24:
                return "PIX_FMT_BGR24: Packed pixel, 3 bytes per pixel, BGRBGR...";
            case PIX_FMT_YUV422P:
                return "PIX_FMT_YUV422P: Planar YUV 4:2:2 (1 Cr & Cb sample per 2x1 Y samples).";
            case PIX_FMT_YUV444P:
                return "PIX_FMT_YUV444P: Planar YUV 4:4:4 (1 Cr & Cb sample per 1x1 Y samples).";
            case PIX_FMT_RGBA32:
                return "PIX_FMT_RGBA32: Packed pixel, 4 bytes per pixel, BGRABGRA..., stored in cpu endianness.";
            case PIX_FMT_YUV410P:
                return "PIX_FMT_YUV410P: Planar YUV 4:1:0 (1 Cr & Cb sample per 4x4 Y samples).";
            case PIX_FMT_YUV411P:
                return "PIX_FMT_YUV411P: Planar YUV 4:1:1 (1 Cr & Cb sample per 4x1 Y samples).";
            case PIX_FMT_RGB565:
                return "PIX_FMT_RGB565: always stored in cpu endianness";
            case PIX_FMT_RGB555:
                return "PIX_FMT_RGB555: always stored in cpu endianness, most significant bit to 1";
            case PIX_FMT_GRAY8:
                return "PIX_FMT_GRAY8";
            case PIX_FMT_MONOWHITE:
                return "PIX_FMT_MONOWHITE: 0 is white";
            case PIX_FMT_MONOBLACK:
                return "PIX_FMT_MONOBLACK: 0 is black";
            case PIX_FMT_PAL8:
                return "PIX_FMT_PAL8: 8 bit with RGBA palette";
            case PIX_FMT_YUVJ420P:
                return "PIX_FMT_YUVJ420P: Planar YUV 4:2:0 full scale (jpeg).";
            case PIX_FMT_YUVJ422P:
                return "PIX_FMT_YUVJ422P: Planar YUV 4:2:2 full scale (jpeg).";
            case PIX_FMT_YUVJ444P:
                return "PIX_FMT_YUVJ444P: YUV 4:4:4 full scale (jpeg).";
            case PIX_FMT_XVMC_MPEG2_MC:
                return "PIX_FMT_XVMC_MPEG2_MC: XVideo Motion Acceleration via common packet passing(xvmc_render.h).";
            case PIX_FMT_XVMC_MPEG2_IDCT:
                return "PIX_FMT_XVMC_MPEG2_IDCT";
#if LIBAVCODEC_BUILD > 4754
            case PIX_FMT_UYVY422:
                return "PIX_FMT_UYVY422: Packed pixel, Cb Y0 Cr Y1.";
            case PIX_FMT_UYVY411:
                return "PIX_FMT_UYVY411: Packed pixel, Cb Y0 Y1 Cr Y2 Y3.";
#endif                
            case PIX_FMT_NB:
                return "PIX_FMT_NB";
            default:
                return "unknown pixelformat";
        }
    }
}


#endif
