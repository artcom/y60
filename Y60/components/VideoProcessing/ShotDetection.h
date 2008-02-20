//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_VideoProcessing_ShotDetection_h_
#define _ac_VideoProcessing_ShotDetection_h_

#include "Algorithm.h"

namespace y60 {

	/**
	* @ingroup Y60video
	* a rudimentary shot-detection algorithm 
	* based on color histogram differences between consecutive frames
	*/
	class ShotDetectionAlgorithm : public Algorithm {
		public:
			static std::string getName() { return "shot-detection"; }

			ShotDetectionAlgorithm(const std::string & theName);
				       
		    virtual void onFrame(double t);
			void configure(const dom::Node & theNode);
			const dom::Node & result() const;

		private:
			static const int BINS_PER_CHANNEL = 5;
			static const int NBINS = BINS_PER_CHANNEL*BINS_PER_CHANNEL*BINS_PER_CHANNEL;
			unsigned long _myHistogram[2][NBINS];
			double _myThreshold;
			double _myLastShotTime, _myMinimalShotLength;
			dom::Element _myResultNode;
            dom::ValuePtr _mySourceRaster;

			void clearHistogram(int theIndex);    	    
		    unsigned long intersectHistograms();
    	        
		    //debug
		    void printHistogram(int theIndex);
		    void addHistogram(int theIndex, dom::Node & theNode);	    
	};
}

#endif

