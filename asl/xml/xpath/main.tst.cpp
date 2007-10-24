#include "asl/UnitTest.h"

#include "xpath_api.h"
#include "Value.h"

using namespace xpath;

class xpath_UnitTest : public UnitTest {
public:
    xpath_UnitTest() : UnitTest("xpath_UnitTest") {  }
    
    bool equals(OrderedNodeSetRef the, OrderedNodeSetRef same)
    {
	bool retval = true;
	if (!includes(the->begin(), the->end(), same->begin(), same->end())) {
	    retval = false;
	}
	if (!includes(same->begin(), same->end(), the->begin(), the->end())) {
	    retval = false;
	}
	return retval;
    }

    bool equals(NodeRef theNode, std::string thePath, OrderedNodeSetRef expectedResult)
    {
	OrderedNodeSetRef myResult = xpath_evaluateOrderedSet(thePath, theNode);
	bool retval = true;
	if (!equals(myResult, expectedResult)) {
	    retval = false;
	}
	delete myResult;
	return retval;
    }

    bool equals(NodeRef theNode, std::string thePath, NodeRef expectedResult)
    {
	OrderedNodeSetRef expectedResultSet = new OrderedNodeSet();
	expectedResultSet->insert(expectedResult);
	bool retval = equals(theNode, thePath, expectedResultSet);
	delete expectedResultSet;
	return retval;
    }

    bool contains(OrderedNodeSetRef larger, OrderedNodeSetRef smaller)
    {
	bool retval = true;
	if (!includes(larger->begin(), larger->end(), smaller->begin(), smaller->end())) {
	    AC_WARNING << "no containment for set ";
	    for (OrderedNodeSet::iterator i = smaller->begin(); i != smaller->end(); ++i) {
		AC_WARNING << **i;
	    }
	    AC_WARNING << "in ";
	    for (OrderedNodeSet::iterator i = larger->begin(); i != larger->end(); ++i) {
		AC_WARNING << **i;
	    }
		    
	    retval = false;
	}
	return retval;
    }

    bool contains(NodeRef theNode, std::string thePath, NodeRef expectedResult)
    {
	NodeSetRef myResult = xpath_evaluateSet(thePath, theNode);
	bool retval = myResult->count(expectedResult);
	delete myResult;
	return retval;
    }

    bool contains(NodeRef theNode, std::string thePath, NodeSetRef expectedResult)
    {
	NodeSetRef myResult = xpath_evaluateSet(thePath, theNode);
	bool retval = true;
	if (!includes(expectedResult->begin(), expectedResult->end(), myResult->begin(), myResult->end())) {
	    retval = false;
	}
	delete myResult;
	return retval;
    }

    bool contains(NodeSetRef larger, NodeSetRef smaller)
    {
	bool retval = true;
	if (!includes(larger->begin(), larger->end(), smaller->begin(), smaller->end())) {
	    retval = false;
	}
	return retval;
    }

    bool parses(std::string theString) {
	Path *p = xpath_parse(theString);
	if (p) {
	    xpath_return(p);
	    return true;
	}
	return false;
    }

    void run() {

	std::string myXML = "<testDoc><body><junk>foo</junk><junk content=\"valuable\">\
                         <junk class=\"more\" content=\"valuable\">not</junk>bar</junk></body>\
                         <numbers><number>1</number><number>2</number><number>3</number>\
                         <number>4</number><number>5</number><number>6</number><number>7</number>\
                         <number>8</number><number>9</number><number>0</number></numbers>\
                         <footer><some><junk>blah</junk><junk class=\"more\">blub</junk></some>\
                         </footer></testDoc>";
	
	dom::Document doc;
	std::istringstream instream(myXML);
	
	if (!(instream >> doc)) {
	    std::cout << "XML parse error.";
	} else {
	    std::cout << "parsed document:\n" << doc;
	}

	ENSURE(equals(&doc, "testDoc", &*doc.childNode(0)));

	// test:  "\"]" is not a valid path ->expect empty parse result.

	ENSURE(!parses("\"]"));

	ENSURE(contains(&doc, "/testDoc//junk[@content = \"valuable\"]", &*doc.childNode(0)->childNode(0)->childNode(1)));
	ENSURE(contains(&doc, "/testDoc//junk[@content = \"valuable\"]", &*doc.childNode(0)->childNode(0)->childNode(1)->childNode(0)));

	ENSURE(contains(&doc, "/testDoc//junk[@content = \"valuable\"]/text()", &*doc.childNode(0)->childNode(0)->childNode(1)->childNode(0)->childNode(0)));

	OrderedNodeSetRef numbers = xpath_evaluateOrderedSet("//numbers", &doc);
	ENSURE(numbers->size() == 1);
	OrderedNodeSetRef numberList = xpath_evaluateOrderedSet("//numbers/number", &doc);
	ENSURE(numberList->size() == 10);

	// nodeset-nodeset comparison greater, gequal, equal, lequal, less, notequal
        OrderedNodeSetRef number1 = xpath_evaluateOrderedSet("//number[self::node() = ../number[1]]", &doc);
	ENSURE(number1->size() == 1);

        OrderedNodeSetRef number1_2 = xpath_evaluateOrderedSet("number[substring(self::node(),1) = ../number[1]]", *numbers->begin());
	ENSURE(equals(number1, number1_2));

        OrderedNodeSetRef number1_3 = xpath_evaluateOrderedSet("number[substring(self::node(),1) = ../number[position() = ( 4 - 3 ) ] ]", *numbers->begin());
	ENSURE(equals(number1, number1_3));

	// find the <number> element whose numeric value
	// (being its string value converted to a number)
	// plus the numeric value of its successor
	// equals the string value of the third node, say, the String "3".
	// logically, this must be number one.
        OrderedNodeSetRef number1_4 = xpath_evaluateOrderedSet("number[(self::node() + following::*) = ../number[3] ]", *numbers->begin());
	ENSURE(equals(number1_4, number1));

	// do a similar thing using the preceding::-axis
        OrderedNodeSetRef number1_5 = xpath_evaluateOrderedSet("number[(self::node() + preceding::*) = ../number[1] ]", *numbers->begin());
	ENSURE(equals(number1_5, number1));

	// do a similar thing using the descendant-or-self::-axis
        OrderedNodeSetRef number1_6 = xpath_evaluateOrderedSet("number[1 + descendant-or-self::* = ../number[2] ]", *numbers->begin());
	ENSURE(equals(number1_6, number1));

	// find an element named "numbers", which has a child node named "number"
	// whose numeric value is 7.
	OrderedNodeSetRef numbers2 = xpath_evaluateOrderedSet("//numbers[number = 7]", &doc);
	ENSURE(equals(numbers, numbers2));

	// find an element named "numbers", which has a child node named "number"
	// whose numeric value is greater than 7.
	OrderedNodeSetRef numbers3 = xpath_evaluateOrderedSet("//numbers[number >= 7]", &doc);
	ENSURE(equals(numbers, numbers3));

	OrderedNodeSetRef numbers4 = xpath_evaluateOrderedSet("//numbers[number >= 11]", &doc);
	ENSURE(numbers4->size() == 0);

	OrderedNodeSetRef numbers4_2 = xpath_evaluateOrderedSet("//numbers[number = 11]", &doc);
	ENSURE(numbers4_2->size() == 0);

	OrderedNodeSetRef numbers5 = xpath_evaluateOrderedSet("//numbers[number < 11]", &doc);
	ENSURE(equals(numbers, numbers5));

	OrderedNodeSetRef numbers6 = xpath_evaluateOrderedSet("//numbers[number != 11]", &doc);
	ENSURE(equals(numbers, numbers6));

	OrderedNodeSetRef numbers7 = xpath_evaluateOrderedSet("//numbers[number != 1]", &doc);
	ENSURE(equals(numbers, numbers7));

	OrderedNodeSetRef numbers8 = xpath_evaluateOrderedSet("//numbers[number = 1]", &doc);
	ENSURE(equals(numbers, numbers8));

	OrderedNodeSetRef numbers9 = xpath_evaluateOrderedSet("//numbers/number[self::node() < 7]", &doc);
	ENSURE(numbers9->size() == 7);
	ENSURE(contains(numberList, numbers9));

	//ENSURE(!parses("//numbers/number[position() <= sum(../number[self::node() < 3))]"));

	OrderedNodeSetRef numbers10 = xpath_evaluateOrderedSet("//numbers/number[position() <= 6]", &doc);
	ENSURE(numbers10->size() == 6);
	ENSURE(contains(numberList, numbers10));

	OrderedNodeSetRef numbers11 = xpath_evaluateOrderedSet("//numbers/number[position() < 7]", &doc);
	ENSURE(numbers11->size() == 6);
	ENSURE(equals(numbers10, numbers11));

	OrderedNodeSetRef numbers12 = xpath_evaluateOrderedSet("//numbers/number[position() = sum(../number[position()<4])]", &doc);
	ENSURE(numbers12->size() == 1);
	ENSURE(contains(numbers10, numbers12));

	OrderedNodeSetRef number6 = xpath_evaluateOrderedSet("//numbers/number[6]", &doc);
	ENSURE(equals(number6, numbers12));

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
	//  string
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
        delete numbers;
        delete numberList;
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
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
