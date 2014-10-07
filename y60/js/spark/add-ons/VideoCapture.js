/*jslint nomen:false*/
/*globals plug, spark, Node, createUniqueId, window, Logger, operatingSystem*/

spark.VideoCapture = spark.ComponentClass("VideoCapture");
spark.VideoCapture.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.Image);

    /////////////////////
    // Private Members //
    /////////////////////

    var _myCaptureNode   = null;
    var _myCaptureType   = "";
    var _myIp            = "unknown";
    var _myDeviceId      = 0;
    var _myAVTCameraFlag = 0;
    var _myFramerate     = 30;
    var _myBitsPerPixel  = 24;
    var _myGain = -1;
    var _myPacketSize = -1;
    var _myExposure = -1;
    var _myWBFlag = 0;
    var _myPacketSize = 0;
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function createNewCaptureNode(theWidth, theHeight) {
        _myCaptureNode = Node.createElement("capture");
        window.scene.images.appendChild(_myCaptureNode);
        _myCaptureNode.name = "CaptureNode";
        _myCaptureNode.id = createUniqueId();
        _myCaptureNode.playmode = "play";
        _myCaptureNode.src = _myCaptureType + "://ip=" + _myIp + ";input=1device=" + _myDeviceId + "avtcamera=" + _myAVTCameraFlag + "width=" + theWidth + "height=" + theHeight + "deinterlace=0fps=" + _myFramerate + "bpp=" + _myBitsPerPixel + "gain=" + _myGain +"exposure=" + _myExposure + "wbflag=" + _myWBFlag + "packetsize=" + _myPacketSize;
        Logger.info("Create Capture node with src: " + _myCaptureNode.src);
        if (_myCaptureNode) {
            window.scene.loadCaptureFrame(_myCaptureNode);
        }
        Public.image = _myCaptureNode;
        Public.width = Protected.getNumber("width", theWidth);
        Public.height = Protected.getNumber("height", theHeight);
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Base.realize = Public.realize;
    Public.realize = function (theMaterialOrShape) {
        Base.realize(theMaterialOrShape);
        
        // TODO The default type is now set to dshow instead of having no default at all
        // This leads to this Widget being creatable by code - which we should target for all
        // widgets anyway. However if we want to dynamically create one dynamically with type
        // 'alliedethernet' this will break. But at least we can create dshow dynamically instead
        // of nothing. This definitely needs a better way - Ideally all spark widgets should be 
        // dynamically creatable and all in the same way... [AM]
        _myCaptureType  = Protected.getString("type", "dshow");
        _myFramerate    = Protected.getNumber("fps", 30);
        _myBitsPerPixel = Protected.getNumber("bpp", 24);
        _myGain         = Protected.getNumber("gain", -1);
        _myExposure     = Protected.getNumber("exposure", -1);
        _myWBFlag       = Protected.getNumber("wbflag", 0);
        _myPacketSize   = Protected.getNumber("packetsize", 0);
        
        if (operatingSystem() === "WIN32") {
            if (_myCaptureType === "dshow") {
                plug("DShowCapture");
                _myDeviceId = Protected.getString("device", "0");
                _myAVTCameraFlag  = Protected.getString("avtcamera", "0");
            } else if (_myCaptureType === "alliedethernet") {
                plug("AlliedEthernetCameraCapture");
                _myIp = Protected.getString("ip");
            }
        } 
        if (_myCaptureType === "vimba") {
            _myDeviceId = Protected.getString("device", "0");
        }
        
        createNewCaptureNode(Protected.getNumber("capturewidth",  800),
                             Protected.getNumber("captureheight", 600));        
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
    };
    Public.play = function() {
        if(_myCaptureNode.nodeName === "capture") {
            _myCaptureNode.playmode = "play";
        }
    };

    Public.stop = function() {
        if(_myCaptureNode.nodeName === "capture") {
            _myCaptureNode.playmode = "stop";
        }
    };

    Public.pause = function() {
        if(_myCaptureNode.nodeName === "capture") {
            _myCaptureNode.playmode = "pause";
        }
    };
    Public.__defineGetter__("capturenode", function () {
        return _myCaptureNode;
    });

};
