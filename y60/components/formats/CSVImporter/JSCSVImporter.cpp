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
*/

#include "JSCSVImporter.h"

#include <asl/csv/CSVParser.h>
#include <y60/jsbase/JSNode.h>

using namespace y60;
using namespace std;
using namespace dom;

#define ASSERT(x) if (!(x)) {cerr << "Assertion failed: " << #x << " " << __FILE__ << " " <<__LINE__ << endl; exit(1);}


namespace jslib {
    vector<vector<string> > JSCSVImporter::csv2array(string theFileName) {
        std::vector<std::vector<std::string> > myResult;
        string myContent = readFile(theFileName);
        asl::parseCSV(myContent, myResult);
        return myResult;
    }

    static JSBool
    Csv2array(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;
        std::string myFileName;
        convertFrom(cx, argv[0], myFileName );
        *rval = as_jsval(cx, JSCSVImporter::csv2array(myFileName));
        return JS_TRUE;
    }


    JSFunctionSpec *
    JSCSVImporter::StaticFunctions() {
        AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
        static JSFunctionSpec myFunctions[] = {
            // name                  native                   nargs
            {"csv2array",            Csv2array,               1},
            {0}
        };
        return myFunctions;
    }
} // namespace jslib


/************************************************************/
extern "C"
EXPORT PlugInBase* CSVImporter_instantiatePlugIn(DLHandle myDLHandle) {
    return new jslib::JSCSVImporter(myDLHandle);
}

