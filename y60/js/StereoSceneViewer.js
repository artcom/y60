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
    _.stages = {};
    
    _.leftViewport              = null;
    _.rightViewport             = null;
    _.stereoCameras             = [];
    _.initial_camera_position   = [0, 0, 0];
    _.initial_camera_orientation = [0, 0, 0, 0];
    
    // XXX make this configurable
    _.focalLengthEyeSeparationFactor = 30; // eyeSeparation = _.focalLength / _.focalLengthEyeSeparationFactor
    _.focalLength     = 3;
    _.cameraAperture  = 45;
    _.cameraPosition  = [0,0,_.focalLength]; // indirectly set in settings via focalLength
    _.projection_flag = 1;    //  1: parallel (left viewport for left eye) -> beamer, 
                              // -1: cross-view (left viewport for right eye)
    
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
    
    _.setupStage = function (theStage) {
        theStage.offscreenRenderer.texture.mag_filter ="nearest";
        
        var myMaterial = Modelling.createUnlitTexturedMaterial(self.scene, theStage.offscreenRenderer.texture);
        myMaterial.transparent = true;
        
        var myQuad = Modelling.createQuad(self.scene, myMaterial.id, new Vector3f(-1, -1, 0), new Vector3f( 1,  1, 0));
        
        theStage.offscreenBody = Modelling.createBody(self.scene.world, myQuad.id);
        theStage.offscreenBody.name = "OffscreenBody";
        if ("name" in theStage) {
            theStage.offscreenBody.name = theStage.offscreenBody.name + "_" + theStage.name;
        }
        theStage.offscreenBody.position = theStage.position;
    };
    
    
    ////////////////////
    // Public Methods //
    ////////////////////
    self.addStage = function (theFile, theId, thePosition) {
        _.stages[theId] = spark.loadFile(theFile);
        _.stages[theId].name = _.stages[theId].name;
        
        _.setupStage(_.stages[theId]);
        
        if (thePosition) {
            _.stages[theId].offscreenBody.position = thePosition;
        }
    };
    
    Base.setup = self.setup;
    self.setup = function() {
        Base.setup.apply(self, arguments);
        
        // setup stereo view
        _.setupStereoCameras();
        _.updateCameraPosition();
        _.updateStereoView();
    };
    
    Base.onFrame = self.onFrame;
    self.onFrame = function (theTime, theDeltaT) {
        Base.onFrame(theTime, theDeltaT);
        
        for (var stage in _.stages) {
            if (_.stages[stage].hasEventListenersWithType(spark.StageEvent.FRAME)) {
                var myEvent = new spark.StageEvent(spark.StageEvent.FRAME, _.stages[stage], theTime, theDeltaT);
                _.stages[stage].dispatchEvent(myEvent);
            }
        }
    };
    
    Base.onKey = self.onKey;
    self.onKey = function (theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        
         switch (theKey) {
            case 's':
                _.stages.first.offscreenRenderer.saveScreenshot("stage.png");
                break;
            default : 
                break;
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