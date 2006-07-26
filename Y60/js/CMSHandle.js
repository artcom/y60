//==============================================================================
// Copyright (C) 2006, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

plug("y60CMSCache");

function CMSHandle( theConfigFile) {
    this.Constructor(this, theConfigFile);
}

CMSHandle.prototype.Constructor = function(obj, theConfigFile) {

    obj.getPresentation = function() {
        return _myPresentation;
    }

    obj.synchronize = function() {
        var myCMSConfig = _myConfig.childNode("cmscache", 0);
        var myUsername = _myConfig.username;
        if ("domain" in myCMSConfig && myCMSConfig.domain.length) {
             myUsername += "@" + myCMSConfig.domain;
        }
        _myCMSCache = new CMSCache(myCMSConfig.localdir, _myPresentation,
                            myCMSConfig.backend, myUsername, _myConfig.password, _myOCSCookie );
        _myCMSCache.verbose = _myCMSVerbosityFlag;
        if ("maxrequests" in myCMSConfig &&
            myCMSConfig.maxrequests)
        {
            _myCMSCache.maxRequests = Number(myCMSConfig.maxrequests);
        }
        if ("useragent" in myCMSConfig &&
            myCMSConfig.useragent)
        {
            _myCMSCache.useragent = myCMSConfig.useragent;
        }
        if ( _mySyncFlag ) {
            _myCMSCache.synchronize();
        }
    }

    obj.isSynchronized = function() {
        if ( !_mySyncFlag ) {
            return true; 
        }

        return _myCMSCache.isSynchronized();
    }

    obj.__defineGetter__('statusReport',
            function() { return ( _myCMSCache ? _myCMSCache.statusReport : null ) } );
    obj.__defineGetter__('assetDir',
            function() { return _myConfig.childNode("cmscache",0).localdir; } );

    function fetchPresentation() {
        _myPresentation = Node.createDocument();
        if ( _myDummyPresentation ) {
            _myPresentation.parseFile( _myDummyPresentation );
            Logger.info("Using dummy presentation file '" + _myConfig.dummypresentation + "'");
            return; 
        }
        
        var myZopeConfig = _myConfig.childNode("zopeconfig", 0);
        var myLoginRequest = new Request( myZopeConfig.baseurl + "/" + myZopeConfig.loginpage,
                                    _myUserAgent );
        myLoginRequest.post("__ac_name=" + _myConfig.username + 
                            "&__ac_password=" + _myConfig.password + "&proxy=" + _myConfig.password);
        _myRequestManager.performRequest( myLoginRequest );

        while ( _myRequestManager.activeCount ) {
            _myRequestManager.handleRequests();
            msleep( 10 );
        }

        verboseZope("Login request response code: " + myLoginRequest.responseCode );
        if ( myLoginRequest.responseCode == 200 || myLoginRequest.responseCode == 302 ) {
            if ( ! myLoginRequest.getResponseHeader("Set-Cookie")) {
                throw "Failed to get zope session cookie at " + fileline() + ".";
            }
            var myPresentationRequest = new Request( myZopeConfig.baseurl + "/" + myZopeConfig.presentationpage,
                    _myUserAgent );
            var myCookies = myLoginRequest.getAllResponseHeaders("Set-Cookie");
            verboseZope("Login request cookies:");
            for (var i = 0; i < myCookies.length; ++i) {
                verboseZope("   " + myCookies[i]);
                myPresentationRequest.setCookie( myCookies[i] );
            }

            _myRequestManager.performRequest( myPresentationRequest );
            while ( _myRequestManager.activeCount ) {
                _myRequestManager.handleRequests();
                msleep( 10 );
            }
            verboseZope("Presentation request response code: " + myPresentationRequest.responseCode );
            if ( myPresentationRequest.responseCode != 200 ) {
                throw "Failed to get presentation file at " + fileline() + ".";
            }
            _myOCSCookie = myPresentationRequest.getResponseHeader("Set-Cookie");
            _myPresentation.parse( myPresentationRequest.responseString );
            //_myPresentation.saveFile("dummy_presentation.xml");

        } else {
            var myMessage = "Login failed on zope server '" + myLoginRequest.URL + "': " +
                    myLoginRequest.errorString + " at " + fileline() + ".";
            throw myMessage;
        }
    }

    function setup() {
        _myConfigDoc = Node.createDocument();
        _myConfigDoc.parseFile( _myConfigFile );
        _myConfig = _myConfigDoc.childNode("cmsconfig");
        var myZopeConfig = _myConfig.childNode("zopeconfig", 0);
        var myCMSConfig = _myConfig.childNode("cmscache", 0);
        
        _myZopeVerbosityFlag = (myZopeConfig && "verbose" in myZopeConfig && myZopeConfig.verbose != 0);
        _myCMSVerbosityFlag = ("verbose" in myCMSConfig && myCMSConfig.verbose != 0);
        if ("dummypresentation" in _myConfig &&
            _myConfig.dummypresentation.length &&
            fileExists(_myConfig.dummypresentation))
        {
            _myDummyPresentation = _myConfig.dummypresentation;
            
        }
        if ("useragent" in _myConfig &&
            _myConfig.useragent.length)
        {
            _myUserAgent = _myConfig.useragent;
        }
        if ("sync" in myCMSConfig &&
            myCMSConfig.sync)
        {
            _mySyncFlag = Number(myCMSConfig.sync) > 0;
        }
        fetchPresentation();
    }

    function verboseZope( theMessage ) {
        if (_myZopeVerbosityFlag) {
            print( theMessage );
        }
    }

    var _myConfigFile = theConfigFile;
    var _myZopeVerbosityFlag = false;
    var _myCMSVerbosityFlag = false;
    var _myRequestManager = new RequestManager();
    var _myConfigDoc = null;
    var _myConfig = null;
    var _myPresentation = null;
    var _myDummyPresentation = null;
    var _mySyncFlag = true;
    var _myCMSCache = null;
    var _myUserAgent = null;
    var _myOCSCookie = null;

    setup();

}

/*=== USAGE SAMPLE =============

try {
    var myCMS = new CMSHandle("CMSConfig.xml");
    myCMS.synchronize();
    while ( ! myCMS.isSynchronized() ) {
        msleep(10);
    }

    print( myCMS.statusReport );

    includePath( myCMS.assetDir );
} catch (ex) {
    Logger.error( ex );
}
*/
