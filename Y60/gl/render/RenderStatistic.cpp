//=============================================================================
// Copyright (C) 2003 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: RenderStatistic.cpp,v $
//     $Author: pavel $
//   $Revision: 1.9 $
//       $Date: 2005/04/24 00:41:18 $
//
//  Description: A simple renderer.
//
//=============================================================================

#include "RenderStatistic.h"
#include "Renderer.h"

#include <asl/Time.h>
#include <asl/Dashboard.h>
#include <asl/linearAlgebra.h>
#include <asl/Exception.h>

#include <string>

using namespace std;
using namespace asl;

namespace y60 {


    static const unsigned ourStatisticLevels  = 2; // please adjust, if u add level
    static const unsigned ourStatisticLevel[] = {
        STATS_NONE,                                         // Level 0
        STATS_FRAMERATE,                                    // Level 1
        STATS_FRAMERATE | STATS_MODEL                       // Level 2
    };

    bool
    renderStatistic(Renderer & theRenderer, unsigned short theStatisticLevel) {
        bool myFoundAStatistic = false;
        int  myStatisticFlags  = 0;
        theRenderer.getTextManager().setColor(Vector4f(1,1,1,1));
        try {
            static double myTime = asl::Time();
            double myElapsedTime = asl::Time() - myTime;

            if (ourStatisticLevels >= theStatisticLevel) {
                myStatisticFlags  = ourStatisticLevel[theStatisticLevel];
                myFoundAStatistic = true;
            } else {
                myFoundAStatistic = false;
            }

            asl::Vector2f myPos(0.85f, 0.05f);
            if (myStatisticFlags & STATS_FRAMERATE) {
                static string myFrameRate = "";

                if (myElapsedTime > 1.0 || myFrameRate == "") {
                    myFrameRate = asl::as_string(getDashboard().getFrameRate()) + " fps";
                }

                theRenderer.getTextManager().addText(myPos, myFrameRate, "SyntaxBold18");
                myPos[1] += 0.03f;
            }
            if (myStatisticFlags & STATS_MODEL) {
                static asl::Time myLastTime;
                static string myVerticeStr = "";
                static string myBodyString = "";
                if ((asl::Time() - myLastTime) > 1.0 || myVerticeStr == "" || myBodyString == "") {
                    myVerticeStr = as_string(getDashboard().getCounterValue("Vertices")) + " vertices";
                    myBodyString = as_string(getDashboard().getCounterValue("RenderedBodies")) + " bodies";
                    myLastTime = asl::Time();
                }
                theRenderer.getTextManager().addText(myPos, myVerticeStr, "SyntaxBold18");
                myPos[1] += 0.03f;
                theRenderer.getTextManager().addText(myPos, myBodyString, "SyntaxBold18");
                myPos[1] += 0.03f;
            }

            if (myElapsedTime > 1.0) {
                myTime = asl::Time();
            }
        } catch (const asl::Exception & ex) {
            AC_ERROR << "ASL exception caught in renderStatistic(): " << ex << endl;
        } catch (const exception & ex) {
            AC_ERROR << "std::exception caught in renderStatistic(): " << ex.what() << endl;
        } catch (...) {
            AC_ERROR << "Unknown exception in renderStatistic()" << endl;
        }
        // could not find a statistic to render
        return myFoundAStatistic;
    }
}
