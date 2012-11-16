/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

/*=====================USAGE SAMPLE=====================
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

========================================================*/

/*=============CMSConfig elements/attributes=============
    cmsconfig:
     - username, password
     - useragent: HTTP User-Agent field string
     - sync: perform cms synchronization [0|1]
    zopeconfig:
     - baseurl: server URL[:portnumber]
     - loginpage: URL part to post login data to
     - presentationpage: URL part to envoke XML export
     - verbose: ZOPE verbosity on/off [0|1]
     - localfallback: local fallback presentation file path (read/write)
     - versiontag: version tag name i.e. "head", "stable", "testing" (read/write)
    cmscache:
     - localdir: base directory for local chache
     - backend: backend type [OCS|SVN]
     - verbose: backend verbosity on/off [0|1]
     - domain: domain/organisation name to be added to the username for login
     - maxrequests: maximum number of concurrent HTTP requests
     - cleanup: remove stalled assets from local cache
========================================================*/

/*=================SAMPLE CMSConfig.xml=================
<cmsconfig username="y60" password="acclienty60" useragent="Wget/1.10.2">
    <zopeconfig baseurl="http://welt.bmw.artcom.de:8080" loginpage="login/logged_in"
                presentationpage="exportXML" localfallback="CONFIG/presentation.xml"/>
    <cmscache localdir="C:/tmp/cmsmirror" backend="OCS" domain="bmw" maxrequests="5"/>
</cmsconfig>
========================================================*/

/*
Check Server-Side logs at
/opt/oracle/ocs/apps/j2ee/OC4J_Content/application-deployments/content/OC4J_Content_default_island_1/application.log

to debug

*/

plug("CMSCache");

use("SoapWsdl.js");

function CMSHandle(theConfigFile, theLocation) {
    this.Constructor(this, theConfigFile, theLocation);
}

CMSHandle.prototype.Constructor = function(obj, theConfigFile, theLocation) {

    obj.getPresentation = function() {
        return _myPresentation;
    }
    obj.setPresentation = function(thePresentation) {
        _myPresentation = thePresentation;
    }

    obj.synchronize = function(theRandomWaitDuration) {
        if ( _mySyncFlag && _myPresentation === null) {
            obj.fetchPresentation(theRandomWaitDuration);
        } else if ( _myLocalFallback && fileExists(_myLocalFallback) ) {
            Logger.info("CMS synchronization disabled.");
            Logger.info("Using local fallback presentation file '" + _myLocalFallback + "'.");
            _myPresentation = Node.createDocument();
            _myPresentation.parseFile( _myLocalFallback );
        } else {
            throw new Exception("Local fallback presentation file not found at '" +
                    _myLocalFallback+"'.", fileline());
        }

        ensureCMSCache();

        if ( _mySyncFlag ) {
            _myCMSCache.synchronize();
        }
    }

    obj.testConsistency = function() {
        ensureCMSCache();
        _myCMSCache.testConsistency();
    }

    obj.isSynchronized = function() {
        if ( !_mySyncFlag ) {
            return true;
        }
        var myRetVal = _myCMSCache.isSynchronized();
        if (myRetVal && _myOCSCookie) {
            logout();
        }
        return myRetVal;
    }

    obj.__defineGetter__('statusReport',
            function() { return ( _myCMSCache ? _myCMSCache.statusReport : null ) } );

    obj.__defineGetter__('localFallback',
            function() { return _myLocalFallback; } );

    obj.__defineGetter__('syncFlag', function() { return _mySyncFlag; } );

    obj.__defineSetter__('syncFlag', function(theFlag) { _mySyncFlag=theFlag; } );

    obj.__defineGetter__('assetDir', function() { return _myLocalPath; } );

    obj.__defineGetter__('versionTag',
            function() { return _myVersionTag; } );

    obj.__defineSetter__('versionTag',
            function(theVersionTag) { _myVersionTag = theVersionTag; } );

    function ensureCMSCache() {
        if (_myCMSCache) {
            return;
        }
        var myCMSConfig  = _myConfig.childNode("cmscache", 0);
        var myUsername   = _myConfig.username;
        if ("domain" in myCMSConfig &&
            myCMSConfig.domain.length &&
            String(myCMSConfig.backend).toUpperCase() == "OCS") {
            myUsername += "@" + myCMSConfig.domain;
        }
        if (_myOCSCookie) {
            Logger.info("using repository cookie:" + _myOCSCookie);
            _myCMSCache = new CMSCache(_myLocalPath, _myPresentation,
                                myCMSConfig.backend, "", "", _myOCSCookie );
        } else {
            Logger.info("Logging on with user :" + myUsername);
            _myCMSCache = new CMSCache(_myLocalPath, _myPresentation,
                                myCMSConfig.backend, myUsername, _myConfig.password, "" );
        }
        if ('proxy' in _myConfig && _myConfig.proxy) {
            _myCMSCache.setProxy(_myConfig.proxy);
        }
        _myCMSCache.verbose = _myCMSVerbosityFlag;

        if ("cleanup" in myCMSConfig &&
            myCMSConfig.cleanup)
        {
            _myCMSCache.cleanup = Number(myCMSConfig.cleanup) > 0;
        }

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
    }

    function initRequest(theRequest) {
        theRequest.verifyPeer = false;
        theRequest.verbose = false; // _myZopeVerbosityFlag; XXX
        theRequest.addHttpHeader("Pragma",""); // remove default "no-cache" pragma
        if ('proxy' in _myConfig && _myConfig.proxy) {
            theRequest.setProxy(_myConfig.proxy);
        }
        for (var i = 0; i < _myZopeCookies.length; ++i) {
            verboseZope("   " + _myZopeCookies[i]);
            theRequest.setCookie( _myZopeCookies[i] );
        }
    }

    function login() {
        var myCMSConfig  = _myConfig.childNode("cmscache", 0);
        if (String(myCMSConfig.backend).toUpperCase() != "OCS") {
            return true;
        }
        var myParams = new SOAPClientParameters();
        var myUsername = _myConfig.username;
        var myPassword = _myConfig.password;
        if ("domain" in myCMSConfig &&
            myCMSConfig.domain.length &&
            String(myCMSConfig.backend).toUpperCase() == "OCS") {
            myUsername += "@" + myCMSConfig.domain;
        }
        myParams.add('username', myUsername);
        myParams.add('password', myPassword);

        // get the WSDL URL from the theme pool
        var myThemePool = _myPresentation.childNode('presentation').childNode('themepool');
        var wsdlUrl = myThemePool.server+":"+myThemePool.wsdlport;

        //var myResponse = SOAPClient.invoke('http://ocs.pi-center.muc:7778/content/wsdl/RemoteLoginManager.wsdl',
        //        "RemoteLoginManagerService","RemoteLoginManager", "login", myParams, null );
        var myResponse = SOAPClient.invoke('http://'+wsdlUrl+'/content/wsdl/RemoteLoginManager.wsdl',
                "RemoteLoginManagerService","RemoteLoginManager", "login", myParams, null );
        if (myResponse.responseCode > 299) {
            Logger.warning("Could not retrieve session cookie from OCS:" + myResponse.responseCode);
            Logger.info(myResponse.responseString);
            return false;
        }
        var allCookies = myResponse.getAllResponseHeaders("Set-Cookie");
        for (var i = 0; i < allCookies.length; ++i) {
            if (allCookies[i].substr(0,11) == "JSESSIONID=") {
                _myOCSCookie = allCookies[i];
                Logger.info("Retreived session cookie from OCS:" + _myOCSCookie);
                return true;
            }
        }
        return false;
    }

    function logout() {
        if (_myOCSCookie) {
            var myParams = new SOAPClientParameters();
            var myResponse = SOAPClient.invoke('http://ocs.pi-center.muc:7778/content/wsdl/RemoteLoginManager.wsdl',
                    "RemoteLoginManagerService","RemoteLoginManager", "logout", myParams, [ _myOCSCookie ]);
            if (myResponse.responseCode > 299) {
                Logger.warning("Could not logout from OCS:" + myResponse.responseCode);
                Logger.info(myResponse.responseString);
                return false;
            }
            _myOCSCookie = null;
        }
        return true;
    }
    obj.fetchPresentation = function(theRandomWaitDuration) {
        Logger.info ("Fetching presentation.xml");
        if (theRandomWaitDuration) {
            msleep(Math.random()*theRandomWaitDuration*1000);
        }
        _myPresentation = Node.createDocument();
        _myZopeCookies = [];

        var myErrorOccurred = false;
        var myZopeConfig = _myConfig.childNode("zopeconfig", 0);
        if ('loginpage' in myZopeConfig && myZopeConfig.loginpage) {
            var myLoginRequest = new Request( myZopeConfig.baseurl + "/" + myZopeConfig.loginpage,
                                        _myUserAgent );
            myLoginRequest.post("__ac_name=" + _myConfig.username +
                                "&__ac_password=" + _myConfig.password);
            initRequest(myLoginRequest);
            _myRequestManager.performRequest( myLoginRequest );

            while ( _myRequestManager.activeCount ) {
                _myRequestManager.handleRequests();
                msleep( 10 );
            }

            verboseZope("Login request response code: " + myLoginRequest.responseCode );
            if ( myLoginRequest.responseCode != 200 && myLoginRequest.responseCode != 302 ) {
                Logger.error("Login failed on zope server '" + myLoginRequest.URL + "': " +
                        myLoginRequest.errorString + ".", fileline());
                myErrorOccurred = true;
            }
            if (!myErrorOccurred) {
                if ( ! myLoginRequest.getResponseHeader("Set-Cookie")) {
                    Logger.error("No ZOPE cookie in server response.");
                    myErrorOccurred = true;
                } else {
                    _myZopeCookies = myLoginRequest.getAllResponseHeaders("Set-Cookie");
                }
            }
        }
        if (!myErrorOccurred) {
            var myRequestURI = myZopeConfig.baseurl + "/" + myZopeConfig.presentationpage;
            if (_myVersionTag && _myVersionTag.length) {
                myRequestURI += "?versionTag=" + _myVersionTag;
            }

            Logger.debug("Sending request '" + myRequestURI + "'");
            var myPresentationRequest = new Request( myRequestURI, _myUserAgent );
            initRequest(myPresentationRequest);
            _myRequestManager.performRequest( myPresentationRequest );
            while ( _myRequestManager.activeCount ) {
                _myRequestManager.handleRequests();
                msleep( 10 );
            }
            verboseZope("Presentation request response code: " + myPresentationRequest.responseCode );
            if ( myPresentationRequest.responseCode != 200 ) {
                Logger.error("Failed to get presentation file. Server response code " +
                                    myPresentationRequest.responseCode + ".");
                myErrorOccurred = true;
            }
        }
        if ( myErrorOccurred ) {
            if ( _myLocalFallback && fileExists(_myLocalFallback) ) {
                Logger.warning("Using local fallback presentation file '" + _myLocalFallback + "'.");
                _myPresentation.parseFile( _myLocalFallback );
                // do not remove new content, when syncing with 'old' local fallback presentation file:
                _mySyncFlag = false;
            } else {
                throw new Exception("Local fallback presentation file not found at '" +
                                    _myLocalFallback+"'.", fileline());
            }
        } else {
            _myPresentation.parse( myPresentationRequest.responseString );
            _myPresentation.saveFile( _myLocalFallback );
        }
        // now get repository cookie
        if (!myErrorOccurred) {
            if (!login()) {
                Logger.error("Failed to get repository cookie - aborting sync");
                myErrorOccurred = true;
            }
        }
    }

    function setup() {
        _myConfigDoc = Node.createDocument();
        _myConfigDoc.parseFile(_myConfigFile);

        if (theLocation && _myConfigDoc.childNodesLength() > 1) {
            _myConfig = _myConfigDoc.find("//*[@location = '" + theLocation + "']");
        }

        if (!_myConfig) {
            _myConfig = _myConfigDoc.childNode("cmsconfig");
        }

        var myZopeConfig = _myConfig.childNode("zopeconfig", 0);
        var myCMSConfig = _myConfig.childNode("cmscache", 0);

        _myLocalPath = expandEnvironment(myCMSConfig.localdir);
        _myZopeVerbosityFlag = (myZopeConfig && "verbose" in myZopeConfig && myZopeConfig.verbose != 0);
        _myCMSVerbosityFlag = ("verbose" in myCMSConfig && myCMSConfig.verbose != 0);

        if ( "localfallback" in myZopeConfig && myZopeConfig.localfallback.length ) {
            _myLocalFallback = myZopeConfig.localfallback;
        }

        if ("useragent" in _myConfig && _myConfig.useragent.length) {
            _myUserAgent = _myConfig.useragent;
        }

        if ("sync" in _myConfig && _myConfig.sync) {
            _mySyncFlag = Number(_myConfig.sync) > 0;
        }

        if ("versiontag" in myZopeConfig && myZopeConfig.versiontag) {
            _myVersionTag = myZopeConfig.versiontag;
        }
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
    var _myLocalFallback = null;
    var _myLocalPath = null;
    var _mySyncFlag = true;
    var _myCMSCache = null;
    var _myUserAgent = null;
    var _myVersionTag = null;
    var _myZopeCookies = null;
    var _myOCSCookie = null;

    setup();
}
