//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: RenderStatistic.h,v $
//   $Author: janbo $
//   $Revision: 1.3 $
//   $Date: 2004/10/18 16:22:56 $
//
//  Description: A simple renderer.
//
//=============================================================================

#ifndef _ac_render_RenderStatistic_h_
#define _ac_render_RenderStatistic_h_

namespace y60 {

    // Render statistic bit-flags
    enum {
        STATS_NONE      = 0,
        STATS_FRAMERATE = 1,
        STATS_MODEL     = 2
    };

    class Renderer;
    bool renderStatistic(Renderer & theRenderer, unsigned short theStatisticLevel);
}

#endif
