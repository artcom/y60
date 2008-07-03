//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef XPATH_API_H
#define XPATH_API_H

#include <stdlib.h>
#include <dom/Nodes.h>

#include "Types.h"

namespace xpath {

    class Path;

    // yields a path pointer to the responsibility of the caller.
    Path * xpath_parse(const std::string &instring);

    // have library delete previously parse()'d Path *
    void xpath_return(Path *);

    // evaluate path on startingElement into results
    void xpath_evaluate(Path *, dom::Node *startingElement,
            std::vector<dom::NodePtr> &results);
    void xpath_evaluate(std::string, dom::Node *startingElement,
            std::vector<dom::NodePtr> &results);

    dom::Node *xpath_evaluate1(Path *, dom::Node *);

    OrderedNodeSetRef xpath_evaluateOrderedSet(Path *, dom::Node *);
    OrderedNodeSetRef xpath_evaluateOrderedSet(std::string, dom::Node *);

    std::set<dom::Node *> *xpath_evaluateSet(Path *, dom::Node *);
    std::set<dom::Node *> *xpath_evaluateSet(std::string, dom::Node *);

    std::vector<dom::Node *> *xpath_evaluate(Path *, dom::Node *);
    std::vector<dom::Node *> *xpath_evaluate(std::string, dom::Node *);

    std::vector<dom::NodePtr> findAll(dom::Node & theRoot, const std::string & theExpression);
    dom::NodePtr find(dom::Node & theRoot, const std::string & theExpression);
};

#endif
