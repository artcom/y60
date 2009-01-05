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

#ifndef _ac_y60_NodeNames_h_
#define _ac_y60_NodeNames_h_

#include <string>

namespace y60 {

    //=== Som basic type names ==============================================================
    static const char * SOM_VECTOR_BLEND_FUNCTION_NAME = "vectorofblendfunction";
    static const char * SOM_VECTOR_BOOL_NAME     = "vectorofbool";
    static const char * SOM_VECTOR_FLOAT_NAME    = "vectoroffloat";
    static const char * SOM_VECTOR_UNSIGNED_NAME = "vectorofunsigend";
    static const char * SOM_VECTOR_STRING_NAME   = "vectorofstring";
    static const char * SOM_VECTOR_QUATERNIONF_NAME  = "vectorofquaternionf";
    static const char * SOM_VECTOR_VECTOR2F_NAME = "vectorofvector2f";
    static const char * SOM_VECTOR_VECTOR3F_NAME = "vectorofvector3f";
    static const char * SOM_VECTOR_VECTOR4F_NAME = "vectorofvector4f";
    static const char * SOM_STRING_NAME          = "string";


    //=== Node names ==============================================================
    static const char * SCENE_ROOT_NAME           = "scene";
    static const char * SHAPE_LIST_NAME           = "shapes";
    static const char * SHAPE_NODE_NAME           = "shape";
    static const char * VERTEX_NODE_NAME          = "vertex";
    static const char * VERTEX_DATA_NAME          = "vertexdata";
    static const char * PRIMITIVE_LIST_NAME       = "primitives";
    static const char * ELEMENTS_NODE_NAME        = "elements";
    static const char * VERTEX_INDICES_NAME       = "indices";
    static const char * HALFEDGES_NODE_NAME       = "halfedges";
    static const char * OVERLAY_LIST_NAME         = "overlays";
    static const char * UNDERLAY_LIST_NAME        = "underlays";
    static const char * OVERLAY_NODE_NAME         = "overlay";
    static const char * MATERIAL_LIST_NAME        = "materials";
    static const char * MATERIAL_NODE_NAME        = "material";
    static const char * SHADER_LIST_NAME          = "shaders";
    static const char * SHADER_NODE_NAME          = "shader";
    static const char * PARAMETER_LIST_NAME       = "vertexparameters";
    static const char * PARAMETER_NODE_NAME       = "value";
    static const char * PROPERTY_LIST_NAME        = "properties";
    static const char * REQUIRES_LIST_NAME        = "requires";
    static const char * FEATURE_NODE_NAME         = "feature";
    static const char * WORLD_LIST_NAME           = "worlds";
    static const char * WORLD_NODE_NAME           = "world";
    static const char * TRANSFORM_NODE_NAME       = "transform";
    static const char * INCLUDE_NODE_NAME         = "include";
    static const char * INCLUDE_SRC_ATTRIB        = "src";
    static const char * LOD_NODE_NAME             = "lod";
    static const char * JOINT_NODE_NAME           = "joint";
    static const char * CAMERA_NODE_NAME          = "camera";
    static const char * PROJECTOR_NODE_NAME       = "projector";
    static const char * BODY_NODE_NAME            = "body";
    static const char * VERTEX_SHADER_NODE_NAME   = "vertexshader";
    static const char * FRAGMENT_SHADER_NODE_NAME = "fragmentshader";
    static const char * FIXED_FUNCTION_SHADER_NODE_NAME = "fixedfunctionshader";
    static const char * LIGHTSOURCE_LIST_NAME     = "lightsources";
    static const char * LIGHTSOURCE_NODE_NAME     = "lightsource";
    static const char * LIGHT_NODE_NAME           = "light";
    static const char * SPOTLIGHT_NODE_NAME       = "spot";
    static const char * ANIMATION_LIST_NAME       = "animations";
    static const char * ANIMATION_NODE_NAME       = "animation";
    static const char * CHARACTER_LIST_NAME       = "characters";
    static const char * CHARACTER_NODE_NAME       = "character";
    static const char * CLIP_NODE_NAME            = "clip";
    static const char * CANVAS_LIST_NAME          = "canvases";
    static const char * CANVAS_NODE_NAME          = "canvas";
    static const char * VIEWPORT_NODE_NAME        = "viewport";
    static const char * RECORD_LIST_NAME          = "records";
    static const char * RECORD_NODE_NAME          = "record";

    //=== Analytic Geometry ==========================================================
    static const char * POINT_NODE_NAME        = "point";
    static const char * VECTOR_NODE_NAME       = "vector";

    /*
    static const char * LINE_NODE_NAME         = "line";
    static const char * RAY_NODE_NAME          = "ray";
    */
    static const char * LINE_SEGMENT_NODE_NAME = "linesegment";

    static const char * PLANE_NODE_NAME        = "plane";

    bool isAnalyticGeometry(const std::string & theNodeName );

    //=== Attribute names ============================================================
    static const char * ID_ATTRIB                   = "id";
    static const char * REVISION_ATTRIB             = "revision";
    static const char * NAME_ATTRIB                 = "name";
    static const char * VISIBLE_ATTRIB              = "visible";
    static const char * WIDTH_ATTRIB                = "width";
    static const char * HEIGHT_ATTRIB               = "height";
    static const char * DEPTH_ATTRIB                = "depth";
    static const char * STICKY_ATTRIB               = "sticky";

    static const char * MATERIAL_REF_ATTRIB         = "material";
    static const char * PARAM_TYPE_ATTRIB           = "type";
    static const char * PROPERTY_TYPE_ATTRIB        = "type";
    static const char * VERTEX_DATA_ATTRIB          = "vertexdata";
    static const char * VERTEX_DATA_ROLE_ATTRIB     = "role";
    static const char * SKYBOX_MATERIAL_ATTRIB      = "skyboxmaterial";

    static const char * LODRANGES_ATTRIB            = "ranges";
    static const char * LODSCALE_ATTRIB             = "lodscale";

    static const char * FOGMODE_ATTRIB              = "fogmode"; // "off" | "linear" | "exp" || "exp2"
    static const char * FOGCOLOR_ATTRIB             = "fogcolor";
    static const char * FOGRANGE_ATTRIB             = "fogrange";
    static const char * FOGDENSITY_ATTRIB           = "fogdensity";

    static const char * PRIMITIVE_TYPE_ATTRIB          = "type";

    static const char * VERTEX_BUFFER_USAGE_ATTRIB     = "vertexbufferusage";
 
    static const char * TEXTUREUNIT_LIST_NAME     = "textureunits";
    static const char * TEXTUREUNIT_NODE_NAME     = "textureunit";
    static const char * TEXTUREUNIT_TEXTURE_ATTRIB   = "texture";
    static const char * TEXTUREUNIT_APPLYMODE_ATTRIB = "applymode";
    static const char * TEXTUREUNIT_COMBINER_ATTRIB  = "combiner";
    static const char * TEXTUREUNIT_ENVCOLOR_ATTRIB  = "envcolor";
    static const char * TEXTUREUNIT_SPRITE_ATTRIB    = "sprite";
    static const char * TEXTUREUNIT_PROJECTOR_ATTRIB = "projector";

    static const char * LIGHTSOURCE_TYPE_ATTRIB     = "type";
    static const char * BODY_SHAPE_ATTRIB           = "shape";
    static const char * CULLABLE_ATTRIB             = "cullable";
    static const char * POSITION_ATTRIB             = "position";
    static const char * SCALE_ATTRIB                = "scale";
    static const char * ORIENTATION_ATTRIB          = "orientation";
    static const char * JOINT_ORIENTATION_ATTRIB    = "jointorientation";
    static const char * PIVOT_ATTRIB                = "pivot";
    static const char * VIEW_DIRECTION_ATTRIB       = "viewdirection";
    static const char * EYE_POINT_ATTRIB            = "eyepoint";
    static const char * FUNCTION_ATTRIB             = "function";
    static const char * CLASS_ATTRIB                = "class";
    static const char * VALUES_ATTRIB               = "values";
    static const char * COST_ATTRIB                 = "cost";
    /*
    static const char * HORIZONTAL_FOV_ATTRIB       = "hfov";
    static const char * ORTHO_WIDTH_ATTRIB          = "width";
    static const char * FAR_PLANE_ATTRIB            = "farplane";
    static const char * NEAR_PLANE_ATTRIB           = "nearplane";
    */
    static const char * CAMERA_ATTRIB               = "camera";
    static const char * LIGHT_SOURCE_ATTRIB         = "lightsource";
    static const char * SPOTLIGHT_CUTOFF_ATTRIB     = "cutoff";
    static const char * SPOTLIGHT_EXPONENT_ATTRIB   = "exponent";
    static const char * BOUNDING_BOX_ATTRIB         = "boundingbox";
    static const char * GLOBAL_MATRIX_ATTRIB        = "globalmatrix";
    static const char * INVERSE_GLOBAL_MATRIX_ATTRIB = "inverseglobalmatrix";
    static const char * LOCAL_MATRIX_ATTRIB         = "localmatrix";
    static const char * RENDER_STYLE_ATTRIB         = "renderstyle";
    static const char * BILLBOARD_ATTRIB            = "billboard";
    static const char * TRANSPARENCY_ATTRIB         = "transparent";
    static const char * DEPTH_TEST_ATTRIB           = "depthtest";
    static const char * INSENSIBLE_ATTRIB           = "insensible";
    static const char * SKELETON_ATTRIB             = "skeleton";
    static const char * CLIPPING_PLANES_ATTRIB      = "clippingplanes";
    static const char * SCISSOR_ATTRIB              = "scissor";

    static const char * ANIM_ENABLED_ATTRIB         = "enabled";
    static const char * ANIM_ATTRIBUTE_ATTRIB       = "attribute";
    static const char * ANIM_PROPERTY_ATTRIB        = "property";
    static const char * ANIM_NODEREF_ATTRIB         = "node";
    static const char * ANIM_BEGIN_ATTRIB           = "begin";
    static const char * ANIM_DURATION_ATTRIB        = "duration";
    static const char * ANIM_CLIPIN_ATTRIB          = "clipin";
    static const char * ANIM_CLIPOUT_ATTRIB         = "clipout";
    static const char * ANIM_PAUSE_ATTRIB           = "pause";
    static const char * ANIM_COUNT_ATTRIB           = "count";
    static const char * ANIM_DIRECTION_ATTRIB       = "direction";

    static const char * CANVAS_BACKGROUNDCOLOR_ATTRIB = "backgroundcolor";
    static const char * CANVAS_TARGET_ATTRIB          = "target";

    static const char * VIEWPORT_SIZE_ATTRIB        = "size"; // normalized
    static const char * VIEWPORT_WIDTH_ATTRIB       = "width";  // in pixels
    static const char * VIEWPORT_HEIGHT_ATTRIB      = "height"; // in pixels
    static const char * VIEWPORT_LEFT_ATTRIB        = "left";   // in pixels
    static const char * VIEWPORT_TOP_ATTRIB         = "top";    // in pixels
    static const char * VIEWPORT_BACKFACECULLING_ATTRIB = "backfaceculling";
    static const char * VIEWPORT_WIREFRAME_ATTRIB   = "wireframe";
    static const char * VIEWPORT_DRAWNORMALS_ATTRIB = "drawnormals";
    static const char * VIEWPORT_LIGHTING_ATTRIB    = "lighting";
    static const char * VIEWPORT_FLATSHADING_ATTRIB = "flatshading";
    static const char * VIEWPORT_TEXTURING_ATTRIB   = "texturing";
    static const char * VIEWPORT_GLOW_ATTRIB        = "glow";
    static const char * VIEWPORT_CULLING_ATTRIB     = "culling";
    static const char * VIEWPORT_DEBUGCULLING_ATTRIB= "debugculling";
    static const char * VIEWPORT_ALPHATEST_ATTRIB   = "alphatest";

    static const char * PROJECTIONMATRIX_ATTRIB     = "projectionmatrix";
    static const char * FRUSTUM_ATTRIB              = "frustum";
    static const char * RESIZE_POLICY_ATTRIB        = "resizepolicy";

    static const char * OVERLAY_VISIBLE_ATTRIB      = "visible";
    static const char * OVERLAY_ALPHA_ATTRIB        = "alpha";
    static const char * OVERLAY_POSITION_ATTRIB     = "position";
    static const char * OVERLAY_SRCORIGIN_ATTRIB    = "srcorigin";
    static const char * OVERLAY_SRCSIZE_ATTRIB      = "srcsize";
    static const char * OVERLAY_BORDERCOLOR_ATTRIB  = "bordercolor";
    static const char * OVERLAY_BORDERWIDTH_ATTRIB  = "borderwidth";
    static const char * OVERLAY_TOPBORDER_ATTRIB    = "topborder";
    static const char * OVERLAY_BOTTOMBORDER_ATTRIB = "bottomborder";
    static const char * OVERLAY_RIGHTBORDER_ATTRIB  = "rightborder";
    static const char * OVERLAY_LEFTBORDER_ATTRIB   = "leftborder";
    static const char * OVERLAY_ROTATION2D_ATTRIB   = "rotation";
    static const char * OVERLAY_SCALE2D_ATTRIB      = "scale";
    static const char * OVERLAY_PIVOT2D_ATTRIB      = "pivot";

    static const char * MATRIX_ATTRIB               = "matrix";

    static const char * TEXTURE_LIST_NAME              = "textures";
    static const char * TEXTURE_NODE_NAME              = "texture";
    static const char * TEXTURE_WRAPMODE_ATTRIB        = "wrapmode";
    static const char * TEXTURE_MIN_FILTER_ATTRIB      = "min_filter";
    static const char * TEXTURE_MAG_FILTER_ATTRIB      = "mag_filter";
    static const char * TEXTURE_IMAGE_ATTRIB           = "image";
    static const char * TEXTURE_TYPE_ATTRIB            = "type";
    static const char * TEXTURE_INTERNAL_FORMAT_ATTRIB = "internal_format";
    static const char * TEXTURE_ID_ATTRIB              = "textureid";
    static const char * TEXTURE_COLOR_SCALE_ATTRIB     = "color_scale";
    static const char * TEXTURE_COLOR_BIAS_ATTRIB      = "color_bias";
    static const char * TEXTURE_MIPMAP_ATTRIB          = "mipmap";
    static const char * TEXTURE_ANISOTROPY_ATTRIB      = "anisotropy";
    static const char * TEXTURE_PIXELFORMAT_ATTRIB     = "texturepixelformat";
    static const char * TEXTURE_IMAGE_INDEX            = "image_index";

    static const char * IMAGE_LIST_NAME                 = "images";
    static const char * IMAGE_NODE_NAME                 = "image";
    static const char * IMAGE_SRC_ATTRIB                = "src";
    static const char * IMAGE_TYPE_ATTRIB               = "type";
    static const char * IMAGE_BYTESPERPIXEL_ATTRIB      = "bytesperpixel";
    static const char * IMAGE_RASTER_PIXELFORMAT_ATTRIB = "rasterpixelformat";
    static const char * IMAGE_TARGET_PIXELFORMAT_ATTRIB = "targetpixelformat";    
    static const char * IMAGE_RESIZE_ATTRIB             = "resize";
    static const char * IMAGE_FILTER_ATTRIB             = "filter";
    static const char * IMAGE_FILTER_PARAMS_ATTRIB      = "filter_params";
    static const char * IMAGE_TILE_ATTRIB               = "tile";
    static const char * IMAGE_RASTER_ATTRIB             = "raster"; // for easy raster access

    static const char * MOVIE_NODE_NAME             = "movie";
    static const char * MOVIE_FRAME_ATTRIB          = "currentframe";
    static const char * MOVIE_FRAME_CACHE_ATTRIB    = "cachesize";
    static const char * MOVIE_FRAME_MAX_CACHE_ATTRIB = "maxcachesize";
    static const char * MOVIE_AVDELAY_ATTRIB        = "avdelay";
    static const char * MOVIE_FRAMERATE_ATTRIB      = "fps";
    static const char * MOVIE_FRAMECOUNT_ATTRIB     = "framecount";
    static const char * MOVIE_FRAMEBLENDFACTOR_ATTRIB = "frameblend_factor";	
    static const char * MOVIE_FRAMEBLENDING_ATTRIB  = "frameblending";	
	
    static const char * MOVIE_PLAYSPEED_ATTRIB      = "playspeed";
    static const char * MOVIE_PLAYMODE_ATTRIB       = "playmode";
    static const char * MOVIE_VOLUME_ATTRIB         = "volume";
    static const char * MOVIE_LOOPCOUNT_ATTRIB      = "loopcount";
    static const char * MOVIE_AUDIO_ATTRIB          = "audio";
    static const char * MOVIE_DECODERHINT_ATTRIB    = "decoderhint";
    static const char * MOVIE_MOVIETIME_ATTRIB      = "movietime";
    static const char * MOVIE_DECODER_ATTRIB        = "decoder";

    static const char * CAPTURE_NODE_NAME           = "capture";
    static const char * CAPTURE_NORM_ATTRIB         = "norm";
    static const char * CAPTURE_DEVICE_ATTRIB       = "device";

    static const char * GEOMETRY_COLOR_ATTRIB        = "color";
    static const char * GEOMETRY_LINE_WIDTH_ATTRIB   = "linewidth";
    static const char * GEOMETRY_POINT_SIZE_ATTRIB   = "pointsize";
    static const char * GEOMETRY_VALUE_ATTRIB        = "value";
    static const char * GEOMETRY_GLOBAL_VALUE_ATTRIB = "globalvalue";

    //=== Attribute constants ============================================================
    static const char * POSITION_ROLE               = "position";
    static const char * NORMAL_ROLE                 = "normal";
    static const char * COLOR_ROLE                  = "color";
    static const char * TEXCOORD0_ROLE              = "texcoord0";
    static const char * TEXCOORD1_ROLE              = "texcoord1";
    static const char * TEXCOORD2_ROLE              = "texcoord2";
    static const char * TEXCOORD3_ROLE              = "texcoord3";
    static const char * TEXCOORD4_ROLE              = "texcoord4";
    static const char * TEXCOORD5_ROLE              = "texcoord5";
    static const char * TEXCOORD6_ROLE              = "texcoord6";
    static const char * TEXCOORD7_ROLE              = "texcoord7";
    static const char * JOINT_WEIGHTS_ROLE_0        = "joint_weights_0";
    static const char * JOINT_INDICES_ROLE_0        = "joint_indices_0";
    static const char * JOINT_WEIGHTS_ROLE_1        = "joint_weights_1";
    static const char * JOINT_INDICES_ROLE_1        = "joint_indices_1";

    //=== Screen orientation constants ====================================================
    static const char * LANDSCAPE_ORIENTATION       = "landscape";
    static const char * PORTRAIT_ORIENTATION        = "portrait";

    //=== Billboard orientation constants ====================================================
    static const char * AXIS_BILLBOARD              = "axis";
    static const char * POINT_BILLBOARD             = "point";
}
#endif