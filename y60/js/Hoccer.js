use("HttpClient.js");

var Hoccer = {};



Hoccer.station = function() {
    var that = {};
    var requestManager = new RequestManager();

    that.userAgent = "Hoccer/0.9dev Y60";
    that.serverUri = "http://beta.hoccer.com";

    //default hoccer station at artcom
    that.longitude = 13.345116;
    that.latitude = 52.501077;
    that.accuracy = 80;
    
    that.upload = function(theFile, theUploadUri) {
        var request = new Request(theUploadUri, that.userAgent);
        
        var body = "--" + "ycKtoN8VURwvDC4sUzYC9Mo7l0IVUyDDVf" + "\r\n" +
        "Content-Disposition: form-data; name=\"" + "upload[attachment]" + "\" "+
        //"filename=\"" + "funkyfilename.txt" + "\"\r\n" +
        //"Content-Type: " + "text/plain" + "\r\n" +
        "Content-Transfer-Encoding: binary\r\n\r\n";

        body += "Hallo Welt.";

        request.onDone  = function() {
            print("handle response: ", this.responseString, "  code: ", this.responseCode);
            var response = eval(this.responseString);
            var uploadUri = response.upload_uri;
            that.upload(theFile, uploadUri);
        }

        request.onError = function () {
            Logger.warning( "HTTP Code received: " 
                            + this.responseCode); 
        };

        request.put(body);
        requestManager.performRequest(request);
    }

    that.buildPeerGroup = function(theParams) {
        var request = new Request(that.serverUri + "/peers",  that.userAgent); 
        request.onDone = (typeof (theParams.onDone)=='undefined'?function(){}:theParams.onDone);
        request.onError = (typeof (theParams.onError)=='undefined'?function(){}:theParams.onError);

        var body = "peer[gesture]=distribute" +
                    "&peer[latitude]=" + that.latitude +
                    "&peer[longitude]=" + that.longitude +
                    "&peer[accuracy]=" + that.accuracy +
                    "&peer[seeder]=1";
        request.post(body);
        requestManager.performRequest(request );
    }

    that.distribute = function(theFile) {
        print("distribute");
        
        that.buildPeerGroup({
           onDone : function() {
               print("handle response: ", this.responseString, "  code: ", this.responseCode);
               var response = eval("("+this.responseString+")");
               var uploadUri = response.upload_uri;
               print("uploadUri: ", uploadUri);
               that.upload(theFile, uploadUri);
           },
           onError : function () {
               Logger.warning( "HTTP Code received: " 
                            + this.responseCode); 
           }
         });
    };

    that.update = function() {
        requestManager.handleRequests();
    };



    return that;
}();

