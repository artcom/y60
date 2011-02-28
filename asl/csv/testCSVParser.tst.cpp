/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
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

#include "CSVParser.h"
#include <asl/base/file_functions.h>
#include <asl/base/UnitTest.h>
#include <asl/zip/DirectoryPackage.h>

using namespace std;
using namespace asl;

class CSVParserUnitTest : public UnitTest {
public:
    explicit CSVParserUnitTest() : UnitTest("CSVParserUnitTest"),
        fixturesDir(asl::getDirectoryPart( __FILE__ )+"/fixtures") 
    {
    }

    void testValidInvalid() {
        vector<std::string> fixtureList = fixturesDir.getFileList();
        for (unsigned int i = 0; i < fixtureList.size(); ++i) {
            std::string fullPath = fixturesDir.getPath()+"/"+fixtureList[i];
            DPRINT(fullPath);
            std::string s;
            readFile(fullPath,s);
            std::vector<std::vector<std::string> > result;

            if (fixtureList[i].find(".invalid.") != string::npos) {
                ENSURE_EXCEPTION(parseCSV(s, result), char *);
            } else {
                parseCSV(s, result);
                DPRINT(result.size());
            }
        }
    }

    void testRecordCount() {
        std::vector<std::vector<std::string> > result;
        // Linux-Style line breaks
        parseCSV("foo;bar;baz\nsnarf;\"ba\nrf\";narf\n", result);
        ENSURE_EQUAL(2, result.size());
        ENSURE_EQUAL(3, result[0].size());
        ENSURE_EQUAL("foo", result[0][0]);
        ENSURE_EQUAL(3, result[1].size());
        ENSURE_EQUAL("snarf", result[1][0]);
        // Windows-Style line breaks
        result.clear();
        parseCSV("foo;bar;baz\r\nsnarf;\"ba\r\nrf\";narf\r\n", result);
        for (AC_SIZE_TYPE r = 0; r < result.size(); ++r) {
            for (AC_SIZE_TYPE f = 0; f < result[r].size(); ++f) {
                cerr << "[" << r << "," << f << "]='" << result[r][f] << "'" << endl;
            }
        }
        ENSURE_EQUAL(2, result.size());
        ENSURE_EQUAL(3, result[0].size());
        ENSURE_EQUAL("foo", result[0][0]);
        ENSURE_EQUAL(3, result[1].size());
        ENSURE_EQUAL("snarf", result[1][0]);
    }

    void run() {
        testValidInvalid();
        testRecordCount();
    }

    DirectoryPackage fixturesDir;
};


int main(int argc, char *argv[]) {

    UnitTestSuite mySuite(argv[0], argc, argv);

    try {
        mySuite.addTest(new CSVParserUnitTest);
        mySuite.run();
    }
    catch (...) {
        cerr << "## An unknown exception occured during execution." << endl;
        mySuite.incrementFailedCount();
    }

    int returnStatus = -1;
    if (mySuite.getPassedCount() != 0) {
        returnStatus = 0;
    } else {
        cerr << "## No tests." << endl;

    }
    cerr << ">> Finsihed test suite '" << argv[0] << "', return status = " << returnStatus << endl;
    return returnStatus;

}

