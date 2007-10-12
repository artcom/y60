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
#include <y60/Scene.h>

namespace y60 {

	typedef asl::raster<asl::BGR> BGRRaster;
	typedef asl::raster<asl::RGB> RGBRaster;

	/**
	* @ingroup Y60video
	* video processing interface class 
	* implement onFrame for processing a frame
    * implement configure to handle xml-node parameters 
	* parameters should contain necessary image ids
	* ouput is handled by result()
	*/
	class Algorithm {
	public:
        Algorithm(const std::string & theName) { 
            _myName = theName;
        }   
		virtual void configure(const dom::Node & theNode) = 0; 

		virtual const dom::Node & result() const { 
			AC_WARNING << "Algorithm::result not implemented.";
			static dom::Node dummy;
			return dummy;
		}
		virtual void onFrame(double t) = 0;
        
        virtual void setScene(const y60::ScenePtr & theScene) {
            _myScene = theScene;
        }
        
        virtual std::string getAlgorithmName() const {
            return _myName;
        }   

    protected:
        y60::ScenePtr _myScene;
        std::string _myName;
	};

	typedef asl::Ptr<Algorithm> AlgorithmPtr;

	/**********************************************************/
	//demo algorithm

	class TestAlgorithm : public Algorithm {
	public:
        TestAlgorithm(const std::string & theName) : Algorithm( theName )  { }

        void configure(const dom::Node & theNode) {
            
        }
		static std::string getName() { return "test"; }

		void onFrame(double t) {
			int x = 0, y = 0;
			const BGRRaster * myFrame = dom::dynamic_cast_Value<BGRRaster>(&*_mySourceImage);
			std::cout << "raster size " << myFrame->xsize() << "x" << myFrame->ysize() 
				<< " value " << x << "," << y << " = " 
				<< int( getRedValue((*myFrame)(x,y)) ) << "," 
				<< int( getGreenValue((*myFrame)(x,y)) )<< ","   
				<< int( getBlueValue((*myFrame)(x,y)) ) 
				<< std::endl;
		}
    private:
        dom::ValuePtr _mySourceImage;
	};
}

#endif

