//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JointWeight.h,v $
//   $Author: david $
//   $Revision: 1.1 $
//   $Date: 2004/02/18 17:50:41 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_jointweight_h_
#define _ac_maya_jointweight_h_

struct JointWeight {
    JointWeight(float theWeight, unsigned int theIndex) :
        weight(theWeight), index(theIndex) {}
    float weight;
    unsigned int index;
    bool operator<(const JointWeight & otherJointWeight) const { return weight < otherJointWeight.weight; }
};

#endif
