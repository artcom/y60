#include "Algorithm.h"

namespace y60 {

    class Canny : public Algorithm {

        public: 

            Canny( const std::string & theName );
            static std::string getName() {
                return "canny";
            }
            void onFrame( double t );
            void configure( const dom::Node & theNode );

        private:

            unsigned _myWidth;
            unsigned _myHeight;

            y60::ImagePtr _mySourceImage;
            std::vector<float> _myGradient
            y60::ImagePtr _myTargetImage;

    };

}
