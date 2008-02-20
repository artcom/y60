//============================================================================
//
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _AC_BACKGROUND_SUBTRACTION_H_
#define _AC_BACKGROUND_SUBTRACTION_H_

#include "Algorithm.h"



namespace y60 {



	class BackgroundSubtraction : public Algorithm {
		public:
            BackgroundSubtraction(const std::string & theName);

			static std::string getName() { return "backgroundsubtraction"; }
		    void onFrame(double t);
            
            void configure(const dom::Node & theNode);
	        const dom::Node & result() const { 
		        return _myResultNode;
	        }



		private:
            unsigned char clampedSub(unsigned char theFirstValue, unsigned char theSecondValue);
            
            dom::Element _myResultNode;
            y60::ImagePtr _mySourceImage;
            y60::ImagePtr _myTargetImage;
            y60::ImagePtr _myBackgroundImage;
            
            float _myThreshold;
            float _myWeight;
            unsigned int _myCounter;
    };
}

#endif
