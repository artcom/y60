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

#include "y60_base_settings.h"

#include <string>

namespace y60 {

    //=== Som basic type names ==============================================================
    const char * const SOM_VECTOR_BLEND_FUNCTION_NAME = "vectorofblendfunction";
    const char * const SOM_VECTOR_BLEND_EQUATION_NAME = "vectorofblendequation";
    const char * const SOM_VECTOR_BOOL_NAME     = "vectorofbool";
    const char * const SOM_VECTOR_FLOAT_NAME    = "vectoroffloat";
    const char * const SOM_VECTOR_UNSIGNED_NAME = "vectorofunsigend";
    const char * const SOM_VECTOR_STRING_NAME   = "vectorofstring";
    const char * const SOM_VECTOR_QUATERNIONF_NAME  = "vectorofquaternionf";
    const char * const SOM_VECTOR_VECTOR2F_NAME = "vectorofvector2f";
    const char * const SOM_VECTOR_VECTOR3F_NAME = "vectorofvector3f";
    const char * const SOM_VECTOR_VECTOR4F_NAME = "vectorofvector4f";
    const char * const SOM_STRING_NAME          = "string";


    //=== Node names ==============================================================
    const char * const SCENE_ROOT_NAME           = "scene";
    const char * const SHAPE_LIST_NAME           = "shapes";
    const char * const SHAPE_NODE_NAME           = "shape";
    const char * const VERTEX_NODE_NAME          = "vertex";
    const char * const VERTEX_DATA_NAME          = "vertexdata";
    const char * const PRIMITIVE_LIST_NAME       = "primitives";
    const char * const ELEMENTS_NODE_NAME        = "elements";
    const char * const VERTEX_INDICES_NAME       = "indices";
    const char * const HALFEDGES_NODE_NAME       = "halfedges";
    const char * const OVERLAY_LIST_NAME         = "overlays";
    const char * const UNDERLAY_LIST_NAME        = "underlays";
    const char * const OVERLAY_NODE_NAME         = "overlay";
    const char * const MATERIAL_LIST_NAME        = "materials";
    const char * const MATERIAL_NODE_NAME        = "material";
    const char * const SHADER_LIST_NAME          = "shaders";
    const char * const SHADER_NODE_NAME          = "shader";
    const char * const PARAMETER_LIST_NAME       = "vertexparameters";
    const char * const PARAMETER_NODE_NAME       = "value";
    const char * const PROPERTY_LIST_NAME        = "properties";
    const char * const REQUIRES_LIST_NAME        = "requires";
    const char * const FEATURE_NODE_NAME         = "feature";
    const char * const WORLD_LIST_NAME           = "worlds";
    const char * const WORLD_NODE_NAME           = "world";
    const char * const TRANSFORM_NODE_NAME       = "transform";
    const char * const INCLUDE_NODE_NAME         = "include";
    const char * const INCLUDE_SRC_ATTRIB        = "src";
    const char * const LOD_NODE_NAME             = "lod";
    const char * const JOINT_NODE_NAME           = "joint";
    const char * const CAMERA_NODE_NAME          = "camera";
    const char * const PROJECTOR_NODE_NAME       = "projector";
    const char * const BODY_NODE_NAME            = "body";
    const char * const VERTEX_SHADER_NODE_NAME   = "vertexshader";
    const char * const FRAGMENT_SHADER_NODE_NAME = "fragmentshader";
    const char * const FIXED_FUNCTION_SHADER_NODE_NAME = "fixedfunctionshader";
    const char * const LIGHTSOURCE_LIST_NAME     = "lightsources";
    const char * const LIGHTSOURCE_NODE_NAME     = "lightsource";
    const char * const LIGHT_NODE_NAME           = "light";
    const char * const SPOTLIGHT_NODE_NAME       = "spot";
    const char * const ANIMATION_LIST_NAME       = "animations";
    const char * const ANIMATION_NODE_NAME       = "animation";
    const char * const CHARACTER_LIST_NAME       = "characters";
    const char * const CHARACTER_NODE_NAME       = "character";
    const char * const CLIP_NODE_NAME            = "clip";
    const char * const CANVAS_LIST_NAME          = "canvases";
    const char * const CANVAS_NODE_NAME          = "canvas";
    const char * const VIEWPORT_NODE_NAME        = "viewport";
    const char * const RECORD_LIST_NAME          = "records";
    const char * const RECORD_NODE_NAME          = "record";

    //=== Analytic Geometry ==========================================================
    const char * const POINT_NODE_NAME        = "point";
    const char * const VECTOR_NODE_NAME       = "vector";

    /*
    const char * const LINE_NODE_NAME         = "line";
    const char * const RAY_NODE_NAME          = "ray";
    */
    const char * const LINE_SEGMENT_NODE_NAME = "linesegment";

    const char * const PLANE_NODE_NAME        = "plane";

    bool isAnalyticGeometry(const std::string & theNodeName );

    //=== Attribute names ============================================================
    const char * const ID_ATTRIB                   = "id";
    const char * const REVISION_ATTRIB             = "revision";
    const char * const NAME_ATTRIB                 = "name";
    const char * const VISIBLE_ATTRIB              = "visible";
    const char * const WIDTH_ATTRIB                = "width";
    const char * const HEIGHT_ATTRIB               = "height";
    const char * const DEPTH_ATTRIB                = "depth";
    const char * const STICKY_ATTRIB               = "sticky";

    const char * const MATERIAL_REF_ATTRIB         = "material";
    const char * const PARAM_TYPE_ATTRIB           = "type";
    const char * const PROPERTY_TYPE_ATTRIB        = "type";
    const char * const VERTEX_DATA_ATTRIB          = "vertexdata";
    const char * const VERTEX_DATA_ROLE_ATTRIB     = "role";
    const char * const SKYBOX_MATERIAL_ATTRIB      = "skyboxmaterial";

    const char * const LODRANGES_ATTRIB            = "ranges";
    const char * const LODSCALE_ATTRIB             = "lodscale";

    const char * const FOGMODE_ATTRIB              = "fogmode"; // "off" | "linear" | "exp" || "exp2"
    const char * const FOGCOLOR_ATTRIB             = "fogcolor";
    const char * const FOGRANGE_ATTRIB             = "fogrange";
    const char * const FOGDENSITY_ATTRIB           = "fogdensity";

    const char * const PRIMITIVE_TYPE_ATTRIB          = "type";

    const char * const VERTEX_BUFFER_USAGE_ATTRIB     = "vertexbufferusage";

    const char * const TEXTUREUNIT_LIST_NAME     = "textureunits";
    const char * const TEXTUREUNIT_NODE_NAME     = "textureunit";
    const char * const TEXTUREUNIT_TEXTURE_ATTRIB   = "texture";
    const char * const TEXTUREUNIT_APPLYMODE_ATTRIB = "applymode";
    const char * const TEXTUREUNIT_COMBINER_ATTRIB  = "combiner";
    const char * const TEXTUREUNIT_ENVCOLOR_ATTRIB  = "envcolor";
    const char * const TEXTUREUNIT_SPRITE_ATTRIB    = "sprite";
    const char * const TEXTUREUNIT_PROJECTOR_ATTRIB = "projector";

    const char * const LIGHTSOURCE_TYPE_ATTRIB     = "type";
    const char * const BODY_SHAPE_ATTRIB           = "shape";
    const char * const CULLABLE_ATTRIB             = "cullable";
    const char * const POSITION_ATTRIB             = "position";
    const char * const SCALE_ATTRIB                = "scale";
    const char * const ORIENTATION_ATTRIB          = "orientation";
    const char * const JOINT_ORIENTATION_ATTRIB    = "jointorientation";
    const char * const PIVOT_ATTRIB                = "pivot";
    const char * const VIEW_DIRECTION_ATTRIB       = "viewdirection";
    const char * const EYE_POINT_ATTRIB            = "eyepoint";
    const char * const FUNCTION_ATTRIB             = "function";
    const char * const CLASS_ATTRIB                = "class";
    const char * const VALUES_ATTRIB               = "values";
    const char * const COST_ATTRIB                 = "cost";
    /*
    const char * const HORIZONTAL_FOV_ATTRIB       = "hfov";
    const char * const ORTHO_WIDTH_ATTRIB          = "width";
    const char * const FAR_PLANE_ATTRIB            = "farplane";
    const char * const NEAR_PLANE_ATTRIB           = "nearplane";
    */
    const char * const CAMERA_ATTRIB               = "camera";
    const char * const LIGHT_SOURCE_ATTRIB         = "lightsource";
    const char * const SPOTLIGHT_CUTOFF_ATTRIB     = "cutoff";
    const char * const SPOTLIGHT_EXPONENT_ATTRIB   = "exponent";
    const char * const BOUNDING_BOX_ATTRIB         = "boundingbox";
    const char * const GLOBAL_MATRIX_ATTRIB        = "globalmatrix";
    const char * const INVERSE_GLOBAL_MATRIX_ATTRIB = "inverseglobalmatrix";
    const char * const LOCAL_MATRIX_ATTRIB         = "localmatrix";
    const char * const RENDER_STYLE_ATTRIB         = "renderstyle";
    const char * const BILLBOARD_ATTRIB            = "billboard";
    const char * const TRANSPARENCY_ATTRIB         = "transparent";
    const char * const DEPTH_TEST_ATTRIB           = "depthtest";
    const char * const INSENSIBLE_ATTRIB           = "insensible";
    const char * const SKELETON_ATTRIB             = "skeleton";
    const char * const CLIPPING_PLANES_ATTRIB      = "clippingplanes";
    const char * const SCISSOR_ATTRIB              = "scissor";

    const char * const ANIM_ENABLED_ATTRIB         = "enabled";
    const char * const ANIM_ATTRIBUTE_ATTRIB       = "attribute";
    const char * const ANIM_PROPERTY_ATTRIB        = "property";
    const char * const ANIM_NODEREF_ATTRIB         = "node";
    const char * const ANIM_BEGIN_ATTRIB           = "begin";
    const char * const ANIM_DURATION_ATTRIB        = "duration";
    const char * const ANIM_CLIPIN_ATTRIB          = "clipin";
    const char * const ANIM_CLIPOUT_ATTRIB         = "clipout";
    const char * const ANIM_PAUSE_ATTRIB           = "pause";
    const char * const ANIM_COUNT_ATTRIB           = "count";
    const char * const ANIM_DIRECTION_ATTRIB       = "direction";

    const char * const CANVAS_BACKGROUNDCOLOR_ATTRIB = "backgroundcolor";
    const char * const CANVAS_TARGET_ATTRIB          = "target";

    const char * const VIEWPORT_SIZE_ATTRIB        = "size"; // normalized
    const char * const VIEWPORT_WIDTH_ATTRIB       = "width";  // in pixels
    const char * const VIEWPORT_HEIGHT_ATTRIB      = "height"; // in pixels
    const char * const VIEWPORT_LEFT_ATTRIB        = "left";   // in pixels
    const char * const VIEWPORT_TOP_ATTRIB         = "top";    // in pixels
    const char * const VIEWPORT_BACKFACECULLING_ATTRIB = "backfaceculling";
    const char * const VIEWPORT_WIREFRAME_ATTRIB   = "wireframe";
    const char * const VIEWPORT_DRAWNORMALS_ATTRIB = "drawnormals";
    const char * const VIEWPORT_LIGHTING_ATTRIB    = "lighting";
    const char * const VIEWPORT_FLATSHADING_ATTRIB = "flatshading";
    const char * const VIEWPORT_TEXTURING_ATTRIB   = "texturing";
    const char * const VIEWPORT_GLOW_ATTRIB        = "glow";
    const char * const VIEWPORT_CULLING_ATTRIB     = "culling";
    const char * const VIEWPORT_DEBUGCULLING_ATTRIB= "debugculling";
    const char * const VIEWPORT_ALPHATEST_ATTRIB   = "alphatest";

    const char * const PROJECTIONMATRIX_ATTRIB     = "projectionmatrix";
    const char * const FRUSTUM_ATTRIB              = "frustum";
    const char * const RESIZE_POLICY_ATTRIB        = "resizepolicy";

    const char * const OVERLAY_VISIBLE_ATTRIB      = "visible";
    const char * const OVERLAY_ALPHA_ATTRIB        = "alpha";
    const char * const OVERLAY_POSITION_ATTRIB     = "position";
    const char * const OVERLAY_SRCORIGIN_ATTRIB    = "srcorigin";
    const char * const OVERLAY_SRCSIZE_ATTRIB      = "srcsize";
    const char * const OVERLAY_BORDERCOLOR_ATTRIB  = "bordercolor";
    const char * const OVERLAY_BORDERWIDTH_ATTRIB  = "borderwidth";
    const char * const OVERLAY_TOPBORDER_ATTRIB    = "topborder";
    const char * const OVERLAY_BOTTOMBORDER_ATTRIB = "bottomborder";
    const char * const OVERLAY_RIGHTBORDER_ATTRIB  = "rightborder";
    const char * const OVERLAY_LEFTBORDER_ATTRIB   = "leftborder";
    const char * const OVERLAY_ROTATION2D_ATTRIB   = "rotation";
    const char * const OVERLAY_SCALE2D_ATTRIB      = "scale";
    const char * const OVERLAY_PIVOT2D_ATTRIB      = "pivot";

    const char * const MATRIX_ATTRIB               = "matrix";

    const char * const TEXTURE_LIST_NAME              = "textures";
    const char * const TEXTURE_NODE_NAME              = "texture";
    const char * const TEXTURE_WRAPMODE_ATTRIB        = "wrapmode";
    const char * const TEXTURE_MIN_FILTER_ATTRIB      = "min_filter";
    const char * const TEXTURE_MAG_FILTER_ATTRIB      = "mag_filter";
    const char * const TEXTURE_IMAGE_ATTRIB           = "image";
    const char * const TEXTURE_TYPE_ATTRIB            = "type";
    const char * const TEXTURE_INTERNAL_FORMAT_ATTRIB = "internal_format";
    const char * const TEXTURE_ID_ATTRIB              = "textureid";
    const char * const TEXTURE_COLOR_SCALE_ATTRIB     = "color_scale";
    const char * const TEXTURE_COLOR_BIAS_ATTRIB      = "color_bias";
    const char * const TEXTURE_MIPMAP_ATTRIB          = "mipmap";
    const char * const TEXTURE_ANISOTROPY_ATTRIB      = "anisotropy";
    const char * const TEXTURE_PIXELFORMAT_ATTRIB     = "texturepixelformat";
    const char * const TEXTURE_IMAGE_INDEX            = "image_index";

    const char * const IMAGE_LIST_NAME                 = "images";
    const char * const IMAGE_NODE_NAME                 = "image";
    const char * const IMAGE_SRC_ATTRIB                = "src";
    const char * const IMAGE_TYPE_ATTRIB               = "type";
    const char * const IMAGE_BYTESPERPIXEL_ATTRIB      = "bytesperpixel";
    const char * const IMAGE_RASTER_PIXELFORMAT_ATTRIB = "rasterpixelformat";
    const char * const IMAGE_TARGET_PIXELFORMAT_ATTRIB = "targetpixelformat";
    const char * const IMAGE_RESIZE_ATTRIB             = "resize";
    const char * const IMAGE_FILTER_ATTRIB             = "filter";
    const char * const IMAGE_FILTER_PARAMS_ATTRIB      = "filter_params";
    const char * const IMAGE_TILE_ATTRIB               = "tile";
    const char * const IMAGE_RASTER_ATTRIB             = "raster"; // for easy raster access

    const char * const MOVIE_NODE_NAME             = "movie";
    const char * const MOVIE_FRAME_ATTRIB          = "currentframe";
    const char * const MOVIE_FRAME_CACHE_ATTRIB    = "cachesize";
    const char * const MOVIE_FRAME_MAX_CACHE_ATTRIB = "maxcachesize";
    const char * const MOVIE_AVDELAY_ATTRIB        = "avdelay";
    const char * const MOVIE_FRAMERATE_ATTRIB      = "fps";
    const char * const MOVIE_FRAMECOUNT_ATTRIB     = "framecount";
    const char * const MOVIE_ASPECT_RATIO_ATTRIB   = "aspectratio";    
    const char * const MOVIE_HASAUDIO_ATTRIB       = "has_audio";

    const char * const MOVIE_PLAYSPEED_ATTRIB      = "playspeed";
    const char * const MOVIE_PLAYMODE_ATTRIB       = "playmode";
    const char * const MOVIE_VOLUME_ATTRIB         = "volume";
    const char * const MOVIE_LOOPCOUNT_ATTRIB      = "loopcount";
    const char * const MOVIE_AUDIO_ATTRIB          = "audio";
    const char * const MOVIE_DECODERHINT_ATTRIB    = "decoderhint";
    const char * const MOVIE_MOVIETIME_ATTRIB      = "movietime";
    const char * const MOVIE_DECODER_ATTRIB        = "decoder";

    const char * const CAPTURE_NODE_NAME           = "capture";
    const char * const CAPTURE_NORM_ATTRIB         = "norm";
    const char * const CAPTURE_DEVICE_ATTRIB       = "device";

    const char * const GEOMETRY_COLOR_ATTRIB        = "color";
    const char * const GEOMETRY_LINE_WIDTH_ATTRIB   = "linewidth";
    const char * const GEOMETRY_POINT_SIZE_ATTRIB   = "pointsize";
    const char * const GEOMETRY_VALUE_ATTRIB        = "value";
    const char * const GEOMETRY_GLOBAL_VALUE_ATTRIB = "globalvalue";

    //=== Attribute constants ============================================================
    const char * const POSITION_ROLE               = "position";
    const char * const NORMAL_ROLE                 = "normal";
    const char * const COLOR_ROLE                  = "color";
    const char * const TEXCOORD0_ROLE              = "texcoord0";
    const char * const TEXCOORD1_ROLE              = "texcoord1";
    const char * const TEXCOORD2_ROLE              = "texcoord2";
    const char * const TEXCOORD3_ROLE              = "texcoord3";
    const char * const TEXCOORD4_ROLE              = "texcoord4";
    const char * const TEXCOORD5_ROLE              = "texcoord5";
    const char * const TEXCOORD6_ROLE              = "texcoord6";
    const char * const TEXCOORD7_ROLE              = "texcoord7";
    const char * const JOINT_WEIGHTS_ROLE_0        = "joint_weights_0";
    const char * const JOINT_INDICES_ROLE_0        = "joint_indices_0";
    const char * const JOINT_WEIGHTS_ROLE_1        = "joint_weights_1";
    const char * const JOINT_INDICES_ROLE_1        = "joint_indices_1";

    //=== Screen orientation constants ====================================================
    const char * const LANDSCAPE_ORIENTATION       = "landscape";
    const char * const PORTRAIT_ORIENTATION        = "portrait";

    //=== Billboard orientation constants ====================================================
    const char * const AXIS_BILLBOARD              = "axis";
    const char * const POINT_BILLBOARD             = "point";
}
#endif
