//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: typedefs.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: typedefs.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: This file contains all the wellknown names for the
//               SOM
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_y60_typedefs_h_
#define _ac_y60_typedefs_h_

#include "AcBool.h"
#include "NodeValueNames.h"

#include <asl/Vector234.h>
#include <asl/Quaternion.h>
#include <asl/raster.h>
#include <asl/standard_pixel_types.h>

#include <string>
#include <vector>

namespace y60 {

    typedef std::vector<BlendFunction>         VectorOfBlendFunction;

    typedef std::vector<AcBool>                VectorOfBool;
    typedef std::vector<unsigned int>          VectorOfUnsignedInt;
    typedef std::vector<int>                   VectorOfSignedInt;
    typedef std::vector<float>                 VectorOfFloat;
    typedef std::vector<asl::Vector2i>         VectorOfVector2i;
    typedef std::vector<asl::Vector2f>         VectorOfVector2f;
    typedef std::vector<asl::Vector3f>         VectorOfVector3f;
    typedef std::vector<asl::Vector4f>         VectorOfVector4f;
    typedef std::vector<asl::Quaternionf>      VectorOfQuaternionf;

    typedef std::vector<std::string>           VectorOfString;
    typedef std::vector<VectorOfString>        VectorOfVectorOfString;
    typedef std::vector<VectorOfFloat>         VectorOfVectorOfFloat;

    typedef asl::raster<asl::RGB>              RasterOfRGB;
    typedef asl::raster<asl::RGBA>             RasterOfRGBA;
    typedef asl::raster<asl::BGR>              RasterOfBGR;
    typedef asl::raster<asl::BGRA>             RasterOfBGRA;
    typedef asl::raster<asl::ABGR>             RasterOfABGR;
    typedef asl::raster<asl::GRAY>             RasterOfGRAY;
    typedef asl::raster<asl::GRAY16>           RasterOfGRAY16;
    typedef asl::raster<asl::GRAY32>           RasterOfGRAY32;
    typedef asl::raster<asl::GRAYS16>          RasterOfGRAYS16;
    typedef asl::raster<asl::DXT1>             RasterOfDXT1;
    typedef asl::raster<asl::DXT1a>            RasterOfDXT1a;
    typedef asl::raster<asl::DXT3>             RasterOfDXT3;
    typedef asl::raster<asl::DXT5>             RasterOfDXT5;

    typedef asl::raster<asl::RGBA_HALF> RasterOfRGBA_HALF;
    typedef asl::raster<asl::RGB_HALF>  RasterOfRGB_HALF;
    typedef asl::raster<asl::RGBA_FLOAT> RasterOfRGBA_FLOAT;
    typedef asl::raster<asl::RGB_FLOAT>  RasterOfRGB_FLOAT;
}

#endif
