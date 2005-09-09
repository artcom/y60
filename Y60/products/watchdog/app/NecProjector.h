//============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: NecProjector.h,v 1.1 2004/09/15 15:03:45 ulrich Exp $
//   $Author: ulrich $
//   $Revision: 1.1 $
//   $Date: 2004/09/15 15:03:45 $
//
// NEC Projector controller.
//
//=============================================================================

#ifndef _ac_watchdog_NecProjector_h_
#define _ac_watchdog_NecProjector_h_

#include "Projector.h"

class NecProjector : public Projector
{
public:
    explicit NecProjector(int thePortNumber);

    virtual void power(bool thePowerFlag);
    virtual void selectInput(VideoSource theSource);
    
    /**
     * Open/Close the shutter
     * \note For NEC projectors do not have a shutter
     *       this enables/disables the PICTURE MUTE
     * \param theShutterFlag 
     */                  
    virtual void shutter(bool theShutterFlag);
};

#endif
