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

#include "ShotDetection.h"

namespace y60 {

	ShotDetectionAlgorithm::ShotDetectionAlgorithm() : _myResultNode("shots") {
		_myThreshold = 0.9;
		_myMinimalShotLength = 1.0;
		_myLastShotTime = 0;
		clearHistogram(0);
		clearHistogram(1);
	}

	void 
    ShotDetectionAlgorithm::onFrame(dom::ValuePtr theRaster, double theTime) {
    	const BGRRaster * myFrame = dom::dynamic_cast_Value<BGRRaster>(&*theRaster);
		static int n = 0;

		//clear current histogram
		clearHistogram(n);
	    
		//compute histogram for current frame 
		int myScaled;
		int myFactors[] = {1, BINS_PER_CHANNEL, BINS_PER_CHANNEL*BINS_PER_CHANNEL };
		BGRRaster::const_iterator it;
		for (it = myFrame->begin(); it != myFrame->end(); ++it) {
			int myBin = 0;
			for (int c = 0; c < 3; ++c) {
				myScaled = (BINS_PER_CHANNEL * int((*it)[c])) / 256;
				myBin += myFactors[c] * myScaled;
			}
	        
			/*
				AC_ERROR << myBin 
					<< " color " << int((*it)[0]) << "," 
					<< int((*it)[1]) << "," << int((*it)[2]);
			*/
			_myHistogram[n][myBin]++;
		}
	    
		if (_myResultNode.childNodesLength() == 0 && theTime > 0.5) {
			//wait some time before taking the first shot
			dom::Element myFirstShotNode("shot");
			myFirstShotNode["start"] = asl::as_string(0);
			addHistogram(n, myFirstShotNode);
			_myResultNode.appendChild(myFirstShotNode);
		}

		//printHistogram(n);

		//compute difference to histogram of last frame
		float myDifference = float(intersectHistograms())/myFrame->size();
		double myLength = theTime - _myLastShotTime;
	    
		if (_myResultNode.childNodesLength() > 0) {
			//update length of current shot node
			dom::NodePtr myCurrentShot 
				= _myResultNode.childNode(_myResultNode.childNodesLength()-1);
			(*myCurrentShot)["length"] = asl::as_string(myLength);
		}

		AC_DEBUG << "************** diff at " << theTime << " is " << myDifference; 
		if (myDifference < _myThreshold) {
			//we have a potential shot
	        
			if (myLength > _myMinimalShotLength) {

				//start a new shot node
				dom::Element myShotNode("shot");
				myShotNode["start"] = asl::as_string(theTime);
				myShotNode["length"] = asl::as_string(0);

				addHistogram(n, myShotNode);
				_myResultNode.appendChild(myShotNode);                

				AC_INFO << "SHOT at " << theTime << " last shot length " << myLength 
						<< " (" << myDifference << ")";
			}
			_myLastShotTime = theTime;
		}

		//toggle current histogram
		n = 1 - n;
	}

	void 
	ShotDetectionAlgorithm::configure(const dom::Node & theNode) { 
		try {
			asl::fromString(theNode["threshold"].nodeValue(), _myThreshold);
			asl::fromString(theNode["minimal_length"].nodeValue(), _myMinimalShotLength);
		} catch(asl::Exception ex) {
			AC_ERROR << "could not parse configuration " << theNode;
		}
		AC_PRINT << "ShotDetection::configure threshold " << _myThreshold;
		AC_PRINT << "ShotDetection::configure min. shot length " << _myMinimalShotLength;
	}

	const dom::Node & 
	ShotDetectionAlgorithm::result() const { 
		return _myResultNode;
	}

	void 
	ShotDetectionAlgorithm::clearHistogram(int theIndex) {
		for(int b = 0; b < NBINS; ++b) {
			_myHistogram[theIndex][b] = 0;
		}
	}
	    
	unsigned long 
	ShotDetectionAlgorithm::intersectHistograms() {
		unsigned long myTotal = 0;
		for(int b = 0; b < NBINS; ++b) {
			myTotal += std::min(_myHistogram[0][b], _myHistogram[1][b]);
		}
		return myTotal;
	}
	    
		//debug
	void 
	ShotDetectionAlgorithm::printHistogram(int theIndex) {
		for(int b = 0; b < NBINS; ++b) {
			std::cout << _myHistogram[theIndex][b] << " ";
		}
		std::cout << std::endl;
	}

	void 
	ShotDetectionAlgorithm::addHistogram(int theIndex, dom::Node & theNode) {
		y60::VectorOfUnsignedInt myHistogram(_myHistogram[theIndex], 
											_myHistogram[theIndex] + NBINS);
		theNode("histogram").appendChild( dom::Text(asl::as_string(myHistogram)) );
	}
}
