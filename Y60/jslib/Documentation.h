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
//   $RCSfile: Documentation.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2005/04/29 13:37:57 $
//
//
//=============================================================================

#ifndef _jslib_Documentation_h_included_
#define _jslib_Documentation_h_included_

#include "jssettings.h"
#include "JScppUtils.h"
#include <js/jsapi.h>

#include <string>
#include <vector>
#include <map>

#define DOC_BEGIN(DESCRIPTION) \
    if ( ! cx) {\
        reinterpret_cast<FunctionDescription*>(obj)->description = DESCRIPTION;

#define DOC_PARAM(NAME, TYPE) \
        describeFunctionParameter(reinterpret_cast<FunctionDescription*>(obj), NAME, TYPE, "");

#define DOC_PARAM_OPT(NAME, TYPE, DEFAULT_VALUE) \
        describeFunctionParameter(reinterpret_cast<FunctionDescription*>(obj), NAME, TYPE, asl::as_string(DEFAULT_VALUE));

#define DOC_RVAL(DESCRIPTION, TYPE) \
        reinterpret_cast<FunctionDescription*>(obj)->return_value = DESCRIPTION; \
        reinterpret_cast<FunctionDescription*>(obj)->return_type = DocTypeDescription[TYPE];

#define DOC_RESET \
        describeFunctionParameter(reinterpret_cast<FunctionDescription*>(obj), "", DOC_TYPE_INTERN, "");


#define DOC_END \
        return JS_TRUE; \
    }


#define DOC_CREATE(CLASSNAME) \
    DOC_MODULE_CREATE("global", CLASSNAME);

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
        DOC_TYPE_VECTOR2F,
        DOC_TYPE_VECTOR3F,
        DOC_TYPE_VECTOR4F,
        DOC_TYPE_POINT3F,
        DOC_TYPE_QUATERNIONF,
        DOC_TYPE_MATRIX4F,
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
        DOC_TYPE_OBJECT
    };

    static const std::string DocTypeDescription[] = {
        "__intern__",
        "string", "integer", "float", "boolean", "enumeration", "array",
        "Vector2", "Vector3", "Vector4", "Point", "Quaternion", "Matrix", "Box",
        "Line", "Ray", "LineSegment", "Plane", "Triangle", "Sphere", "Frustum",
        "Node", "NodeList", "Object"
    };


    struct ParameterDescription {
        std::string name;
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

    typedef std::map<std::pair<std::string,std::string>, ObjectDocumentation> DocumentationMap;

    void documentConstructor(const char *theModule, const char *theClassName,
        JSBool (*theConstructor)(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval));

    void describeFunctionParameter(FunctionDescription *myFunctionDescription,
        const std::string & theName, const DocType & theType, const std::string & theDefaultValue);


    void
    createClassModuleDocumentation(const char *theModule, const char * theClassName,
            JSPropertySpec * theProperties,
            JSFunctionSpec * theFunctions,
            JSConstIntPropertySpec * theConstants = 0,
            JSPropertySpec * theStaticProperties = 0,
            JSFunctionSpec * theStaticFunctions = 0,
            const char * theBaseClassName = 0);

    void
    createClassDocumentation(const char * theClassName,
            JSPropertySpec * theProperties,
            JSFunctionSpec * theFunctions,
            JSConstIntPropertySpec * theConstants = 0,
            JSPropertySpec * theStaticProperties = 0,
            JSFunctionSpec * theStaticFunctions = 0,
            const char * theBaseClassName = 0);

    void
    createFunctionDocumentation(const char * theSection, JSFunctionSpec * theFunctions);
}

#endif

