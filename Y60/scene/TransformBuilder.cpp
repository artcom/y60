#include "TransformBuilder.h"
#include <y60/NodeNames.h>

namespace y60 {
    TransformBuilder::TransformBuilder(const std::string & theName)
        : TransformBuilderBase(TRANSFORM_NODE_NAME, theName)
    {}
}
