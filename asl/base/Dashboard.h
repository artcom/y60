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
//   $RCSfile: Dashboard.h,v $
//   $Author: pavel $
//   $Revision: 1.12 $
//   $Date: 2005/04/24 00:30:18 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#ifndef _ac_renderer_Dashboard_h_
#define _ac_renderer_Dashboard_h_

#include "Time.h"
#include "Ptr.h"
#include "Singleton.h"
#include "Logger.h"

#include <map>
#include <vector>
#include <deque>
#include <string>

namespace asl {


    /*! \addtogroup aslbase */
    /* @{ */

    class Table {
    public:
        enum { JUSTIFIED_LEFT = 0, JUSTIFIED_MIDDLE = 1, JUSTIFIED_RIGHT = 2, JUSTIFIED_CHAR_MIN = ' ' };
        struct Column {
            std::string id;
            unsigned index;
            unsigned char justify;
            unsigned char justify_title;
            unsigned width;
            unsigned width_pre;
            unsigned width_post;
            unsigned width_post_max;
        };
        void addColumn(const std::string & theID,
            const std::string & theTitle,
            unsigned char theJustify,
            unsigned char theTitleJustify = JUSTIFIED_MIDDLE,
            unsigned thePostJustifyLimit = 0) {
            if (_myColumnIDs.find(theID)==_myColumnIDs.end()) {
                unsigned myIndex = _myColumns.size();
                _myColumnIDs[theID] = myIndex ;
                _myColumns.push_back(Column());
                _myColumns[myIndex].id = theID;
                _myColumns[myIndex].index = _myColumns.size()-1;
                _myColumns[myIndex].width = 0;
                _myColumns[myIndex].width_pre = 0;
                _myColumns[myIndex].width_post = 0;
                _myColumns[myIndex].width_post_max = thePostJustifyLimit;
                _myColumns[myIndex].justify = theJustify;
                _myColumns[myIndex].justify_title = theTitleJustify;
                _myHeader[theID] = theTitle;
                adjustWidths(_myHeader[theID],_myColumns[myIndex], _myColumns[myIndex].justify_title);
                return;
            }
            AC_ERROR << "Table::addColumn: column with id '"<<theID<<"' already exists" << std::endl;
        }
        static
        void adjustWidths(std::string & theValue, Column & theColumn, unsigned char theJustify) {

            if (theJustify > JUSTIFIED_CHAR_MIN) {
                std::string::size_type myPos = theValue.find(theJustify);
                if (myPos == std::string::npos) {
                    myPos = theValue.size();
                }
                if (theColumn.width_pre < myPos) {
                    theColumn.width_pre = myPos;
                }
                unsigned myPostWidth = theValue.size() - myPos;
                if (theColumn.width_post_max) {
                    if (myPostWidth > theColumn.width_post_max) {
                        theValue = theValue.substr(0,theValue.size()-(myPostWidth-theColumn.width_post_max));
                        myPostWidth = theColumn.width_post_max;
                    }
                }
                if (theColumn.width_post < myPostWidth) {
                    theColumn.width_post = myPostWidth;
                }
                if (theColumn.width < theColumn.width_pre + theColumn.width_post) {
                    theColumn.width = theColumn.width_pre + theColumn.width_post;
                }
            } else {
                if (theColumn.width < theValue.size()) {
                    theColumn.width = theValue.size();
                }
            }
        }
        unsigned long addRow() {
            _myLines.push_back(std::map<std::string,std::string>());
            return _myLines.size()-1;
        }
        void setField(const std::string & theColumnID, std::string theValue, int theLine = -1) {
            if (_myLines.size() == 0) {
                addRow();
            }
            if (theLine == -1) {
                theLine = _myLines.size()-1;
            }
            if (_myLines.size()<theLine+1) {
                _myLines.resize(theLine+1);
            }

            std::map<std::string,unsigned long>::const_iterator cit =
                _myColumnIDs.find(theColumnID);
            if (cit!=_myColumnIDs.end()) {
                adjustWidths(theValue,_myColumns[cit->second],_myColumns[cit->second].justify);
                _myLines[theLine][theColumnID] = theValue;
            } else {
                AC_ERROR << "Table::setField: unknown column id:"<<theColumnID<<std::endl;
            }
        }
        std::ostream & print(std::ostream & os) const {
            // now print header
            printSeparator(os);
            printLine(os, assembleLine(_myHeader), true);
            printSeparator(os);
            for (unsigned l = 0; l < _myLines.size();++l) {
                printLine(os, assembleLine(_myLines[l]), false);
            }
            printSeparator(os);
            return os;
        }
        std::vector<std::string>
        assembleLine(const std::map<std::string,std::string> & theLine) const {
            std::vector<std::string> myResult;
            for (unsigned c = 0; c < _myColumns.size();++c) {
                std::string & myID = _myColumns[c].id;
                std::map<std::string,std::string>::const_iterator myLineIt =
                    theLine.find(myID);
                if (myLineIt != theLine.end()) {
                    myResult.push_back(myLineIt->second);
                } else {
                    myResult.push_back("");
                }
            }
            return myResult;
        }
        std::ostream & printLine(std::ostream & os, const std::vector<std::string> & theLine, bool isHeader) const {
            os << "|";
            for (unsigned c = 0; c < _myColumns.size();++c) {
                unsigned myContentWidth = theLine[c].size();
                unsigned myColumnWidth = _myColumns[c].width;
                unsigned myBeforeWidth = 0;
                unsigned myAfterWidth = 0;
                unsigned myRest = myColumnWidth - myContentWidth;
                unsigned char myJustify = _myColumns[c].justify;
                if (isHeader) {
                    myJustify =_myColumns[c].justify_title;
                }
                switch (myJustify) {
                    case JUSTIFIED_LEFT:
                        myAfterWidth = myRest;
                        break;
                    case JUSTIFIED_MIDDLE:
                        myBeforeWidth = myRest/2;
                        myAfterWidth = myRest - myBeforeWidth;
                        break;
                    case JUSTIFIED_RIGHT:
                        myBeforeWidth = myRest;
                        break;
                    default:
                        std::string::size_type myPos = theLine[c].find(myJustify);
                        if (myPos == std::string::npos) {
                            // no justific. char in string, put all in ahead
                            myBeforeWidth = _myColumns[c].width_pre - myContentWidth;
                            myAfterWidth = myRest - myBeforeWidth;
                        } else {
                            // align on just.-char
                            myBeforeWidth = _myColumns[c].width_pre - myPos;
                            myAfterWidth = myRest - myBeforeWidth;
                        }
                }
                for (unsigned i = 0; i < myBeforeWidth;++i) {
                    os << " ";
                }
                os << theLine[c];
                for (unsigned i = 0; i < myAfterWidth;++i) {
                    os << " ";
                }
                os << "|";
            }
            os << std::endl;
            return os;
       }
       std::ostream & printSeparator(std::ostream & os) const {
            os << "+";
            for (unsigned c = 0; c < _myColumns.size();++c) {
                for (unsigned i = 0; i < _myColumns[c].width;++i) {
                    os << "-";
                }
                os << "+";
            }
            os << std::endl;
            return os;
       }
    private:
        std::map<std::string,unsigned long> _myColumnIDs;
        mutable std::vector<Column> _myColumns;
        std::vector<std::map<std::string,std::string> > _myLines;
        std::map<std::string,std::string> _myHeader;
    };

    class Timer;
    typedef asl::Ptr<Timer> TimerPtr;

    class Counter;
    typedef asl::Ptr<Counter> CounterPtr;
    class Counter {
        public:
            Counter(unsigned int theGroup = 1)
                :  _myGroup(theGroup)
           {
                reset();
            };
            Counter(const Counter & theOther)
                :  _myCount(theOther._myCount), _myGroup(theOther._myGroup)
            {}
            Counter & operator=(const Counter & theOther) {
                _myCount = theOther._myCount;
                _myGroup = theOther._myGroup;
                return *this;
            }
            void count(unsigned long theIncrement = 1){
                _myCount += theIncrement;
            };
            void reset(){
                _myCount = 0;
            };
            unsigned long getCount() const {
                return _myCount;
            }
            unsigned long getGroup() const {
                return _myGroup;
            }
        private:
            unsigned long _myCount;
            unsigned long _myGroup;
    };

    class Timer {
        public:
            Timer(TimerPtr theParent=TimerPtr(0),unsigned int theGroup = 1)
                : _myParent(theParent), _myGroup(theGroup) {
                reset();
            };
            Timer(const Timer & theTimer)
                :
                _myElapsed(theTimer._myElapsed),
                _myStartTime(theTimer._myStartTime),
                _myGroup(theTimer._myGroup),
                _isRunning(false),
                _myParent(theTimer._myParent),
                _myCounter(theTimer._myCounter),
                _myMinTime(theTimer._myMinTime),
                _myMaxTime(theTimer._myMaxTime)
            {
            }
            Timer & operator=(const Timer & theTimer) {
                _myElapsed = theTimer._myElapsed;
                _myStartTime = theTimer._myStartTime;
                _myGroup = theTimer._myGroup;
                _isRunning = false;
                _myParent = theTimer._myParent;
                _myCounter = theTimer._myCounter;
                _myMinTime = theTimer._myMinTime;
                _myMaxTime = theTimer._myMaxTime;
                return *this;
            }
            void start(){
                if (!_isRunning) {
                    _myStartTime.setNow();
                    _isRunning = true;
                }
            };
            void stop(unsigned long count = 1){
                if (_isRunning) {
                    asl::NanoTime myTimeDiff = asl::NanoTime() - _myStartTime;
                    _myElapsed += myTimeDiff;
                    _isRunning = false;
                    _myCounter.count(count);
                    if (myTimeDiff < _myMinTime) {
                        _myMinTime = myTimeDiff;
                    }
                    if (myTimeDiff > _myMaxTime) {
                        _myMaxTime = myTimeDiff;
                    }
                }
            };
            void reset(){
                _isRunning = false;
                _myElapsed = 0;
                _myStartTime = 0;
                _myMinTime.setNow();  // This is a time difference, but setNow should be large enough...
                _myMaxTime = 0;
                _myCounter.reset();
            };
            asl::NanoTime getElapsed() const {
                if (_isRunning) {
                    return _myElapsed + (asl::NanoTime() - _myStartTime);
                } else {
                    return _myElapsed;
                }
            }
            const asl::NanoTime & getStartTime() const {
                return _myStartTime;
            }
            const TimerPtr getParent() const {
                return _myParent;
            }
            unsigned long getGroup() const {
                return _myGroup;
            }
            void setGroup(unsigned long theGroup) {
                _myGroup = theGroup;
            }
            bool isRunning() const {
                return _isRunning;
            }
            const Counter & getCounter() const {
                return _myCounter;
            }

            const asl::NanoTime & getMin() const {
                return _myMinTime;
            }

            const asl::NanoTime & getMax() const {
                return _myMaxTime;
            }


        private:
			asl::NanoTime	_myElapsed;
            asl::NanoTime   _myStartTime;
            asl::NanoTime   _myMinTime;
            asl::NanoTime   _myMaxTime;
            unsigned long _myGroup;
            bool    _isRunning;
            TimerPtr _myParent;
            Counter _myCounter;
    };

    class Dashboard : public Singleton<Dashboard> {
        public:
            Dashboard() {}
            TimerPtr getTimer(const std::string & theName);
            CounterPtr getCounter(const std::string & theName);
            unsigned long getCounterValue(const std::string & theName);
            void printTimers(Table & theTable, TimerPtr theParent = TimerPtr(0), const std::string & theIndent="");
            std::ostream & print(std::ostream & os);

            // reset all timers/counter with matching reset group ids
            // calling with theGroup = 0 resets all timers/counters
            // regardless of object group id
			void reset(unsigned int theGroup = 1);
            void cycle(unsigned int theGroup = 1, unsigned long theIncrement=1);
            double getFrameRate();
            TimerPtr findParent();
        private:
			std::map<std::string,TimerPtr> _myTimers;
			std::map<std::string,CounterPtr> _myCounters;
			std::map<unsigned long,Counter> _myGroupCounters;
			std::map<std::string,Timer> _myCompleteCycleTimers;
			std::map<std::string,Counter> _myCompleteCycleCounters;
			std::vector<std::pair<std::string, TimerPtr> > _mySortedTimers;
			std::deque<double> _myCycleTimes;
    };

    class ScopeTimer {
	public:
		ScopeTimer(TimerPtr theTimer) : _myTimer(theTimer) {
			_myTimer->start();
		}
		~ScopeTimer() {
			_myTimer->stop();
		}
	private:
		TimerPtr _myTimer;
	};

    extern Dashboard & getDashboard();
    inline TimerPtr getDashboardTimer(const std::string theName) {
        return getDashboard().getTimer(theName);
    }
    inline CounterPtr getDashboardCounter(const std::string theName) {
        return getDashboard().getCounter(theName);
    }
    #define MAKE_NAMED_SCOPE_TIMER(VARNAME,NAME) \
    static asl::TimerPtr myScopeTimer ## VARNAME = asl::getDashboard().getTimer(NAME);\
        asl::ScopeTimer myScopeTimerWrapper ## VARNAME ( myScopeTimer ## VARNAME);

    #define MAKE_SCOPE_TIMER(NAME) \
    static asl::TimerPtr myScopeTimer ## NAME = asl::getDashboard().getTimer(#NAME);\
        asl::ScopeTimer myScopeTimerWrapper ## NAME ( myScopeTimer ## NAME);

    #define SCOPE_TIMER(NAME) myScopeTimer ## NAME

    #define START_TIMER(NAME) \
    {static asl::TimerPtr myTimer ## NAME = asl::getDashboard().getTimer(#NAME);\
    myTimer ## NAME->start();}

    #define STOP_TIMER(NAME) \
    {static asl::TimerPtr myTimer ## NAME = asl::getDashboard().getTimer(#NAME);\
    myTimer ## NAME->stop(); }

    #define STOP_TIMER_N(NAME, COUNT) \
    {static asl::TimerPtr myTimer ## NAME = asl::getDashboard().getTimer(#NAME);\
    myTimer ## NAME->stop(COUNT); }

    #define COUNT(NAME) \
    static asl::CounterPtr myCounter ## NAME = asl::getDashboard().getCounter(#NAME);\
        myCounter ## NAME->count();

    #define COUNT_N(NAME,N) \
    static asl::CounterPtr myCounter ## NAME = asl::getDashboard().getCounter(#NAME);\
        myCounter ## NAME->count(N);

    #define COUNTER(NAME) asl::getDashboard().getCounter(#NAME)

    /* @} */


}
#endif

