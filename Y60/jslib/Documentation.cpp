//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Documentation.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "Documentation.h"

#include <asl/Logger.h>
#include <asl/string_functions.h>

using namespace std;

namespace jslib {

    void describeFunctionParameter(FunctionDescription *myFunctionDescription,
            const string & theName, const string & theDescription, const DocType & theType, const string & theDefaultValue)
    {
        ParameterDescription myDescription;
        myDescription.name = theName;
        myDescription.description = theDescription;
        myDescription.type = DocTypeDescription[theType];
        myDescription.default_value = theDefaultValue;
        myFunctionDescription->parameters.push_back(myDescription);
    }


    void documentConstructor(const char *theModule, const char *theClassName,
         JSBool (*theConstructor)(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval))
    {
        FunctionDescription myFunctionDocumentation;
        myFunctionDocumentation.name = theClassName;
        theConstructor(0, reinterpret_cast<JSObject*>(&myFunctionDocumentation),
                            0, 0, 0);
        DocumentationSingleton::get()[make_pair(theModule,theClassName)].constructors.push_back(myFunctionDocumentation);
        AC_TRACE << "added doku-ctor for " << theClassName << " in module " << theModule << endl;
    }

    void
    document(ObjectDocumentation & theDocumentation,
        const char * theClassName,
        JSPropertySpec * theProperties,
        JSFunctionSpec * theFunctions,
        JSConstIntPropertySpec * theConstants,
        JSPropertySpec * theStaticProperties,
        JSFunctionSpec * theStaticFunctions,
        const char * theBaseClassName)
    {

        if (theFunctions) {
            while (theFunctions->name) {
                FunctionDescription myFunctionDocumentation;
                myFunctionDocumentation.name = theFunctions->name;
                theFunctions->call( 0, reinterpret_cast<JSObject*>(&myFunctionDocumentation),
                                    0, 0, 0);
                theDocumentation.functions.push_back(myFunctionDocumentation);
                ++theFunctions;
            }
        } else {
            AC_TRACE << "No functions defined for class " << theClassName << endl;
        }

        if (theProperties) {
            while (theProperties->name) {
                string myDescription(theProperties->name);
                if (theProperties->flags & JSPROP_READONLY) {
                     myDescription += string(" (readonly)");
                }
                theDocumentation.properties.push_back(myDescription);
                ++theProperties;
            }
        } else {
            AC_TRACE << "No properties defined for class " << theClassName << endl;
        }

        if (theStaticFunctions) {
            while (theStaticFunctions->name) {
                FunctionDescription myFunctionDocumentation;
                myFunctionDocumentation.name = theStaticFunctions->name;
                theStaticFunctions->call( 0, reinterpret_cast<JSObject*>(&myFunctionDocumentation),
                                          0, 0, 0);
                theDocumentation.static_functions.push_back(myFunctionDocumentation);
                ++theStaticFunctions;
            }
        }

        if (theStaticProperties) {
            while (theStaticProperties->name) {
                string myDescription(theStaticProperties->name);
                if (theStaticProperties->flags & JSPROP_READONLY) {
                     myDescription += string(" (readonly)");
                }
                theDocumentation.static_properties.push_back(myDescription);
                ++theStaticProperties;
            }
        }

        if (theConstants) {
            while (theConstants->name) {
                theDocumentation.static_properties.push_back(string(theConstants->name) + " = " + asl::as_string(theConstants->value));
                ++theConstants;
            }
        }

        if (theBaseClassName) {
            theDocumentation.base_class = string(theBaseClassName);
        }
    }

    void
    createClassModuleDocumentation(const char *theModule, const char * theClassName,
            JSPropertySpec * theProperties,
            JSFunctionSpec * theFunctions,
            JSConstIntPropertySpec * theConstants,
            JSPropertySpec * theStaticProperties,
            JSFunctionSpec * theStaticFunctions,
            const char * theBaseClassName)
    {
        ObjectDocumentation & myDocumentation = DocumentationSingleton::get()[make_pair(theModule, theClassName)];
        AC_TRACE << "creating doku for " << theClassName << " in module " << theModule << endl;
        document(myDocumentation, theClassName, theProperties, theFunctions, theConstants,
                theStaticProperties, theStaticFunctions, theBaseClassName);

        //DocumentationSingleton::get()[make_pair(theModule,theClassName)] = myDocumentation;
        AC_TRACE << "...created doku for " << theClassName << " in module " << theModule << endl;
    }

    void
    createClassDocumentation(const char * theClassName,
            JSPropertySpec * theProperties,
            JSFunctionSpec * theFunctions,
            JSConstIntPropertySpec * theConstants,
            JSPropertySpec * theStaticProperties,
            JSFunctionSpec * theStaticFunctions,
            const char * theBaseClassName)
    {
        createClassModuleDocumentation("Global", theClassName, theProperties, theFunctions,
                theConstants, theStaticProperties, theStaticFunctions, theBaseClassName);
    }

    void
    createFunctionDocumentation(const char * theSection, JSFunctionSpec * theFunctions) {
        ObjectDocumentation & myDocumentation = DocumentationSingleton::get()[make_pair(theSection,"")];
        AC_TRACE << "adding doku for free-functions in module " << theSection << endl;

        if (theFunctions) {
            while (theFunctions->name) {
                FunctionDescription myFunctionDocumentation;
                myFunctionDocumentation.name = theFunctions->name;
                theFunctions->call( 0, reinterpret_cast<JSObject*>(&myFunctionDocumentation),
                                    0, 0, 0);
                AC_TRACE << "called " << theFunctions->name;
                myDocumentation.functions.push_back(myFunctionDocumentation);
                ++theFunctions;
            }
        } else {
            AC_WARNING << "No functions defined for section " << theSection << endl;
        }
        AC_TRACE << "added doku for free-functions in module " << theSection << endl;
    }
}
