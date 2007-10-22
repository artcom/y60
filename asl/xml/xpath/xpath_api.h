#ifndef XPATH_API_H
#define XPATH_API_H

#include <stdlib.h>
#include <dom/Nodes.h>

namespace xpath {

    class Path;

    // yields a path pointer to the responsibility of the caller.
    Path *xpath_parse(const std::string &instring);

    // have library delete previously parse()'d Path *
    void xpath_return(Path *);

    void xpath_evaluate(Path *, dom::Node *startingElement, std::vector<dom::NodePtr> &results);

    dom::Node *xpath_evaluate1(Path *, dom::Node *);
    std::set<dom::Node *> *xpath_evaluate(Path *, dom::Node *);
    std::set<dom::Node *> *xpath_evaluate(std::string path, dom::Node *);

};

#endif
