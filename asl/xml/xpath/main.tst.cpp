#include "parser.h"

int main(int argc, char **argv) {
    std::string myXML = "<testDoc><body><junk>foo</junk><junk content=\"valuable\"><junk class=\"more\" content=\"valuable\">not</junk>bar</junk></body>\
                         <footer><some><junk>blah</junk><junk class=\"more\">blub</junk></some></footer></testDoc>";

    dom::Document root;
    std::istringstream instream(myXML);
    if (!(instream >> root)) {
	std::cout << "XML parse error.";
    } else {
	std::cout << "parsed document " << root;
    }

    xpath::NodeRef documentElement = &*root.childNode(0);

    std::string myPathToValuableContent = "/testDoc/body//junk[@content = \"valuable\"]";
    std::string myAbsPathToValuableContent = "/testDoc/body//junk[@content = \"valuable\"]";
    std::string myAbsPathToJunk = "/testDoc/body//junk[@content = \"valuable\"]/text()";

    for (int i = 0; i < 100; i++) {
	xpath::evaluate(myPathToValuableContent, documentElement);
	xpath::evaluate(myAbsPathToValuableContent, documentElement);
	xpath::evaluate(myAbsPathToJunk, documentElement);
    }
}
