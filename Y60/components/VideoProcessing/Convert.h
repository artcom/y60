#include "Algorithm.h"

namespace y60 {
    class Convert : public Algorithm {

        public: 

            Convert( const std::string & theName ) : Algorithm( theName ) {}
            void configure( const dom::Node & theNode );
            static std::string getName() { return "convert"; }
            void onFrame(double t);

        private:

            y60::ImagePtr _mySourceImage;
            y60::ImagePtr _myTargetImage;
    };
}


