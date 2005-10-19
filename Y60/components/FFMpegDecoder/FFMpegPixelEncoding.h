//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: FFMpegDecoder.h,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2005/04/01 10:12:21 $
//
//  ffmpeg movie decoder.
//
//=============================================================================

#ifndef _ac_y60_FFMpegPixelEncoding_h_
#define _ac_y60_FFMpegPixelEncoding_h_

#include <y60/PixelEncoding.h>
#include <string>

#ifdef WIN32
#pragma warning( disable : 4244 ) // Disable ffmpeg warning
#define EMULATE_INTTYPES
#endif
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#ifdef WIN32
#pragma warning( default : 4244 ) // Renable ffmpeg warning
#endif

namespace y60 {

    inline PixelEncoding 
    getPixelEncoding(AVStream * theStream) {
        switch (theStream->codec.pix_fmt) {
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
        switch (theStream->codec.pix_fmt) {
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
