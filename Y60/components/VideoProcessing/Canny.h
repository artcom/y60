#ifndef _CANNY_H_
#define _CANNY_H_

#include "Algorithm.h"

namespace y60 {

    enum output_t {
        CANNY_OUTPUT,
        GRADIENT_OUTPUT,
        DIRECTION_OUTPUT
    };

    class Canny : public Algorithm {

        public: 

            Canny( const std::string & theName );
            static std::string getName() {
                return "canny";
            }
            void onFrame( double t );
            void configure( const dom::Node & theNode );

        private:

            bool isOnEdge(unsigned theIndex, int theXOffset, int theYOffset);
            bool doHysteresisStep();

            unsigned _myWidth;
            unsigned _myHeight;

            float _myLowThreshold;
            float _myHighThreshold;
            float _myGradientThreshold;
            float _myMaxGradient;

            output_t _myOutput;

            y60::ImagePtr _mySourceImage;
            y60::ImagePtr _myTargetImage;
            y60::ImagePtr _myDirectionImage;

            std::vector<float> _myGradients;
            std::vector<float> _myDirections;
            std::vector<float> _myResultImage;
            std::vector<float> _myResult;
    };
}

#endif // _CANNY_H_
