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

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/Documentation.h>

#include "JSAssimpLoader.h"
#include <assimp/aiPostProcess.h>

namespace y60 {
    using namespace jslib;

    static JSClass Package = {
        "Package",
        JSCLASS_HAS_PRIVATE,
        JS_PropertyStub, JS_PropertyStub,
        JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,
        JS_ConvertStub, JS_FinalizeStub
    };

    class JSAssimpPlugin : public asl::PlugInBase, public jslib::IScriptablePlugin {
        public:
            enum PropertyNumbers {
                CALC_TANGENTSPACE, JOIN_IDENTICAL_VERTICES, MAKE_LEFTHANDED, TRIANGULATE,
                REMOVE_COMPONENT, GEN_NORMALS, GEN_SMOOTH_NORMALS, SPLIT_LARGE_MESHES,
                PRETRANSFORM_VERTICES, LIMIT_BONE_WEIGHTS, VALIDATE_DATASTRUCTURE, IMPROVE_CACHE_lOCALITY,
                REMOVE_REDUNDANT_MATERIALS, FIX_INFACING_NORMALS, SORT_BY_PTYPE, FIND_DEGENERATES,
                FIND_INVALID_DATA, GEN_UV_COORDS, TRANSFORM_UV_COORDS, FIND_INSTANCES,
                OPTIMIZE_MESHES, OPTIMIZE_GRAPH, FLIP_UVS, FLIP_WINDING_ORDER, 
                // PRESETS
                PRESET_TARGET_REALTIME_FAST, PRESET_TARGET_REALTIME_QUALITY, PRESET_TARGET_REALTIME_MAX_QUALITY,
                PROP_END
            };
                


            JSAssimpPlugin(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}
            
            // static properties
            JSConstIntPropertySpec * ConstIntProperties() {
                static JSConstIntPropertySpec myProperties[] = {
                    {"CALC_TANGENTSPACE",                  CALC_TANGENTSPACE,                  aiProcess_CalcTangentSpace},
                    {"JOIN_IDENTICAL_VERTICES",            JOIN_IDENTICAL_VERTICES,            aiProcess_JoinIdenticalVertices},
                    {"MAKE_LEFTHANDED",                    MAKE_LEFTHANDED,                    aiProcess_MakeLeftHanded},
                    {"TRIANGULATE",                        TRIANGULATE,                        aiProcess_Triangulate},
                    {"REMOVE_COMPONENT",                   REMOVE_COMPONENT,                   aiProcess_RemoveComponent},
                    {"GEN_NORMALS",                        GEN_NORMALS,                        aiProcess_GenNormals},
                    {"GEN_SMOOTH_NORMALS",                 GEN_SMOOTH_NORMALS,                 aiProcess_GenSmoothNormals},
                    {"SPLIT_LARGE_MESHES",                 SPLIT_LARGE_MESHES,                 aiProcess_SplitLargeMeshes},
                    {"PRETRANSFORM_VERTICES",              PRETRANSFORM_VERTICES,              aiProcess_PreTransformVertices},
                    {"LIMIT_BONE_WEIGHTS",                 LIMIT_BONE_WEIGHTS,                 aiProcess_LimitBoneWeights},
                    {"VALIDATE_DATASTRUCTURE",             VALIDATE_DATASTRUCTURE,             aiProcess_ValidateDataStructure},
                    {"IMPROVE_CACHE_lOCALITY",             IMPROVE_CACHE_lOCALITY,             aiProcess_ImproveCacheLocality},
                    {"REMOVE_REDUNDANT_MATERIALS",         REMOVE_REDUNDANT_MATERIALS,         aiProcess_RemoveRedundantMaterials},
                    {"FIX_INFACING_NORMALS",               FIX_INFACING_NORMALS,               aiProcess_FixInfacingNormals},
                    {"SORT_BY_PTYPE",                      SORT_BY_PTYPE,                      aiProcess_SortByPType},
                    {"FIND_DEGENERATES",                   FIND_DEGENERATES,                   aiProcess_FindDegenerates},
                    {"FIND_INVALID_DATA",                  FIND_INVALID_DATA,                  aiProcess_FindInvalidData},
                    {"GEN_UV_COORDS",                      GEN_UV_COORDS,                      aiProcess_GenUVCoords},
                    {"TRANSFORM_UV_COORDS",                TRANSFORM_UV_COORDS,                aiProcess_TransformUVCoords},
                    {"FIND_INSTANCES",                     FIND_INSTANCES,                     aiProcess_FindInstances},
                    {"OPTIMIZE_MESHES",                    OPTIMIZE_MESHES,                    aiProcess_OptimizeMeshes},
                    {"OPTIMIZE_GRAPH",                     OPTIMIZE_GRAPH,                     aiProcess_OptimizeGraph},
                    {"FLIP_UVS",                           FLIP_UVS,                           aiProcess_FlipUVs},
                    {"FLIP_WINDING_ORDER",                 FLIP_WINDING_ORDER,                 aiProcess_FlipWindingOrder},
                    {"PRESET_TARGET_REALTIME_FAST",        PRESET_TARGET_REALTIME_FAST,        aiProcessPreset_TargetRealtime_Fast}, 
                    {"PRESET_TARGET_REALTIME_QUALITY",     PRESET_TARGET_REALTIME_QUALITY,     aiProcessPreset_TargetRealtime_Quality},
                    {"PRESET_TARGET_REALTIME_MAX_QUALITY", PRESET_TARGET_REALTIME_MAX_QUALITY, aiProcessPreset_TargetRealtime_MaxQuality},
                    {0}
                };
                return myProperties;
            };
            
            // static functions 
            JSFunctionSpec * StaticFunctions() {
                static JSFunctionSpec myFunctions[] = {
                    // name         native       nargs
                    {0}
                };
                return myFunctions;
            }

            virtual void initClasses(JSContext * theContext, JSObject * theGlobalObject) {
                // start berkelium namespace
                JSObject *assimpNamespace = JS_DefineObject(theContext, theGlobalObject, "Assimp", &Package, NULL, JSPROP_PERMANENT | JSPROP_READONLY);
                JSA_DefineConstInts(theContext, assimpNamespace, ConstIntProperties()); 
                JSA_AddFunctions(theContext, assimpNamespace, StaticFunctions());

                JSAssimpLoader::initClass(theContext, assimpNamespace);
            }

            const char * ClassName() {
                static const char * myClassName = "Assimp";
                return myClassName;
            }
    };
}

extern "C"
EXPORT asl::PlugInBase * AssimpLoader_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::JSAssimpPlugin(myDLHandle);
}

