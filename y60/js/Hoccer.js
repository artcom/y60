use("HttpClient.js");

var Hoccer = {};



Hoccer.station = function() {
    var that = {};
    var requestManager = new RequestManager();

    that.userAgent = "Hoccer/0.9dev Y60";

    //default hoccer station at artcom
    that.longitude = 13.345116;
    that.latitude = 52.501077;
    that.accuracy = 80;
    


    that.distribute = function(theFile) {
        print("distribute");
        
        var request = new Request("http://www.artcom.de/",  that.userAgent ); 
        
        request.onDone  = function() {
            print("handle response: ", this.responseString, "  code: ", this.responseCode);
        }


        request.onError = function () {
            Logger.warning( "HTTP Code received: " 
                            + this.responseCode + " for request '" 
                            + thePath + "'." ); 
        };

        request.get(); 
        requestManager.performRequest(request );

    };

    that.update = function() {
        requestManager.handleRequests();
    };



    return that;
}();

