// Wiim API ©2006 Eric B.
// http://digitalretrograde.com/projects/wiim/

// May be used and modified freely as long as this message is left intact

// This is a "game" demo of using the Wiimote interface class. It's a partial 
// implementation of "Lunder Lander" that uses SDL for display. Skip down to main(..)
// for the interesting stuff. 

#include <vector>
using namespace std;

#include "Utils.h"
#include "WiimoteDriver.h"

#include <y60/GenericEvent.h>


#include <assert.h>
#include <iostream>



//vector<MotionData> last_positions;

// This function takes the motion data and averages it over the last several inputs to
// keep the ship from jumping around because of errant sensor readings.

// It also assumes that the x sensor reading is an angle, which it generally isn't. 
// When the remote is pitched, gravity increases the values. We clamp these to make sure
// the angle doesn't get to be too big (try removing the test and shaking the remote to see).
// double GetAveragedAngle(MotionData p)
// {
// 	if (last_positions.size() <= 2)
// 	{	
// 		last_positions.push_back(p);
// 		return p.x;
// 	}

// 	last_positions.erase(last_positions.begin());

// 	if (p.x > 30) p.x = 30; // Restrict the values to -30 to 30
// 	if (p.x < -30) p.x = -30;

// 	last_positions.push_back(p);

// 	return  (last_positions[0].x + last_positions[1].x + last_positions[2].x) / 3.0;
// }


// int main( int argc, char* argv[] )
// {

//     y60::Wiimote myDriver;
//     //myDriver.setLEDs(1, 1, 0, 1);
//     //myDr
//     unsigned myNumWiimotes = myDriver.getNumWiimotes();
//     for(unsigned i=0; i<myNumWiimotes; ++i) {
//         myDriver.setRumble(i,false);
//         myDriver.setLEDs(i, i+1 & (1<<0) ,i+1 & (1<<1),i+1 & (1<<2),i+1 & (1<<3));
//     }
//     myDriver.requestMotionData();
//     bool quit = false;
//     while (!quit)
//     {
//         y60::EventPtrList myEvents = myDriver.poll();
//         for(unsigned i=0 ; i < myEvents.size(); ++i) {
//             y60::GenericEventPtr myGEvent = dynamic_cast_Ptr<y60::GenericEvent>( myEvents[i] );
//             if ( ! myGEvent ) {
//                 cerr << "CAST FAILED" << endl;
//             }
//             dom::NodePtr myEvent = myGEvent->getNode();
            
//             if(myEvent->getAttributeString("type") == "button" && myEvent->getAttributeString("buttonname") == "Home") {
//                 if( myEvent->getAttributeValue<bool>("pressed") == true) {
//                     quit = true;
//                 }
//             }
//             if(myEvent->getAttributeString("type") == "button" && myEvent->getAttributeString("buttonname") == "B") {
//                 cout << *myEvent << endl;
//                 myDriver.setRumble(myEvent->getAttributeValue<int>("id"), myEvent->getAttributeValue<bool>("pressed"));
//             }
//         }
        
//     }
//     for(unsigned i=0; i<myNumWiimotes; ++i) {
//         myDriver.setLEDs(i, 0,0,0,0);
//         myDriver.setRumble(i, false);
//     }

// 	return 0;
// }
