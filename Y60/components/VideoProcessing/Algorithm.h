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

#ifndef _ac_VideoProcessing_Algorithm_h_
#define _ac_VideoProcessing_Algorithm_h_

#include <y60/Image.h>

namespace y60 {

	typedef asl::raster<asl::BGR> BGRRaster;
	typedef asl::raster<asl::RGB> RGBRaster;

	/**
	* @ingroup Y60video
	* video processing interface class 
	* implement onFrame for processing a frame
	* input parameters are handled by configure()
	* ouput is handled by result()
	*/
	class Algorithm {
	public:
		virtual void configure(const dom::Node & theNode) { 
			AC_WARNING << "Algorithm::configure not implemented. " << theNode;
		}
		virtual const dom::Node & result() const { 
			AC_WARNING << "Algorithm::result not implemented.";
			static dom::Node dummy;
			return dummy;
		}
		virtual void onFrame(dom::ValuePtr theRaster, double t) = 0;
	};

	typedef asl::Ptr<Algorithm> AlgorithmPtr;

	/**********************************************************/
	//demo algorithm

	class TestAlgorithm : public Algorithm {
	public:
		static std::string getName() { return "test"; }

		void onFrame(dom::ValuePtr theRaster, double t) {
			int x = 0, y = 0;
			const BGRRaster * myFrame = dom::dynamic_cast_Value<BGRRaster>(&*theRaster);
			std::cout << "raster size " << myFrame->xsize() << "x" << myFrame->ysize() 
				<< " value " << x << "," << y << " = " 
				<< int( getRedValue((*myFrame)(x,y)) ) << "," 
				<< int( getGreenValue((*myFrame)(x,y)) )<< ","   
				<< int( getBlueValue((*myFrame)(x,y)) ) 
				<< std::endl;
		}
	};
}

#endif

