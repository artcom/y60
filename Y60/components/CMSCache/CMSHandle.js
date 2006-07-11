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

CMSHandle.prototype.Constructor = function(obj, theConfigFile) {

    obj.getPresentation = function() {
        return _myPresentation;
    }

    obj.getSychronizer = function() {
        var myCMSConfig = _myConfig.childNode("cmscache", 0);
        return new CMSCache( myCMSConfig.baseurl, myCMSConfig.localdir,
                _myPresentation, _myConfig.username, _myConfig.password );

    }

    obj.getAssetDir = function() {
        return _myConfig.childNode("cmscache",0).localdir;
    }

    function fetchPresentation() {
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

        if ( myLoginRequest.responseCode == 200 || myLoginRequest.responseCode == 302 ) {
            if ( ! myLoginRequest.getResponseHeader("Set-Cookie")) {
                Logger.error("Failed to get zope session cookie.");
                // TODO: throw or something ....
                return;
            }
            var myPresentationRequest = new Request( myZopeConfig.baseurl + "/" + myZopeConfig.presentationpage,
                    CMSHandle.USER_AGNET );
            var myCookies = myLoginRequest.getAllResponseHeaders("Set-Cookie");
            for (var i = 0; i < myCookies.length; ++i) {
                myPresentationRequest.setCookie( myCookies[i] );
            }

            _myRequestManager.performRequest( myPresentationRequest );
            while ( _myRequestManager.activeCount ) {
                _myRequestManager.handleRequests();
                msleep( 10 );
            }
            if ( myPresentationRequest.responseCode != 200 ) {
                Logger.error("Failed to get presentation file.");
                // TODO: throw or something ....
                return;
            }
            _myPresentation = Node.createDocument();
            _myPresentation.parse( myPresentationRequest.responseString );

        } else {
            Logger.error("Login failed on zope server '" + myLoginRequest.URL + "': " +
                    myLoginRequest.errorString );
            // TODO: throw or something ....
        }
    }

    function setup() {
        _myConfigDoc = Node.createDocument();
        _myConfigDoc.parseFile( _myConfigFile );
        _myConfig = _myConfigDoc.childNode(0);

        fetchPresentation();
    }

    var _myConfigFile = theConfigFile;
    var _myRequestManager = new RequestManager();
    var _myConfigDoc = null;
    var _myConfig = null;
    var _myPresentation = null;

    setup();
}

//=== XXX ===========

try {
    var myCMS = new CMSHandle("CMSConfig.xml");
    var mySychronizer = myCMS.getSychronizer();

    mySychronizer.synchronize();
    while ( ! mySychronizer.isSynchronized() ) {
        msleep(10);
    }

    print( mySychronizer.statusReport );

    includePath( myCMS.getAssetDir() );
} catch (ex) {
    print("==================");
    print(ex);
}
