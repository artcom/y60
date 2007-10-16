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

	typedef asl::raster<asl::BGR>  BGRRaster;
	typedef asl::raster<asl::RGB>  RGBRaster;
	typedef asl::raster<asl::GRAY> GRAYRaster;

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

        inline
        void rgb_to_hsl(unsigned char theR, unsigned char theG, unsigned char theB, 
                        asl::Vector3f & theResult) {       
            unsigned char myMax = asl::maximum(theR, asl::maximum(theG, theB));
            unsigned char myMin = asl::minimum(theR, asl::minimum(theG, theB));
            float myDelta = float(myMax - myMin);
        
            theResult[0] = 0;    // should be undefined (for grey colors)    
            theResult[1] = myMax ? (255 * myDelta / myMax) : 0;
            theResult[2] = (myMin + myMax) / 2.0f;
        
            if (theResult[1]) {            
                if (theR == myMax) {
                    theResult[0] = (theG - theB) / myDelta;
                } else if (theG == myMax) {
                    theResult[0] = 2 + (theB - theR) / myDelta;
                } else if (theB == myMax) {
                    theResult[0] = 4 + (theR - theG) / myDelta;
                }
                theResult[0] *= 60;
                if (theResult[0] < 0){
                    theResult[0] += 360;    
                }
            }   
        }
        
        inline
            void rgb_to_intensity(const unsigned char theR, const unsigned char theG, 
                                  const unsigned char theB, unsigned int & theResult) {
            theResult = static_cast<unsigned int>(0.2989*theR + 0.5870*theG + 0.1140*theB);
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

