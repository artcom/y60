plug("ProcessFunctions");


function HttpLogger(theHttpServer) {
    this.Constructor(this, theHttpServer);
}

HttpLogger.prototype.Constructor = function(Public, theHttpServer) {

    const SERVER_PORT = 1337;

    var _myHttpServer = theHttpServer;
    var _myStatusHook = function() { return {} };

    Public.registerStatusHook = function( theFunction ) {
        _myStatusHook = theFunction;
    }

    Public.logfileCallback = function( theMethod, theBody, theURI ) {
        var myParameters = { 
            filter: null,
            lines:  -1
        };

        try {
            var myHtmlBegin = "<html>" +
                "    <head>" +
                "        <title> y60 logfile | " + window.title + "</title>" +
                "             <style type=\"text/css\">\n"+ 
                "                 *, BODY, TD, P, HR, LI { \n"+  
                "                     line-height:  125%; \n"+ 
                "                     font-family:  sans-serif; \n"+ 
                "                     font-size:    12px; \n"+ 
                "                 } \n"+ 
                "                 PRE, TT, CODE, PRE * { \n"+ 
                "                     line-height:  110%; \n"+ 
                "                     font-family:  monospace; \n"+ 
                "                     font-size:    14pt; \n"+ 
                "                 } \n"+ 
                "                 B, H1, H2, H3 { \n"+ 
                "                     color:        #f0e58a; \n"+ 
                "                 } \n"+ 
                "                 p.error { \n"+ 
                "                     color:        #ce8887; \n"+ 
                "                 } \n"+ 
                "                 p.warning { \n"+ 
                "                     color:        #f0e58a; \n"+ 
                "                 } \n"+ 
                "                 p.debug { \n"+ 
                "                     color:        #83cceb; \n"+ 
                "                 } \n"+ 
                "                 p.trace { \n"+ 
                "                     color:        #83cceb; \n"+ 
                "                 } \n"+ 
                "             </style> \n"+  
                "         </head> \n"+ 
                "         <body bgcolor=\"#303030\" text=\"#cccccc\" vlink=\"#00ff00\" alink=\"#00ff00\" link=\"#00ff00\"> " + 
                "         <table cellspacing=\"3\" cellpadding=\"0\" border=\"0\"> \n"+ 
                "             <tr>\n"+ 
                "                 <td colspan=\"3\" bgcolor=\"#aaaaaa\"></td>\n"+ 
                "             </tr>\n"; 

            var myClose =  "</table>\n"+ 
                "</body>\n"+ 
                "</html>"; 

            // blaschnabel?lines=100&severity=warning&filter=foo

            Logger.info("theUri: " + theURI );

            var myUriParts = theURI.split("?");
            if ( myUriParts.length==2 ) {
                myParameters = parseParameters( myUriParts[1], myParameters );
            }
            
            var myLines = Number( myParameters.lines );

            var myStatistics = window.scene.statistics;

            var myStatsTable = tableize("<b>log:</b>", window.title ); 
            myStatsTable    += tableize("","");
            myStatsTable    += tableize("<b>current fps:</b>", window.fps.toFixed(1) + " swap=" + window.swapInterval);
            myStatsTable    += tableize("<b>Vertices:</b>", myStatistics.renderedVertices + "/" + myStatistics.vertices);
            myStatsTable    += tableize("<b>Primitives:</b>", myStatistics.renderedPrimitives + "/" + myStatistics.primitives);
            myStatsTable    += tableize("<b>Worldnodes:</b>", myStatistics.worldNodes);
            myStatsTable    += tableize("<b>Bodies:</b>", myStatistics.bodies);
            myStatsTable    += tableize("<b>Lights:</b>", myStatistics.activeLights + "/" + myStatistics.lights);
            myStatsTable    += tableize("<b>Overlays:</b>", myStatistics.overlays);
            myStatsTable    += tableize("<b>Materials:</b>", myStatistics.materials );
            myStatsTable    += tableize("<b>Memory:</b>", asMemoryString(getProcessMemoryUsage()));
            myStatsTable    += tableize("","");

            var myHookData   = _myStatusHook();
            var myHookString = tableizeBlock( myHookData );

            var myLog = "";
            var myLogFileName   = expandEnvironment("${AC_LOG_FILE_NAME}");
            var myLogFileFormat = expandEnvironment("${AC_LOG_FILE_FORMAT}");
            try {
                var myLog = tableize( "<b>opening log:</b> ", myLogFileName );
                var myLog = tableize();
                myLog += formatLog( readFileAsString( myLogFileName ), myLogFileFormat, myLines, myParameters.filter ); 
            } catch (ex) {
                myLog = tableize( "log error: ", "file not found (most likely the log file has not been written.): \"" + myLogFileName + "\" exception: "  + ex );
            }

            return myHtmlBegin + myStatsTable + myHookString + myLog + myClose; 
        } catch (ex) {
            Logger.error( "Exception caught in logfileCallback: " + ex );
            return "exception caught: " + ex;
        }

        return "no log";
    }

    // private functions
    function setup() {
        if (!_myHttpServer) {
            plug("HttpServer");
            _myHttpServer = new HttpServer();
            _myHttpServer.start( "0.0.0.0", SERVER_PORT );
        }
    
        _myHttpServer.registerCallback( "/currentlog", Public, Public.logfileCallback, "text/html" ); 
    }
 
    function parseParameters(theParams, theDefaults) {
        var myParamsArray = theParams.split("&");
        for (var i=0; i<myParamsArray.length; i++) {
            var myKeyValue = myParamsArray[i].split("=");
            theDefaults[myKeyValue[0]] = myKeyValue.length==2 ? myKeyValue[1] : null; 
        }
        return theDefaults;
    }

    // has to be called on frame.
    Public.handleRequests = function() {
        _myHttpServer && _myHttpServer.handleRequests();
    }

    function tableizeBlock(theBlock) {
        var myReturn = "";

        if (!theBlock) {
            return myReturn;
        }
        
        myReturn += tableize( "<b>Application status</b>:" );
        for (var key in theBlock) {
            if (key == "clone") {
                continue;
            }
            myReturn += tableize( "<b>"+key+"</b>:", theBlock[key] );
        }
        myReturn += tableize( "", "" );
        
        return myReturn;
    }
    
    function tableize(firstString, secondString, thirdString, cssClass) {
        firstString  = firstString?firstString:"";
        secondString = secondString?secondString:"";
        thirdString  = thirdString?thirdString:"";

        return "<tr>\n"+ 
            "    <td width=\"20%\" valign=\"top\">\n"+ 
            "        <p "+ (cssClass?("class="+cssClass):"") + ">" + firstString + "</p>\n"+ 
            "    </td>\n"+ 
            "    <td width=\"60%\" valign=\"top\">\n"+ 
            "        <p "+ (cssClass?("class="+cssClass):"") + ">" + secondString + "</p>\n"+ 
            "    </td>\n"+ 
            "    <td width=\"20%\" valign=\"top\">\n"+ 
            "        <p "+ (cssClass?("class="+cssClass):"") + ">" + thirdString + "</p>\n"+ 
            "    </td>\n"+ 
            "</tr>\n";  
    }

    function parseFullFormat(theLine) {
        if (theLine.indexOf("[") != 0) {
            return false;
        }

        try {
            var myLog = {};
            myLog.time = theLine.substring( 0, theLine.indexOf("]") + 1 );
            var myTimeStrippedLine = theLine.substring( theLine.indexOf("]") + 2 );

            myLog.severity = myTimeStrippedLine.substring( 0, myTimeStrippedLine.indexOf("]") + 1 ); 
            var mySevStrippedLine = myTimeStrippedLine.substring( myTimeStrippedLine.indexOf("]") + 1);

            myLog.file = mySevStrippedLine.substring( 0, mySevStrippedLine.indexOf("]") + 1 ); 
            var myFileStrippedLine = mySevStrippedLine.substring( mySevStrippedLine.indexOf("]") + 1);

            myLog.message = myFileStrippedLine;

            return myLog;
        } catch (ex) {
            return false;
        }
    }

    function formatLog( theLogString, theLogFormat, theLineCount, theFilter ) {
        if (!theLogFormat) {
            theLogFormat = "TERSE";
        }

        var myReturnString = "";
        var myStringArray = theLogString.split("\n"); 
        
        var myStartLine = 0;
        if ( theLineCount > 0 && theLineCount < myStringArray.length ) {
            myStartLine = myStringArray.length - theLineCount;
        }
    
        switch(theLogFormat) {
            case "FULL": 
                for (var i=myStartLine; i<myStringArray.length; i++) {
                    
                    var myLine = myStringArray[i];
                    if (theFilter && myLine.indexOf(theFilter) == -1 ) {
                        continue;
                    }

                    var myLog = parseFullFormat( myLine );

                    if (myLog) {
                        myReturnString += tableize( myLog.severity + ":" + myLog.file, 
                                myLog.message, 
                                myLog.time, 
                                myLog.severity.substring(1, myLog.severity.length - 1).toLowerCase() );
                    } else {
                        // printed line
                        myReturnString += tableize( "---- ", myLine, "----", "debug" );
                    }
                }
                break;
            case "TERSE":
                for (var i=myStartLine; i<myStringArray.length; i++) {

                    var myLine = myStringArray[i];
                    if (theFilter && myLine.indexOf(theFilter) == -1 ) {
                        continue;
                    }

                    var mySeverity = myLine.substring( 0, myLine.indexOf(": "));
                    var myLogline  = myLine.substring( myLine.indexOf(": ") +1, myLine.lastIndexOf("[") - 1);
                    var myPostfix  = myLine.substring( myLine.lastIndexOf("[") ); 
                    if (mySeverity.indexOf("###") == 0) {
                        myReturnString += tableize( mySeverity, myLogline, myPostfix, mySeverity.split(" ")[1].toLowerCase() );
                    } else {
                        // printed line
                        myReturnString += tableize( "---- ", myLine, "----", "debug" );
                    }
                }
                break;
            default:
                break;
        }

        return myReturnString;
    }
    
    setup();
}





