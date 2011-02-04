use("SceneTester.js");

var ourShow = new SceneTester(arguments);

try {
    ourShow.setup();
    var Base = {};

    // create orthonormal projector
    var myCameraOrtho                = window.scene.world.find(".//*[@name = 'Camera_ortho']");
    myCameraOrtho.frustum.type       = ProjectionType.orthonormal;
    myCameraOrtho.frustum.width      = 1;
    myCameraOrtho.frustum.height     = 1;
    var myProjectorOrthoNode         = Node.createElement("projector");
    myProjectorOrthoNode.name        = "Projector_ortho";
    myProjectorOrthoNode.position    = myCameraOrtho.position;
    myProjectorOrthoNode.scale       = myCameraOrtho.scale;
    myProjectorOrthoNode.orientation = myCameraOrtho.orientation;
    myProjectorOrthoNode.pivot       = myCameraOrtho.pivot;
    myProjectorOrthoNode.billboard   = "";
    myProjectorOrthoNode.frustum     = myCameraOrtho.frustum;
    myProjectorOrthoNode.id          = createUniqueId();
    window.scene.world.appendChild(myProjectorOrthoNode); 

    // create perspective projector
    var myCameraPersp                = window.scene.world.find(".//*[@name = 'Camera_persp']");
    var myProjectorPerspNode         = Node.createElement("projector");
    myProjectorPerspNode.name        = "Projector_persp";
    myProjectorPerspNode.position    = myCameraPersp.position;
    myProjectorPerspNode.scale       = myCameraPersp.scale;
    myProjectorPerspNode.orientation = myCameraPersp.orientation;
    myProjectorPerspNode.pivot       = myCameraPersp.pivot;
    myProjectorPerspNode.billboard   = "";
    myProjectorPerspNode.frustum     = myCameraPersp.frustum;
    myProjectorPerspNode.id          = createUniqueId();
    window.scene.world.appendChild(myProjectorPerspNode); 

    // create projective textures
    var myImageNode = Modelling.createImage(window.scene, "tex/testbild02.png");
    var myTextureNode = Modelling.createTexture(window.scene, myImageNode);
    myTextureNode.wrapmode = "clamp";

    // create projective textures

    var myMaterialOrtho = window.scene.materials.find("material[@name = 'Plane_ortho']");
    var myMaterialPersp = window.scene.materials.find("material[@name = 'Plane_persp']");

    var myTextureOrthoNode          = Node.createElement("textureunit");
    myTextureOrthoNode.texture      = myTextureNode.id;
    myTextureOrthoNode.projector    = myProjectorOrthoNode.id;
    myTextureOrthoNode.applymode    = "decal";
    myTextureOrthoNode.sprite       = 0;
    myTextureOrthoNode.resizepolicy = "no_adaption";
    myMaterialOrtho.childNode("textureunits").appendChild(myTextureOrthoNode);

    var myTexturePerspNode          = Node.createElement("textureunit");
    myTexturePerspNode.texture      = myTextureNode.id;
    myTexturePerspNode.projector    = myProjectorPerspNode.id;
    myTexturePerspNode.applymode    = "decal";
    myTexturePerspNode.sprite       = 0;
    myTexturePerspNode.matrix       = new Matrix4f();
    myTexturePerspNode.resizepolicy = "no_adaption";
    myMaterialPersp.childNode("textureunits").appendChild(myTexturePerspNode);


    var myFragment = Node.createDocumentFragment();
    myFragment.appendChild(new Node("<feature name='textures'>[100[paint]]</feature>").firstChild);
    myFragment.appendChild(new Node("<feature name='texcoord'>[100[frontal_projection]]</feature>").firstChild);

    myMaterialOrtho.requires.appendChild(myFragment.cloneNode(true));
    myMaterialPersp.requires.appendChild(myFragment);

/*
    var myFeatureTexturesNode  = Node.createElement("feature");
    myFeatureTexturesNode.name = "textures";
    myFeatureTexturesNode.appendChild(Node.createTextNode("[100[paint]]"));
    myMaterialOrtho.childNode("requires").appendChild(myFeatureTexturesNode);
    myMaterialPersp.childNode("requires").appendChild(myFeatureTexturesNode.cloneNode(true));

    var myFeatureTexcoordNode  = Node.createElement("feature");
    myFeatureTexcoordNode.name = "texcoord";
    myFeatureTexcoordNode.appendChild(Node.createTextNode("[100[frontal_projection]]"));
    myMaterialOrtho.childNode("requires").appendChild(myFeatureTexcoordNode);
    myMaterialPersp.childNode("requires").appendChild(myFeatureTexcoordNode.cloneNode(true));
*/
    Base.onPostRender = ourShow.onPostRender;
    ourShow.onPostRender = function() {
        var myCameras = ourShow.getScene().cameras;
        var myViewport = ourShow.getActiveViewport();
        var myAspect = myViewport.width / myViewport.height;
        for (var i = 0; i < myCameras.length; ++i) {
            if (myCameras[i].id != myViewport.camera) {
                ourShow.getRenderer().drawFrustum( myCameras[i], myAspect);
            }
        }
        Base.onPostRender();
    }
    
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
