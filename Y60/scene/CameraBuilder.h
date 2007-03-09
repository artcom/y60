#ifndef AC_SOM_CAMERA_BUILDER_INCLUDED
#define AC_SOM_CAMERA_BUILDER_INCLUDED

#include "TransformBuilderBase.h"

#include <asl/Frustum.h>

namespace y60 {

    class CameraBuilder : public TransformBuilderBase {
        public:
            CameraBuilder(const std::string & theName);
            virtual ~CameraBuilder();

            void setHFov(const float theViewingAngle);
            void setShift( float theHShift, float theVShift);
        private:
            void getFrustum( asl::Frustum & theFrustum );
            void setFrustum( const asl::Frustum & theFrustum );
    };

    typedef asl::Ptr<CameraBuilder>    CameraBuilderPtr;
}


#endif // AC_SOM_CAMERA_BUILDER_INCLUDED
