#ifndef _MASKPERSON_H_
#define _MASKPERSON_H_

#include "Algorithm.h"

namespace y60 {

    class MaskPerson : public Algorithm {
        public:

            MaskPerson( const std::string & theName );
            static std::string getName() {
                return "maskperson";
            }
            void onFrame( double t );
            void configure( const dom::Node & theNode );

        private:
    
            unsigned int _myThreshold;
            unsigned int _myBlurRadius;
            unsigned int _myGradientRadius;

            y60::ImagePtr _mySourceImage;
            y60::ImagePtr _myTargetImage;
            y60::ImagePtr _myTargetMask;

    };

}

#endif

