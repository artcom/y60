#ifndef XPATH_PARSER_H
#define XPATH_PARSER_H

#include "value.h"

namespace xpath {

    NodeSetRef evaluate(std::string path, xpath::NodeRef startingElement);

}

#endif
