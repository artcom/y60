#ifndef AC_SOM_CAMERA_BUILDER_INCLUDED
#define AC_SOM_CAMERA_BUILDER_INCLUDED

#include "TransformBuilderBase.h"

namespace y60 {

    class CameraBuilder : public TransformBuilderBase {
        public:
            CameraBuilder(const std::string & theName);
            virtual ~CameraBuilder();

            void setHFov(const double theViewingAngle);
        private:            
    };

    typedef asl::Ptr<CameraBuilder>    CameraBuilderPtr;
}


#endif // AC_SOM_CAMERA_BUILDER_INCLUDED
