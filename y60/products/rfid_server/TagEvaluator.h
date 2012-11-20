/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $RCSfile: TagEvaluator.h,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/08/17 15:24:14 $
//
//
//=============================================================================

#ifndef TAG_EVALUATOR_INCLUDED
#define TAG_EVALUATOR_INCLUDED

#include <string>
#include <map>

#include <asl/base/Time.h>

// The purpose of this class is to determine the single
// tag that is closest to the antenna. Due to scan faillures
// this task is not as trivial at it might seem at first glance.
// TagEvaluator keeps state across scans to account for these errors.

// The idea is as follows:
// - each time we see a certain tag, we load an associated virtual capacitor
//   with a load proportional to the tag's signal strength.
// - capacitors unload over time
// - the tag associated with the fullest capacitor is considered the most attractive.
// - when all capacitors drop below a certain threshold value, we assume there is no
//   tag in range. In this case getMostAttractive() returns 0.
// - when a single capacitor's load value drops below this threshold, it si
//   removed from the list of known tags.

class TagEvaluator {
public:
    TagEvaluator(float min_load, double t_load_half, float saturation);

    // tell the Evaluator that a new scan is taking place
    void startScan();
    // for each tag found during a scan call tagFound
    void tagFound(unsigned uid, int signal_strength);

    // return the single most attractive tag id
    unsigned getMostAttractive() const;

protected:
    class Capacitor {
    public:
        Capacitor();
        Capacitor(unsigned tagid, double t_load_half, float saturation);
        void load(float signal_strength, const asl::Time& t);
        float getLoad(const asl::Time& t) const;
        Capacitor& operator=(const Capacitor& tha);
    protected:
        unsigned _tagid;
        float _initial_load;
        asl::Time _loaded_at;
        double _t_load_half;
        float _saturation;
    };

    float _min_load;    // destroy capacitors below this load
    const double _t_load_half; // l(t_load_half) = l0/2 time after which caacitor's load drops to one half of the initial value
    typedef std::map<unsigned, Capacitor> CapacitorMap;
    mutable CapacitorMap _capacitors;
    asl::Time _scan_started_at;
    float _saturation;

};

#endif