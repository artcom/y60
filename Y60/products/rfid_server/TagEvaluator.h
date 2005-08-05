//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
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

#include <asl/Time.h>

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