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

#ifndef _ac_NodeValueNames_h_
#define _ac_NodeValueNames_h_

#include "y60_base_settings.h"

#include <asl/base/Enum.h>

namespace y60 {

    //=== Lighting Model ==========================================================
    enum LightingModel {
        BLINN, PHONG, GOURAUD, LAMBERT, FLAT, UNLIT, MAX_LIGHTING_MODEL
    };
    const char * const LIGHTING_MODEL_BLINN = "blinn";
    const char * const LIGHTING_MODEL_PHONG = "phong";
    const char * const LIGHTING_MODEL_GOURAUD = "gouraud";
    const char * const LIGHTING_MODEL_LAMBERT = "lambert";
    const char * const LIGHTING_MODEL_FLAT = "flat";
    const char * const LIGHTING_MODEL_UNLIT = "unlit";

    const char * const LightingModelString[] = {
        LIGHTING_MODEL_BLINN,
        LIGHTING_MODEL_PHONG,
        LIGHTING_MODEL_GOURAUD,
        LIGHTING_MODEL_LAMBERT,
        LIGHTING_MODEL_FLAT,
        LIGHTING_MODEL_UNLIT,
        0
    };

    const char * const REGISTER_ATTRIB            = "register";

    //=== Material feature names ==================================================
    const char * const TEXCOORD_FEATURE           = "texcoord";
    const char * const LIGHTING_FEATURE           = "lighting";
    const char * const TEXTURE_FEATURE            = "textures";
    const char * const MAPPING_FEATURE            = "texcoord";
    const char * const FEATURE_DROPPED            = "drop_feature";

     //=== Cg specific property name ===============================================
    const char * const CG_FILES_PROPERTY           = "files";
    const char * const CG_PROFILES_PROPERTY        = "profiles";
    const char * const CG_ENTRY_FUNCTIONS_PROPERTY = "entryfunctions";
    const char * const CG_COMPILERARGS2_PROPERTY   = "compilerargs";

    //=== VertexParameter names ===============================================
    const char * const VERTEXPARAM_PASS_THROUGH   = "pass_through";
    const char * const VERTEXPARAM_TANGENT        = "tangent";

    //=== Texture apply modes =====================================================
    const char * const TEXTURE_APPLY_MODULATE   = "modulate";
    const char * const TEXTURE_APPLY_DECAL      = "decal";
    const char * const TEXTURE_APPLY_REPLACE    = "replace";
    const char * const TEXTURE_APPLY_BLEND      = "blend";
    const char * const TEXTURE_APPLY_ADD        = "add";
    const char * const TEXTURE_APPLY_SUBTRACT   = "subtract";
    const char * const TEXTURE_APPLY_COMBINE    = "combine";

    enum TextureApplyModeEnum {
        MODULATE,
        DECAL,
        REPLACE,
        BLEND,
        ADD,
        SUBTRACT,
        COMBINE,
        TextureApplyModeEnum_MAX
    };

    const char * const TextureApplyModeStrings[] = {
        TEXTURE_APPLY_MODULATE,
        TEXTURE_APPLY_DECAL,
        TEXTURE_APPLY_REPLACE,
        TEXTURE_APPLY_BLEND,
        TEXTURE_APPLY_ADD,
        TEXTURE_APPLY_SUBTRACT,
        TEXTURE_APPLY_COMBINE,
        ""
    };
    DEFINE_ENUM(TextureApplyMode, TextureApplyModeEnum, Y60_BASE_DECL);

    //=== Texture magnification filters =====================================================
    const char * const TEXTURE_SAMPLE_FILTER_NEAREST       = "nearest";
    const char * const TEXTURE_SAMPLE_FILTER_LINEAR        = "linear";

    enum TextureSampleFilterEnum {
        NEAREST,
        LINEAR,
        TextureSampleFilterEnum_MAX
    };

    const char * const TextureSampleFilterStrings[] = {
        TEXTURE_SAMPLE_FILTER_NEAREST,
        TEXTURE_SAMPLE_FILTER_LINEAR,
        ""
    };
    DEFINE_ENUM(TextureSampleFilter, TextureSampleFilterEnum, Y60_BASE_DECL);

    //=== Texture repeat modes =====================================================
    const char * const TEXTURE_WRAP_CLAMP           = "clamp";
    const char * const TEXTURE_WRAP_CLAMP_TO_EDGE   = "clamp_to_edge";
    const char * const TEXTURE_WRAP_REPEAT          = "repeat";
    const char * const TEXTURE_WRAP_MIRROR          = "mirror";

    enum TextureWrapModeEnum {
        CLAMP,
        CLAMP_TO_EDGE,
        REPEAT,
        MIRROR,
        TextureWrapModeEnum_MAX
    };

    const char * const TextureWrapModeStrings[] = {
        TEXTURE_WRAP_CLAMP,
        TEXTURE_WRAP_CLAMP_TO_EDGE,
        TEXTURE_WRAP_REPEAT,
        TEXTURE_WRAP_MIRROR,
        ""
    };
    DEFINE_ENUM(TextureWrapMode, TextureWrapModeEnum, Y60_BASE_DECL);

    //=== Texture Internal Formats ============================================
    const char * const TEXTURE_INTERNAL_FORMAT_DEPTH               = "DEPTH";
    const char * const TEXTURE_INTERNAL_FORMAT_ALPHA               = "ALPHA";
    const char * const TEXTURE_INTERNAL_FORMAT_ALPHA4              = "ALPHA4";
    const char * const TEXTURE_INTERNAL_FORMAT_ALPHA8              = "ALPHA8";
    const char * const TEXTURE_INTERNAL_FORMAT_ALPHA12             = "ALPHA12";
    const char * const TEXTURE_INTERNAL_FORMAT_ALPHA16             = "ALPHA16";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE           = "LUMINANCE";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE4          = "LUMINANCE4";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE8          = "LUMINANCE8";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE12         = "LUMINANCE12";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE16         = "LUMINANCE16";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE_ALPHA     = "LUMINANCE_ALPHA";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE4_ALPHA4   = "LUMINANCE4_ALPHA4";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE6_ALPHA2   = "LUMINANCE6_ALPHA2";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE8_ALPHA8   = "LUMINANCE8_ALPHA8";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE12_ALPHA4  = "LUMINANCE12_ALPHA4";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE12_ALPHA12 = "LUMINANCE12_ALPHA12";
    const char * const TEXTURE_INTERNAL_FORMAT_LUMINANCE16_ALPHA16 = "LUMINANCE16_ALPHA16";
    const char * const TEXTURE_INTERNAL_FORMAT_INTENSITY           = "INTENSITY";
    const char * const TEXTURE_INTERNAL_FORMAT_INTENSITY4          = "INTENSITY4";
    const char * const TEXTURE_INTERNAL_FORMAT_INTENSITY8          = "INTENSITY8";
    const char * const TEXTURE_INTERNAL_FORMAT_INTENSITY12         = "INTENSITY12";
    const char * const TEXTURE_INTERNAL_FORMAT_INTENSITY16         = "INTENSITY16";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB                 = "RGB";
    const char * const TEXTURE_INTERNAL_FORMAT_BGR                 = "BGR";
    const char * const TEXTURE_INTERNAL_FORMAT_R3_G3_B2            = "R3_G3_B2";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB4                = "RGB4";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB5                = "RGB5";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB8                = "RGB8";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB10               = "RGB10";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB12               = "RGB12";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB16               = "RGB16";
    const char * const TEXTURE_INTERNAL_FORMAT_RGBA                = "RGBA";
    const char * const TEXTURE_INTERNAL_FORMAT_RGBA2               = "RGBA2";
    const char * const TEXTURE_INTERNAL_FORMAT_RGBA4               = "RGBA4";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB5_A1             = "RGB5_A1";
    const char * const TEXTURE_INTERNAL_FORMAT_RGBA8               = "RGBA8";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB10_A2            = "RGB10_A2";
    const char * const TEXTURE_INTERNAL_FORMAT_RGBA12              = "RGBA12";
    const char * const TEXTURE_INTERNAL_FORMAT_RGBA16              = "RGBA16";
    const char * const TEXTURE_INTERNAL_FORMAT_RGBA_HALF           = "RGBA_HALF";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB_HALF            = "RGB_HALF";
    const char * const TEXTURE_INTERNAL_FORMAT_RGBA_FLOAT          = "RGBA_FLOAT";
    const char * const TEXTURE_INTERNAL_FORMAT_RGB_FLOAT           = "RGB_FLOAT";
    const char * const TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGB_ARB  = "COMPRESSED_RGB_ARB";
    const char * const TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_ARB             = "COMPRESSED_RGBA_ARB";
    const char * const TEXTURE_INTERNAL_FORMAT_COMPRESSED_ALPHA_ARB            = "COMPRESSED_ALPHA_ARB";
    const char * const TEXTURE_INTERNAL_FORMAT_COMPRESSED_LUMINANCE_ARB        = "COMPRESSED_LUMINANCE_ARB";
    const char * const TEXTURE_INTERNAL_FORMAT_COMPRESSED_LUMINANCE_ALPHA_ARB  = "COMPRESSED_LUMINANCE_ALPHA_ARB";
    const char * const TEXTURE_INTERNAL_FORMAT_COMPRESSED_INTENSITY_ARB        = "COMPRESSED_INTENSITY_ARB";
    const char * const TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGB_S3TC_DXT1_EXT    = "COMPRESSED_RGB_S3TC_DXT1_EXT";
    const char * const TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_S3TC_DXT1_EXT   = "COMPRESSED_RGBA_S3TC_DXT1_EXT";
    const char * const TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_S3TC_DXT3_EXT   = "COMPRESSED_RGBA_S3TC_DXT3_EXT";
    const char * const TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_S3TC_DXT5_EXT   = "COMPRESSED_RGBA_S3TC_DXT5_EXT";
    const char * const TEXTURE_INTERNAL_FORMAT_YUV420              = "YUV420";
    const char * const TEXTURE_INTERNAL_FORMAT_YUV422              = "YUV422";
    const char * const TEXTURE_INTERNAL_FORMAT_YUV444              = "YUV444";
    const char * const TEXTURE_INTERNAL_FORMAT_YUVA420             = "YUVA420";

    enum TextureInternalFormat {
        TEXTURE_IFMT_DEPTH,
        TEXTURE_IFMT_ALPHA,
        TEXTURE_IFMT_ALPHA4,
        TEXTURE_IFMT_ALPHA8,
        TEXTURE_IFMT_ALPHA12,
        TEXTURE_IFMT_ALPHA16,
        TEXTURE_IFMT_LUMINANCE,
        TEXTURE_IFMT_LUMINANCE4,
        TEXTURE_IFMT_LUMINANCE8,
        TEXTURE_IFMT_LUMINANCE12,
        TEXTURE_IFMT_LUMINANCE16,
        TEXTURE_IFMT_LUMINANCE_ALPHA,
        TEXTURE_IFMT_LUMINANCE4_ALPHA4,
        TEXTURE_IFMT_LUMINANCE6_ALPHA2,
        TEXTURE_IFMT_LUMINANCE8_ALPHA8,
        TEXTURE_IFMT_LUMINANCE12_ALPHA4,
        TEXTURE_IFMT_LUMINANCE12_ALPHA12,
        TEXTURE_IFMT_LUMINANCE16_ALPHA16,
        TEXTURE_IFMT_INTENSITY,
        TEXTURE_IFMT_INTENSITY4,
        TEXTURE_IFMT_INTENSITY8,
        TEXTURE_IFMT_INTENSITY12,
        TEXTURE_IFMT_INTENSITY16,
        TEXTURE_IFMT_RGB,
        TEXTURE_IFMT_BGR,
        TEXTURE_IFMT_R3_G3_B2,
        TEXTURE_IFMT_RGB4,
        TEXTURE_IFMT_RGB5,
        TEXTURE_IFMT_RGB8,
        TEXTURE_IFMT_RGB10,
        TEXTURE_IFMT_RGB12,
        TEXTURE_IFMT_RGB16,
        TEXTURE_IFMT_RGBA,
        TEXTURE_IFMT_RGBA2,
        TEXTURE_IFMT_RGBA4,
        TEXTURE_IFMT_RGB5_A1,
        TEXTURE_IFMT_RGBA8,
        TEXTURE_IFMT_RGB10_A2,
        TEXTURE_IFMT_RGBA12,
        TEXTURE_IFMT_RGBA16,
        TEXTURE_IFMT_RGBA_HALF,
        TEXTURE_IFMT_RGB_HALF,
        TEXTURE_IFMT_RGBA_FLOAT,
        TEXTURE_IFMT_RGB_FLOAT,
        TEXTURE_IFMT_COMPRESSED_RGB_ARB,
        TEXTURE_IFMT_COMPRESSED_RGBA_ARB,
        TEXTURE_IFMT_COMPRESSED_ALPHA_ARB,
        TEXTURE_IFMT_COMPRESSED_LUMINANCE_ARB,
        TEXTURE_IFMT_COMPRESSED_LUMINANCE_ALPHA_ARB,
        TEXTURE_IFMT_COMPRESSED_INTENSITY_ARB,
        TEXTURE_IFMT_COMPRESSED_RGB_S3TC_DXT1_EXT,
        TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT1_EXT,
        TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT3_EXT,
        TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT5_EXT,
        TEXTURE_IFMT_YUV420,
        TEXTURE_IFMT_YUV422,
        TEXTURE_IFMT_YUV444,
        TEXTURE_IFMT_YUVA420
    };

    const char * const TextureInternalFormatStrings[] = {
        TEXTURE_INTERNAL_FORMAT_DEPTH,
        TEXTURE_INTERNAL_FORMAT_ALPHA,
        TEXTURE_INTERNAL_FORMAT_ALPHA4,
        TEXTURE_INTERNAL_FORMAT_ALPHA8,
        TEXTURE_INTERNAL_FORMAT_ALPHA12,
        TEXTURE_INTERNAL_FORMAT_ALPHA16,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE4,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE8,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE12,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE16,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE_ALPHA,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE4_ALPHA4,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE6_ALPHA2,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE8_ALPHA8,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE12_ALPHA4,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE12_ALPHA12,
        TEXTURE_INTERNAL_FORMAT_LUMINANCE16_ALPHA16,
        TEXTURE_INTERNAL_FORMAT_INTENSITY,
        TEXTURE_INTERNAL_FORMAT_INTENSITY4,
        TEXTURE_INTERNAL_FORMAT_INTENSITY8,
        TEXTURE_INTERNAL_FORMAT_INTENSITY12,
        TEXTURE_INTERNAL_FORMAT_INTENSITY16,
        TEXTURE_INTERNAL_FORMAT_RGB,
        TEXTURE_INTERNAL_FORMAT_BGR,
        TEXTURE_INTERNAL_FORMAT_R3_G3_B2,
        TEXTURE_INTERNAL_FORMAT_RGB4,
        TEXTURE_INTERNAL_FORMAT_RGB5,
        TEXTURE_INTERNAL_FORMAT_RGB8,
        TEXTURE_INTERNAL_FORMAT_RGB10,
        TEXTURE_INTERNAL_FORMAT_RGB12,
        TEXTURE_INTERNAL_FORMAT_RGB16,
        TEXTURE_INTERNAL_FORMAT_RGBA,
        TEXTURE_INTERNAL_FORMAT_RGBA2,
        TEXTURE_INTERNAL_FORMAT_RGBA4,
        TEXTURE_INTERNAL_FORMAT_RGB5_A1,
        TEXTURE_INTERNAL_FORMAT_RGBA8,
        TEXTURE_INTERNAL_FORMAT_RGB10_A2,
        TEXTURE_INTERNAL_FORMAT_RGBA12,
        TEXTURE_INTERNAL_FORMAT_RGBA16,
        TEXTURE_INTERNAL_FORMAT_RGBA_HALF,
        TEXTURE_INTERNAL_FORMAT_RGB_HALF,
        TEXTURE_INTERNAL_FORMAT_RGBA_FLOAT,
        TEXTURE_INTERNAL_FORMAT_RGB_FLOAT,
        TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGB_ARB,
        TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_ARB,
        TEXTURE_INTERNAL_FORMAT_COMPRESSED_ALPHA_ARB,
        TEXTURE_INTERNAL_FORMAT_COMPRESSED_LUMINANCE_ARB,
        TEXTURE_INTERNAL_FORMAT_COMPRESSED_LUMINANCE_ALPHA_ARB,
        TEXTURE_INTERNAL_FORMAT_COMPRESSED_INTENSITY_ARB,
        TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGB_S3TC_DXT1_EXT,
        TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_S3TC_DXT1_EXT,
        TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_S3TC_DXT3_EXT,
        TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_S3TC_DXT5_EXT,
        TEXTURE_INTERNAL_FORMAT_YUV420,
        TEXTURE_INTERNAL_FORMAT_YUV422,
        TEXTURE_INTERNAL_FORMAT_YUV444,
        TEXTURE_INTERNAL_FORMAT_YUVA420,
        0
    };

    //=== Automatic Texture Coord Generation Modes NEW ============================
    const char * const TEXCOORD_PLANAR          = "planar_projection";
    const char * const TEXCOORD_CUBE            = "cube_projection";
    const char * const TEXCOORD_FRONTAL         = "frontal_projection";
    const char * const TEXCOORD_PERSPECTIVE     = "perspective_projection";
    const char * const TEXCOORD_SPHERICAL       = "spherical_projection";
    const char * const TEXCOORD_CYLINDRICAL     = "cylindrical_projection";
    const char * const TEXCOORD_UV_MAP          = "uv_map";
    const char * const TEXCOORD_REFLECTIVE      = "reflective";
    const char * const TEXCOORD_SPATIAL         = "spatial_projection"; // cinemax modes, do not
    const char * const TEXCOORD_SHRINKMAP       = "shrinkmap_projection"; // know how to map in opengl

    enum TexCoordMapping {
        SPHERICAL_PROJECTION,
        CYLINDRICAL_PROJECTION,
        PLANAR_PROJECTION,
        CUBE_PROJECTION,
        FRONTAL_PROJECTION,
        SPATIAL_PROJECTION,
        UV_MAP,
        REFLECTIVE,
        SHRINKMAP_PROJECTION,
        PERSPECTIVE_PROJECTION
    };

    const char * const TexCoordMappingStrings[] = {
        TEXCOORD_SPHERICAL,
        TEXCOORD_CYLINDRICAL,
        TEXCOORD_PLANAR,
        TEXCOORD_CUBE,
        TEXCOORD_FRONTAL,
        TEXCOORD_SPATIAL,
        TEXCOORD_UV_MAP,
        TEXCOORD_REFLECTIVE,
        TEXCOORD_SHRINKMAP,
        TEXCOORD_PERSPECTIVE,
        0
    };

    enum TexCoordMode {
        OBJECT_LINEAR,
        EYE_LINEAR,
        SPHERE_MAP,
        REFLECTION,
        NONE
    };

    //=== Image Resize Modes ==================================================
    const char * const IMAGE_RESIZE_NONE  = "none";
    const char * const IMAGE_RESIZE_SCALE = "scale";
    const char * const IMAGE_RESIZE_PAD   = "pad";

    //=== Image Types =========================================================
    const char * const IMAGE_TYPE_SINGLE  = "single";
    const char * const IMAGE_TYPE_VOXEL   = "voxel";
    const char * const IMAGE_TYPE_CUBEMAP = "cubemap";

    enum ImageTypeEnum {
        SINGLE,
        VOXEL,
        CUBEMAP,
        ImageTypeEnum_MAX
    };

    const char * const ImageTypeStrings[] = {
        IMAGE_TYPE_SINGLE,
        IMAGE_TYPE_VOXEL,
        IMAGE_TYPE_CUBEMAP,
        ""
    };
    DEFINE_ENUM(ImageType, ImageTypeEnum, Y60_BASE_DECL);

    //=== Texture usage =====================================================
    const char * const TEXTURE_USAGE_PAINT        = "paint";
    const char * const TEXTURE_USAGE_HDR_PAINT    = "hdr_paint";
    const char * const TEXTURE_USAGE_BUMP         = "bump";
    const char * const TEXTURE_USAGE_SKYBOX       = "skybox";
    const char * const TEXTURE_USAGE_ENVIRONMENT  = "environment";
    const char * const TEXTURE_USAGE_DISPLACEMENT = "displacement";
    const char * const TEXTURE_USAGE_EMISSIVE     = "emissive";
    const char * const TEXTURE_USAGE_OCCLUSION    = "occlusion";
    const char * const TEXTURE_USAGE_GLOW         = "glow";
    const char * const TEXTURE_USAGE_FLOW         = "flow";
    const char * const TEXTURE_USAGE_SPLAT        = "splat";
    const char * const TEXTURE_USAGE_BLUR         = "blur";
    const char * const TEXTURE_USAGE_FADE         = "fade";

    enum TextureUsageEnum {
        PAINT,
        HDR_PAINT,
        BUMP,
        SKYBOX,
        ENVIRONMENT,
        DISPLACEMENT,
        EMISSIVE,
        OCCLUSION,
        GLOW,
        FLOW,
        SPLAT,
        BLUR,
        FADE,
        TextureUsageEnum_MAX
    };

    const char * const TextureUsageStrings[] = {
        TEXTURE_USAGE_PAINT,
        TEXTURE_USAGE_HDR_PAINT,
        TEXTURE_USAGE_BUMP,
        TEXTURE_USAGE_SKYBOX,
        TEXTURE_USAGE_ENVIRONMENT,
        TEXTURE_USAGE_DISPLACEMENT,
        TEXTURE_USAGE_EMISSIVE,
        TEXTURE_USAGE_OCCLUSION,
        TEXTURE_USAGE_GLOW,
        TEXTURE_USAGE_FLOW,
        TEXTURE_USAGE_SPLAT,
        TEXTURE_USAGE_BLUR,
        TEXTURE_USAGE_FADE,
        ""
    };
    DEFINE_ENUM(TextureUsage, TextureUsageEnum, Y60_BASE_DECL);
    typedef std::vector<TextureUsageEnum> VectorOfTextureUsage;

    //=== Image filter =====================================================
    const char * const IMAGE_FILTER_NONE                = "";
    const char * const IMAGE_FILTER_HEIGHT_TO_NORMALMAP = "heightToNormal";

    enum ImageFilter {
        NO_FILTER,
        HEIGHT_TO_NORMALMAP
    };

    const char * const ImageFilterStrings[] = {
        IMAGE_FILTER_NONE,
        IMAGE_FILTER_HEIGHT_TO_NORMALMAP,
        0
    };

    //=== Texture type =====================================================
    const char * const TEXTURE_TYPE_2D        = "texture_2d";
    const char * const TEXTURE_TYPE_RECTANGLE = "texture_rectangle";
    const char * const TEXTURE_TYPE_3D        = "texture_3d";
    const char * const TEXTURE_TYPE_CUBEMAP   = "texture_cubemap";

    enum TextureTypeEnum {
        TEXTURE_2D,
        TEXTURE_RECTANGLE,
        TEXTURE_3D,
        TEXTURE_CUBEMAP,
        TextureTypeEnum_MAX
    };

    const char * const TextureTypeStrings[] = {
        TEXTURE_TYPE_2D,
        TEXTURE_TYPE_RECTANGLE,
        TEXTURE_TYPE_3D,
        TEXTURE_TYPE_CUBEMAP,
        ""
    };
    DEFINE_ENUM(TextureType, TextureTypeEnum, Y60_BASE_DECL);

    enum CubemapFacesEnum {
        CUBEMAP_BEHIND,
        CUBEMAP_RIGHT,
        CUBEMAP_FRONT,
        CUBEMAP_LEFT,
        CUBEMAP_TOP,
        CUBEMAP_BOTTOM
    };


    //=== Light Souce types =====================================================

    const char * const DIRECTIONAL_LIGHT = "directional";
    const char * const POSITIONAL_LIGHT  = "positional";
    const char * const SPOT_LIGHT        = "spot";
    const char * const AMBIENT_LIGHT     = "ambient";
    const char * const UNSUPPORTED_LIGHT = "unsupported";

    enum LightSourceType {
        DIRECTIONAL, POSITIONAL, SPOT, AMBIENT, UNSUPPORTED, MAX_LIGHTSOURCE_TYPE
    };

    const char * const LightSourceTypeString[] = {
        DIRECTIONAL_LIGHT,
        POSITIONAL_LIGHT,
        SPOT_LIGHT,
        AMBIENT_LIGHT,
        UNSUPPORTED_LIGHT,
        0
    };

    //=== Renderstyle types =====================================================
    enum RenderStyleEnum {
        FRONT,
        BACK,
        BOUNDING_VOLUME,
        IGNORE_DEPTH,
        NO_DEPTH_WRITES,
        POLYGON_OFFSET,
        RenderStyleEnum_MAX
    };

    const char * const RenderStyleStrings[] = {
       "frontfacing",
       "backfacing",
       "bounding_volume",
       "ignore_depth",
       "no_depth_writes",
       "polygon_offset",
       ""
    };
    DEFINE_BITSET(RenderStyles, RenderStyle, RenderStyleEnum, Y60_BASE_DECL);

    enum BlendFunctionEnum {
        ONE,
        ZERO,
        DST_COLOR,
        SRC_COLOR,
        ONE_MINUS_DST_COLOR,
        ONE_MINUS_SRC_COLOR,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
        SRC_ALPHA_SATURATE,
        CONSTANT_COLOR,
        ONE_MINUS_CONSTANT_COLOR,
        CONSTANT_ALPHA,
        ONE_MINUS_CONSTANT_ALPHA,
        BlendFunctionEnum_MAX
    };

    const char * const BlendFunctionStrings[] = {
        "one",
        "zero",
        "dst_color",
        "src_color",
        "one_minus_dst_color",
        "one_minus_src_color",
        "src_alpha",
        "one_minus_src_alpha",
        "dst_alpha",
        "one_minus_dst_alpha",
        "src_alpha_saturate",
        "constant_color",
        "one_minus_constant_color",
        "constant_alpha",
        "one_minus_constant_alpha",
        ""
    };
    DEFINE_ENUM(BlendFunction, BlendFunctionEnum, Y60_BASE_DECL);

    // same as GL_POINTS, GL_LINES etc.
    enum PrimitiveTypeEnum {
        POINTS         = 0,
        LINES          = 1,
        LINE_LOOP      = 2,
        LINE_STRIP     = 3,
        TRIANGLES      = 4,
        TRIANGLE_STRIP = 5,
        TRIANGLE_FAN   = 6,
        QUADS          = 7,
        QUAD_STRIP     = 8,
        POLYGON        = 9,
        PrimitiveTypeEnum_MAX
    };

    const char * const PRIMITIVE_TYPE_POINTS          = "points";
    const char * const PRIMITIVE_TYPE_LINES           = "lines";
    const char * const PRIMITIVE_TYPE_LINE_STRIP      = "linestrip";
    const char * const PRIMITIVE_TYPE_LINE_LOOP       = "lineloop";
    const char * const PRIMITIVE_TYPE_TRIANGLES       = "triangles";
    const char * const PRIMITIVE_TYPE_TRIANGLE_STRIP  = "trianglestrip";
    const char * const PRIMITIVE_TYPE_TRIANGLE_FAN    = "trianglefan";
    const char * const PRIMITIVE_TYPE_QUADS           = "quads";
    const char * const PRIMITIVE_TYPE_QUAD_STRIP      = "quadstrip";
    const char * const PRIMITIVE_TYPE_POLYGON         = "polygon";

    const char * const PrimitiveTypeStrings[] = {
        PRIMITIVE_TYPE_POINTS,
        PRIMITIVE_TYPE_LINES,
        PRIMITIVE_TYPE_LINE_LOOP,
        PRIMITIVE_TYPE_LINE_STRIP,
        PRIMITIVE_TYPE_TRIANGLES,
        PRIMITIVE_TYPE_TRIANGLE_STRIP,
        PRIMITIVE_TYPE_TRIANGLE_FAN,
        PRIMITIVE_TYPE_QUADS,
        PRIMITIVE_TYPE_QUAD_STRIP,
        PRIMITIVE_TYPE_POLYGON,
        ""
    };
    DEFINE_ENUM(PrimitiveType, PrimitiveTypeEnum, Y60_BASE_DECL);

    enum TargetBuffersEnum {
        RED_MASK,
        GREEN_MASK,
        BLUE_MASK,
        ALPHA_MASK,
        DEPTH_MASK,
        TargetBuffersEnum_MAX
    };

    const char * const TargetBuffersStrings[] = {
        "red",
        "green",
        "blue",
        "alpha",
        "depth",
        ""
    };

    enum BlendEquationEnum {
        EQUATION_MIN,
        EQUATION_MAX,
        EQUATION_ADD,
        EQUATION_SUBTRACT,
        EQUATION_REVERSE_SUBTRACT,
        BlendEquationEnum_MAX
    };

    const char * const BlendEquationStrings[] = {
        "min",
        "max",
        "add",
        "subtract",
        "reverse_subtract",
        ""
    };

    DEFINE_BITSET(TargetBuffers, TargetBuffer, TargetBuffersEnum, Y60_BASE_DECL);
    DEFINE_ENUM(BlendEquation, BlendEquationEnum, Y60_BASE_DECL);

    //=== Animation types ===============================================
    enum AnimationDirection {
        FORWARD, REVERSE, PONG
    };

    const char * const AnimationDirectionString[] = {
        "forward",
        "reverse",
        "pong",
        0
    };

    //=== Fog types =====================================================
    enum FogMode {
        FOG_OFF = 0,
        FOG_LINEAR,
        FOG_EXP,
        FOG_EXP2
    };

    const char * const FogModeStrings[] = {
        "off",
        "linear",
        "exp",
        "exp2",
        0
    };

    //=== Movie play modes =============================================
    enum MoviePlayMode {
        PLAY_MODE_NODISK = 0,      ///< No disk inserted or no movie found.
        PLAY_MODE_PLAY,            ///< Playing
        PLAY_MODE_PAUSE,           ///< Paused. Play will continue at the current time/frame
        PLAY_MODE_STOP             ///< Stopped. Play will start at beginnig of the movie
    };

    const char * const MoviePlayModeStrings[] = {
        "nodisk",
        "play",
        "pause",
        "stop",
        0
    };

    // === VBO usage ====================================================
    enum VertexBufferUsageEnum {
        VERTEX_USAGE_UNDEFINED = 0,
        VERTEX_USAGE_DISABLED = 1,
        VERTEX_USAGE_STREAM_DRAW,
        VERTEX_USAGE_STREAM_READ,
        VERTEX_USAGE_STREAM_COPY,
        VERTEX_USAGE_STATIC_DRAW,
        VERTEX_USAGE_STATIC_READ,
        VERTEX_USAGE_STATIC_COPY,
        VERTEX_USAGE_DYNAMIC_DRAW,
        VERTEX_USAGE_DYNAMIC_READ,
        VERTEX_USAGE_DYNAMIC_COPY,
        VertexBufferUsageEnum_MAX
    };

    //Y60_VBO_USAGE env variable
    const char * const VertexBufferUsageStrings[]= {
            "undefined",
            "disabled",
            "stream_draw",
            "stream_read",
            "stream_copy",
            "static_draw",
            "static_read",
            "static_copy",
            "dynamic_draw",
            "dynamic_read",
            "dynamic_copy",
            ""
    };
    DEFINE_ENUM(VertexBufferUsage, VertexBufferUsageEnum, Y60_BASE_DECL);

    Y60_BASE_DECL VertexBufferUsage getDefaultVertexBufferUsage();

}

#endif
