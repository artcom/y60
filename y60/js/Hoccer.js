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
    


    that.distribute = function(theFile) {
        print("distribute");
        
        var request = new Request(that.serverUri + "/peers",  that.userAgent); 
        var body = "peer[gesture]=distribute" +
                    "&peer[latitude]=" + that.latitude +
                    "&peer[longitude]=" + that.longitude +
                    "&peer[accuracy]=" + that.accuracy +
                    "&peer[seeder]=1";
    

        request.onDone  = function() {
            print("handle response: ", this.responseString, "  code: ", this.responseCode);
        }

        request.onError = function () {
            Logger.warning( "HTTP Code received: " 
                            + this.responseCode); 
        };

        request.post(body);
        requestManager.performRequest(request );

    };

    that.update = function() {
        requestManager.handleRequests();
    };



    return that;
}();

