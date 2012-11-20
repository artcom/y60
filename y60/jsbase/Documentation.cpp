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
//   $RCSfile: Documentation.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

// own header
#include "Documentation.h"

#include <asl/base/Logger.h>
#include <asl/base/string_functions.h>

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
