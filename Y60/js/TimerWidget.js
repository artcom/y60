//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
// 
// Timer widget
// 
// For a definition of what widgets are, see WidgetFactory.js and widgetBase.js.
// 
// This widget represents a timer. When started via start(), the timer widget runs
// for a number of seconds determined by its "timeout" property and calls onTime()
// and onProgress() each frame until the total timeout is exceeded. If timeout 
// is exceeded, onTimeout() is called once, and from then on, onExcess() is called
// during each frame as long as it yields a nonzero return value.
// Timers can be stopped, reset, paused and continued, and their current progress
// and time values can be modified and synchronize each other.
// 
// Properties:
// 
// - timeout:      a double value with the expected time until onTimeout().
//                 timeout may be negative. in that case, onTime() callbacks
//                 will have descending negative values, but onT() will still run from
//                 0 to 1.
// 
// - repeat:       - if set to "maybe", timer will restart after timeout if the
//                   onTimeout() callback returned a nonzero value from its call.
//                 - if set to any positive number, value will be decreased by one on timeout and timer will be restarted.
//                 - if set to any other nonzero value, timer will restart.
// 
// - timedout:     the last absolute time a timeout event occurred since last activation.
// 
// - running:      a boolean value indicating whether this timer is active and not paused.
//
// - active:       a boolean value indicating whether this timer is running or paused.
// 
// Functions:
//
// - start():      starts the timer. behaves identical to { silent_continue(); onStart(); }.
// - restart():    starts the timer from zero. behaves identical to { silent_reset(); start(); }.
// - stop():       stops and resets the timer. behaves identical to { silent_pause(); reset(); onStop(); };
//
// - pause():      suspends the timer and calls onPause().
// - Continue():   starts the timer and calls onContinue().
// - reset():      resets internal counters to zero, unsuspends the timer and calls onReset().
// 
// - silent_pause():     suspends the timer.
// - silent_continue():  starts the timer. after this call, incoming onFrame() events are
//                       translated into calls to onTime() and onT(), until timeout is exceeded.
//                       when timeout is exceeded, onTimeout() is called. If onTimeout() returns
//                       true, start() is invoked again.
// - silent_reset():     resets the internal counter to zero.
// 
// 
// - isRunning():  may be used to query the current running state.
// 
// Notifications:
// 
// onStart():          issued when start() is called.
// onStop():           issued when stop() is called.
// onReset():          issued when reset() is called.
// onPause():          issued when pause() is called.
// onContinue():       issued when Continue() is called.
// onTimeout():        issued once when the timeout is exceeded
// onTime():           issued each frame as long as the timer is running.
//
// onTimeChanged(theNewTime):          issued when current time value was changed by a client.
// onProgressChanged(theNewProgress):  issued when the progress value was changed by a client.
// 
// 
// onTime(theTime): if timer is active, is called onFrame with theTime since activation.
//
// onProgress(theProgress, theProgressDelta):
//                  called onFrame when timer is running with a theProgress between 0 and 1.
//                  If timeout is positive, theProgress increases linearly with the time, 
//                  and decreases otherwise. TheDeltaProgress is the difference
//                  to the last value of onProgress. by returning a nonzero value,
//                  the client may prevent the timer from issuing the timeout or restarting
//                  in autorepeat mode.
//
// onOverTime(theTime,theTimeDelta):
// onOverProgress(theProgress,theProgressDelta):
//                  issued after the timeout is exceeded. The timer continues
//                  running as long as one of the callbacks return a nonzero value.
// 
//=============================================================================


function TimerWidget() {
        return this.Constructor(this);
}

TimerWidget.prototype.Constructor = function(Public) {

    ////////////////////////////////////////
    // Member 
    ////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////

    Public.WIDGET_META = {};
    Public.WIDGET_META.callbackParams = {};

    Public.WIDGET_META.callbackParams["onTime"] = ["theTime, theTimeIncrement"];
    Public.WIDGET_META.callbackParams["onProgress"] = ["theProgress, theProgressIncrement"];
    Public.WIDGET_META.callbackParams["onOverTime"] = ["theTime, theTimeIncrement"];
    Public.WIDGET_META.callbackParams["onOverProgress"] = ["theProgress, theProgressIncrement"];
    Public.WIDGET_META.callbackParams["onTimeChanged"] = ["theNewTime"];
    Public.WIDGET_META.callbackParams["onProgressChanged"] = ["theNewProgress"];

    ////////////////////////////////////////
    // Public properties
    ////////////////////////////////////////

    Public.repeat = false;
    Public.lastTimeout = 0;

    // internal properties

    Public._myLastTime = 0;
    Public._myStartTime = 0;         // when this timer was started. 0 means "not active".
    Public._myPauseStartTime = 0;    // when the timer was last paused. nonzero means a pause is currently running.


    Public.time getter = function() {
	var myLastTime;
	if (Public._myPauseStartTime) {
	    myLastTime = Public._myPauseStartTime;
	} else {
	    myLastTime = Public._myLastTime;
	}

        if (Public.timeout < 0) {
            return Public._myStartTime - myLastTime;
        }
        return myLastTime - Public._myStartTime;
    }

    Public.time setter = function(theNewTime) {
        if (Public.timeout > 0) {
            Public._myStartTime = (Public._myPauseStartTime?Public._myPauseStartTime:Public._myLastTime) - theNewTime;
        } else {
            Public._myStartTime = (Public._myPauseStartTime?Public._myPauseStartTime:Public._myLastTime) + theNewTime;
        }

        if ("onTimeChanged" in Public) {
            Public.onTimeChanged(theNewTime);
        }
        if ("onProgressChanged" in Public) {
            Public.onProgressChanged(Public.progress);
        }
    }

    Public.progress getter = function() {
        if (Public.timeout > 0) {
           return Public.time / Public.timeout;
       } else {
           return 1 - Public.time / Public.timeout
       }
    }

    Public.progress setter = function(theNewProgress) {
        if (Public.timeout > 0) {
            Public.time = theNewProgress * Public.timeout;
        } else {
            Public.time = (1 - theNewProgress) * Public.timeout;
        }

        if ("onProgressChanged" in Public) {
            Public.onProgressChanged(theNewProgress);
        }
        if ("onTimeChanged" in Public) {
            Public.onTimeChanged(Public.time);
        }
    }

    Public.active getter = function() {
        return Public._myStartTime != 0;
    }
    Public.active setter = function(theFlag) {
        if (theFlag) {
            if (Public.active) {
                Public.Continue();
            } else {
                Public.start();
            }
        } else {
            Public.stop();
        }
    }

    Public.running getter = function() {
        return Public.active && !Public._myPauseStartTime;
    }
    Public.running setter = function(theFlag) {
        if (theFlag == Public.running) {
            return;
        }
        if (theFlag) {
            if (Public.active) {
                Public.Continue();
            } else {
                Public.start();
            }
        } else {
            Public.pause();
        }
    }

    Public.silent_reset = function() {
        if (Public._myStartTime) {
            Public._myStartTime = Public._myLastTime;
        } 
	if (Public._myPauseStartTime) {
	    Public._myPauseStartTime = Public._myLastTime;
	}
        Public.timedout = 0;
	Public._doRepeat = false;
    }

    Public.silent_stop = function() {
	// reset to inactive state, but save current progress.

	if (Public._myStartTime) {
	    if (!Public._myPauseStartTime) {
		Public._myPauseStartTime = Public._myLastTime;
	    }
	    Public._myPauseStartTime -= Public._myStartTime;
	}

	Public._myStartTime = 0;

        Public.timedout = 0;
	Public._doRepeat = false;
    }

    Public.silent_pause = function() {
	if (Public.running) {
	    Public._myPauseStartTime = Public._myLastTime;
        } else if (!Public.active) {
	    Public._myStartTime = Public._myLastTime;
	    Public._myPauseStartTime = Public._myLastTime;
	} 
    }

    Public.silent_continue = function() {
	if (!Public.active) {
	    Public._myStartTime = Public._myLastTime;
	    Public._myPauseStartTime = 0;
	} else if (!Public.running) {
            Public._myStartTime +=  Public._myLastTime - Public._myPauseStartTime;
            Public._myPauseStartTime = 0;
        }
    }



    Public.start = function() {
        if (!Public.active) {
            Public.silent_reset();
	}
	Public.silent_continue();
        Logger.trace(Public.WIDGET_META.name + ": starting timer...");
        if ("onStart" in Public && Public.onStart) {
            Public.onStart(Public);
        }
    }

    Public.stop = function() {
	Public.silent_stop();
        Logger.trace(Public.WIDGET_META.name + ": stopping timer");
        if ("onStop" in Public) {
            Public.onStop(Public);
        }
    }

    // there is no silent_restart - internally, we use
    // restart() only for repeat and the client probably
    // wants to be notified in such a case, too.
    //
    Public.restart = function() {
        Public.silent_stop();
        Public.start();
    }

    Public.reset = function() {
        Public.silent_reset();
        if ("onReset" in Public) {
            Public.onReset(Public);
        }
    }

    Public.pause = function() {
        Public.silent_pause();
        if ("onPause" in Public) {
            Public.onPause(Public);
        }
    }

    Public.Continue = function() {
        Public.silent_continue();
        if ("onContinue" in Public) {
            Public.onContinue(Public);
        }
    }

    ////////////////////////////////////////
    // Public functions
    ////////////////////////////////////////

    Public.onFrame = function(theTime) {

        if (!theTime) {
                return;
        }

        if (!Public._myLastTime) {
            Public._myLastTime = theTime;
        }

        if (Public.running) {

            var myTimeIncrement;
            if (Public.timeout > 0) {
                myTimeIncrement = theTime - Public._myLastTime;
            } else {
                myTimeIncrement = Public._myLastTime - theTime;
            }

            var myProgressIncrement = 1;
            if (Public.timeout != 0) {
                myProgressIncrement = (theTime - Public._myLastTime) / Public.timeout;
	    }

            var overTime = true;
            if (Math.abs(Public.time) < Math.abs(Public.timeout)) {
                overTime = false;
            }

            if (("onProgress" in Public) && Public.onProgress(Public.progress + myProgressIncrement, myProgressIncrement)) {
                overTime = false;
            }
            if (("onTime" in Public && Public.onTime(Public.time + myTimeIncrement, myTimeIncrement))) {
                overTime = false;
            }

	    if (overTime) {
		if (("onOverProgress" in Public) && Public.onOverProgress(Public.time, myProgressIncrement)) {
		    Logger.trace(Public.WIDGET_META.name + " exceeding the time limit is okay. ")
		    overTime = false;
		}
		if (("onOverTime" in Public) && Public.onOverTime(Public.time, myTimeIncrement)) {
		    Logger.trace(Public.WIDGET_META.name + " exceeding the progress limit is okay. ")
		    overTime = false;
		}
	    }

            if (overTime) {
		if (!Public.timedout) {

		    Public.timedout = theTime;

		    Logger.info(Public.WIDGET_META.name + ": timeout.");

		    if ("onTimeout" in Public && Public.onTimeout()) {
			Logger.info(Public.WIDGET_META.name + ": repeat requested...");
			if (Public.repeat == "maybe") {
			    Public._doRepeat = true;
			} else {
			    Logger.info("                              ... and ignored.");

			}
		    }
		    if (Public.repeat && !Public._doRepeat) {
			Logger.info(Public.WIDGET_META.name + ": we repeat automatically.");
			if (typeof Public.repeat == "number") {
			    Public.repeat--;
			}
			Public._doRepeat = true
		    }
		}
	    }
	    if (overTime && Public.timedout) {
		if (Public._doRepeat) {
		    Public.restart();
		} else {
		    Public.stop();
		}
	    }
	}
	Public._myLastTime = theTime;
    }

    Public.setup = function() {
    }
}
