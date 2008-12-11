/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description:  ??
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      poor
//    formatting              :      ok
//    documentation           :      poor
//    test coverage           :      disaster
//    names                   :      ok
//    style guide conformance :      fair
//    technological soundness :      notapp
//    dead code               :      disaster
//    readability             :      ok
//    understandability       :      notapp
//    interfaces              :      notapp
//    confidence              :      notapp
//    integration             :      notapp
//    dependencies            :      ok
//    error handling          :      notapp
//    logging                 :      notapp
//    cheesyness              :      disaster
//
//    overall review status   :      disaster
//
//    recommendations:  delete this class 
*/
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
        for (std::vector<Hop>::size_type i = 0 ; i < theRoute.size(); i++) {
            s << theRoute[i];
        }
        return s;
    }
}
