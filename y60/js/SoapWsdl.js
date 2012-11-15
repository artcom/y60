/***************************************************************************\

 2007-07-05 (ms):
  Ported to Y60 DOM, upgraded some parts to SOAP1.1 and deleted the rest.
  TODO: parse the response acccording to the WSDL

 Javascript "SOAP Client" library

 @version: 1.4 - 2005.12.10
 @author: Matteo Casati, Ihar Voitka - http://www.guru4.net/
 @description: (1) SOAPClientParameters.add() method returns 'this' pointer.
               (2) "_getElementsByTagName" method added for xpath queries.
               (3) "_getXmlHttpPrefix" refactored to "_getXmlHttpProgID" (full
                   ActiveX ProgID).

 @version: 1.3 - 2005.12.06
 @author: Matteo Casati - http://www.guru4.net/
 @description: callback function now receives (as second - optional - parameter)
               the SOAP response too. Thanks to Ihar Voitka.

 @version: 1.2 - 2005.12.02
 @author: Matteo Casati - http://www.guru4.net/
 @description: (1) fixed update in v. 1.1 for no string params.
               (2) the "_loadWsdl" method has been updated to fix a bug when
               the wsdl is cached and the call is sync. Thanks to Linh Hoang.

 @version: 1.1 - 2005.11.11
 @author: Matteo Casati - http://www.guru4.net/
 @description: the SOAPClientParameters.toXML method has been updated to allow
               special characters ("<", ">" and "&"). Thanks to Linh Hoang.

 @version: 1.0 - 2005.09.08
 @author: Matteo Casati - http://www.guru4.net/
 @notes: first release.

\*****************************************************************************/

function SOAPClientParameters()
{
    var _pl = new Array();
    this.add = function(name, value)
    {
        _pl[name] = value;
        return this;
    }
    this.toXml = function()
    {
        var xml = "";
        for(var p in _pl)
        {
            if(typeof(_pl[p]) != "function")
                xml += "<" + p + ">" + _pl[p].toString().replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;") + "</" + p + ">";
        }
        return xml;
    }
}

function SOAPClient() {}

SOAPClient.invoke = function(theWsdlUrl, theService, thePort, theOperation, theParams, theCookies)
{
    return SOAPClient._loadWsdl(theWsdlUrl, theService, thePort, theOperation, theParams, theCookies);
}

// private: wsdl cache
var SOAPClient_cacheWsdl = new Array();
var _myRequestManager = new RequestManager();

// private: invoke async
SOAPClient._loadWsdl = function(theWsdlUrl, theService, thePort, theOperation, theParams, theCookies)
{
    if ( !(theWsdlUrl in SOAPClient_cacheWsdl) ) {
        // get wsdl
        var xmlHttp = new Request(theWsdlUrl);
        // xmlHttp.verbose = true;
        Logger.info("requesting WSDL from "+theWsdlUrl);
        _myRequestManager.performRequest( xmlHttp );
        while ( _myRequestManager.activeCount ) {
            _myRequestManager.handleRequests();
            msleep( 10 );
        }
        if (xmlHttp.responseCode != 200) {
            throw Exception("SOAPClient_cacheWsdl request failed:" + xmlHttp.responseCode);
        }
        SOAPClient_cacheWsdl[theWsdlUrl] = new Node(xmlHttp.responseString);    // save a copy in cache
    }
    return SOAPClient._sendSoapRequest(theService, thePort, theOperation, theParams, theCookies, SOAPClient_cacheWsdl[theWsdlUrl]);
}

SOAPClient._sendSoapRequest = function(theService, thePort, theOperation, theParams, theCookies, theWSDL)
{
    // get namespace
    var ns =  (theWSDL.childNode('wsdl:definitions').targetNamespace);
    // build SOAP request
    var soapRequest = '<?xml version="1.0" encoding="UTF-8"?>'+
'<soapenv:Envelope'+
' xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/"'+
' xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/"'+
' xmlns:xsd="http://www.w3.org/2001/XMLSchema"'+
' xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'+
'>'+
' <soapenv:Body>'+
'   <ns1:'+theOperation+' '+
'     soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"'+
'     xmlns:ns1="'+ns+'">'+
theParams.toXml()+
'   </ns1:'+theOperation+'>'+
' </soapenv:Body>'+
'</soapenv:Envelope>';
    // Logger.warning(new Node(soapRequest) );
    // TODO: get URL from the WDSL
    var myServiceNodes = theWSDL.childNode('wsdl:definitions').
        getNodesByAttribute('wsdl:service', 'name', theService, false);
    if (myServiceNodes.length != 1) {
        throw Exception("WSDL Service node '"+theService+"' not found. Expected one, found "+myServiceNodes.length);
    }
    var myPortNodes = myServiceNodes[0].getNodesByAttribute('wsdl:port','name', thePort, false);
    if (myPortNodes.length != 1) {
        throw Exception("WSDL Port node '"+thePort+"' not found. Expected one, found "+myPortNodes.length);
    }
    var myAddressNode = myPortNodes[0].childNode('wsdlsoap:address');
    if (! myAddressNode || !('location' in myAddressNode)) {
        throw Exception("Could not find address or location in WSDL Port "+thePort);
    }

    Logger.info("invoking "+theOperation+" at "+myAddressNode.location);
    var xmlHttp = new Request(myAddressNode.location);
    // xmlHttp.verbose = true;
    xmlHttp.post(soapRequest);
    var soapAction = ((ns.lastIndexOf("/") != ns.length - 1) ? ns + "/" : ns) + theOperation;
    xmlHttp.addHttpHeader("SOAPAction", soapAction);
    xmlHttp.addHttpHeader("Content-Type", "text/xml; charset=utf-8");
    if (theCookies) {
        for (var i = 0; i < theCookies.length; ++i) {
            xmlHttp.setCookie( theCookies[i] );
        }
    }

    _myRequestManager.performRequest( xmlHttp );
    while ( _myRequestManager.activeCount ) {
        _myRequestManager.handleRequests();
        msleep( 10 );
    }
    var myResponseXML = new Node(xmlHttp.responseString);

    // TODO: parse the response acccording to the WSDL
    return xmlHttp;
}

