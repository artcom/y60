
var Hoccer = {};

Hoccer.station = function() {
    var that = {};
    var myRequestManager = new RequestManager();
    var myOnErrorFunc = function () {
            Logger.warning( "HTTP Code received: " 
                            + this.responseCode); 
        };

    that.userAgent = "Hoccer/0.9dev Y60";
    that.serverUri = "http://beta.hoccer.com";

    //default hoccer station at artcom
    that.longitude = 13.345116;
    that.latitude = 52.501077;
    that.accuracy = 80;
    that.onImageCaught = function(){};
    that.onTextCaught = function(){};
    that.onSomethingElseCaught = function(){};

    var MimeTypes = {
        jpg : "image/jpeg",
        png : "image/png",
        txt : "text/plain",
        vcf : "text/x-vcard",
        bin : "raw/binary"
    };
    
    that.upload = function(theFile, theUploadUri) {
        print("try to upload ", theFile, " to ", theUploadUri);
        /*
        var border = "ycKtoN8VURwvDC4sUzYC9Mo7l0IVUyDDVf";
        var request = new Request(theUploadUri, that.userAgent);
        request.addHttpHeader("Content-Type", "multipart/form-data; boundary=" + border);
        var prebodystring = "--" + border + "\r\n" +
        "Content-Disposition: form-data; name=\"" + "upload[attachment]" + "\" "+
        "filename=\"" + theFile + "\"\r\n" +
        "Content-Type: " + getMimeType(theFile) + "\r\n" +
        "Content-Transfer-Encoding: binary\r\n\r\n";
       
        var bodyblock = readFileAsBlock(theFile);
        var postbodystring = "\r\n--" + border + "--\r\n";
        writeStringToFile("body1.help", prebodystring);
        writeBlockToFile("body2.help", bodyblock);
        writeStringToFile("body3.help", postbodystring);
        exec("cat body1.help body2.help body3.help > body.help");
        var block = readFileAsBlock("body.help");

        request.onDone = function() {
            print("upload done. handle response: ", this.responseString, "  code: ", this.responseCode);
        };
        request.onError = myOnErrorFunc;

        request.putBlock(block);
        myRequestManager.performRequest(request);
        
        //helps to make ist fast but is never left...
        while( myRequestManager.activeCount > 0 ) {
            print(myRequestManager.activeCount);
            myRequestManager.handleRequests();
        }
        */
        exec("curl -X PUT -F 'upload[attachment]=@"+theFile+";type="+getMimeType(theFile)+"' "+theUploadUri);
    };

    that.download = function(theDownloadUri) {
        print("download ", theDownloadUri);
        var request = new Request(theDownloadUri, that.userAgent);
        request.onDone = function() {
           
            var contentType = this.getResponseHeader("Content-Type");
            var fileName = this.getResponseHeader("name");
            if (fileName.length == 0) {
                fileName = "tempfile";
                if (contentType.indexOf(MimeTypes.png) > -1) {
                    fileName += ".png";
                } else if (contentType.indexOf(MimeTypes.jpg) > -1) {
                    fileName += ".jpg";
                }
            }
            print("filename: ",fileName);
            print("content-type: ",contentType);
            if (contentType.indexOf(MimeTypes.png) > -1 || contentType.indexOf(MimeTypes.jpg) > -1) {
                writeBlockToFile(fileName, this.responseBlock);
                var f = function(theFileName) {return that.onImageCaught;}();
                f(fileName);
            } else if (contentType.indexOf(MimeTypes.txt) > -1 || contentType.indexOf(MimeTypes.vcf) > -1) {
                var f = function(theString) {return that.onTextCaught;}();
                f(this.responseString);
            } else {
                var f = function(theString) {return that.onSomethingElseCaught;}();
                f(this.responseString);
            }
            //print("download done. handle response: ", this.responseString, "  code: ", this.responseCode);
        };
        request.onError = myOnErrorFunc;
        request.get();
        myRequestManager.performRequest(request);
    };

    that.prepareDownload = function(thePeerUri, theRepeatCount) {
        if (typeof (theRepeatCount) === 'undefined') {
            theRepeatCount = 0;
        }
        print("prepare Download from peer uri ", thePeerUri, "repeatcount: ", theRepeatCount);
        var request = new Request(thePeerUri, that.userAgent);
        request.onDone = function() {
            print("prepare download done. handle response: ", this.responseString, "  code: ", this.responseCode);
            var response = eval("("+this.responseString+")");
            var resources = response.resources;
            var expires = response.expires;
            if (resources.length > 0 && resources[0].length > 0) {
                that.download(resources[0]);
            } else {
                if (theRepeatCount < 7) {
                    theRepeatCount+=1;
                    var delayAni = new GUI.DelayAnimation(1000);
                    delayAni.onFinish = function() {
                        print("call again with repeatcount ", theRepeatCount);
                        that.prepareDownload(thePeerUri,theRepeatCount);
                    };
                    playAnimation(delayAni);
                    //window.setTimeout("that.prepareDownload(\""+thePeerUri+"\","+theRepeatCount+")",1000);
                }
            }
        };
        request.onError = myOnErrorFunc;
        request.get();
        myRequestManager.performRequest(request);
    };

    that.buildPeerGroup = function(theParams) {
        print("buildPeerGroup");
        if (typeof (theParams.isSharing) === 'undefined') {
            theParams.isSharing = false;
        }
        var request = new Request(that.serverUri + "/peers",  that.userAgent); 
        request.onDone = (typeof (theParams.onDone)==='undefined'?function(){}:theParams.onDone);
        request.onError = (typeof (theParams.onError)==='undefined'?function(){}:theParams.onError);

        var body = "peer[gesture]=distribute" +
                    "&peer[latitude]=" + that.latitude +
                    "&peer[longitude]=" + that.longitude +
                    "&peer[accuracy]=" + that.accuracy + 
                    (theParams.isSharing?"&peer[seeder]=1":"");
        request.post(body);
        myRequestManager.performRequest(request);
        print("posted ",body);
    };



    that.distribute = function(theFile) {
        print("distribute");
        
        that.buildPeerGroup({
           isSharing : true,
           onDone : function() {
               print("build peergroup for distribute done. handle response: ", this.responseString, "  code: ", this.responseCode);
               var response = eval("("+this.responseString+")");
               var uploadUri = response.upload_uri;
               print("uploadUri: ", uploadUri);
               that.upload(theFile, uploadUri);
           },
           onError : myOnErrorFunc
         });
    };

    that.catchIt = function() {
        print("catch it");

        that.buildPeerGroup({
            isSharing : false,
            onDone : function() {
               print("build peergroup for catch it done. handle response: ", this.responseString, "  code: ", this.responseCode);
               var response = eval("("+this.responseString+")");
               var peerUri = response.peer_uri;
               that.prepareDownload(peerUri);
            },
            onError : myOnErrorFunc
        });
    };

    that.update = function() {
        myRequestManager.handleRequests();
    };

    that.setCallbacks = function(theCallbacks) {
        if (typeof (theCallbacks.onTextCaught) !== 'undefined') {
            that.onTextCaught = theCallbacks.onTextCaught;
        }
        if (typeof (theCallbacks.onImageCaught) !== 'undefined') {
            that.onImageCaught = theCallbacks.onImageCaught;
        }
        if (typeof (theCallbacks.onSomethingElseCaught) !== 'undefined') {
            that.onSomethingElseCaught = theCallbacks.onSomethingElseCaught;
        }     
    };


    function getMimeType(theFile) {
        if (theFile.substring(theFile.length-4,theFile.length) == ".vcf") {
            return MimeTypes.vcf;
        } else if (theFile.substring(theFile.length-4, theFile.length) == ".txt") {
            return MimeTypes.txt;
        } else if (theFile.substring(theFile.length-4, theFile.length) == ".png") {
            return MimeTypes.png;
        } else if (theFile.substring(theFile.length-4, theFile.length) == ".jpg") {
            return MimeTypes.jpg;
        } else {
            return MimeTypes.bin;
        }
    }



    return that;
}();

