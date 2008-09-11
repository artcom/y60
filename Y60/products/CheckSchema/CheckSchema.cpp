//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include <asl/base/Exception.h>
#include <asl/base/Logger.h>
#include <asl/base/file_functions.h>

#include <asl/dom/Nodes.h>
#include <asl/dom/Schema.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace dom;


bool parseFile(string theFilename, NodePtr theDocument) {
    string myXmlString;
    if (asl::readFile(theFilename, myXmlString)) {
        cout << "Parsing file " << theFilename << "." << endl;
        int LastParsePos = theDocument->parseAll(myXmlString);
        if (LastParsePos != myXmlString.length()) {
            return false;
        }
        cout << "File parsed sucessfully." << endl;
        return true;
    } else {
        cerr << "File " << theFilename << " not found." << endl;
        return false;
    }
}

int
main(int argc, char **argv) {
    int rv = 0;
    try {
        if (argc == 1 || argc > 3) {
            cout << endl;
            cout << "CheckSchema: Validates an xml schema and optionally checks an" << endl;
            cout << "xml file against the schema." << endl;
            cout << "Usage: " << endl;
            cout << "    CheckSchema <SchemaName> [<FileName>]" << endl;
            cout << endl;
            return 1;
        }
            
        asl::Ptr<dom::ValueFactory> myFactory = asl::Ptr<dom::ValueFactory>(new dom::ValueFactory());
        registerStandardTypes(*myFactory);
//        registerSomTypes(*myFactory);
        
        string mySchemaName = argv[1];
        dom::NodePtr mySchema = dom::NodePtr(new dom::Document());
//        mySchema->setValueFactory(myFactory);
//        mySchema->addSchema(dom::Schema::getBuiltinSchema(), "xs");

        bool ParseOK = parseFile(mySchemaName, mySchema);
        if (!ParseOK) {
            return 1;
        }
        cout << endl;

        if (argc == 3 && ParseOK) {
            string myFilename = argv[2];
            dom::NodePtr myDocument = dom::NodePtr(new dom::Document());
            myDocument->setValueFactory(myFactory);
            myDocument->addSchema(*mySchema, "");
            ParseOK = parseFile(myFilename, myDocument);
            if (!ParseOK) {
                return 1;
            }
        }
    
    } catch (asl::Exception & ex) {
        rv = 1;
        AC_FATAL << "Exception caught in main: " << ex << endl;
    } catch (std::exception & ex) {
        rv = 1;
        AC_FATAL << "std::exception caught in main: " << ex.what() << endl;
    } catch (...) {
        rv = 1;
        AC_FATAL << "unknown exception caught in main" << endl;
    }

    return rv;
}
