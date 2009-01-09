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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//    $RCSfile: Statistic.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.2 $
//
// Description: 
//
// (CVS log at the bottom of this file)
//
//=============================================================================

//own header
#include "Statistic.h"

#include <asl/base/Time.h>
#include <asl/base/Logger.h>

#include <float.h>
#include <math.h>


using namespace std;  // automatically added!



namespace asl {

    Statistic::Statistic() :
        _myName(""), _myOutputIntervalSamples(0), _myOutputIntervalTime(0.0),
        _isEnabled(true) 
    {
        reset();
    }

    Statistic::Statistic(char * theName, int theOutputIntervalSamples) : 
        _myName(theName), _myOutputIntervalSamples(theOutputIntervalSamples), 
        _myOutputIntervalTime(0.0), _isEnabled(true)
    {
        reset();
    }

    Statistic::Statistic(char * theName, double theOutputIntervalTime) : 
        _myName(theName), _myOutputIntervalSamples(0),
        _myOutputIntervalTime(theOutputIntervalTime), _isEnabled(true) 
    {
        reset();
    }

    void
    Statistic::reset() {
        _myNumberOfSamples  = 0;
        _myMaximum          = DBL_MIN;
        _myMinimum          = DBL_MAX;
        _myVariance         = 0.0;
        _mySampleSum        = 0.0;
        _mySquaredSampleSum = 0.0;
        _myLastOutputTime   = asl::Time();        
    }

    void
    Statistic::addSample(double theSample) {        
        if (_isEnabled) {
            if (theSample > _myMaximum) {
                _myMaximum = theSample;
            }

            if (theSample < _myMinimum) {
                _myMinimum = theSample;
            }

            _myNumberOfSamples++;
            _mySampleSum += theSample;            
            _mySquaredSampleSum += (theSample * theSample); 
           
            if ((_myOutputIntervalSamples > 0 &&                     
                 _myNumberOfSamples >= _myOutputIntervalSamples) ||                        
               ((_myOutputIntervalTime > 0.0) && 
                (asl::Time() - _myLastOutputTime) >= _myOutputIntervalTime)) 
            {
                AC_INFO << "*** Statistic: [" << asl::Time() << "] " 
                    << _myName << ": " << *this << endl;
                reset();                
            }
        }
    }

    double 
    Statistic::getAverage() const {
        return (_mySampleSum / _myNumberOfSamples);
    }

    double
    Statistic::getVariance() const {
        double myAverage = getAverage();
        return (_mySquaredSampleSum / _myNumberOfSamples) - myAverage * myAverage;
    }

    double
    Statistic::getStandardDeviation() const {
        return sqrt(getVariance());
    }


    ///////////////////////////////////////////////////////////////////////////

    BoolStatistic::BoolStatistic() : Statistic() {
    }

    BoolStatistic::BoolStatistic(char * theName, int theOutputIntervalSamples) :
        Statistic(theName, theOutputIntervalSamples)
    {}

    BoolStatistic::BoolStatistic(char * theName, double theOutputIntervalTime) : 
        Statistic(theName, theOutputIntervalTime)
    {}

    void
    BoolStatistic::addSample(bool theSample) {
        Statistic::addSample(double(theSample));
    }

    int 
    BoolStatistic::getNumberOfHits() const {
        return (int)(getSampleSum());
    }
    
    ///////////////////////////////////////////////////////////////////////////

    FrequencyStatistic::FrequencyStatistic() : Statistic(), _lastAddSampleTime(-1) {        
    }

    FrequencyStatistic::FrequencyStatistic(char * theName, int theOutputIntervalSamples) :
        Statistic(theName, theOutputIntervalSamples), _lastAddSampleTime(-1)
    {}

    FrequencyStatistic::FrequencyStatistic(char * theName, double theOutputIntervalTime) : 
        Statistic(theName, theOutputIntervalTime), _lastAddSampleTime(-1)
    {}

    void
    FrequencyStatistic::reset() {
        _lastAddSampleTime = -1;
        Statistic::reset();
    }


    void
    FrequencyStatistic::addSample() {
        if (isEnabled()) {
            double nowTime = asl::Time();
            if (_lastAddSampleTime > -1) {
                Statistic::addSample(1. / (nowTime - _lastAddSampleTime));                        
            }
            _lastAddSampleTime = nowTime;
        }
    }
}
