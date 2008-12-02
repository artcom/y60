//============================================================================
//
// Copyright (C) 2000-2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: Time.h,v $
//
//     $Author: thomas $
//
//   $Revision: 1.23 $
//
// Description: Class for simple measurement of time, resolution depends on
//              hardware, maximal 1 micro second
//
//
//=============================================================================

#ifndef included_asl_Time_h
#define included_asl_Time_h

#include "TimeStreamFormater.h"
#include "Exception.h"
#include "settings.h"

#include <iostream>
#include <iomanip>
#include <time.h>  /* for tm */
#include <math.h>
#include <string>
#include <pthread.h> /* for timespec */

#ifdef WIN32
    #include <windows.h>
    #include <mmsystem.h>
#else
    #ifdef OSX
        #include <Carbon/Carbon.h>
    #endif
    #include <sys/time.h>
    #include <unistd.h>
#endif

typedef asl::Unsigned64 cycles_t;

#define USE_TIME_OF_DAY  

#ifdef LINUX
#    if defined __x86_64__
    inline
    cycles_t
    get_cycles() {
        cycles_t ret;
        asl::Unsigned32 a = 0;
        asl::Unsigned32 d = 0; 
        asm volatile("rdtsc" : "=a" (a), "=d" (d)); 
        (ret) = ((asl::Unsigned64)a) | (((asl::Unsigned64)d)<<32); 
        return ret;
    }
#    endif
#    if defined __i386__
    inline
    cycles_t
    get_cycles() {
        cycles_t ret = 0;
        __asm__ __volatile__("rdtsc" : "=A" (ret));
        return ret;
    }
#    endif
#endif

#ifdef OSX

#include <mach/mach_time.h>

inline
cycles_t
get_cycles() {
    return mach_absolute_time();
}

#endif

    
namespace asl {


        /**
         * @ingroup aslbase
         * Wrapper around a milliseconds precise time.
         *
         */
        class Time {
        private:
#ifdef WIN32
        static const long long SECS_BETWEEN_EPOCHS = 11644473600;
        static const long long SECS_TO_100NS = 10000000; /* 10^7 */
                struct timeval {
                        long tv_sec;
                        long tv_usec;
                };
#endif
                struct timeval when;

        public:
                /**
                 * Default Constructor. Creates an instance that holds the current
         * time (GMT).
                 */
                Time() {
                        setNow();
                }
                /**
                 * Constructor. 
                 * @param secs Number of seconds. If this has to be a absolute time,
         *             the number of seconds since 1.1.1970 0:00.
                 */
                Time(const double secs) {
                        when.tv_sec=static_cast<long>(floor(secs));
                        when.tv_usec=static_cast<long>(fmod(secs, 1.0)*1.0e6);
                }
                /**
                 * Sets the instance to the current time (GMT).
                 * @return *this
                 */
                Time& setNow() {
#ifdef WIN32

            long long myBigValue = 0;
            GetSystemTimeAsFileTime((LPFILETIME) &myBigValue);
            myBigValue -= (SECS_BETWEEN_EPOCHS * SECS_TO_100NS);
            long long myBigTempValue = myBigValue / SECS_TO_100NS; /*10e7*/;
            when.tv_sec = (long) myBigTempValue;
            myBigTempValue = (myBigValue % SECS_TO_100NS) / 10;
            when.tv_usec = (unsigned long) myBigTempValue;
                        //unsigned long myTime = timeGetTime();

                        //when.tv_sec  = myTime / 1000;
                        //when.tv_usec = (myTime % 1000) * 1000;
#else
                        gettimeofday(&when, 0);
#endif
                        return *this;
                }
                int secs() {
                        return when.tv_sec;
                }
                int usecs() {
                        return when.tv_usec;
                }
                bool operator==(const Time & t) const {
                        return((when.tv_sec==t.when.tv_sec) && (when.tv_usec==t.when.tv_usec));
                }
                bool operator!=(const Time & t) const {
                        return((when.tv_sec!=t.when.tv_sec) || (when.tv_usec!=t.when.tv_usec));
                }
                bool operator>(const Time & t) const {
                        return( (when.tv_sec==t.when.tv_sec) ? (when.tv_usec>t.when.tv_usec) : (when.tv_sec>t.when.tv_sec));
                }
                bool operator<(const Time & t) const {
                        return( (when.tv_sec==t.when.tv_sec) ? (when.tv_usec<t.when.tv_usec) : (when.tv_sec<t.when.tv_sec));
                }
                bool operator>=(const Time & t) const {
                        return *this > t || t == *this;
                }
                bool operator<=(const Time & t) const {
                        return *this < t || t == *this;
                }
                operator double() const {
                        return (double)when.tv_sec+(double)when.tv_usec/1.0e6;
                }
                operator const timeval() const {
                    return when;
                }
                operator const timespec() const {
                    timespec myTimeSpec = {when.tv_sec, when.tv_usec * 1000};
                    //myTimeSpec.tv_sec = when.tv_sec;
                    //myTimeSpec.tv_nsec = when.tv_usec * 1000;
                    return myTimeSpec;
                }

                long long micros() const {
                        return when.tv_sec * 1000000L + when.tv_usec;
                }

                long long millis() const {
                        return when.tv_sec * 1000L + when.tv_usec/1000L;
                }
                inline std::ostream& print(std::ostream& s) const;
                bool parse(const std::string & theString);
        };


        inline std::ostream& Time::print(std::ostream& s) const
        {
            const time_t myTvSec = static_cast<time_t>(when.tv_sec);
            tm *tp=gmtime(&myTvSec);
            if ( s.iword(TimeStreamFormater::ourIsFormatedFlagIndex) ) {
                std::string myFormatString( static_cast<const char * >(
                    s.pword(TimeStreamFormater::ourFormatStringIndex)));
                bool isFieldToken = false;
                for (unsigned i = 0; i < myFormatString.size(); ++i) {
                    if ( isFieldToken ) {
                        switch (myFormatString[i]) {
                            case 'Y':
                                // add 1900 to get correct bc year
                                s << 1900 + tp->tm_year;
                                break;
                            case 'M':
                                // add one cause this is meant as an index into an array with month names
                                s << std::setw(2) << std::setfill('0') << 1 + tp->tm_mon;
                                break;
                            case 'D':
                                s << std::setw(2) << std::setfill('0') << tp->tm_mday;
                                break;
                            case 'h':
                                s << std::setw(2) << std::setfill('0') << tp->tm_hour;
                                break;
                            case 'm':
                                s << std::setw(2) << std::setfill('0') << tp->tm_min;
                                break;
                            case 's':
                                s << std::setw(2) << std::setfill('0') << tp->tm_sec;
                                break;
                            case 'l':
                                s << std::setw(3) << std::setfill('0') << when.tv_usec/1000;
                                break;
                            case 'u':
                                s << std::setw(3) << std::setfill('0') << when.tv_usec/1000 << ','
                                  << std::setw(3) << std::setfill('0') << when.tv_usec%1000;
                                break;
                            case '%':
                                // %% -> %
                                s << myFormatString[i];
                                break;
                            default:
                                throw ParseException(std::string("Unknown token %") +
                                    myFormatString[i] + " in time format string.",
                                    PLUS_FILE_LINE);
                                break;
                        }
                        isFieldToken = false;
                    } else {
                        if (myFormatString[i] == '%') {
                            isFieldToken = true;
                        } else {
                            s.put(myFormatString[i]);
                        }
                    }
                }

            } else {
                s << std::setw(2) << std::setfill('0') << tp->tm_hour << ':'
                  << std::setw(2) << std::setfill('0') << tp->tm_min << ':'
                  << std::setw(2) << std::setfill('0') << tp->tm_sec
                  << '.'
                  << std::setw(3) << std::setfill('0') << when.tv_usec/1000 << ','
                  << std::setw(3) << std::setfill('0') << when.tv_usec%1000;
            }

            return s;
        }

#ifdef WIN32
        inline void msleep(unsigned long theMilliSeconds) {
                Sleep(theMilliSeconds);
        }
#else
        inline void msleep(unsigned long theMilliSeconds) {
                usleep(theMilliSeconds * 1000);
        }
#endif

        class NanoTime {
        public:
                NanoTime() {
                        setNow();
                }
                NanoTime(unsigned long long t) {
                        _myCounter = t;
                }
                unsigned long long ticks() const {
                        return _myCounter;
                }
                double nanos() const {
                        return _myCounter * nanoSecondsPerTick();
                }
                double micros() const {
                        return _myCounter * microSecondPerTick();
                }
                double millis() const {
                        return _myCounter * milliSecondPerTick();
                }
                double secs() const {
                        return seconds();
                }
                double seconds() const {
                        return double(_myCounter)/double(perSecond());
                }

                void setNow() {
#ifdef WIN32
                    QueryPerformanceCounter((LARGE_INTEGER*)&_myCounter);
#else
    #ifdef OSX
                    Nanoseconds myNanos=AbsoluteToNanoseconds(UpTime());
                    _myCounter = *(asl::Unsigned64*)(&myNanos);
    #else
        #ifdef USE_TIME_OF_DAY
                    timespec tv;
                    clock_gettime(CLOCK_REALTIME, &tv);
                    _myCounter = tv.tv_sec * asl::Unsigned64(1000000000) + tv.tv_nsec;
        #else
                    _myCounter = get_cycles();
        #endif
    #endif
 #endif
                }
                static unsigned long long perSecond() {
                        static unsigned long long perSecond = 0;
                        if (perSecond == 0) {
#ifdef WIN32
                                QueryPerformanceFrequency((LARGE_INTEGER*)&perSecond);
#else
#ifdef USE_TIME_OF_DAY                
                perSecond = asl::Unsigned64(1000000000); // nsec            
#else                
                /*
                perSecond = cpu_khz;
                perSecond *=1000;
                */

                                double myCalibTime = Time();
                                unsigned long long myCalibCounter = get_cycles();
                                msleep(200);
                                double myCalibDurationTime = Time() - myCalibTime;
                                unsigned long long myCalibCounterElapsed = get_cycles() - myCalibCounter;
                                perSecond = static_cast<unsigned long long>(myCalibCounterElapsed/myCalibDurationTime);
#endif                
#endif
                        }
                        return perSecond;
                }

        static double nanoSecondsPerTick() {
            static double myNanoSecondsPerTick = 1E9 / double(perSecond());
            return myNanoSecondsPerTick;
        }

        static double microSecondPerTick() {
            static double myMicroSecondPerTick = 1E6 / double(perSecond());
            return myMicroSecondPerTick;
        }

        static double milliSecondPerTick() {
            static double myMilliSecondPerTick =  1E3 / double(perSecond());
            return myMilliSecondPerTick;
        }


        static double _myTicksPerMilliSecond;

                NanoTime & operator+=(const NanoTime & theOther) {
                        _myCounter += theOther.ticks();
                        return *this;
                }
        private:
                unsigned long long _myCounter;
        };
        inline
        NanoTime operator-(const NanoTime & a, const NanoTime & b) {
                return NanoTime(a.ticks() - b.ticks());
        }
        inline
        NanoTime operator+(const NanoTime & a, const NanoTime & b) {
                return NanoTime(a.ticks() + b.ticks());
        }
        inline
        bool operator>(const NanoTime & a, const NanoTime & b) {
                return a.ticks() > b.ticks();
        }
    inline
        bool operator<(const NanoTime & a, const NanoTime & b) {
                return a.ticks() < b.ticks();
        }
    inline
        bool operator>=(const NanoTime & a, const NanoTime & b) {
                return a.ticks() >= b.ticks();
        }
    inline
        bool operator<=(const NanoTime & a, const NanoTime & b) {
                return a.ticks() <= b.ticks();
        }
    inline
        bool operator==(const NanoTime & a, const NanoTime & b) {
                return a.ticks() == b.ticks();
        }
    inline
        bool operator!=(const NanoTime & a, const NanoTime & b) {
                return a.ticks() != b.ticks();
        }

    class NoisyScopeTimer {
        public:
        NoisyScopeTimer(const std::string & theTitle = "", int theCount = 1)
            : _myCount(theCount), _myTitle(theTitle)
        {
            std::cerr << ":: NoisyScopeTimer start: " << theTitle << std::endl;
                        _myStartTime.setNow();
                }
                ~NoisyScopeTimer() {
            NanoTime myTimeTaken(NanoTime().ticks()-_myStartTime.ticks());
            std::cerr << ":: Time " << myTimeTaken.micros()/1000.0 << " ms, count = "<<_myCount<<", mio.per sec. = "<<_myCount/myTimeTaken.secs()/1000/1000<<std::endl;
            std::cerr << ":: NoisyScopeTimer ready: " << _myTitle << std::endl;
                }
        private:
        std::string _myTitle;
        int _myCount;

                NanoTime _myStartTime;
        };
    inline std::ostream & operator<<(std::ostream& s, const asl::Time & t) {
        return t.print(s);
    }
} // end of namespace asl


#endif

