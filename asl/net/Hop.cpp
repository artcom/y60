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
//=============================================================================
//
//    $RCSfile: Hop.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1.1.1 $
//
//
// Description: 
//
//
//=============================================================================

#include "Hop.h"
#include <iostream>

using namespace std;  // automatically added!




namespace inet {
    Hop::Hop(const string & theHostname, const string & theIp, const string & theNiftyStatus)
        : _myIp(theIp), _myNiftyStatus(theNiftyStatus), _myHostname(theHostname) 
    {
    }

    Hop::~Hop() {
    }

    ostream& operator<<(ostream& s, const Hop & theHop)
    {
        return s << theHop._myHostname << " (" << theHop._myIp << " ), nifty status string: " 
                 << theHop._myNiftyStatus << endl;
    }

    ostream& operator<<(ostream& s, const Route & theRoute)
    {
        for (int i = 0 ; i < theRoute.size(); i++) {
            s << theRoute[i];
        }
        return s;
    }
}
