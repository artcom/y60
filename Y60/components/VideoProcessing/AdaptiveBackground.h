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

#ifndef _AC_ADAPTIVEBACKGROUND_H_
#define _AC_ADAPTIVEBACKGROUND_H_

#include "Algorithm.h"

namespace y60 {
	class AdaptiveBackground : public Algorithm {
		public:
            AdaptiveBackground(const std::string & theName);

			static std::string getName() { return "adaptivebackground"; }
		    void onFrame(double t);
            
            void configure(const dom::Node & theNode);
	        const dom::Node & result() const { 
		        return _myResultNode;
	        }

		private:
            float lookup(float theValue);
            float revlookup(float theValue);
            dom::Element  _myResultNode;
            y60::ImagePtr _mySourceImage;
            y60::ImagePtr _myBlobImage;
            y60::ImagePtr _myTargetImage;
            float _myStrength;
            unsigned int _myCounter;
    };
}

#endif

