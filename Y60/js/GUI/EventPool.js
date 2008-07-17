//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function EventPool(theEventPoolNode, theMarket) {
    this.Constructor(this, theEventPoolNode, theMarket);
}

EventPool.prototype.Constructor = function(Public, theEventPoolNode, theMarket) {
    
    Public = this;
   
    var _myConfigNode   = theEventPoolNode;
    var _myActiveEvents = new Array();
    var _myMarket       = theMarket;
    var _mySilentFlag   = false; 

    // layouts
    var _myNewspaperLayouts = null;
    var _myNewspaperStyles  = null;
    var _myNewspaperContent = null;
    
    ////////////////////////////////////////
    // public 
    ////////////////////////////////////////
    
    Public.silent setter = function(theSilentFlag) {
        _mySilentFlag = theSilentFlag;
    }

    Public.evaluateEvents = function(theTime) {
        // evaluate Events
        var myResults = {};
        
        for (var i=0; i<_myActiveEvents.length; ++i) {
            var myEventResults = _myActiveEvents[i].evaluate(theTime);
            
            for (key in myEventResults) {
                if (key in myResults) {
                    myResults[key] += myEventResults[key];
                } else {
                    myResults[key] = myEventResults[key];
                }
            }

            if (myEventResults.finished) {
                _myActiveEvents.splice(i, 1);
                i--;
                Logger.info("removed finished event. New length: "+_myActiveEvents.length);
            }
        }
    
        return myResults;
    }
   
    var _myRumours = {};
    var _myHistory = [];

    Public.triggerRandomEvent = function() {
        var myEvent = null;
        var myRumourQuench = 5; // XXX: configurable

        var myRumourCount = 0;
        for(myRumour in _myRumours) {
            myRumourCount++;
        }

        Selection: while (true) {
            var myIndex = Math.floor(Math.random() * _myConfigNode.childNodesLength("event"));
            myEvent = _myConfigNode.childNode("event", myIndex);

            // skip allready-active rumours
            if(myEvent.name in _myRumours) {
                continue;
            }

            // skip rumour-answers for inactive rumours
            if("endrumour" in myEvent && !(myEvent.endrumour in _myRumours)) {
                continue;
            }

            var myIsHistory = false;
            for(var i = 0; i < _myHistory.length; i++) {
                if(myEvent.name == _myHistory[i]) {
                    continue Selection;
                }
            }

            // do some more rounds if we have rumours
            // so the chance of answering a rumour early is increased
            if(myRumourCount > 0 && myRumourQuench > 0
               && (!("endrumour" in myEvent)
                   || (!myEvent.endrumour in _myRumours))) {
                myRumourQuench--;
                continue;
            }

            break;
        }

        // remember activated rumour
        if("isrumour" in myEvent) {
            _myRumours[myEvent.name] = myEvent;
        }

        // remove rumour list entry if appropriate
        if("endrumour" in myEvent) {
            delete _myRumours[myEvent.endrumour];
        }

        // remember some history
        _myHistory.push(myEvent.name);
        while(_myHistory.length > EVENT_HISTORY) {
            _myHistory.shift();
        }

        Public.triggerEvent(myEvent); 
    }

    Public.triggerEventByName = function(theName) {
        
        var myNode = getDescendantByName(_myConfigNode, theName, true);
        
        if (myNode) {
            Public.triggerEvent(myNode);
        }
    }

    Public.triggerEvent = function(theNode) {
        
        _myActiveEvents.push(new Event(theNode)); 
       
        if (!_mySilentFlag) {
            var myLayer = ourShow.layermanager.getLayerByName("newspaper");
            myLayer.showNews(theNode.name);
        }


        Logger.info("adding new event to queue: "+theNode.name+". current queuesize: "+_myActiveEvents.length);
    }

    Public.preRenderMessages = function() {
        for(var i = 0; i < _myConfigNode.childNodesLength("event"); i++) {
            var myEvent = _myConfigNode.childNode("event", i);
            var myInfo = myEvent.childNode("info");
            var myLayer = ourShow.layermanager.getLayerByName("newspaper");
            myLayer.renderNews(myEvent.name, myInfo);
        }
    }
    
    ////////////////////////////////////////
    // private
    ////////////////////////////////////////
   
    function setup() {
        if (!_mySilentFlag) {
            // setup layout specifics
            Logger.info("Loading newspaper layouts...");
            _myNewspaperLayouts = new Node();
            _myNewspaperLayouts.parseFile(NEWSPAPER_LAYOUTS);
            _myNewspaperLayouts = _myNewspaperLayouts.childNode("layouts", 0);
            ourNewspaperLayouts = _myNewspaperLayouts;

            assignArticleIds();

            Logger.info("Loading newspaper styles...");
            _myNewspaperStyles = new Node();
            _myNewspaperStyles.parseFile(NEWSPAPER_STYLES);
            _myNewspaperStyles = _myNewspaperStyles.childNode("styles", 0);
            ourNewspaperStyles = _myNewspaperStyles;
        }
    }
    
    ////////////////////////////////////////
    // Newspaper goo
    ////////////////////////////////////////

    function assignArticleIds() {
        var myContent = _myConfigNode;
        for(var i = 0; i < myContent.childNodesLength("event"); i++) {
            var myInfo = myContent.childNode("event", i).childNode("info");
            if (myInfo) {
                myInfo.id = "item" + i;
            }
        }
    }

    setup();
}

