
var ourDebugInputFlag = false;
spark.VideoCapture = spark.ComponentClass("VideoCapture");

spark.VideoCapture.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Image);

    var _myCaptureNode   = null;
    var _myCaptureType   = "";
    var _myIp            = "unknown";
	var _myDeviceId      = 0;
	var _myAVTCameraFlag = 0;
	var _myFramerate     = 30;
	var _myBitsPerPixel  = 24;
	
    Base.realize = Public.realize;
    Public.realize = function(theMaterialOrShape) {
        Base.realize();
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();
        _myCaptureType  = Protected.getString("type");
        _myFramerate    = Protected.getNumber("fps", 30);
        _myBitsPerPixel = Protected.getNumber("bpp", 24);
        
        if (operatingSystem() == "WIN32") {
            if (_myCaptureType == "dshow") {
                plug("DShowCapture");
                _myDeviceId = Protected.getString("device", "0");
                _myAVTCameraFlag  = Protected.getString("avtcamera", "0");
            } else if(_myCaptureType == "alliedethernet") {
                plug("AlliedEthernetCameraCapture");
                _myIp = Protected.getString("ip");
            }
        }         
        		
        createNewCaptureNode(Protected.getNumber("capturewidth", 800),Protected.getNumber("captureheight", 600));        
    }
    
    function createNewCaptureNode(theWidth, theHeight) {
       _myCaptureNode = Node.createElement("capture");
        window.scene.images.appendChild(_myCaptureNode);
        _myCaptureNode.name = "CaptureNode";
        _myCaptureNode.id = createUniqueId();
        _myCaptureNode.playmode = "play";
        _myCaptureNode.src = _myCaptureType + "://ip=" + _myIp + ";input=1device=" + _myDeviceId + "avtcamera=" + _myAVTCameraFlag+"width=" + theWidth + "height="+theHeight+ "deinterlace=0fps=" + _myFramerate + "bpp=" + _myBitsPerPixel;
        Logger.info("Create Capture node with src: " + _myCaptureNode.src);
        if (_myCaptureNode) {
            window.scene.loadCaptureFrame(_myCaptureNode);
        }
        Public.image = _myCaptureNode;
		Public.width = theWidth;
		Public.height = theHeight;        
    }
}
