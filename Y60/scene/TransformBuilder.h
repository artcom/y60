#ifndef AC_SOM_TRANSFORM_BUILDER_INCLUDED
#define AC_SOM_TRANSFORM_BUILDER_INCLUDED

#include "TransformBuilderBase.h"

namespace y60 {

    class TransformBuilder : public TransformBuilderBase {
        public:
            TransformBuilder(const std::string & theName);
            virtual ~TransformBuilder() {};
        private:            

    };

    typedef asl::Ptr<TransformBuilder> TransformBuilderPtr;

}
 
#endif //  AC_SOM_TRANSFORM_BUILDER_INCLUDED
