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
//    $RCSfile: test_string_functions.tst.cpp,v $
//
//   $Revision: 1.9 $
//
// Description: unit test for string functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "string_functions.h"
#include "os_functions.h"
#include "UnitTest.h"

#include <iostream>

#ifdef LINUX
  #include <sys/resource.h>
  #include <sys/time.h>
  #include <unistd.h>
#endif

using namespace std;
using namespace asl;

static const char * myStringList[] = {"apfel", "birne", "kiwi", "aprikose", 0};
enum Obst { apfel, birne, kiwi, kirsche, banane };

class string_functions_UnitTest : public UnitTest {
    public:
        string_functions_UnitTest() : UnitTest("string_functions_UnitTest") {  }

        string foundAndReplaced2(const string & theString, const string & theSearch, const string & theReplace) {
            string myResult = theString;
            findAndReplace(myResult, theSearch.c_str(), theReplace.c_str());
            return myResult;
        }
        void run() {
            ENSURE(as_string("bla") == "bla");
            ENSURE(as_string(1) == "1");

                // double/float <-> string conversion
            ENSURE(as<float>("1.0") == 1.0f);
            ENSURE(as<double>("1.0") == 1.0);
            ENSURE(as<int>("1") == 1);
            ENSURE(as<int>("-1") == -1);
            ENSURE(as<long>("1") == 1);
            ENSURE(as<bool>("1") == 1);
            ENSURE(as<bool>("0") == 0);

            // lowercase uppercase
            {
                ENSURE(asl::toLowerCase("HELLOWORLD") == "helloworld");
                ENSURE(asl::toUpperCase("helloworld") == "HELLOWORLD");
            }

#ifdef LINUX
            rusage myResourceUsage;
            ENSURE(getrusage(RUSAGE_SELF,&myResourceUsage) == 0);
            cerr << getTracePrefix() << "ru_maxrss = " <<myResourceUsage.ru_maxrss << endl;
            cerr << getTracePrefix() << "ru_ixrss = " <<myResourceUsage.ru_ixrss << endl;
            cerr << getTracePrefix() << "ru_idrss = " <<myResourceUsage.ru_idrss << endl;
            cerr << getTracePrefix() << "ru_isrss = " <<myResourceUsage.ru_isrss << endl;
            int usedDataSize = myResourceUsage.ru_idrss;
            cerr << getTracePrefix() << "usedDataSize before = " << usedDataSize << endl;
            rlimit my_rlimit;
            ENSURE(getrlimit(RLIMIT_DATA, &my_rlimit) == 0);
            cerr << getTracePrefix() << "RLIMIT_DATA rlim_cur = " << my_rlimit.rlim_cur<< endl;
            cerr << getTracePrefix() << "RLIMIT_DATA rlim_max = " << my_rlimit.rlim_max<< endl;
            my_rlimit.rlim_cur = usedDataSize + 10000;
            ENSURE(setrlimit(RLIMIT_DATA, &my_rlimit) == 0);
            ENSURE(getrlimit(RLIMIT_DATA, &my_rlimit) == 0);
            cerr << getTracePrefix() << "RLIMIT_DATA rlim_cur = " << my_rlimit.rlim_cur<< endl;
            cerr << getTracePrefix() << "RLIMIT_DATA rlim_max = " << my_rlimit.rlim_max<< endl;

            for (int i = 0; i < 100000; ++i) {
                string myString = as_string(2);
                char *x = new char[10];
            }
            ENSURE(getrusage(RUSAGE_SELF,&myResourceUsage) == 0);
            cerr << getTracePrefix() << "usedDataSize after = " <<myResourceUsage.ru_idrss << endl;
            cerr << getTracePrefix() << "ru_maxrss = " <<myResourceUsage.ru_maxrss << endl;
            cerr << getTracePrefix() << "ru_ixrss = " <<myResourceUsage.ru_ixrss << endl;
            cerr << getTracePrefix() << "ru_idrss = " <<myResourceUsage.ru_idrss << endl;
            cerr << getTracePrefix() << "ru_isrss = " <<myResourceUsage.ru_isrss << endl;
            ENSURE(myResourceUsage.ru_idrss - usedDataSize < 1000);
#endif
			unsigned char myBinData[8] = {0x0,0x1,0x10,0x90,0x0a,0x0f,0xaf,0xf9};
			std::string myHexData;
			binToString(myBinData,sizeof(myBinData),myHexData);
			DPRINT(myHexData);
			ENSURE(myHexData == "000110900A0FAFF9");
			unsigned char myOtherBinData[8];
			ENSURE(stringToBin(myHexData,myOtherBinData, sizeof(myOtherBinData)));
			ENSURE(std::equal(myBinData,myBinData+sizeof(myBinData),myOtherBinData));

			std::vector<std::string> myList = splitString("");
			ENSURE(myList.size() == 0);

			std::vector<std::string> myList1 = splitString("eins");
			ENSURE(myList1.size() == 1);
			ENSURE(myList1[0] == "eins");

			std::vector<std::string> myList2 = splitString(" eins zwei,drei  vier, fuenf "," ,;");
            for (std::vector<std::string>::size_type i = 0; i < myList2.size();++i) {
				DPRINT(i);
				DPRINT(myList2[i]);
			}
			DPRINT(myList2.size());
			ENSURE(myList2.size() == 5);
			ENSURE(myList2[0] == "eins");
			ENSURE(myList2[1] == "zwei");
			ENSURE(myList2[2] == "drei");
			ENSURE(myList2[3] == "vier");
			ENSURE(myList2[4] == "fuenf");

			std::vector<std::string> myList3 = splitString(" eins zwei,drei  vier, fuenf "," ");
			DPRINT(myList3.size());
			ENSURE(myList3.size() == 4);
			ENSURE(myList3[0] == "eins");
			ENSURE(myList3[1] == "zwei,drei");
			ENSURE(myList3[2] == "vier,");
			ENSURE(myList3[3] == "fuenf");

			{
                ENSURE(getStringFromEnum(apfel, myStringList) == std::string("apfel"));
			    ENSURE(getStringFromEnum(birne, myStringList) == std::string("birne"));
			    ENSURE(getStringFromEnum(kiwi, myStringList)  == std::string("kiwi"));
			    ENSURE_EXCEPTION(getStringFromEnum(banane, myStringList), ParseException);

			    ENSURE(getEnumFromString("apfel", myStringList) == apfel);
			    ENSURE(getEnumFromString("birne", myStringList) == birne);
			    ENSURE(getEnumFromString("kiwi", myStringList)  == kiwi);
			    ENSURE(getEnumFromString("aprikose", myStringList)  == kirsche);
			    ENSURE_EXCEPTION(getEnumFromString("zitrone", myStringList), ParseException);
			}

            {
			    string myString="C:\\WinNT\\foo";
			    ENSURE(foundAndReplaced(myString, "\\","/") == "C:/WinNT/foo");
			    myString="abcdefghijklmnopqrstuvwxyz";
			    ENSURE(foundAndReplaced(myString,myString,myString) == myString);
			    ENSURE(foundAndReplaced(myString,"012","345") == myString);
			    ENSURE(foundAndReplaced(myString,"abc","012") == "012defghijklmnopqrstuvwxyz");
			    ENSURE(foundAndReplaced(myString,"xyz","345") == "abcdefghijklmnopqrstuvw345");
			    ENSURE(foundAndReplaced("","","") == "");
			    ENSURE(foundAndReplaced("","a","b") == "");
			    ENSURE(foundAndReplaced("","a","b") == "");
			    ENSURE(foundAndReplaced(myString,"","b") == myString);

                myString="abcdefabcjklmnopabctuvwabc";
			    ENSURE(foundAndReplaced(myString,"abc","012") == "012def012jklmnop012tuvw012");
			    ENSURE(foundAndReplaced(myString,"a","012") == "012bcdef012bcjklmnop012bctuvw012bc");
			    ENSURE(foundAndReplaced(myString,"abc","") == "defjklmnoptuvw");

                myString="abcabcabcabcabc";
			    ENSURE(foundAndReplaced(myString,"abc","") == "");
			    ENSURE(foundAndReplaced(myString,"abc","a") == "aaaaa");
			    ENSURE(foundAndReplaced("aaaaaa","a","b") == "bbbbbb");
			    ENSURE(foundAndReplaced("caaaaa","a","b") == "cbbbbb");
			    ENSURE(foundAndReplaced("aaaaac","a","b") == "bbbbbc");

			    myString="C:\\WinNT\\foo";
			    ENSURE(foundAndReplaced2(myString, "\\","/") == "C:/WinNT/foo");
			    myString="abcdefghijklmnopqrstuvwxyz";
			    ENSURE(foundAndReplaced2(myString,myString,myString) == myString);
			    ENSURE(foundAndReplaced2(myString,"012","345") == myString);
			    ENSURE(foundAndReplaced2(myString,"abc","012") == "012defghijklmnopqrstuvwxyz");
			    ENSURE(foundAndReplaced2(myString,"xyz","345") == "abcdefghijklmnopqrstuvw345");
			    ENSURE(foundAndReplaced2("","","") == "");
			    ENSURE(foundAndReplaced2("","a","b") == "");
			    ENSURE(foundAndReplaced2("","a","b") == "");
			    ENSURE(foundAndReplaced2(myString,"","b") == myString);

                myString="abcdefabcjklmnopabctuvwabc";
			    ENSURE(foundAndReplaced2(myString,"abc","012") == "012def012jklmnop012tuvw012");
			    ENSURE(foundAndReplaced2(myString,"a","012") == "012bcdef012bcjklmnop012bctuvw012bc");
			    ENSURE(foundAndReplaced2(myString,"abc","") == "defjklmnoptuvw");

                myString="abcabcabcabcabc";
			    ENSURE(foundAndReplaced2(myString,"abc","") == "");
			    ENSURE(foundAndReplaced2(myString,"abc","a") == "aaaaa");
			    ENSURE(foundAndReplaced2("aaaaaa","a","b") == "bbbbbb");
			    ENSURE(foundAndReplaced2("caaaaa","a","b") == "cbbbbb");
			    ENSURE(foundAndReplaced2("aaaaac","a","b") == "bbbbbc");
            }

            // expand enviroment
#ifdef _WIN32
            _putenv("AC_TEST_VAR=foo");
#else
            setenv("AC_TEST_VAR","foo",1);
#endif
            ENSURE(expandEnvironment(string("${AC_TEST_VAR}")) == "foo");
            ENSURE(expandEnvironment(string("${AC_TEST_VAR}bar")) == "foobar");
            ENSURE(expandEnvironment(string("bar${AC_TEST_VAR}")) == "barfoo");
            ENSURE(expandEnvironment(string("${AC_TEST_VAR}${AC_TEST_VAR}")) == "foofoo");
            ENSURE(expandEnvironment(string("${}${AC_TEST_VAR}")) == "foo");
            ENSURE_EXCEPTION(expandEnvironment(string("${XXX${AC_TEST_VAR}}")), ParseException);
            ENSURE_EXCEPTION(expandEnvironment(string("${AC_TEST_VAR")), ParseException);

#ifdef _WIN32
            _putenv("AC_TEST_VAR=");
#else
            unsetenv("AC_TEST_VAR");
#endif
            ENSURE(expandEnvironment(string("${AC_TEST_VAR}")) == "");
            ENSURE(expandEnvironment(string("${AC_TEST_VAR}bar")) == "bar");
            ENSURE(expandEnvironment(string("bar${AC_TEST_VAR}")) == "bar");
            ENSURE(expandEnvironment(string("${AC_TEST_VAR}${AC_TEST_VAR}")) == "");
            ENSURE_EXCEPTION(expandEnvironment(string("${AC_TEST_VAR")), ParseException);


            ENSURE(trimLeft("Foo") == "Foo");
            ENSURE(trimLeft("  Foo") == "Foo");
            ENSURE(trimLeft(" Foo Bar ") == "Foo Bar ");

            ENSURE(trimRight("Foo") == "Foo");
            ENSURE(trimRight("  Foo") == "  Foo");
            ENSURE(trimRight(" Foo Bar ") == " Foo Bar");

            ENSURE(trim("Foo") == "Foo");
            ENSURE(trim("  Foo") == "Foo");
            ENSURE(trim(" Foo Bar ") == "Foo Bar");

            testBase64();
		}

        void testBase64Encode(const char * IN, const char * OUT, const Char * CB) {
            testBase64Encode(string(IN), string(OUT), CB);
        }
        void testBase64Encode(const string & theIn, const string & theOut, const Char * CB) {
            string myIn(theIn);
            string myOut(theOut); 
            string myDestination1; 
            string myDestination2;
            binToBase64((unsigned char *)&myIn[0], myIn.size(), myDestination1, CB); 
            ENSURE_EQUAL(myDestination1, myOut);

            const char SENTINAL = '@';
            myDestination2.resize(myIn.size()+1);
            myDestination2[myIn.size()] = SENTINAL;
            
            unsigned mySize = base64ToBin(myDestination1, (unsigned char *)&myDestination2[0], myIn.size());
            ENSURE_EQUAL(mySize, myIn.size());
            ENSURE_EQUAL(myDestination2[myIn.size()], SENTINAL);
            myDestination2.resize(mySize);
            ENSURE_EQUAL(myDestination2, myIn); 
        } 

        void testBase64() {
            // case 0: Empty file:
            testBase64Encode("", "", cb64);

            // case 1: One input character:
            testBase64Encode("A", "QQƒƒ", cb67);

            // case 1: One input character:
            testBase64Encode("A", "QQƒƒ", cb67);            

            // case 2: Two input characters:
            testBase64Encode("AB", "QUI=", cb64);

            // case 3: Three input characters:
            testBase64Encode("ABC", "QUJD", cb67);

            // case 4: Four input characters:
    	    testBase64Encode("ABCD", "QUJDRA==", cb64);
            
            testBase64Encode("ABCABCD", "QUJDQUJDRA==", cb64);
 
            // case 5: All chars from 0 to ff, linesize set to 50:
            {
                string myInput;
                myInput.resize(256);
                for (unsigned int i = 0; i < 256; ++i) {
                    myInput[i] = static_cast<unsigned char>(i);
                }
                char * myOutput =
                    "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIj"
                    "JCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZH"
                    "SElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWpr"
                    "bG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
                    "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKz"
                    "tLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX"
                    "2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7"
                    "/P3+/w==";

                testBase64Encode(myInput, myOutput, cb64);

                myOutput =
                    "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIj"
                    "JCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0øP0BBQkNERUZH"
                    "SElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWpr"
                    "bG1ub3BxcnN0dXZ3eHl6e3x9fnøAgYKDhIWGh4iJiouMjY6P"
                    "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrqøwsbKz"
                    "tLW2t7i5uru8vb6þwMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX"
                    "2Nna29zd3tþg4eLj5OXm5øjp6uvs7e7v8PHy8þT19vf4øfr7"
                    "þP3øþwƒƒ";
                testBase64Encode(myInput, myOutput, cb67);
            }

            // case 6: test all possible exceptions
            unsigned char myDest[10];
            ENSURE_EXCEPTION(base64ToBin("ABCDE", myDest, 10), ParseException);
            ENSURE_EXCEPTION(base64ToBin("QUI=", myDest, 1), BufferTooSmall);
            ENSURE_EXCEPTION(base64ToBin("QUJDRA==", myDest, 3), BufferTooSmall); 
            ENSURE_EXCEPTION(base64ToBin("!abc", myDest, 10), ParseException);
            ENSURE_EXCEPTION(base64ToBin("abc*", myDest, 10), ParseException);
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new string_functions_UnitTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
