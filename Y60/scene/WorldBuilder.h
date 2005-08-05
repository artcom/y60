#ifndef AC_SOM_WORLD_BUILDER_INCLUDED
#define AC_SOM_WORLD_BUILDER_INCLUDED

#include "WorldBuilderBase.h"

#include <string>

namespace y60 {

    class WorldBuilder : public WorldBuilderBase {
        public:
            WorldBuilder();
            WorldBuilder(dom::NodePtr theNode) : WorldBuilderBase(theNode) {};
            virtual ~WorldBuilder() {
            };
        private:
    };

    typedef asl::Ptr<WorldBuilder> WorldBuilderPtr;
}

#endif // AC_SOM_WORLD_BUILDER_INCLUDED
