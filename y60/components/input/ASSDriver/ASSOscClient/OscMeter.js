use("SceneViewer.js");

const BOX_WIDTH         = 600;
const BOX_HEIGHT        = 200;
const T_MAX             = 0.1; // 500 ms 
const SLOT_WIDTH        = 2;
const H_VALUES          = (BOX_WIDTH-2*SLOT_WIDTH)/SLOT_WIDTH;

function OscMeter(theArguments, theWidth, theHeight, theTitle) {

    //////////////////////////////////////////////////////////////////////
    // Baseclass construction
    //////////////////////////////////////////////////////////////////////

    var Base   = {};
    var Public = this;
    SceneViewer.prototype.Constructor(Public, theArguments);

    var _myOscReceiver = null;
    var _myUdpSocket = null;
    var _myMode = null;
    var _myOverlay = null;
    var _myImage = null;

    var _myTimeColor         = new Vector4f(1,0,0,1);
    var _myTranspColor       = new Vector4f(0,0,0,1);
    var _myLineColor       = new Vector4f(0.5, 0.5, 0.5, 1);
    var _myBGColor           = _myTranspColor;
    var _myOscPacketCount = 0;
    var _lastTime = 0;
        
    Public.Constructor = function() {
        window = new RenderWindow();
        window.position = [0, 0];

        // Call base class setup
        Public.setup(theWidth, theHeight, false, theTitle);
        window.swapInterval    = 0;
        window.canvas.backgroundcolor = asColor("000000");
    
        var myReceivePort = 6567;
        var myArgs = parseArguments(theArguments, { "port" : "%s",
                                                    "mode" : "%s",
                                                    "host" : "%s"});
        if ("port" in myArgs.options) {
            myReceivePort = Number(myArgs.options.port);
        }
        if ("mode" in myArgs.options &&
            myArgs.options.mode.toLowerCase() == "udp")
        {
            plug("Network");

            _myUdpSocket = new Socket(Socket.UDP, myReceivePort, "INADDR_ANY");
            _myUdpSocket.setBlockingMode(false);
            Logger.debug("socket: " + _myUdpSocket);
        } else {
            plug("OscReceiver");
            
            _myOscReceiver = new OscReceiver(myReceivePort);
            _myOscReceiver.start();        
        }

        var myResult = createPerfOverlay();
        _myOverlay =  myResult.overlay;
        _myImage = myResult.image;
        
        print("Running in " + (_myUdpSocket?"UDP":"OSC") + "-mode on port " + myReceivePort + ".");
    }
    
    function createPerfOverlay() {
        var myImage = window.scene.images.find("image[@name = 'PERF_Overlay']");
        if(!myImage) {
            myImage = Modelling.createImage(window.scene,BOX_WIDTH,BOX_HEIGHT,"RGBA");
            myImage.name = "OSD_Overlay";
        }
        myImage.raster.fillRect(0,0,BOX_WIDTH,BOX_HEIGHT, _myTranspColor);

        var myViewport = Public.getViewportAtWindowCoordinates(0, 0); // get viewport containing upper left pixel
        var myBoxOverlay = new ImageOverlay(window.scene, myImage, [0,0], myViewport.childNode("overlays"));
        myBoxOverlay.width  = BOX_WIDTH;
        myBoxOverlay.height = BOX_HEIGHT;
        myBoxOverlay.position = new Vector2f(((myViewport.size[0] * window.width) - myBoxOverlay.width) / 2,
                                             (((myViewport.size[1] * window.height) - myBoxOverlay.height - 100) * 1));
        myBoxOverlay.visible = true;

        return { "overlay": myBoxOverlay, "image": myImage };
    }

    function YtoMap(theValue) {
        if (theValue < 0) {
            Logger.warning("PerfMeter.drawValue:  value out of range" + theValue);
            return 0;
        }
        if (theValue > 1)  {
            return 1;
        }
        return BOX_HEIGHT*(1-theValue);
    }

    function drawValue(theFrame, theBaseValue, theValue, theColor) {
        var mySlotPos = (theFrame % H_VALUES)*SLOT_WIDTH;
        _myImage.raster.fillRect(mySlotPos,YtoMap(theValue), mySlotPos+SLOT_WIDTH,YtoMap(theBaseValue), theColor);  
        _myImage.raster.fillRect(mySlotPos+SLOT_WIDTH, YtoMap(1),BOX_WIDTH, YtoMap(0), _myTranspColor);  
     }

    function simulationStep(theTime){
        var deltaT = theTime - _lastTime;
        if ((_myOscPacketCount % H_VALUES) == 0) {
            _myImage.raster.fillRect(0, YtoMap(1),BOX_WIDTH, YtoMap(0), _myTranspColor);  
            if (_myTimeColor.x == 1) {
                _myTimeColor = new Vector4f(0,1,0,1);
            } else {
                _myTimeColor = new Vector4f(1,0,0,1);
            }
        }
        drawValue(_myOscPacketCount,  0, deltaT/T_MAX, _myTimeColor);
        _myOscPacketCount++;
        _lastTime = theTime; 
    }

    //////////////////////////////////////////////////////////////////////
    // Callbacks
    //////////////////////////////////////////////////////////////////////

    Public.onOscEvent = function(theNode){
        Logger.trace("node: " +  theNode);
        print(theNode);
        simulationStep(theNode.simulation_time);
    }

    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);
        
        if (_myUdpSocket) {
            while (_myUdpSocket.peek(1)) {
                var myPacket = _myUdpSocket.read();
                Logger.debug("recv: " + myPacket);
                simulationStep(Public.getCurrentTime());
            }
        }
    }

    // Will be called on a keyboard event
    Base.onKey = Public.onKey;
    Public.onKey = function(theKey, theKeyState, theX, theY,
                         theShiftFlag, theControlFlag, theAltFlag)
    {

        if (theControlFlag) {
            Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag);
        } else if (theKeyState) {    
            var cmd = "";
            switch (theKey) {
                case 'space':
                    break;
                default:
                    return;
            }
        }
    }

    Public.Constructor();
}
try {
    var ourShow = new OscMeter(arguments, 800, 640, "Diagnose Tool");
    ourShow.go();
} catch (ex) {
    reportException(ex);
    exit(1);
}
