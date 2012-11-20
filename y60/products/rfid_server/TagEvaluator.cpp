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
//   $RCSfile: TagEvaluator.cpp,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/08/17 15:24:14 $
//
//
//=============================================================================


#include "TagEvaluator.h"

using namespace std;
using namespace asl;

TagEvaluator :: TagEvaluator(float min_load, double t_load_half, float saturation) :
    _min_load(min_load),
    _t_load_half(t_load_half),
    _saturation(saturation)
{}

// tell the Evaluator that a new scan is taking place
void TagEvaluator :: startScan() {
    _scan_started_at.setNow();
}

// for each tag found during a scan call tagFound
void TagEvaluator :: tagFound(unsigned uid, int signal_strength) {
    // do we know this tag already?
    CapacitorMap::iterator i = _capacitors.find(uid);
    if (i == _capacitors.end()) {
        // No: create one and load it
        Capacitor c(uid, _t_load_half, _saturation);
        c.load((float)signal_strength, _scan_started_at);
        _capacitors[uid] = c;
    } else {
        // we already created a capacitor for this tag. Just load it.
        i->second.load((float)signal_strength, _scan_started_at);
    }
}

// return the single most attractive tag id
unsigned TagEvaluator :: getMostAttractive() const {
    Time now;

    // find the capacitor with the highest load.
    // and while we're at it, remove the capacitors that
    // dropped below minimal load.
    float max_load = _min_load;
    unsigned most_attractive = 0;

    bool keep_going;
    do {
        keep_going = false;
        CapacitorMap::iterator i;
        for(i=_capacitors.begin(); i!=_capacitors.end(); ++i) {
            float load = i->second.getLoad(now);
            if (load < _min_load) {
                _capacitors.erase(i);
                keep_going = true;
                break;
            } else if (load > max_load) {
                max_load = load;
                most_attractive = i->first;
            }
        }
    } while(keep_going);
    cout << "most attractive load value =" << max_load << endl;
    return most_attractive;

}

TagEvaluator :: Capacitor :: Capacitor() :
    _tagid(0),
    _t_load_half(0),
    _initial_load(0)
{}


TagEvaluator :: Capacitor :: Capacitor(unsigned tagid, double t_load_half, float saturation) :
    _tagid(tagid),
    _t_load_half(t_load_half),
    _initial_load(0),
    _saturation(saturation)
{}

void TagEvaluator :: Capacitor :: load(float signal_strength, const Time& t) {
    // ignore this call when the load is already higher
    float newload = getLoad(t) + signal_strength;
    if (newload > _saturation) {
        newload = _saturation;
    }

    _initial_load = newload;
    _loaded_at = t;
}

float TagEvaluator :: Capacitor :: getLoad(const Time& t) const {
    double delta_t = t.operator double() - _loaded_at.operator double();

    // l = exp(-f * delta_t)
    // exp(-f * _t_load_half) = 0.5
    // ln(0.5) = -f * _t_load_half
    // - ln(0.5) / _t_load_half = f

    double f = - log(0.5) / _t_load_half;
    return (float)(_initial_load * exp(-f * delta_t));
}

TagEvaluator :: Capacitor& TagEvaluator :: Capacitor :: operator=(const TagEvaluator :: Capacitor& that) {
    _tagid = that._tagid;
    _initial_load = that._initial_load;
    _loaded_at = that._loaded_at;
    _t_load_half = that._t_load_half;
    _saturation = that._saturation;
    return *this;
}