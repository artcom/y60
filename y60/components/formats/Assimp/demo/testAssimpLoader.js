use("SceneViewer.js");
use("Overlay.js");
plug("AssimpLoader");


function AssimpDemo() {
    this.Constructor(this);
}

AssimpDemo.prototype.Constructor = function(Public) {
    SceneViewer.prototype.Constructor(Public);
    var Base ={};

    Base.setup = Public.setup;
    Public.setup = function(width, height, title, theArguments)
    {
        window = new RenderWindow();
        window.multisamples = 4;
        Base.setup(width, height, false, title);
      
        print(arguments[3]);

        var myArguments = arguments[3];

        var myFilename = "duck.dae";
        if ( fileExists(myArguments[0])) {
            myFilename = myArguments[0];
        }
       
        var myForceMaterial = false;
        if (myArguments[1]) {
            print("??");
            myForceMaterial = true;  
        }
        
        print("loading file: " + myFilename + " forcing material: " + myForceMaterial);

        var myTransform = Modelling.createTransform(window.scene.world, "assimpTransform");
        
        var myMaterial = Modelling.createLambertMaterial(window.scene, [1.0,1.0,1.0,1.0], [0.5, 0.5, 0.5, 1.0]);

        var myAssimp = new Assimp.Loader(window.scene);
        myAssimp.readFile(
            myFilename,
            Assimp.GEN_SMOOTH_NORMALS | 
            Assimp.FLIP_UVS| Assimp.PRESET_TARGET_REALTIME_FAST);
        
        myForceMaterial && myAssimp.forceMaterial(myMaterial);
        myAssimp.createScene(myTransform);
       
        print(myTransform);
        window.camera.position = [15.2837,9.40845,68.7203];
    }

    Base.onFrame = Public.onFrame;
    Public.onFrame = function(time) {
        Base.onFrame(time);
    }
}


var ourApp = new AssimpDemo(arguments);
ourApp.setup(1024, 768, "Assimp Demo", arguments);
ourApp.fullscreen = false;
ourApp.go();
