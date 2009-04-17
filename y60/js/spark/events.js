
spark.EventPhase = {
    CAPTURING: "capturing",
    TARGET:    "target",
    BUBBLING:  "bubbling"
};


spark.Event = spark.AbstractClass("Event");

spark.Event.Constructor = function(Protected, theType, theBubbles, theCancelable) {
    var Public = this;


    var _myBubbles = theBubbles != null ? theBubbles : false;

    Public.bubbles getter = function() {
        return _myBubbles;
    };

    var _myCancelable = theCancelable != null ? theCancelable : false;

    Public.cancelable getter = function() {
        return _myCancelable;
    };

    var _myCurrentTarget = null;

    Public.currentTarget getter = function() {
        return _myCurrentTarget;
    };

    var _myCurrentPhase = spark.EventPhase.CAPTURING;

    Public.currentPhase getter = function() {
        return _myCurrentPhase;
    };

    var _myTarget = null;

    Public.target getter = function() {
        return _myTarget;
    };

    var _myType = theType;

    Public.type getter = function() {
        return _myType;
    };


    Public.startDispatch = function(theTarget) {
        _myTarget = theTarget;
    };

    Public.dispatchTo = function(theCurrentTarget, theCurrentPhase) {
        _myCurrentTarget = theCurrentTarget;
        _myCurrentPhase  = theCurrentPhase;
    }

};

spark.EventDispatcher = spark.AbstractClass("EventDispatcher");

spark.EventDispatcher.Constructor = function(Protected) {
    var Public = this;

    this.Inherit(spark.Container);

    var _myListenersByType = {};

    Public.addEventListener = function(theType, theListener, theUseCapture) {
        if(theUseCapture == null) {
            theUseCapture = false;
        }

        var myListener = {
            type:       theType,
            listener:   theListener,
            useCapture: theUseCapture,
        };

        if(! (theType in _myListenersByType)) {
            _myListenersByType[theType] = [];
        }

        _myListenersByType[theType] = _myListenersByType[theType].concat(myListener);
    };

    Public.getEventListeners = function(theType) {
        if(theType in _myListenersByType) {
            var myListeners = _myListenersByType[theType];
            if(myListeners.length > 0) {
                return myListeners;
            }
        }
        return [];
    };

    Public.dispatchEvent = function(theEvent) {
        theEvent.startDispatch(Public);

        // collect events to capture on
        var myCaptureList = [Public];
        while(myCaptureList[0].parent != null) {
            myCaptureList.unshift(myCaptureList[0].parent);
        }
        myCaptureList.pop();

        // capture phase
        for(var i = 0; i < myCaptureList.length; i++) {
            var myCurrentTarget = myCaptureList[i];
            if(myCurrentTarget.hasEventListener(theEvent.type)) {
                theEvent.dispatchTo(myCurrentTarget, spark.EventPhase.CAPTURING);
                var myListeners = myCurrentTarget.getEventListeners(theEvent.type);
                for(var j = 0; j < myListeners.length; j++) {
                    var myListener = myListeners[j];
                    if(myListener.useCapture) {
                        myListener.listener.call(myCurrentTarget, theEvent);
                    }
                }
            }
        }

        // target phase
        theEvent.dispatchTo(Public, spark.EventPhase.TARGET);
        if(Public.hasEventListener(theEvent.type)) {
            var myListeners = Public.getEventListeners(theEvent.type);
            for(var j = 0; j < myListeners.length; j++) {
                var myListener = myListeners[j];
                myListener.listener.call(Public, theEvent);
            }
        }

        // bubbling phase
        if(theEvent.bubbles) {
            Logger.fatal("spark event bubbling has not been implemented yet");
        }
        
    };

    Public.hasEventListener = function(theType) {
        if(theType in _myListenersByType) {
            var myListeners = _myListenersByType[theType];
            if(myListeners.length > 0) {
                return true;
            }
        }
        return false;
    };

    Public.removeEventListener = function(theType, theListener, theUseCapture) {
        Logger.fatal("removeEventListener has not been implemented yet");
    };

    Public.willTrigger = function(theType) {
        if(Public.hasEventListener(theType)) {
            return true;
        }
        for(var i = 0; i < Public.children.length; i++) {
            var myChild = Public.children[i];
            if(myChild.willTrigger(theType)) {
                return true;
            }
        }
        return false;
    };

};

spark.TestEvent = spark.Class("TestEvent");

spark.TestEvent.Constructor = function(Protected) {

    this.Inherit(spark.Event, "test-event");

};
