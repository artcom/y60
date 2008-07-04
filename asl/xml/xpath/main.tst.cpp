//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "asl/UnitTest.h"

#include "xpath_api.h"
#include "Value.h"

using namespace xpath;

class xpath_UnitTest : public UnitTest {
    public:
        xpath_UnitTest() : UnitTest("xpath_UnitTest") {  }

        bool equals(OrderedNodeSetPtr the, OrderedNodeSetPtr same) {
            if (!includes(the->begin(), the->end(), same->begin(), same->end())) {
                return false;
            }
            if (!includes(same->begin(), same->end(), the->begin(), the->end())) {
                return false;
            }
            return true;
        }

        bool equals(dom::NodePtr theNode, std::string thePath, OrderedNodeSetPtr expectedResult) {
            OrderedNodeSetPtr myResult = xpath_evaluateOrderedSet(thePath, theNode);
            return equals(myResult, expectedResult);
        }

        bool equals(dom::NodePtr theNode, std::string thePath, dom::NodePtr expectedResult) {
            OrderedNodeSetPtr expectedResultSet( new OrderedNodeSet() );
            expectedResultSet->insert(expectedResult);
            return equals(theNode, thePath, expectedResultSet);
        }

        bool contains(OrderedNodeSetPtr larger, OrderedNodeSetPtr smaller) {
            if (!includes(larger->begin(), larger->end(), smaller->begin(), smaller->end())) {
                AC_WARNING << "no containment for set ";
                for (OrderedNodeSet::iterator i = smaller->begin(); i != smaller->end(); ++i) {
                    AC_WARNING << **i;
                }
                AC_WARNING << "in ";
                for (OrderedNodeSet::iterator i = larger->begin(); i != larger->end(); ++i) {
                    AC_WARNING << **i;
                }
                return false;
            }
            return true;
        }

        bool contains(dom::NodePtr theNode, std::string thePath, dom::NodePtr expectedResult) {
            NodeSetPtr myResult = xpath_evaluateSet(thePath, theNode);
            return myResult->count(expectedResult);
        }

        bool contains(dom::NodePtr theNode, std::string thePath, NodeSetPtr expectedResult) {
            NodeSetPtr myResult = xpath_evaluateSet(thePath, theNode);
            if (!includes(expectedResult->begin(), expectedResult->end(), myResult->begin(), myResult->end())) {
                return false;
            }
            return true;
        }

        bool contains(NodeSetPtr larger, NodeSetPtr smaller) {
            if (!includes(larger->begin(), larger->end(), smaller->begin(), smaller->end())) {
                return false;
            }
            return true;
        }

        bool parses(const std::string& theString) {
            return xpath_parse(theString);
        }

        void run() {

            std::string myXML = "<testDoc>\
                                 <body>\
                                 <junk name='valuable'>\
                                 <junk class='more' name='valuable'>not</junk>\
                                 <junk class='less' name='valuable'>not</junk>\
                                 bar\
                                 </junk>\
                                 <junk name='valuable2'>not</junk>\
                                 </body>\
                                 <numbers>\
                                 <number>1</number>\
                                 <number>2</number>\
                                 <number>3</number>\
                                 <number>4</number>\
                                 <number>5</number>\
                                 <number>6</number>\
                                 <number>7</number>\
                                 <number>8</number>\
                                 <number>9</number>\
                                 <number>0</number>\
                                 </numbers>\
                                 <footer>\
                                 <some>\
                                 <junk>blah</junk>\
                                 <junk class='more'>blub</junk>\
                                 </some>\
                                 </footer>\
                                 </testDoc>";




            /* <testDoc><body><junk name=\"valuable\">\
               <junk class=\"more\" name=\"valuable\">not</junk>bar</junk>\
               <junk name=\"valuable2\"/></body>\
               <numbers><number>1</number><number>2</number><number>3</number>\
               <number>4</number><number>5</number><number>6</number><number>7</number>\
               <number>8</number><number>9</number><number>0</number></numbers>\
               <footer><some><junk>blah</junk><junk class=\"more\">blub</junk></some>\
               </footer></testDoc>";*/

            dom::NodePtr doc( new dom::Document );
            std::istringstream instream(myXML);

            if (!(instream >> *doc)) {
                std::cout << "XML parse error.";
            } else {
                std::cout << "parsed document:\n" << *doc;
            }
            //std::string myPathString("/testDoc//junk[@name = \"valuable\" and @class = \"more\"]");
            //xpath::Path *myPath = xpath::xpath_parse(myPathString);
            //dom::Node *res = xpath::xpath_evaluate1(myPath, doc);
            //AC_PRINT << *res;
            /*OrderedNodeSetPtr values = xpath_evaluateOrderedSet("/testDoc//junk[@name = \"valuable2\"]", doc);
              std::cout << "result of content search: " << std::endl;
              for (OrderedNodeSet::iterator i = values->begin(); i != values->end(); ++i) {
              AC_PRINT << **i;
              }*/

            ENSURE(equals(doc, "testDoc", doc->childNode(0)));

            // test:  "\"]" is not a valid path ->expect empty parse result.

            ENSURE(!parses("\"]"));
            ENSURE(contains(doc, "/testDoc//junk[@name = \"valuable\" and @class = \"less\"]", doc->childNode(0)->childNode(0)->childNode(0)->childNode(1)));

            ENSURE(contains(doc, "/testDoc//junk[@name = \"valuable\"]", doc->childNode(0)->childNode(0)->childNode(0)));
            ENSURE(contains(doc, "/testDoc//junk[@name = \"valuable2\"]", doc->childNode(0)->childNode(0)->childNode(1)));

            ENSURE(contains(doc, "/testDoc//junk[@name = \"valuable2\"]/text()", doc->childNode(0)->childNode(0)->childNode(1)->childNode(0)));

            std::list<OrderedNodeSetPtr> containers;

            OrderedNodeSetPtr numbers = xpath_evaluateOrderedSet("//numbers", doc);
            ENSURE(numbers->size() == 1);
            containers.push_back(numbers);

            OrderedNodeSetPtr numberList = xpath_evaluateOrderedSet("//numbers/number", doc);
            ENSURE(numberList->size() == 10);
            containers.push_back(numberList);

            // nodeset-nodeset comparison greater, gequal, equal, lequal, less, notequal
            OrderedNodeSetPtr number1 = xpath_evaluateOrderedSet("//number[self::node() = ../number[1]]", doc);
            ENSURE(number1->size() == 1);
            containers.push_back(number1);

            OrderedNodeSetPtr number1_2 = xpath_evaluateOrderedSet("number[substring(self::node(),1) = ../number[1]]", *numbers->begin());
            ENSURE(equals(number1, number1_2));
            containers.push_back(number1_2);

            OrderedNodeSetPtr number1_3 = xpath_evaluateOrderedSet("number[substring(self::node(),1) = ../number[position() = ( 4 - 3 ) ] ]", *numbers->begin());
            ENSURE(equals(number1, number1_3));
            containers.push_back(number1_3);

            // find the <number> element whose numeric value
            // (being its std::string value converted to a number)
            // plus the numeric value of its successor
            // equals the std::string value of the third node, say, the String "3".
            // logically, this must be number one.
            OrderedNodeSetPtr number1_4 = xpath_evaluateOrderedSet("number[(self::node() + following::*) = ../number[3] ]", *numbers->begin());
            ENSURE(equals(number1_4, number1));
            containers.push_back(number1_4);

            // do a similar thing using the preceding::-axis
            OrderedNodeSetPtr number1_5 = xpath_evaluateOrderedSet("number[(self::node() + preceding::*) = ../number[1] ]", *numbers->begin());
            ENSURE(equals(number1_5, number1));
            containers.push_back(number1_5);

            // do a similar thing using the descendant-or-self::-axis
            OrderedNodeSetPtr number1_6 = xpath_evaluateOrderedSet("number[1 + descendant-or-self::* = ../number[2] ]", *numbers->begin());
            ENSURE(equals(number1_6, number1));
            containers.push_back(number1_6);

            // find an element named "numbers", which has a child node named "number"
            // whose numeric value is 7.
            OrderedNodeSetPtr numbers2 = xpath_evaluateOrderedSet("//numbers[number = 7]", doc);
            ENSURE(equals(numbers, numbers2));
            containers.push_back(numbers2);

            // find an element named "numbers", which has a child node named "number"
            // whose numeric value is greater than 7.
            OrderedNodeSetPtr numbers3 = xpath_evaluateOrderedSet("//numbers[number >= 7]", doc);
            ENSURE(equals(numbers, numbers3));
            containers.push_back(numbers3);

            OrderedNodeSetPtr numbers4 = xpath_evaluateOrderedSet("//numbers[number >= 11]", doc);
            ENSURE(numbers4->size() == 0);
            containers.push_back(numbers4);

            OrderedNodeSetPtr numbers4_2 = xpath_evaluateOrderedSet("//numbers[number = 11]", doc);
            ENSURE(numbers4_2->size() == 0);
            containers.push_back(numbers4_2);

            OrderedNodeSetPtr numbers5 = xpath_evaluateOrderedSet("//numbers[number < 11]", doc);
            ENSURE(equals(numbers, numbers5));
            containers.push_back(numbers5);

            OrderedNodeSetPtr numbers6 = xpath_evaluateOrderedSet("//numbers[number != 11]", doc);
            ENSURE(equals(numbers, numbers6));
            containers.push_back(numbers6);

            OrderedNodeSetPtr numbers7 = xpath_evaluateOrderedSet("//numbers[number != 1]", doc);
            ENSURE(equals(numbers, numbers7));
            containers.push_back(numbers7);

            OrderedNodeSetPtr numbers8 = xpath_evaluateOrderedSet("//numbers[number = 1]", doc);
            ENSURE(equals(numbers, numbers8));
            containers.push_back(numbers8);

            OrderedNodeSetPtr numbers9 = xpath_evaluateOrderedSet("//numbers/number[self::node() < 7]", doc);
            ENSURE(numbers9->size() == 7);
            ENSURE(contains(numberList, numbers9));
            containers.push_back(numbers9);

            //ENSURE(!parses("//numbers/number[position() <= sum(../number[self::node() < 3))]"));

            OrderedNodeSetPtr numbers10 = xpath_evaluateOrderedSet("//numbers/number[position() <= 6]", doc);
            ENSURE(numbers10->size() == 6);
            ENSURE(contains(numberList, numbers10));
            containers.push_back(numbers10);

            OrderedNodeSetPtr numbers11 = xpath_evaluateOrderedSet("//numbers/number[position() < 7]", doc);
            ENSURE(numbers11->size() == 6);
            ENSURE(equals(numbers10, numbers11));
            containers.push_back(numbers11);

            OrderedNodeSetPtr numbers12 = xpath_evaluateOrderedSet("//numbers/number[position() = sum(../number[position()<4])]", doc);
            ENSURE(numbers12->size() == 1);
            ENSURE(contains(numbers10, numbers12));
            containers.push_back(numbers12);

            OrderedNodeSetPtr number6 = xpath_evaluateOrderedSet("//numbers/number[6]", doc);
            ENSURE(equals(number6, numbers12));
            containers.push_back(number6);

            OrderedNodeSetPtr numbers13 = xpath_evaluateOrderedSet("//numbers/number[self::node() = 6 or self::node() = 5]", doc);
            ENSURE(numbers13->size() == 2);
            ENSURE(contains(numbers10, numbers13));
            containers.push_back(numbers13);

            OrderedNodeSetPtr numbers14 = xpath_evaluateOrderedSet("//numbers/number[self::node() >= 5 and self::node() < 7]", doc);
            ENSURE(numbers14->size() == 2);
            ENSURE(equals(numbers14, numbers13));
            containers.push_back(numbers14);

            OrderedNodeSetPtr number62 = xpath_evaluateOrderedSet("//numbers/number[text() = 6]", doc);
            ENSURE(number62->size() == 1);
            ENSURE(equals(number62, number6));
            containers.push_back(number62);

            OrderedNodeSetPtr number63 = xpath_evaluateOrderedSet("//numbers/number[position() = last()-4]", doc);
            ENSURE(number63->size() == 1);
            ENSURE(equals(number63, number62));
            containers.push_back(number63);

            OrderedNodeSetPtr numbers15 = xpath_evaluateOrderedSet("//*[count(number) = 10]", doc);
            ENSURE(equals(numbers15, numbers));
            containers.push_back(numbers15);

            // XXX tests fail
            // xpath interpreter seems to have problems with the last() function in case its used without position() function       
            //        OrderedNodeSetPtr number64 = xpath_evaluateOrderedSet("//numbers/number[last()-4]", doc);
            //        ENSURE(number64->size() == 1);
            //        ENSURE(equals(number64, number62));
            //        containers.push_back(number64);

            // xpath interpreter can not handle "|"        
            //        OrderedNodeSetPtr numbers16 = xpath_evaluateOrderedSet("//numbers/number | //numbers/number[text() = 5]", doc);
            //        ENSURE(numbers16->size() == 2);
            //        ENSURE(equals(numbers16, numbers13));
            //        containers.push_back(numbers16);


            // missing tests:
            //
            // - functions
            //
            //  substring-before
            //  substring-after
            //  substring
            //  concat
            //  starts-with
            //  
            //  true
            //  false
            //  not
            //  boolean
            //  number
            //  std::string
            //  sum
            //  floor
            //  ceiling
            //  round
            //  stringlength
            //  normalize-space
            //  translate

            /*
               memleak test
               std::string myPath = "/testDoc";
               std::string myAbsPathToJunk = "/testDoc/body//junk[@content = \"valuable\"]/text()";

               for (int i = 0; i < 100; i++) {

               xpath::evaluate(myAbsPathToJunk, documentElement);
               }
             */
        }
};

class MyTestSuite : public UnitTestSuite {
    public:
        MyTestSuite(const char * myName, int argc, char *argv[]) :
            UnitTestSuite(myName, argc, argv) {}
        void setup() {
            UnitTestSuite::setup(); // called to print a launch message
            addTest(new xpath_UnitTest);
        }
};

int main(int argc, char **argv) {
    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();
    std::cerr << ">> Finished test suite '" << argv[0] << "'"
        << ", return status = " << mySuite.returnStatus() << std::endl;

    return mySuite.returnStatus();
}
