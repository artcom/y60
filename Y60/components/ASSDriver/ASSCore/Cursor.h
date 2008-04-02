//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef Y60_ASS_CURSOR_INCLUDED
#define Y60_ASS_CURSOR_INCLUDED

namespace y60 {

    struct MaximumResults {
        asl::Vector2f center;
        float max;
    };


    class Cursor {

    public:
    
        
      Cursor() :
          position(0.0, 0.0), 
          major_direction(0.0, 0.0),
          minor_direction(0.0, 0.0),
          firstDerivative(0.0),
          lastTouchTime(0.0),
          intensity(0.0),
          previousIntensity(0.0),
          motion(0.0,0.0),
          correlatedPosition(-1)
          {
              roi.makeEmpty();
              previousRoi.makeEmpty();
          }
    
        Cursor(const asl::Box2f & theBox) :
            roi( theBox),
            firstDerivative(0.0),
            lastTouchTime(0.0),
            intensity(0.0),
            previousIntensity(0.0),
            motion(0.0,0.0),
            correlatedPosition(-1)
                {
                    previousRoi.makeEmpty();
                }

    
        asl::Vector2f position;
        asl::Vector2f major_direction;
        asl::Vector2f minor_direction;
        asl::Box2f    roi;
        asl::Box2f    previousRoi;
        float         intensity;
        float         previousIntensity;
        float         firstDerivative;
        double        lastTouchTime;

        std::deque<float> intensityHistory;
        asl::Vector2f motion;
        int correlatedPosition;

    };


    class MomentCursor : public Cursor {
    public: 
    MomentCursor( const asl::MomentResults & theMomentResult, const asl::Box2f & theBox ) : 
        Cursor( theBox )
        { 
            position =  theMomentResult.center; 
        }
    }; 

    class MaximumCursor :public Cursor {
    public: 
    MaximumCursor(const MaximumResults & theMaximumResult, const asl::Box2f & theBox) : 
        Cursor( theBox ) 
        {
            position = theMaximumResult.center;
        }
         
    };


    typedef std::map<int, Cursor> CursorMap;

}

#endif // Y60_ASS_CURSOR
