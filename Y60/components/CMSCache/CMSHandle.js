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

CMSHandle.USER_AGNET = "Wget/1.10.2";
CMSHandle.VERBOSE_ZOPE_SESSION = true;
CMSHandle.VERBOSE_BACKEND_SESSION = true;
//CMSHandle.DUMMY_PRESENTATION_FILE = "dummy_presentation.xml";

CMSHandle.prototype.Constructor = function(obj, theConfigFile) {

    obj.getPresentation = function() {
        return _myPresentation;
    }

    obj.synchronize = function() {
        var myCMSConfig = _myConfig.childNode("cmscache", 0);
        _myCMSCache = new CMSCache(myCMSConfig.localdir, _myPresentation,
                            myCMSConfig.backend, _myConfig.username, _myConfig.password );
        _myCMSCache.verbose = CMSHandle.VERBOSE_BACKEND_SESSION;
        _myCMSCache.synchronize();
    }

    obj.isSynchronized = function() {
        return _myCMSCache.isSynchronized();
    }

    obj.__defineGetter__('statusReport',
            function() { return ( _myCMSCache ? _myCMSCache.statusReport : null ) } );
    obj.__defineGetter__('assetDir',
            function() { return _myConfig.childNode("cmscache",0).localdir; } );

    function fetchPresentation() {
        _myPresentation = Node.createDocument();
        if ( "DUMMY_PRESENTATION_FILE" in CMSHandle ) {
            _myPresentation.parseFile( CMSHandle.DUMMY_PRESENTATION_FILE );
            return; 
        }
        
        var myZopeConfig = _myConfig.childNode("zopeconfig", 0);
        var myLoginRequest = new Request( myZopeConfig.baseurl + "/" + myZopeConfig.loginpage,
                                    CMSHandle.USER_AGNET );
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
                    CMSHandle.USER_AGNET );
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
        _myConfig = _myConfigDoc.childNode(0);

        fetchPresentation();
    }

    function verboseZope( theMessage ) {
        if (CMSHandle.VERBOSE_ZOPE_SESSION) {
            print( theMessage );
        }
    }

    var _myConfigFile = theConfigFile;
    var _myRequestManager = new RequestManager();
    var _myConfigDoc = null;
    var _myConfig = null;
    var _myPresentation = null;
    var _myCMSCache = null;

    setup();
}

//=== XXX ===========

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
