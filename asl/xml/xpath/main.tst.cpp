#include "asl/UnitTest.h"
#include "parser.h"

using namespace xpath;

class xpath_UnitTest : public UnitTest {
public:
    xpath_UnitTest() : UnitTest("xpath_UnitTest") {  }
    
    bool search_equals(NodeRef theNode, std::string thePath, NodeSetRef expectedResult)
    {
	NodeSetRef myResult = evaluate(thePath, theNode);
	bool retval = true;
	if (!std::includes(myResult->begin(), myResult->end(), expectedResult->begin(), expectedResult->end())) {
	    retval = false;
	}
	if (!std::includes(expectedResult->begin(), expectedResult->end(), myResult->begin(), myResult->end())) {
	    retval = false;
	}
	delete myResult;
	return retval;
    }

    bool search_equals(NodeRef theNode, std::string thePath, NodeRef expectedResult)
    {
	NodeSetRef expectedResultSet = new NodeSet();
	expectedResultSet->insert(expectedResult);
	bool retval = search_equals(theNode, thePath, expectedResultSet);
	delete expectedResultSet;
	return retval;
    }

    bool search_contains(NodeRef theNode, std::string thePath, NodeRef expectedResult)
    {
	NodeSetRef myResult = evaluate(thePath, theNode);
	bool retval = myResult->count(expectedResult);
	delete myResult;
	return retval;
    }

    bool search_contains(NodeRef theNode, std::string thePath, NodeSetRef expectedResult)
    {
	NodeSetRef myResult = evaluate(thePath, theNode);
	bool retval = true;
	if (!includes(expectedResult->begin(), expectedResult->end(), myResult->begin(), myResult->end())) {
	    retval = false;
	}
	delete myResult;
	return retval;
    }

    bool parses(std::string theString) {
	Path *p = xpath_parse(theString);
	if (p) {
	    delete p;
	    return true;
	}
	return false;
    }

    void run() {

	std::string myXML = "<testDoc><body><junk>foo</junk><junk content=\"valuable\">\
                         <junk class=\"more\" content=\"valuable\">not</junk>bar</junk></body>\
                         <footer><some><junk>blah</junk><junk class=\"more\">blub</junk></some>\
                         </footer></testDoc>";
	
	dom::Document doc;
	std::istringstream instream(myXML);
	
	if (!(instream >> doc)) {
	    std::cout << "XML parse error.";
	} else {
	    std::cout << "parsed document:\n" << doc;
	}

	ENSURE(search_equals(&doc, "testDoc", &*doc.childNode(0)));

	// test:  "\"]" is not a valid path ->expect empty parse result.

	ENSURE(!parses("\"]"));

	// test:
	// nodeset-nodeset comparison greater, gequal, equal, lequal, less, notequal
	// nodeset-number comparison


	/*
	  std::string myPath = "/testDoc";
	  std::string myAbsPathToJunk = "/testDoc/body//junk[@content = \"valuable\"]/text()";
	  
	  for (int i = 0; i < 100; i++) {

	  xpath::evaluate(myAbsPathToValuableContent, documentElement);
	  xpath::evaluate(myAbsPathToJunk, documentElement);
	  }
	*/

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
