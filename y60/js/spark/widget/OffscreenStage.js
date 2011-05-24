/*jslint*/
/*globals spark*/

spark.OffscreenStage = spark.ComponentClass("OffscreenStage");
spark.OffscreenStage.Constructor = function (Protected) {
    var Public = this;
    var Base = {};
    var _ = {};
    Public.Inherit(spark.Stage);
    
    
    /////////////////////
    // PRIVATE MEMBERS //
    /////////////////////
    _.width  = null;
    _.height = null;
    _.backgroundColor = null;
    _.position  = null;
    
    
    ////////////////////
    // PUBLIC MEMBERS //
    ////////////////////
    Public.offscreenRenderer = null;
    Public.world = null;
    Public.name = null;
    
    
    /////////////////////
    // PRIVATE METHODS //
    /////////////////////
    _.setupStageEnvironment = function () {
        // setup world
        Public.world = Node.createElement("world");
        window.scene.worlds.appendChild(Public.world);
        
        // setup camera
        var myCamera = Node.createElement("camera");
        myCamera.name = "stagex";
        Public.world.appendChild(myCamera);
        spark.setupCameraOrtho(myCamera, _.width, _.height);
        
        // prepare offscreenImage
        var offScreenImage = Modelling.createImage(window.scene, _.width, _.height, "RGBA");
        offScreenImage.resize = "none";

        // Flip vertically since framebuffer content is upside-down
        var mirrorMatrix = new Matrix4f;
        mirrorMatrix.makeScaling(new Vector3f(1,-1,1));
        offScreenImage.matrix.postMultiply(mirrorMatrix);
        
        // tie camera to offscreenRenderer
        Public.offscreenRenderer = new OffscreenRenderer([_.width, _.height], myCamera, "RGBA", offScreenImage, undefined, true, 0);
        Public.offscreenRenderer.texture.wrapmode = "clamp_to_edge";
        Public.offscreenRenderer.setBackgroundColor(_.backgroundColor);
    };
    
    _.setupSparkEvents = function () {
        Public.addEventListener(spark.StageEvent.FRAME, Public.onFrame);
    };
    
    
    ////////////////////
    // PUBLIC METHODS //
    ////////////////////
    Base.realize = Public.realize;
    Public.realize = function () {
        _.height = Protected.getNumber("width", 200);
        _.width  = Protected.getNumber("height", 200);
        _.backgroundColor = Protected.getVector4f("backgroundColor", [1,1,1,0]);
        _.position  = Protected.getVector3f("position", [0,0,0]);
        Public.name = Protected.getString("name", "offscreen-stage");
        
        _.setupStageEnvironment();
        
        Base.realize(Public.world);
        
        _.setupSparkEvents();
    };
    
    Public.onFrame = function (theEvent) {
        Public.offscreenRenderer.renderarea.renderToCanvas();
    };
    
    
    // width property
    Public.__defineGetter__("width", function () {
        return _.width;
    });
    Public.__defineSetter__("width", function (theWidth) {
        //XXX implement-me
    });
    
    // height property
    Public.__defineGetter__("height", function () {
        return _.height;
    });
    Public.__defineSetter__("height", function (theHeight) {
        //XXX implement-me
    });
    
    // backgroundColor
    Public.__defineGetter__("backgroundColor", function () {
        return _.backgroundColor;
    });
    Public.__defineSetter__("backgroundColor", function (theVector4f) {
        _.backgroundColor = theVector4f;
        Public.offscreenRenderer.setBackgroundColor(_.backgroundColor);
    });
    
    // position
    Public.__defineGetter__("position", function () {
        return _.position;
    });
    Public.__defineSetter__("position", function (theVector3f) {
        _.position = new Vector3f(theVector3f);
    });
};