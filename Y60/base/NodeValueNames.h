//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_NodeValueNames_h_
#define _ac_NodeValueNames_h_

#include <asl/Enum.h>

namespace y60 {

    //=== Lighting Model ==========================================================
    enum LightingModel {
        BLINN, PHONG, GOURAUD, LAMBERT, FLAT, UNLIT, MAX_LIGHTING_MODEL
    };
    static const char * LIGHTING_MODEL_BLINN = "blinn";
    static const char * LIGHTING_MODEL_PHONG = "phong";
    static const char * LIGHTING_MODEL_GOURAUD = "gouraud";
    static const char * LIGHTING_MODEL_LAMBERT = "lambert";
    static const char * LIGHTING_MODEL_FLAT = "flat";
    static const char * LIGHTING_MODEL_UNLIT = "unlit";

    static const char * LightingModelString[] = {
        LIGHTING_MODEL_BLINN,
        LIGHTING_MODEL_PHONG,
        LIGHTING_MODEL_GOURAUD,
        LIGHTING_MODEL_LAMBERT,
        LIGHTING_MODEL_FLAT,
        LIGHTING_MODEL_UNLIT,
        0
    };

    static const char * REGISTER_ATTRIB            = "register";

    //=== Material feature names ==================================================
    static const char * TEXCOORD_FEATURE           = "texcoord";
    static const char * LIGHTING_FEATURE           = "lighting";
    static const char * TEXTURE_FEATURE            = "textures";
    static const char * MAPPING_FEATURE            = "texcoord";
    static const char * FEATURE_DROPPED            = "drop_feature";

     //=== Cg specific property name ===============================================
    static const char * CG_FILE_PROPERTY           = "file";
    static const char * CG_PROFILE_PROPERTY        = "profile";
    static const char * CG_ENTRY_FUNCTION_PROPERTY = "entryfunction";
    static const char * CG_COMPILERARGS_PROPERTY   = "compilerargs";

    //=== VertexParameter names ===============================================
    static const char * VERTEXPARAM_PASS_THROUGH   = "pass_through";
    static const char * VERTEXPARAM_TANGENT        = "tangent";

    //=== Texture apply modes =====================================================
    static const char * TEXTURE_APPLY_MODULATE   = "modulate";
    static const char * TEXTURE_APPLY_DECAL      = "decal";
    static const char * TEXTURE_APPLY_REPLACE    = "replace";
    static const char * TEXTURE_APPLY_BLEND      = "blend";
    static const char * TEXTURE_APPLY_ADD        = "add";

    enum TextureApplyMode {
        MODULATE,
        DECAL,
        REPLACE,
        BLEND,
        ADD
    };

    static const char * TextureApplyModeStrings[] = {
        TEXTURE_APPLY_MODULATE,
        TEXTURE_APPLY_DECAL,
        TEXTURE_APPLY_REPLACE,
        TEXTURE_APPLY_BLEND,
        TEXTURE_APPLY_ADD,
        0
    };

    //=== Texture magnification filters =====================================================
    static const char * TEXTURE_SAMPLE_FILTER_NEAREST       = "nearest";
    static const char * TEXTURE_SAMPLE_FILTER_LINEAR        = "linear";

    enum TextureSampleFilter {
        NEAREST,
        LINEAR
    };

    static const char * TextureSampleFilterStrings[] = {
        TEXTURE_SAMPLE_FILTER_NEAREST,
        TEXTURE_SAMPLE_FILTER_LINEAR,
        0
    };
    //=== Texture repeat modes =====================================================
    static const char * TEXTURE_WRAP_CLAMP           = "clamp";
    static const char * TEXTURE_WRAP_CLAMP_TO_EDGE   = "clamp_to_edge";
    static const char * TEXTURE_WRAP_REPEAT          = "repeat";

    enum TextureWrapMode {
        CLAMP,
        CLAMP_TO_EDGE,
        REPEAT
    };

    static const char * TextureWrapModeStrings[] = {
        TEXTURE_WRAP_CLAMP,
        TEXTURE_WRAP_CLAMP_TO_EDGE,
        TEXTURE_WRAP_REPEAT,
        0
    };

    //=== Texture Internal Formats ============================================
    static const char * TEXTURE_INTERNAL_FORMAT_DEPTH               = "DEPTH";
    static const char * TEXTURE_INTERNAL_FORMAT_ALPHA               = "ALPHA";
    static const char * TEXTURE_INTERNAL_FORMAT_ALPHA4              = "ALPHA4";
    static const char * TEXTURE_INTERNAL_FORMAT_ALPHA8              = "ALPHA8";
    static const char * TEXTURE_INTERNAL_FORMAT_ALPHA12             = "ALPHA12";
    static const char * TEXTURE_INTERNAL_FORMAT_ALPHA16             = "ALPHA16";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE           = "LUMINANCE";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE4          = "LUMINANCE4";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE8          = "LUMINANCE8";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE12         = "LUMINANCE12";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE16         = "LUMINANCE16";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE_ALPHA     = "LUMINANCE_ALPHA";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE4_ALPHA4   = "LUMINANCE4_ALPHA4";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE6_ALPHA2   = "LUMINANCE6_ALPHA2";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE8_ALPHA8   = "LUMINANCE8_ALPHA8";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE12_ALPHA4  = "LUMINANCE12_ALPHA4";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE12_ALPHA12 = "LUMINANCE12_ALPHA12";
    static const char * TEXTURE_INTERNAL_FORMAT_LUMINANCE16_ALPHA16 = "LUMINANCE16_ALPHA16";
    static const char * TEXTURE_INTERNAL_FORMAT_INTENSITY           = "INTENSITY";
    static const char * TEXTURE_INTERNAL_FORMAT_INTENSITY4          = "INTENSITY4";
    static const char * TEXTURE_INTERNAL_FORMAT_INTENSITY8          = "INTENSITY8";
    static const char * TEXTURE_INTERNAL_FORMAT_INTENSITY12         = "INTENSITY12";
    static const char * TEXTURE_INTERNAL_FORMAT_INTENSITY16         = "INTENSITY16";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB                 = "RGB";
    static const char * TEXTURE_INTERNAL_FORMAT_R3_G3_B2            = "R3_G3_B2";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB4                = "RGB4";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB5                = "RGB5";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB8                = "RGB8";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB10               = "RGB10";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB12               = "RGB12";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB16               = "RGB16";
    static const char * TEXTURE_INTERNAL_FORMAT_RGBA                = "RGBA";
    static const char * TEXTURE_INTERNAL_FORMAT_RGBA2               = "RGBA2";
    static const char * TEXTURE_INTERNAL_FORMAT_RGBA4               = "RGBA4";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB5_A1             = "RGB5_A1";
    static const char * TEXTURE_INTERNAL_FORMAT_RGBA8               = "RGBA8";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB10_A2            = "RGB10_A2";
    static const char * TEXTURE_INTERNAL_FORMAT_RGBA12              = "RGBA12";
    static const char * TEXTURE_INTERNAL_FORMAT_RGBA16              = "RGBA16";
    static const char * TEXTURE_INTERNAL_FORMAT_RGBA_HALF           = "RGBA_HALF";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB_HALF            = "RGB_HALF";
    static const char * TEXTURE_INTERNAL_FORMAT_RGBA_FLOAT          = "RGBA_FLOAT";
    static const char * TEXTURE_INTERNAL_FORMAT_RGB_FLOAT           = "RGB_FLOAT";
    static const char * TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGB_ARB  = "COMPRESSED_RGB_ARB";
    static const char * TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_ARB             = "COMPRESSED_RGBA_ARB";
    static const char * TEXTURE_INTERNAL_FORMAT_COMPRESSED_ALPHA_ARB            = "COMPRESSED_ALPHA_ARB";
    static const char * TEXTURE_INTERNAL_FORMAT_COMPRESSED_LUMINANCE_ARB        = "COMPRESSED_LUMINANCE_ARB";
    static const char * TEXTURE_INTERNAL_FORMAT_COMPRESSED_LUMINANCE_ALPHA_ARB  = "COMPRESSED_LUMINANCE_ALPHA_ARB";
    static const char * TEXTURE_INTERNAL_FORMAT_COMPRESSED_INTENSITY_ARB        = "COMPRESSED_INTENSITY_ARB";
    static const char * TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGB_S3TC_DXT1_EXT    = "COMPRESSED_RGB_S3TC_DXT1_EXT";
    static const char * TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_S3TC_DXT1_EXT   = "COMPRESSED_RGBA_S3TC_DXT1_EXT";
    static const char * TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_S3TC_DXT3_EXT   = "COMPRESSED_RGBA_S3TC_DXT3_EXT";
    static const char * TEXTURE_INTERNAL_FORMAT_COMPRESSED_RGBA_S3TC_DXT5_EXT   = "COMPRESSED_RGBA_S3TC_DXT5_EXT";


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
    };

    static const char * TextureInternalFormatStrings[] = {
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
        0
    };

    //=== Automatic Texture Coord Generation Modes NEW ============================
    static const char * TEXCOORD_PLANAR          = "planar_projection";
    static const char * TEXCOORD_CUBE            = "cube_projection";
    static const char * TEXCOORD_FRONTAL         = "frontal_projection";
    static const char * TEXCOORD_PERSPECTIVE     = "perspective_projection";
    static const char * TEXCOORD_SPHERICAL       = "spherical_projection";
    static const char * TEXCOORD_CYLINDRICAL     = "cylindrical_projection";
    static const char * TEXCOORD_UV_MAP          = "uv_map";
    static const char * TEXCOORD_REFLECTIVE      = "reflective";
    static const char * TEXCOORD_SPATIAL         = "spatial_projection"; // cinemax modes, do not
    static const char * TEXCOORD_SHRINKMAP       = "shrinkmap_projection"; // know how to map in opengl

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

    static const char * TexCoordMappingStrings[] = {
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
    static const char * IMAGE_RESIZE_NONE  = "none";
    static const char * IMAGE_RESIZE_SCALE = "scale";
    static const char * IMAGE_RESIZE_PAD   = "pad";

    //=== Texture usage =====================================================
    static const char * TEXTURE_USAGE_PAINT        = "paint";
    static const char * TEXTURE_USAGE_BUMP         = "bump";
    static const char * TEXTURE_USAGE_SKYBOX       = "skybox";
    static const char * TEXTURE_USAGE_ENVIRONMENT  = "environment";
    static const char * TEXTURE_USAGE_DISPLACEMENT = "displacement";
    static const char * TEXTURE_USAGE_EMISSIVE     = "emissive";

    enum TextureUsage {
        PAINT,
        BUMP,
        SKYBOX,
        ENVIRONMENT,
        DISPLACEMENT,
        EMISSIVE
    };


    static const char * TextureUsageStrings[] = {
        TEXTURE_USAGE_PAINT,
        TEXTURE_USAGE_BUMP,
        TEXTURE_USAGE_SKYBOX,
        TEXTURE_USAGE_ENVIRONMENT,
        TEXTURE_USAGE_DISPLACEMENT,
        TEXTURE_USAGE_EMISSIVE,
        0
    };

    //=== Image filter =====================================================
    static const char * IMAGE_FILTER_NONE                = "";
    static const char * IMAGE_FILTER_HEIGHT_TO_NORMALMAP = "heightToNormal";

    enum ImageFilter {
        NO_FILTER,
        HEIGHT_TO_NORMALMAP
    };

    static const char * ImageFilterStrings[] = {
        IMAGE_FILTER_NONE,
        IMAGE_FILTER_HEIGHT_TO_NORMALMAP,
        0
    };

    //=== Texture type =====================================================
    static const char * IMAGE_TYPE_SINGLE        = "single";
    static const char * IMAGE_TYPE_CUBEMAP       = "cubemap";

    enum ImageType {
        SINGLE,
        CUBEMAP
    };

    static const char * ImageTypeStrings[] = {
        IMAGE_TYPE_SINGLE,
        IMAGE_TYPE_CUBEMAP,
        0
    };

    //=== Light Souce types =====================================================

    static const char * DIRECTIONAL_LIGHT = "directional";
    static const char * POSITIONAL_LIGHT  = "positional";
    static const char * SPOT_LIGHT        = "spot";
    static const char * AMBIENT_LIGHT     = "ambient";
    static const char * UNSUPPORTED_LIGHT = "unsupported";

    enum LightSourceType {
        DIRECTIONAL, POSITIONAL, SPOT, AMBIENT, UNSUPPORTED, MAX_LIGHTSOURCE_TYPE
    };

    static const char * LightSourceTypeString[] = {
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

    static const char * RenderStyleStrings[] = {
       "frontfacing",     
       "backfacing",      
       "bounding_volume",
       "ignore_depth", 
       "no_depth_writes",
       "polygon_offset", 
       "" 
    };
    DEFINE_BITSET(RenderStyles, RenderStyle, RenderStyleEnum);

    enum BlendFunction {
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
        ONE_MINUS_CONSTANT_ALPHA
    };

    static const char * BlendFunctionStrings[] = {
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
        0
    };
    
    enum TargetBuffersEnum {
        RED_MASK,
        GREEN_MASK,
        BLUE_MASK,
        ALPHA_MASK,
        DEPTH_MASK,
        TargetBuffersEnum_MAX
    };

    static const char * TargetBuffersStrings[] = {
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

    static const char * BlendEquationStrings[] = {
        "min",
        "max",
        "add",
        "subtract",
        "reverse_subtract",
        ""
    };

    DEFINE_BITSET(TargetBuffers, TargetBuffer, TargetBuffersEnum);
    DEFINE_ENUM(BlendEquation, BlendEquationEnum);

    //=== Animation types ===============================================
    enum AnimationDirection {
        FORWARD, REVERSE, PONG
    };

    static const char * AnimationDirectionString[] = {
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

    static const char * FogModeStrings[] = {
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

    static const char * MoviePlayModeStrings[] = {
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
    static const char * VertexBufferUsageStrings[]= {
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
    DEFINE_ENUM(VertexBufferUsage, VertexBufferUsageEnum);   
}

#endif
