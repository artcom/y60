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
//    $RCSfile: Statistic.h,v $
//
//     $Author: david $
//
//   $Revision: 1.5 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_asl_Statistic_h_
#define _ac_asl_Statistic_h_

#include <iostream>

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    /**
        The statistic class calculates maximum, minimum, average, variance
        and standard deviation for any number of samples of the type double
    */
    class Statistic {
        public:

            /**
                Use the standard constructor if your do not want automatic
                output of the statistics. You can output it manually at any
                time with the output operator
            */
            Statistic();

            /**
                This constructor causes the class to automatically output its
                values after <theOutputIntervalSamples> samples have been collected.
                The statistic is reset after each output.
            */
            Statistic(char * theName, int theOutputIntervalSamples);

            /**
                This constructor causes the class to automatically output its
                values after <theOutputIntervalTime> has passed.
                The statistic is reset after each output.
            */
            Statistic(char * theName, double theOutputIntervalTime);

            virtual ~Statistic() {
            }

            virtual void addSample(double theSample);

            virtual void reset();

            virtual int getNumberOfSamples() const {
                return _myNumberOfSamples;
            }

            virtual double getMaximum() const {
                return _myMaximum;
            }

            virtual double getMinimum() const {
                return _myMinimum;
            }

            virtual double getSampleSum() const {
                return _mySampleSum;
            }

            virtual double getAverage() const;
            virtual double getVariance() const;
            virtual double getStandardDeviation() const;

            /**
                Use enable and disable to temporay disable the data collection
                to avoid outlier data
            */
            void enable() {
                _isEnabled = true;
            }

            void disable() {
                _isEnabled = false;
            }

            bool isEnabled() {
                return _isEnabled;
            }

            double getOutputIntervalTime() const {
                return _myOutputIntervalTime;
            }

        private:
#ifdef LINUX
            long long _myNumberOfSamples;
#else
            long      _myNumberOfSamples;
#endif
            double    _mySampleSum;
            double    _mySquaredSampleSum;
            double    _myMaximum;
            double    _myMinimum;
            double    _myVariance;

            char *    _myName;
            int       _myOutputIntervalSamples;
            double    _myOutputIntervalTime;
            double    _myLastOutputTime;

            bool      _isEnabled;
    };

    inline
    std::ostream & operator<<(std::ostream& os, const Statistic & theStatistic) {
        if (theStatistic.getOutputIntervalTime() > 0.0) {
            os << "T: " << theStatistic.getOutputIntervalTime() << " ";
        }

        os << "N: " << theStatistic.getNumberOfSamples()
           << " Max: " << theStatistic.getMaximum()
           << " Min: " << theStatistic.getMinimum()
           << " Avg: " << theStatistic.getAverage()
           << " StdDev: " << theStatistic.getStandardDeviation() << std::endl;
        return os;
    }



    ////////////////////////////////////////////////////////////////////////////////

    /**
        BoolStatistic should be used to count successes and failures
    */
    class BoolStatistic : public Statistic {
        public:
            BoolStatistic();
            BoolStatistic(char * theName, int theOutputIntervalSamples);
            BoolStatistic(char * theName, double theOutputIntervalTime);
            virtual ~BoolStatistic() {};

            void addSample(bool theSample);

            int getNumberOfHits() const;

        private:
    };

    inline
    std::ostream & operator<<(std::ostream& os, const BoolStatistic & theStatistic) {
        if (theStatistic.getOutputIntervalTime() > 0.0) {
            os << "T: " << theStatistic.getOutputIntervalTime() << " ";
        }

        os << "N: " << theStatistic.getNumberOfSamples()
           << " X: " << theStatistic.getNumberOfHits() << std::endl;

        return os;
    }


    //////////////////////////////////////////////////////////////////////////////////

    /**
        FrequencyStatistic should be used to determine how often an event occurs per
        second
    */
    class FrequencyStatistic : public Statistic {
        public:
            FrequencyStatistic();
            FrequencyStatistic(char * theName, int theOutputIntervalSamples);
            FrequencyStatistic(char * theName, double theOutputIntervalTime);

            virtual ~FrequencyStatistic() {};

            void addSample();

            void reset();

        private:
            double _lastAddSampleTime;
    };

    /* @} */
}

#endif
