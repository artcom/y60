/**************************************************************************
 *                                                                        *
 *  This file is part of TaXTable, an XPath implementation for Y60.       *
 *                                                                        *
 *                                                                        *
 *  (C) 2005-2007  Tobias Anton <tobias.anton@web.de>                     *
 *                                                                        *
 *                                                                        *
 *  TaXTable is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  TaXTable is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with TaXTable.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                        *
 **************************************************************************/

#ifndef XPATH_API_H
#define XPATH_API_H

#include <stdlib.h>
#include <dom/Nodes.h>

#include "Types.h"

namespace xpath {

    class Path;

    // yields a path pointer to the responsibility of the caller.
    Path *xpath_parse(const std::string &instring);

    // have library delete previously parse()'d Path *
    void xpath_return(Path *);

    // evaluate path on startingElement into results
    void xpath_evaluate(Path *, dom::Node *startingElement, std::vector<dom::NodePtr> &results);
    void xpath_evaluate(std::string, dom::Node *startingElement, std::vector<dom::NodePtr> &results);

    dom::Node *xpath_evaluate1(Path *, dom::Node *);

    OrderedNodeSetRef xpath_evaluateOrderedSet(Path *, dom::Node *);
    OrderedNodeSetRef xpath_evaluateOrderedSet(std::string, dom::Node *);

    std::set<dom::Node *> *xpath_evaluateSet(Path *, dom::Node *);
    std::set<dom::Node *> *xpath_evaluateSet(std::string, dom::Node *);

    std::vector<dom::Node *> *xpath_evaluate(Path *, dom::Node *);
    std::vector<dom::Node *> *xpath_evaluate(std::string, dom::Node *);
};

#endif
