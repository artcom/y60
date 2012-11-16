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
*/
//
//   $RCSfile: Documentation.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2005/04/29 13:37:57 $
//
//
//=============================================================================

#ifndef _jslib_Documentation_h_included_
#define _jslib_Documentation_h_included_

#include "y60_jsbase_settings.h"

#include "jssettings.h"
#include "JScppUtils.h"

#ifdef USE_LEGACY_SPIDERMONKEY
#include <js/spidermonkey/jsapi.h>
#else
#include <js/jsapi.h>
#endif

#include <string>
#include <vector>
#include <map>

#define DOC_BEGIN(DESCRIPTION) \
    if ( ! cx) {\
        reinterpret_cast<jslib::FunctionDescription*>(obj)->description = DESCRIPTION;

#define DOC_PARAM(NAME, DESCRIPTION, TYPE) \
        describeFunctionParameter(reinterpret_cast<jslib::FunctionDescription*>(obj), NAME, DESCRIPTION, TYPE, "");

#define DOC_PARAM_OPT(NAME, DESCRIPTION, TYPE, DEFAULT_VALUE) \
        describeFunctionParameter(reinterpret_cast<jslib::FunctionDescription*>(obj), NAME, DESCRIPTION, TYPE, asl::as_string(DEFAULT_VALUE));

#define DOC_RVAL(DESCRIPTION, TYPE) \
        reinterpret_cast<jslib::FunctionDescription*>(obj)->return_value = DESCRIPTION; \
        reinterpret_cast<jslib::FunctionDescription*>(obj)->return_type = jslib::DocTypeDescription[TYPE];

#define DOC_RESET \
        describeFunctionParameter(reinterpret_cast<jslib::FunctionDescription*>(obj), "", "", DOC_TYPE_INTERN, "");


#define DOC_END \
        return JS_TRUE; \
    }


#define DOC_CREATE(CLASSNAME) \
    DOC_MODULE_CREATE("Global", CLASSNAME);

#define DOC_MODULE_CREATE(MODULE, CLASSNAME) \
    createClassModuleDocumentation(MODULE, CLASSNAME::ClassName(), CLASSNAME::Properties(), \
                                   CLASSNAME::Functions(), CLASSNAME::ConstIntProperties(), \
                                   CLASSNAME::StaticProperties(), CLASSNAME::StaticFunctions(), \
                                   ""); \
    documentConstructor(MODULE, CLASSNAME::ClassName(), CLASSNAME::Constructor);

namespace jslib {

    enum DocType {
        DOC_TYPE_INTERN = 0, //not a real js-type but a hack, see create_documentation.js
        DOC_TYPE_STRING,
        DOC_TYPE_INTEGER,
        DOC_TYPE_FLOAT,
        DOC_TYPE_BOOLEAN,
        DOC_TYPE_ENUMERATION,
        DOC_TYPE_ARRAY, // js-array
        DOC_TYPE_VECTOR2I,
        DOC_TYPE_VECTOR2F,
        DOC_TYPE_VECTOR3I,
        DOC_TYPE_VECTOR3F,
        DOC_TYPE_VECTOR4F,
        DOC_TYPE_VECTOROFFLOAT,
        DOC_TYPE_POINT2F,
        DOC_TYPE_POINT3F,
        DOC_TYPE_QUATERNIONF,
        DOC_TYPE_MATRIX4F,
        DOC_TYPE_BOX2F,
        DOC_TYPE_BOX3F,
        DOC_TYPE_LINE,
        DOC_TYPE_RAY,
        DOC_TYPE_LINESEGMENT,
        DOC_TYPE_PLANE,
        DOC_TYPE_TRIANGLE,
        DOC_TYPE_SPHERE,
        DOC_TYPE_FRUSTUM,
        DOC_TYPE_NODE,
        DOC_TYPE_NODELIST,
        DOC_TYPE_REQUEST,
        DOC_TYPE_OBJECT,
        DOC_TYPE_BLOCK,
        DOC_TYPE_SCENE,
        DOC_TYPE_VECTOROFVECTOR2F,
        DOC_TYPE_VECTOROFVECTOR3F,
        DOC_TYPE_VECTOROFVECTOR4F,
        DOC_TYPE_BSPLINE,
        DOC_TYPE_SVGPATH,
        DOC_TYPE_RASTER
   };

    static const std::string DocTypeDescription[] = {
        "__intern__",
        "string", "int", "float", "bool", "enum", "array",
        "Vector2i", "Vector2f", "Vector3i", "Vector3f", "Vector4f", "VectorOfFloat", "Point2f", "Point3f", "Quaternion", "Matrix",
        "Box2f", "Box3f",
        "Line", "Ray", "LineSegment", "Plane", "Triangle", "Sphere", "Frustum",
        "Node", "NodeList", "Request", "object", "Block", "Scene",
        "VectorOfVector2f", "VectorOfVector3f", "VectorOfVector4f",
        "BSpline", "SvgPath", "Raster"
    };

    struct ParameterDescription {
        std::string name;
        std::string description;
        std::string type;
        std::string default_value;
    };

    struct FunctionDescription {
        std::string name;
        std::string description;
        std::vector<ParameterDescription> parameters;
        std::string return_value;
        std::string return_type;
    };

    struct ObjectDocumentation {
        //std::vector<std::string> functions;
        std::vector<FunctionDescription> constructors;
        std::vector<FunctionDescription> functions;
        std::vector<std::string> properties;
        std::vector<FunctionDescription> static_functions;
        std::vector<std::string> static_properties;
        std::vector<std::string> constants;
        std::string              base_class;
    };

    typedef std::pair<std::string, std::string> DokumentationKey;
    typedef std::map<DokumentationKey, ObjectDocumentation> DocumentationMap;

    class Y60_JSBASE_DECL DocumentationSingleton : public asl::Singleton<DocumentationSingleton> {
        friend class asl::SingletonManager;
        public:
            ObjectDocumentation & operator[](const DokumentationKey & theKey) {
                return _myDocumentation[theKey];
            }
            DocumentationMap::iterator begin() {
                return _myDocumentation.begin();
            }
            DocumentationMap::iterator end() {
                return _myDocumentation.end();
            }

        private:
             DocumentationMap _myDocumentation;
    };

    Y60_JSBASE_DECL void documentConstructor(const char *theModule, const char *theClassName,
        JSBool (*theConstructor)(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval));

    Y60_JSBASE_DECL void describeFunctionParameter(FunctionDescription *myFunctionDescription,
        const std::string & theName, const std::string & theDescription, const DocType & theType, const std::string & theDefaultValue);


    Y60_JSBASE_DECL void
    createClassModuleDocumentation(const char *theModule, const char * theClassName,
            JSPropertySpec * theProperties,
            JSFunctionSpec * theFunctions,
            JSConstIntPropertySpec * theConstants = 0,
            JSPropertySpec * theStaticProperties = 0,
            JSFunctionSpec * theStaticFunctions = 0,
            const char * theBaseClassName = 0);

    Y60_JSBASE_DECL void
    createClassDocumentation(const char * theClassName,
            JSPropertySpec * theProperties,
            JSFunctionSpec * theFunctions,
            JSConstIntPropertySpec * theConstants = 0,
            JSPropertySpec * theStaticProperties = 0,
            JSFunctionSpec * theStaticFunctions = 0,
            const char * theBaseClassName = 0);

    Y60_JSBASE_DECL void
    createFunctionDocumentation(const char * theSection, JSFunctionSpec * theFunctions);
}

#endif

