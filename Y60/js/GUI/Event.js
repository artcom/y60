//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Y60JSSL.js");

function Event(theEventNode) {
    this.Constructor(this, theEventNode);
}

Event.prototype.Constructor = function(Public, theEventNode) {
    
    Public = this;
   
    const NONE   = "none";
    const STATIC = "static";
    const NEWS   = "news";
    const EASE_IN_TIME  = 5;
    const EASE_OUT_TIME = 1;

    var _myConfigNode   = theEventNode;
    var _myActiveEvents = new Array();
    var _myName         = "eventname not set";
    var _myStartTime    = 0;
    var _myDuration     = 0;

    ////////////////////////////////////////
    // public 
    ////////////////////////////////////////
    
    Public.name getter = function() {
        return _myName;
    }

    Public.evaluate = function(theTime) { 
        if (!_myStartTime) {
            _myStartTime = theTime; 
        }
        
        var myReturn = {};

        var myInfluences     = _myConfigNode.childNode("influences");
        var myEventTime      = theTime - _myStartTime;
        var myMultiplicator  = calculateInfluence(myEventTime);
    
        for (var i=0; i<myInfluences.childNodesLength(); ++i) {
            var myCompany    = myInfluences.childNode(i).company;
            var myPercentage = Number(myInfluences.childNode(i).percentage);

            myReturn[myCompany] = myMultiplicator * (myPercentage * 0.01); 
        }
        
        myReturn["finished"] = (myEventTime > _myDuration); 

        return myReturn;
    }
    
    ////////////////////////////////////////
    // private
    ////////////////////////////////////////
  
    function calculateInfluence(theTime) {
        if (theTime < EASE_IN_TIME) {
            return easeIn(theTime/EASE_IN_TIME);
        } else if (_myDuration-theTime < EASE_OUT_TIME ) {
            return easeOut((_myDuration-theTime)/EASE_OUT_TIME);
        }        

        return 1;  
    }

    function setup() {
         
        _myName = _myConfigNode.name;
        _myDuration = Number(_myConfigNode.duration);
        _myStartTime = 0;
    }

    setup();
}

