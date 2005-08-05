#ifndef AC_SOM_BODY_BUILDER_INCLUDED
#define AC_SOM_BODY_BUILDER_INCLUDED

#include "TransformBuilderBase.h"

namespace y60 {
    class BodyBuilder : public TransformBuilderBase {
        public:
            BodyBuilder(const dom::NodePtr & theNode) :
                TransformBuilderBase(theNode)
            {}
            BodyBuilder(const std::string & theShapeId,
                        const std::string & theName = "");
            virtual ~BodyBuilder();

            void appendJointReference(std::string theJointId);
        private:            
    };

    typedef asl::Ptr<BodyBuilder>      BodyBuilderPtr;
}


#endif // AC_SOM_BODY_BUILDER_INCLUDED
