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

#ifndef _AC_CONTRAST_H_
#define _AC_CONTRAST_H_

#include "Algorithm.h"

namespace y60 {
	class Contrast : public Algorithm {
		public:
            Contrast(const std::string & theName);

			static std::string getName() { return "contrast"; }
		    void onFrame(double t);
            
            void configure(const dom::Node & theNode);
	        const dom::Node & result() const { 
		        return _myResultNode;
	        }

		private:
            void updateLookupTable();
            dom::Element  _myResultNode;
            y60::ImagePtr _mySourceImage;
            y60::ImagePtr _myTargetImage;
            
            float _myUpper;
            float _myLower;
            
            std::vector<unsigned char> _myLookupTable;
    };
}

#endif


