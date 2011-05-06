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

#include "y60_base_settings.h"

#include "AcBool.h"
#include "NodeValueNames.h"

#include <asl/math/Vector234.h>
#include <asl/math/Quaternion.h>
#include <asl/raster/raster.h>
#include <asl/raster/standard_pixel_types.h>

#include <string>
#include <vector>

namespace y60 {

    typedef std::vector<BlendFunction>         VectorOfBlendFunction;
    typedef std::vector<BlendEquation>         VectorOfBlendEquation;

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
