#ifndef XPATH_PARSER_H
#define XPATH_PARSER_H

#include "xpath/value.h"

namespace xpath {

    NodeSetRef evaluate(std::string path, xpath::NodeRef documentElement);

}

#endif
