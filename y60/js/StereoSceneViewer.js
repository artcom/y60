use("SceneViewer.js");
use("spark/spark.js");

function StereoSceneViewer(theArguments) {
    this.Constructor(this, theArguments);
}

StereoSceneViewer.prototype.Constructor = function (self, theArguments) {
    SceneViewer.prototype.Constructor(self, theArguments);
    var _ = {};
    var Base = {};
    
    
    /////////////////////
    // Private Members //
    /////////////////////
    _.viewports = {
        mono : null,
        stereo : {
            left: null,
            right: null
        }
    }
    _.cameraTransform   = null;
    _.stereoCameras     = [];
    
    // NOTE: eyeSeparation = _.focalLength / _.focalLengthEyeSeparationFactor
    _.focalLengthEyeSeparationFactor = 30;  // configurable via settings.xml
    _.focalLength     = 3;                  // configurable via settings.xml
    _.cameraAperture  = 45;                 // configurable via settings.xml
    _.projection_flag = 1;                  // configurable via settings.xml
                                            // NOTE: projection_flag = 1: parallel (left viewport for left eye) -> beamer, 
                                            //      projection_flag = -1: cross-view (left viewport for right eye)
    
    _.NEARPLANE_FOCALLENGTH_FACTOR = 1/5; // guessing, but should in any case 
                                          // be < 1 as nearplane = focallength * _.NEARPLANE_FOCALLENGTH_FACTOR
    
    ////////////////////
    // Public Members //
    ////////////////////
    self.stages     = {}; // XXX DEPRECATED: stages should be hold in specific application
    self.stereoFlag = true;
    self.monoCanvas = null;
    self.stereoCanvas = null;
    
    
    /////////////////////
    // Private Methods //
    /////////////////////
    _.setupCameras = function () {
        Logger.info("<StereoSceneViewer::_.setupCameras>");
        
        // create Transform
        _.cameraTransform           = Node.createElement("transform");
        _.cameraTransform.name      = "stereoCamera";
        _.cameraTransform.position  = new Vector3f(self.getActiveCamera().position);         // use position of initially active camera
        _.cameraTransform.orientation = new Quaternionf(self.getActiveCamera().orientation); // use orientation of initially active camera
        window.scene.world.appendChild(_.cameraTransform);
        
        // --- ARRANGE MONO ---
        // viewport
        _.viewports.mono            = self.getActiveViewport();
        _.viewports.mono.name       = "mono-viewport";
        
        self.monoCanvas             = _.viewports.mono.parentNode;
        
        // camera
        _.monoCamera                = self.getActiveCamera();
        _.monoCamera.name           = "mono-camera";
        _.monoCamera.position       = new Vector3f(0, 0, 0);
        _.monoCamera.orientation    = Quaternionf.createFromEuler(new Vector3f(0, 0, 0));
        _.cameraTransform.appendChild(_.monoCamera);
        
        // --- ARRANGE STEREO ---
        // create own canvas
        self.stereoCanvas           = Node.createElement("canvas");
        self.stereoCanvas.name      = "stereo-canvas";
        self.scene.canvases.appendChild(self.stereoCanvas);
        
        // setup cameras
        _.setupStereoCameras("left", [0, 0]);
        _.setupStereoCameras("right", [0.5, 0]);
    };
    
    _.setupStereoCameras = function (theName, thePosition) {
        // create viewport
        _.viewports.stereo[theName]             = Node.createElement("viewport");
        _.viewports.stereo[theName].name        = "stereo-" + theName + "-viewport";
        self.stereoCanvas.appendChild(_.viewports.stereo[theName]);
        _.viewports.stereo[theName].size        = new Vector2f(0.5, 1);
        _.viewports.stereo[theName].position    = new Vector2f(thePosition); // left: [0,0], right: [0.5,0]
        _.viewports.stereo[theName].resizepolicy = "no_adaption";
        
        // create camera
        var myCamera            = Node.createElement("camera");
        myCamera.name           = "stereo-" + theName + "-camera";
        _.cameraTransform.appendChild(myCamera);
        myCamera.frustum        = new Frustum();
        myCamera.frustum.type   = "perspective";
        myCamera.position       = new Vector3f(_.monoCamera.position);
        myCamera.orientation    = new Quaternionf(_.monoCamera.orientation);
        _.stereoCameras.push(myCamera);
        
        // attach camera to viewport
        _.viewports.stereo[theName].camera = myCamera.id;
        
        // create headlight for right eye
        var myHeadLight         = Node.createElement('light');
        myHeadLight.name        = _.viewports.stereo[theName].id + "-headlight";
        var monoHeadLight       = self.scene.world.getElementById(_.viewports.mono.camera).find(".//light");
        myHeadLight.lightsource = monoHeadLight.lightsource;
        myHeadLight.position    = monoHeadLight.position;
        myCamera.appendChild(myHeadLight);
        self.getLightManager().registerHeadlightWithViewport(_.viewports.stereo[theName], myHeadLight);
    };
    
    _.updateStereoView = function () {
        Logger.info("<StereoSceneViewer::_.updateStereoView>");
        
        var horizontalAngleDiv2 = radFromDeg(_.monoCamera.frustum.hfov/2); //radFromDeg(_.cameraAperture/2);
        var verticalAngleDiv2   = radFromDeg(_.monoCamera.frustum.vfov/2);
        var ratio               = self.width / self.height;
        var eyeSeparation       = _.focalLength / _.focalLengthEyeSeparationFactor;
        
        var i, widthDiv2, heightDiv2;
        for (i = 0; i < _.stereoCameras.length; i++) {
            // adjust camera position to focallength
            _.stereoCameras[i].position.z = _.focalLength - magnitude(_.cameraTransform.position);
            
            // calculate asymmetric frustum
            _.stereoCameras[i].frustum.near = _.focalLength * _.NEARPLANE_FOCALLENGTH_FACTOR;
            widthDiv2   = _.stereoCameras[i].frustum.near * Math.tan(horizontalAngleDiv2);
            heightDiv2  = _.stereoCameras[i].frustum.near * Math.tan(verticalAngleDiv2);
            
            _.stereoCameras[i].frustum.top    =   heightDiv2;
            _.stereoCameras[i].frustum.bottom = - heightDiv2;
            
            if (_.stereoCameras[i].id === _.viewports.stereo.left.camera) {
                _.stereoCameras[i].frustum.left   = -ratio * widthDiv2 + 0.5 * eyeSeparation * _.NEARPLANE_FOCALLENGTH_FACTOR * _.projection_flag;
                _.stereoCameras[i].frustum.right  =  ratio * widthDiv2 + 0.5 * eyeSeparation * _.NEARPLANE_FOCALLENGTH_FACTOR * _.projection_flag;
                // adjust camera position to eyeSeparation
                _.stereoCameras[i].position.x     =  _.monoCamera.position.x - eyeSeparation/2 * _.projection_flag;
            } 
            else if (_.stereoCameras[i].id === _.viewports.stereo.right.camera) {
                _.stereoCameras[i].frustum.left  = -ratio * widthDiv2 - 0.5 * eyeSeparation * _.NEARPLANE_FOCALLENGTH_FACTOR * _.projection_flag;
                _.stereoCameras[i].frustum.right =  ratio * widthDiv2 - 0.5 * eyeSeparation * _.NEARPLANE_FOCALLENGTH_FACTOR * _.projection_flag;
                // adjust camera position to eyeSeparation
                _.stereoCameras[i].position.x    =  _.monoCamera.position.x + eyeSeparation/2 * _.projection_flag;
            }
        }
    };
    
    
    ////////////////////
    // Public Methods //
    ////////////////////
    self.setStereo = function () {
        self.getRenderWindow().canvas = self.stereoCanvas;
        self.setActiveViewport(_.viewports.stereo.left); // wether left or right
    };
    
    self.setMono = function () {
        self.getRenderWindow().canvas = self.monoCanvas;
        self.setActiveViewport(_.viewports.mono);
    };
    
    self.setBackgroundColor = function (theColor) {
        self.stereoCanvas.backgroundcolor = theColor;
        self.monoCanvas.backgroundcolor = theColor;
    };
    
    // XXX DEPRECATED: do not use this - actually StereoSceneViewer itself should not now anything 
    //     about how to load its containing stages since this depends on specific application
    self.loadStage = function (theFile, theName) {
        self.stages[theName] = spark.loadFile(theFile);
        self.stages[theName].name = theName;
    };
    
    Base.setup = self.setup;
    self.setup = function() {
        Base.setup.apply(self, arguments);
        
        // setup stereo view
        if (self.stereoFlag) {
            _.setupCameras();
            _.updateStereoView();
        }
        
        // initially set stereo view
        self.setStereo();
        
        // updating stereo-view-parameters according to specified settings.xml
        self.registerSettingsListener(self, "StereoView");
    };
    
    Base.onFrame = self.onFrame;
    self.onFrame = function (theTime, theDeltaT) {
        Base.onFrame(theTime, theDeltaT);
        
        // XXX DEPRECATED: as StereoSceneViewer should not now anything about its 
        //     containing stages this must actually be done in specific application
        for (var stage in self.stages) {
            if (self.stages[stage].hasEventListenersWithType(spark.StageEvent.FRAME)) {
                var myEvent = new spark.StageEvent(spark.StageEvent.FRAME, self.stages[stage], theTime, theDeltaT);
                self.stages[stage].dispatchEvent(myEvent);
            }
        }
    };
    
    Base.onKey = self.onKey;
    self.onKey = function (theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        
         switch (theKey) {
            case 't':
                //if (self.getRenderWindow().canvas.id === self.monoCanvas.id) {
                //    self.setStereo();
                //} else {
                //    self.setMono();
                //}
                break;
            default : 
                break;
        }
    };
    
    self.onUpdateSettings = function (theNode) {
        if (!theNode) {
            return;
        }
        var nodeValue;
        // XXX aperture values (horizontal and vertical) are currently derived 
        //     directly from initially active camera node
        //if (theNode.childNode("CameraAperture")) {
        //    nodeValue = Number(theNode.childNode("CameraAperture").firstChild.nodeValue);
        //    if (nodeValue != _.cameraAperture) {
        //        _.cameraAperture = nodeValue;
        //        _.updateStereoView();
        //    }
        //}
        if (theNode.childNode("FocalLength")) {
            nodeValue = Number(theNode.childNode("FocalLength").firstChild.nodeValue);
            if (nodeValue != _.focalLength) {
                _.focalLength = nodeValue;
                _.updateStereoView();
            }
        }
        if (theNode.childNode("FocalLengthEyeSeparationFactor")) {
            nodeValue = Number(theNode.childNode("FocalLengthEyeSeparationFactor").firstChild.nodeValue);
            if (nodeValue != _.focalLengthEyeSeparationFactor) {
                _.focalLengthEyeSeparationFactor = nodeValue;
                _.updateStereoView();
            }
        }
        if (theNode.childNode("ProjectionType")) {
            nodeValue = Number(theNode.childNode("ProjectionType").firstChild.nodeValue);
            if (nodeValue != _.projection_flag) {
                _.projection_flag = nodeValue;
                _.updateStereoView();
            }
        }
    };
    
    
    self.__defineGetter__("width", function () {
        return window.width;
    });
    
    self.__defineGetter__("height", function () {
        return window.height;
    });
    
    self.__defineGetter__("scene", function () {
        return window.scene;
    });
};