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
    _.leftViewport              = null;
    _.rightViewport             = null;
    _.stereoCameras             = [];
    _.initial_camera_position   = [0, 0, 0];
    _.initial_camera_orientation = [0, 0, 0, 0];
    
    _.focalLengthEyeSeparationFactor = 30;  // configurable via settings.xml
    _.focalLength     = 3;                  // configurable via settings.xml
    _.cameraAperture  = 45;                 // configurable via settings.xml
    _.cameraPosition  = [0,0,_.focalLength];
    _.projection_flag = 1;                  // configurable via settings.xml
                                            // NOTE: projection_flag = 1: parallel (left viewport for left eye) -> beamer, 
                                            //      projection_flag = -1: cross-view (left viewport for right eye)
    // NOTE:  
    // eyeSeparation = _.focalLength / _.focalLengthEyeSeparationFactor
    
    
    ////////////////////
    // Public Members //
    ////////////////////
    self.stages = {};
    
    
    /////////////////////
    // Private Methods //
    /////////////////////
    _.setupStereoCameras = function () {
        Logger.info("<StereoSceneViewer::_.setupStereoCameras>");
        
        // --- LEFT EYE ---
        // viewport
        _.leftViewport              = self.getActiveViewport();
        _.leftViewport.name         = "leftEye";
        _.leftViewport.size         = new Vector2f(0.5, 1);
        _.leftViewport.position     = new Vector2f(0, 0);
        _.leftViewport.resizepolicy = "no_adaption";
        
        // camera
        var cameraLeft              = self.getActiveCamera();
        cameraLeft.name             = "leftEye";
        cameraLeft.position         = new Vector3f(_.initial_camera_position);
        cameraLeft.orientation      = new Vector4f(_.initial_camera_orientation);
        _.stereoCameras.push(cameraLeft);
        
        // --- RIGHT EYE ---
        // create viewport
        _.rightViewport             = Node.createElement("viewport");
        _.rightViewport.name        = "rightEye";
        _.leftViewport.parentNode.appendChild(_.rightViewport);
        _.rightViewport.size        = new Vector2f(0.5,1);
        _.rightViewport.position    = new Vector2f(0.5,0);
        _.rightViewport.resizepolicy = "no_adaption";
        
        // create camera
        var cameraRight             = Node.createElement("camera");
        cameraRight.name            = "rightEye";
        window.scene.world.appendChild(cameraRight); // XXX is this always the correct world?!
        cameraRight.frustum         = new Frustum();
        cameraRight.frustum.type    = "perspective";
        cameraRight.position        = new Vector3f(_.initial_camera_position);
        cameraRight.orientation     = new Vector4f(_.initial_camera_orientation);
        _.stereoCameras.push(cameraRight);
        
        // attach camera to viewport
        _.rightViewport.camera      = cameraRight.id;
        
        // create headlight for right eye
        var rightHeadLight          = Node.createElement('light');
        rightHeadLight.name         = "Headlight_" + _.rightViewport.id;
        var leftHeadLight           = cameraLeft.find(".//light");
        rightHeadLight.lightsource  = leftHeadLight.lightsource;
        rightHeadLight.position     = leftHeadLight.position;
        cameraRight.appendChild(rightHeadLight);
        self.getLightManager().registerHeadlightWithViewport(_.rightViewport, rightHeadLight);
    };
    
    _.updateCameraPosition = function () {
        Logger.info("<StereoSceneViewer::_.updateCameraPosition>");
        _.cameraPosition = new Vector3f(0,0,_.focalLength);
    };
    
    _.updateStereoView = function () {
        Logger.info("<StereoSceneViewer::_.updateStereoView>");
        
        var radians       = radFromDeg(_.cameraAperture/2);
        var ratio         = self.width / self.height;
        var eyeSeparation = _.focalLength / _.focalLengthEyeSeparationFactor;
        
        var i, wd2, ndfl;
        for (i = 0; i < _.stereoCameras.length; i++) {
            // adjust camera position to focallength
            _.stereoCameras[i].position.z = _.focalLength;
            
            // calculate asymmetric frustum
            _.stereoCameras[i].frustum.near = _.focalLength / 5;
            wd2  = _.stereoCameras[i].frustum.near * Math.tan(radians);
            ndfl = _.stereoCameras[i].frustum.near / _.focalLength;
            _.stereoCameras[i].frustum.top    = wd2;
            _.stereoCameras[i].frustum.bottom = -wd2;
            
            if (_.stereoCameras[i].name === "leftEye") {
                _.stereoCameras[i].frustum.left   = -ratio * wd2 + 0.5 * eyeSeparation * ndfl * _.projection_flag;
                _.stereoCameras[i].frustum.right  =  ratio * wd2 + 0.5 * eyeSeparation * ndfl * _.projection_flag;
                // adjust camera position to eyeSeparation
                _.stereoCameras[i].position.x     =  _.cameraPosition.x - eyeSeparation/2 * _.projection_flag;
            } 
            else if (_.stereoCameras[i].name === "rightEye") {
                _.stereoCameras[i].frustum.left  = -ratio * wd2 - 0.5 * eyeSeparation * ndfl * _.projection_flag;
                _.stereoCameras[i].frustum.right =  ratio * wd2 - 0.5 * eyeSeparation * ndfl * _.projection_flag;
                // adjust camera position to eyeSeparation
                _.stereoCameras[i].position.x    =  _.cameraPosition.x + eyeSeparation/2 * _.projection_flag;
            }
        }
    };
    
    
    
    ////////////////////
    // Public Methods //
    ////////////////////
    self.loadStage = function (theFile, theId) {
        self.stages[theId] = spark.loadFile(theFile);
        self.stages[theId].name = self.stages[theId].name;
    };
    
    Base.setup = self.setup;
    self.setup = function() {
        Base.setup.apply(self, arguments);
        
        // setup stereo view
        _.setupStereoCameras();
        _.updateCameraPosition();
        _.updateStereoView();
        
        self.registerSettingsListener(self, "StereoView"); // updates stereo-view-parameters according to specified settings.xml
    };
    
    Base.onFrame = self.onFrame;
    self.onFrame = function (theTime, theDeltaT) {
        Base.onFrame(theTime, theDeltaT);
        
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
            case 's':
                self.stages.first.offscreenRenderer.saveScreenshot("stage.png");
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
        if (theNode.childNode("CameraAperture")) {
            nodeValue = Number(theNode.childNode("CameraAperture").firstChild.nodeValue);
            if (nodeValue != _.cameraAperture) {
                _.cameraAperture = nodeValue;
                _.updateStereoView();
            }
        }
        if (theNode.childNode("FocalLength")) {
            nodeValue = Number(theNode.childNode("FocalLength").firstChild.nodeValue);
            if (nodeValue != _.focalLength) {
                _.focalLength = nodeValue;
                _.updateCameraPosition();
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