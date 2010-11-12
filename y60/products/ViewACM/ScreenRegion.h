//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.


#if !defined(_ac_video_Region_h_)
#define _ac_video_Region_h_

#include <vector>

namespace video {

    class ScreenRegion {
        public:
            enum EDGE {
                TOP_LEFT,
                TOP_RIGHT,
                BOTTOM_RIGHT,
                BOTTOM_LEFT
            };
            
            ScreenRegion();
            virtual ~ScreenRegion(); 

            // access
            void set(const EDGE& theEdge, const double & s, const double & t);
            void get(const EDGE& theEdge, double & s, double & t) const;
            std::vector<float>  get(const EDGE& theEdge) const;

            // transform
            void rotate(const double & angle);
            void stretch(const double & myXStretch, const double & myYStretch);
            void dump();
            void reset();
        private:
            double _myTopLeft[2];
            double _myTopRight[2];
            double _myBottomRight[2];
            double _myBottomLeft[2];
    };
    
}
#endif
