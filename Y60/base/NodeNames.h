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
//   $Id: NodeNames.h,v 1.7 2005/04/26 08:58:32 thomas Exp $
//   $RCSfile: NodeNames.h,v $
//   $Author: thomas $
//   $Revision: 1.7 $
//   $Date: 2005/04/26 08:58:32 $
//
//
//
//  Description: This file contains all the wellknown names for the
//               SOM
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_y60_NodeNames_h_
#define _ac_y60_NodeNames_h_

namespace y60 {

    //=== Som basic type names ==============================================================
    static const char * SOM_VECTOR_BOOL_NAME     = "vectorofbool";
    static const char * SOM_VECTOR_FLOAT_NAME    = "vectoroffloat";
    static const char * SOM_VECTOR_UNSIGNED_NAME = "vectorofunsigend";
    static const char * SOM_VECTOR_STRING_NAME   = "vectorofstring";
    static const char * SOM_VECTOR_QUATERNIONF_NAME  = "vectorofquaternionf";
    static const char * SOM_VECTOR_VECTOR2F_NAME = "vectorofvector2f";
    static const char * SOM_VECTOR_VECTOR3F_NAME = "vectorofvector3f";
    static const char * SOM_VECTOR_VECTOR4F_NAME = "vectorofvector4f";


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
    static const char * OVERLAY_NODE_NAME         = "overlay";
    static const char * MATERIAL_LIST_NAME        = "materials";
    static const char * MATERIAL_NODE_NAME        = "material";
    static const char * SHADER_LIST_NAME          = "shaders";
    static const char * SHADER_NODE_NAME          = "shader";
    static const char * PARAMETER_LIST_NAME       = "vertexparameters";
    static const char * PARAMETER_NODE_NAME       = "value";
    static const char * PROPERTY_LIST_NAME        = "properties";
    static const char * REQUIRES_LIST_NAME        = "requires";
    static const char * TEXTURE_LIST_NAME         = "textures";
    static const char * FEATURE_NODE_NAME         = "feature";
    static const char * TEXTURE_NODE_NAME         = "texture";
    static const char * WORLD_LIST_NAME           = "worlds";
    static const char * WORLD_NODE_NAME           = "world";
    static const char * TRANSFORM_NODE_NAME       = "transform";
    static const char * INCLUDE_NODE_NAME         = "include";
    static const char * INCLUDE_SRC_ATTRIB        = "src";
    static const char * LOD_NODE_NAME             = "lod";
    static const char * JOINT_NODE_NAME           = "joint";
    static const char * CAMERA_NODE_NAME          = "camera";
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

    //=== Attribute names ============================================================
    static const char * ID_ATTRIB                   = "id";
    static const char * REVISION_ATTRIB             = "revision";
    static const char * NAME_ATTRIB                 = "name";
    static const char * VISIBLE_ATTRIB              = "visible";
    static const char * FROZEN_ATTRIB               = "frozen";
    static const char * WIDTH_ATTRIB                = "width";
    static const char * HEIGHT_ATTRIB               = "height";
    static const char * DEPTH_ATTRIB                = "depth";

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
    static const char * PRIMITIVE_TYPE_POINTS          = "points";
    static const char * PRIMITIVE_TYPE_LINES           = "lines";
    static const char * PRIMITIVE_TYPE_LINE_STRIP      = "linestrip";
    static const char * PRIMITIVE_TYPE_LINE_LOOP       = "lineloop";
    static const char * PRIMITIVE_TYPE_TRIANGLES       = "triangles";
    static const char * PRIMITIVE_TYPE_TRIANGLE_STRIP  = "trianglestrip";
    static const char * PRIMITIVE_TYPE_TRIANGLE_FAN    = "trianglefan";
    static const char * PRIMITIVE_TYPE_QUADS           = "quads";
    static const char * PRIMITIVE_TYPE_QUAD_STRIP      = "quadstrip";
    static const char * PRIMITIVE_TYPE_POLYGON         = "polygon";

	static const char * AUTOTEXCOORDMODES_ATTRIB    = "coordgenmodes";
    static const char * AUTOTEXCOORDPARAMS_ATTRIB   = "coordgenparams"; 
    static const char * TEXTURE_APPLYMODE_ATTRIB    = "applymode";
    static const char * TEXTURE_WRAPMODE_ATTRIB     = "wrapmode";
    static const char * TEXTURE_IMAGE_ATTRIB        = "image";
    static const char * TEXTURE_SPRITE_ATTRIB       = "sprite";
    static const char * TEXTURE_MATRIX_ATTRIB       = "matrix";
    static const char * LIGHTSOURCE_TYPE_ATTRIB     = "type";
    static const char * BODY_SHAPE_ATTRIB           = "shape";
    static const char * CULLABLE_ATTRIB             = "cullable";
    static const char * POSITION_ATTRIB             = "position";
    static const char * SCALE_ATTRIB                = "scale";
    static const char * ORIENTATION_ATTRIB          = "orientation";
    static const char * JOINT_ORIENTATION_ATTRIB    = "jointorientation";
    static const char * SHEAR_ATTRIB                = "shear";
    static const char * PIVOT_ATTRIB                = "pivot";
    static const char * VIEW_DIRECTION_ATTRIB       = "viewdirection";
    static const char * EYE_POINT_ATTRIB            = "eyepoint";
    static const char * FUNCTION_ATTRIB             = "function";
    static const char * CLASS_ATTRIB                = "class";
    static const char * VALUES_ATTRIB               = "values";
    static const char * COST_ATTRIB                 = "cost";
    static const char * HORIZONTAL_FOV_ATTRIB       = "hfov";
    static const char * ORTHO_WIDTH_ATTRIB          = "width";
    static const char * FAR_PLANE_ATTRIB            = "farplane";
    static const char * NEAR_PLANE_ATTRIB           = "nearplane";
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
    static const char * SENSOR_RADIUS_ATTRIB        = "sensorradius";
    static const char * SENSOR_ATTRIB               = "sensor";
    static const char * TRANSPARENCY_ATTRIB         = "transparent";
    static const char * WRITE_DEPTH_BUFFER_ATTRIB   = "writedepthbuffer";
    static const char * INSENSIBLE_ATTRIB           = "insensible";
    static const char * SKELETON_ATTRIB             = "skeleton";
    static const char * PIVOT_TRANSLATION_ATTRIB    = "pivottranslation";

    static const char * ANIM_ENABLED_ATTRIB         = "enabled";
    static const char * ANIM_ATTRIBUTE_ATTRIB       = "attribute";
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
    static const char * VIEWPORT_CULLING_ATTRIB     = "culling";
    static const char * VIEWPORT_DEBUGCULLING_ATTRIB= "debugculling";
    static const char * VIEWPORT_FRUSTUM_ATTRIB     = "frustum";
    static const char * PROJECTIONMATRIX_ATTRIB     = "projectionmatrix";

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

    static const char * IMAGE_LIST_NAME                   = "images";
    static const char * IMAGE_NODE_NAME                   = "image";
    static const char * IMAGE_SRC_ATTRIB                  = "src";
    static const char * IMAGE_PIXELFORMAT_ATTRIB          = "pixelformat";
    static const char * IMAGE_INTERNAL_FORMAT_ATTRIB      = "internal_format";
    static const char * IMAGE_MIPMAP_ATTRIB               = "mipmap";
    static const char * IMAGE_TYPE_ATTRIB                 = "type"; // "single" | "cubemap", see NodeValueNames.h
    static const char * IMAGE_RESIZE_ATTRIB               = "resize"; // "scale" || "pad"
    static const char * IMAGE_FILTER_ATTRIB               = "filter";
    static const char * IMAGE_FILTER_PARAMS_ATTRIB        = "filter_params";
    static const char * IMAGE_MATRIX_ATTRIB               = "matrix";
    static const char * IMAGE_TILE_ATTRIB                 = "tile";
    static const char * IMAGE_COLOR_SCALE_ATTRIB          = "color_scale";
    static const char * IMAGE_COLOR_BIAS_ATTRIB           = "color_bias";
    static const char * IMAGE_BYTESPERPIXEL_ATTRIB        = "bytesperpixel";

    static const char * MOVIE_NODE_NAME             = "movie";
    static const char * MOVIE_FRAME_ATTRIB          = "currentframe";
    static const char * MOVIE_FRAME_CACHE_ATTRIB    = "cachesize";
    static const char * MOVIE_AVDELAY_ATTRIB        = "avdelay";
    static const char * MOVIE_FRAMECOUNT_ATTRIB     = "framecount";
    static const char * MOVIE_FRAMERATE_ATTRIB      = "fps";
    static const char * MOVIE_PLAYSPEED_ATTRIB      = "playspeed";
    static const char * MOVIE_PLAYMODE_ATTRIB       = "playmode";
    static const char * MOVIE_VOLUME_ATTRIB         = "volume";
    static const char * MOVIE_LOOPCOUNT_ATTRIB      = "loopcount";
    static const char * MOVIE_AUDIO_ATTRIB          = "audio";

    static const char * CAPTURE_NODE_NAME           = "capture";
    static const char * CAPTURE_NORM_ATTRIB         = "norm";
    static const char * CAPTURE_DEVICE_ATTRIB       = "device";

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

