/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testArguments.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test template file - change Arguments to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"

#include "Arguments.h"

#include <string>
#include <iostream>


using namespace std;  // automatically added!



class ArgumentsUnitTest : public UnitTest {
    public:
        ArgumentsUnitTest() : UnitTest("ArgumentsUnitTest") {  }
        void run() {

            asl::Arguments myArguments;
            try {
                myArguments.haveOption("--blah-fasel");
                FAILURE( "No exception thrown" );
            }
            catch ( asl::Arguments::IllegalOptionQuery &) {
                SUCCESS("correct exception thrown and caught")
            }

            try {
                myArguments.getOptionArgument("--blah-fasel");
                FAILURE( "No exception thrown" );
            }
            catch ( asl::Arguments::IllegalOptionQuery &) {
                SUCCESS("correct exception thrown and caught")
            }


            SUCCESS( "Reached 1" );
            asl::Arguments::AllowedOption myOptions[] = {
                {"--blah-fasel", ""},
                {"--bottles-in-fridge", "%d"},
                //{"", "sourceFileNames"},
                //{"", "destinationFileName"},
                {"", ""}
            };
            SUCCESS( "Reached 2" );
            myArguments.addAllowedOptions(myOptions);
            SUCCESS( "Reached 3" );
            
            try {
                ENSURE( ! myArguments.haveOption("--blah-fasel"));
                SUCCESS( "No exception thrown" );
            }
            catch ( asl::Arguments::IllegalOptionQuery &) {
                FAILURE("Exception thrown and caught")
            }

            try {
                ENSURE( myArguments.getOptionArgument("--blah-fasel") == "" );
                SUCCESS( "No exception thrown" );
            }
            catch ( asl::Arguments::IllegalOptionQuery &) {
                FAILURE("Exception thrown and caught")
            }

            ENSURE( ! myArguments.parse( 0, 0 ));    

            int dummyArgcBad = 7;
            char * dummyArgvBad[] = { 
                "dummyProgramName", "firstFileName", "--bottles-in-fridge", "12" ,
                "--check-beer-in-the-fridge", "someFileName", "anotherFileName"}; 

            ENSURE( myArguments.parse( 1, dummyArgvBad ));    

            ENSURE( ! myArguments.parse(dummyArgcBad, dummyArgvBad));
            ENSURE( ! myArguments.haveOption("--blah-fasel"));
           
            ENSURE( !myArguments.parse( 3, dummyArgvBad ));    
            
            myArguments = asl::Arguments(myOptions);

            int dummyArgcGood = 7;
            char * dummyArgvGood[] = { "dummyProgramName", "--blah-fasel", 
                                       "firstFileName", "--bottles-in-fridge", "12" , 
                                       "someFileName", "anotherFileName"}; 
            ENSURE( myArguments.parse(dummyArgcGood, dummyArgvGood));
            ENSURE( myArguments.getProgramName() == "dummyProgramName");
            ENSURE( myArguments.getCount() == 3); 
            ENSURE( myArguments.getOptionCount() == 2); 
            ENSURE( myArguments.haveOption("--blah-fasel"));
            ENSURE( myArguments.getOptionArgument("--bottles-in-fridge")=="12" );
            ENSURE( myArguments.getArgument(0)=="firstFileName" );
            ENSURE( myArguments.getArgument(1)=="someFileName" );
            ENSURE( myArguments.getArgument(2)=="anotherFileName" );

            // new flags to test non-destructible arguments
            ENSURE( string(dummyArgvGood[0]) == "dummyProgramName");
            ENSURE( string(dummyArgvGood[1]) == "--blah-fasel");
            ENSURE( string(dummyArgvGood[2]) == "firstFileName");
            ENSURE( string(dummyArgvGood[3]) == "--bottles-in-fridge");

            { 
                myArguments = asl::Arguments(myOptions);
                // test "--" stops parsing arguments
                int dummyArgcGood = 7;
                char * dummyArgvGood[] = { "myProgramName", "foo", "--bottles-in-fridge", "-1", "--", "--not-parsed", "--" }; 
                ENSURE( myArguments.parse(dummyArgcGood, dummyArgvGood));
                ENSURE( myArguments.haveOption("--bottles-in-fridge"));
                DPRINT( myArguments.getCount() );
                ENSURE( myArguments.getCount() == 3 );
                ENSURE( myArguments.getArgument(0) == "foo" );
                ENSURE( myArguments.getArgument(1) == "--not-parsed" );
                ENSURE( myArguments.getArgument(2) == "--" );
            }
            //exit(1);
            //FAILURE("test failure");       
        }
};

/* Does anyone need this?
template <class T>
class ArgumentsTemplateUnitTest : public TemplateUnitTest {
public:
    ArgumentsTemplateUnitTest(const char * theTemplateArgument) 
        : TemplateUnitTest("ArgumentsTemplateUnitTest",theTemplateArgument) {}
    void run() {
        T someVariable = 1;
        ENSURE(someVariable);
        DPRINT2(get_myName(),someVariable);
    }
};
*/

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new ArgumentsUnitTest, 2000);
        //addTest(new ArgumentsTemplateUnitTest<bool>("<bool>"));
        //addTest(new ArgumentsTemplateUnitTest<int>("<int>"));
    }

};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
