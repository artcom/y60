//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Dashboard.cpp,v $
//   $Author: pavel $
//   $Revision: 1.9 $
//   $Date: 2005/04/24 00:30:18 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#include "Dashboard.h"
#include "string_functions.h"

#include <iomanip>
#include <iostream>

#define DB(x) // x

namespace asl {

    void
    Dashboard::reset(unsigned int theGroup) {
		for (std::map<std::string,TimerPtr>::iterator it=_myTimers.begin();
			it != _myTimers.end(); ++it)
		{
            if (theGroup == 0 ||
                theGroup == it->second->getGroup())
            {
			    it->second->reset();
		    }
        }
		for (std::map<std::string,CounterPtr>::iterator it=_myCounters.begin();
			it != _myCounters.end(); ++it)
		{
            if (theGroup == 0 ||
                theGroup == it->second->getGroup())
            {
			    it->second->reset();
		    }
        }
        _myGroupCounters[theGroup].reset();
	}

    // find our most recently started running timer
    TimerPtr
    Dashboard::findParent() {
        asl::NanoTime nowTime;
        asl::NanoTime minDeltaTime;
        TimerPtr myParent(0);
        DB(AC_TRACE << "findParent()"<<std::endl);
		for (std::map<std::string,TimerPtr>::iterator it=_myTimers.begin();
			it != _myTimers.end(); ++it)
		{
            DB(AC_TRACE << "considering timer "<<it->first<<std::endl);
            if (it->second->isRunning()) {
                asl::NanoTime deltaTime = nowTime - it->second->getStartTime();
                DB(AC_TRACE << "timer running:"<<it->first<<std::endl;
                   AC_TRACE << "deltaTime = "<<deltaTime.ticks()<<std::endl;
                   AC_TRACE << "minDeltaTime = "<<minDeltaTime.ticks()<<std::endl);
                if (deltaTime < minDeltaTime) {
                    minDeltaTime = deltaTime;
                    myParent = it->second;
                    DB(AC_TRACE << "myParent found"<<std::endl);
                }
            }
        }
        DB(AC_TRACE << "myParent="<<(void*)(myParent.getNativePtr())<<std::endl);
		return myParent;
	}

    TimerPtr
    Dashboard::getTimer(const std::string & theName) {
		std::map<std::string,TimerPtr>::iterator found = _myTimers.find(theName);
		if (found == _myTimers.end()) {
            TimerPtr myParent   = findParent();
			TimerPtr myNewTimer = TimerPtr(new Timer(myParent));
			_myTimers[theName]  = myNewTimer;
			_mySortedTimers.push_back(std::make_pair(theName, myNewTimer));
			return myNewTimer;
		}
		return found->second;
	}

    CounterPtr
    Dashboard::getCounter(const std::string & theName) {
		std::map<std::string,CounterPtr>::iterator found = _myCounters.find(theName);
		if (found == _myCounters.end()) {
			CounterPtr myNewCounter = CounterPtr(new Counter());
			_myCounters[theName] = myNewCounter;
			return myNewCounter;
		}
		return found->second;
	};

    unsigned long
    Dashboard::getCounterValue(const std::string & theName) {
        // [CH] An averaged count is often misleading. Therefore we reset the count
        // after each cycle and return the last completed count.
        const asl::Counter & myCounter = _myCompleteCycleCounters[theName];
        return myCounter.getCount();
    }

    Dashboard & getDashboard() {
        return Dashboard::get();
    }

    double
    Dashboard::getFrameRate() {
        // We only want to update the frame rate value once per second
        if ((Time() - _myLastFrameRateTime) > 1) {
            if (_myCycleTimes.size() > 1) {
                double myFrameTime = (_myCycleTimes.back() - _myCycleTimes.front()) / _myCycleTimes.size();
                if (myFrameTime > 0) {
                    _myFrameRate = 1 / myFrameTime;                    
                    _myLastFrameRateTime = asl::Time();
                }
            }
        }        
        return _myFrameRate;
    }

    const double FRAME_AVERAGING_TIME = 2;

    void Dashboard::cycle(unsigned int theGroup, unsigned long theIncrement) {
        _myGroupCounters[theGroup].count(theIncrement);
        for (std::map<std::string,TimerPtr>::iterator it=_myTimers.begin();
			it != _myTimers.end(); ++it)
		{
            if (theGroup == 0 ||
                theGroup == it->second->getGroup())
            {
                _myCompleteCycleTimers[it->first] = *(it->second);
		    }
        }
        for (std::map<std::string,CounterPtr>::iterator it=_myCounters.begin();
			it != _myCounters.end(); ++it)
		{
            if (theGroup == 0 ||
                theGroup == it->second->getGroup())
            {
                _myCompleteCycleCounters[it->first] = *(it->second);
                it->second->reset();
		    }
        }        

        // Calculate an sliding-window average 
        _myCycleTimes.push_back(Time());        
        while (_myCycleTimes.size() > 2 && (_myCycleTimes.back() - _myCycleTimes.front()) > FRAME_AVERAGING_TIME) {
            _myCycleTimes.pop_front();
        }
    }

    void
    Dashboard::printTimers(Table & theTable, TimerPtr theParent, const std::string & theIndent) {
        //for (std::map<std::string,TimerPtr>::iterator it=_myTimers.begin();
		//	it != _myTimers.end(); ++it)
		//{

        for (unsigned i = 0; i < _mySortedTimers.size(); ++i) {
            std::string & myTimerName = _mySortedTimers[i].first;
            TimerPtr myTimerPtr       = _mySortedTimers[i].second;

            if (myTimerPtr->getParent() == theParent) {
                theTable.addRow();
                theTable.setField("timername", theIndent + myTimerName);
                unsigned long myCycleCount = _myGroupCounters[myTimerPtr->getGroup()].getCount();
                if (myCycleCount) {
                    const asl::Timer & myTimer = _myCompleteCycleTimers[myTimerName];
                    theTable.setField("elapsed",as_string((double)(myTimer.getElapsed().micros())/1000.0/myCycleCount));
                    const asl::Counter & myCounter = myTimer.getCounter();
                    if (myCounter.getCount()>1) {
                        theTable.setField("intervals",as_string((myCounter.getCount()+1.0)/myCycleCount));
                        theTable.setField("persec",as_string(myCounter.getCount()/myTimer.getElapsed().seconds()));
                        theTable.setField("average",as_string(myTimer.getElapsed().micros()/1000.0/myCounter.getCount()));
                        theTable.setField("minimum",as_string(myTimer.getMin().micros()/1000.0));
                        theTable.setField("maximum",as_string(myTimer.getMax().micros()/1000.0));
                        theTable.setField("cycles",as_string(myCycleCount));
                    }
                } else {
                    const asl::Timer & myTimer = *myTimerPtr;
                    const asl::Counter & myCounter = myTimer.getCounter();
                    theTable.setField("elapsed",as_string((double)(myTimer.getElapsed().micros())/1000.0));
                    if (myCounter.getCount()>1) {
                        theTable.setField("intervals",as_string(myCounter.getCount()));
                        theTable.setField("persec",as_string(myCounter.getCount()/myTimer.getElapsed().seconds()));
                        theTable.setField("average",as_string(myTimer.getElapsed().micros()/1000.0));
                    }
                    theTable.setField("cycles","incomplete");
                }
                printTimers(theTable, myTimerPtr, theIndent+"  ");
            }
        }
    }
    std::ostream &
    Dashboard::print(std::ostream & os) {
 		Table myTable;
        myTable.addColumn("timername","Timer Name",Table::JUSTIFIED_LEFT,Table::JUSTIFIED_LEFT);
        myTable.addColumn("elapsed","ms",'.',Table::JUSTIFIED_MIDDLE,4);
        myTable.addColumn("average","avrg.",'.',Table::JUSTIFIED_MIDDLE,4);
        myTable.addColumn("minimum","min.",'.',Table::JUSTIFIED_MIDDLE,4);
        myTable.addColumn("maximum","max.",'.',Table::JUSTIFIED_MIDDLE,4);
        myTable.addColumn("intervals","i",Table::JUSTIFIED_RIGHT);
        myTable.addColumn("persec","i/sec",Table::JUSTIFIED_MIDDLE);
        myTable.addColumn("cycles","cyc.",Table::JUSTIFIED_RIGHT);
        printTimers(myTable);
        myTable.print(os);

        if (!_myCounters.empty()) {
            Table myCounterTable;
            myCounterTable.addColumn("countername","Counter Name",Table::JUSTIFIED_LEFT);
            myCounterTable.addColumn("count","Count",'.');
            myCounterTable.addColumn("cycles","Cycles",Table::JUSTIFIED_MIDDLE);

            for (std::map<std::string,CounterPtr>::iterator it=_myCounters.begin();
			    it != _myCounters.end(); ++it)
		    {
                myCounterTable.addRow();
                const asl::Counter & myCounter = _myCompleteCycleCounters[it->first];
                unsigned long myCycleCount = _myGroupCounters[myCounter.getGroup()].getCount();
                myCounterTable.setField("countername",it->first);
                if (myCycleCount > 1) {
                    myCounterTable.setField("count",as_string(myCounter.getCount() / myCycleCount ));
                    myCounterTable.setField("cycles",as_string(myCycleCount));
                } else {
                    myCounterTable.setField("count",as_string(myCounter.getCount()));
                    myCounterTable.setField("cycles","incomplete");
                }
            }
            myCounterTable.print(os);
        }
        return os;
    }

}
