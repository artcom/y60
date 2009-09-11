/**
 * And even less done... movie nodes.
 */
use("BuildUtils.js");

spark.Movie = spark.ComponentClass("Movie");

spark.Movie.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    // XXX: refactor to ResizableRectangle
    this.Inherit(spark.Body);

    var _myMovie    = null;
    var _myTexture  = null;
    var _myMaterial = null;
    var _myShape    = null;
    var _myBody     = null;

    
    var _myInitialSize = null;

    Public.play = function() {
        _myMovie.playmode = "play";
    };
    
    Public.stop = function() {
        _myMovie.playmode = "stop";
    };
    // XXX A decent movie API should only have two states: playing or stopped. The stopped state
    //     should display the current frame and not blackness, like the y60 player currently does.
    //     To get the functionality I need I have to clutter spark with a bit of y60 legacy. Sorry.
    //     [DS]
    Public.pause = function() {
        _myMovie.playmode = "pause";
    };

    Public.currentFrame getter = function() { return _myMovie.currentframe;};
    Public.currentFrame setter = function(f) {
        _myMovie.currentframe = f;
    };

    Public.src getter = function() { return _myMovie.src;};
    Public.src setter = function(theFileName) {
        _myMovie.src = theFileName;
        window.scene.loadMovieFrame(_myMovie);
        var mySize = getImageSize(_myMovie);
        internalResize(mySize, Public.sceneNode);
    };
    
    Public.volume getter = function() { return _myMovie.volume; };
    Public.volume setter = function(theVolume) {
        _myMovie.volume = theVolume;
    };
    
    Public.framecount getter = function() { return _myMovie.framecount;};
    
    Public.playmode getter = function() { return _myMovie.playmode;};

    Public.movieNode getter = function()  { return _myMovie;};
    
    Public.hasaudio getter = function() { return _myMovie.has_audio;};

    Public.loopcount getter = function() {
        return _myMovie.loopcount;
    };
    Public.loopcount setter = function(theCount) {
        _myMovie.loopcount = theCount;
    };

    Base.realize = Public.realize;
    Public.realize = function() {
        var myMovieSource = Protected.getString("src");

        var myTargetPixelFormat = Protected.getString("targetpixelformat", "YUV420");
        var myAudioFlag = Protected.getBoolean("audio", true);
        var myLoopcount = Protected.getNumber("loopcount", 1);
        var myCacheSize = Protected.getNumber("maxcachesize", 32);
        var myHint = Protected.getString("decoderhint", "FFMpegDecoder2");

        setupMovie(myMovieSource, myAudioFlag, myCacheSize, myTargetPixelFormat, myHint);

        _myTexture  = Modelling.createTexture(window.scene, _myMovie);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";

        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, _myTexture, Public.name + "-material", true);

        if(myTargetPixelFormat == "YUV420") {
            // YUV targetrasterformat allows us to use a shader to convert YUV2RGB, 
            // loadMovieFrame created 3 rasters for us, therefore we need 3 textures
		    _myMaterial.enabled = false;
		    
    		for (var i = 1; i < _myMovie.childNodesLength(); i++) {
        		var myTextureUnit = _myMaterial.childNode("textureunits").appendChild(Node.createElement("textureunit"));
        		myTextureUnit.applymode = TextureApplyMode.modulate;
    			var myTexture = Modelling.createTexture(window.scene, _myMovie);
    			myTexture.image = _myMovie.id;
    			myTexture.image_index = i;
        		myTextureUnit.texture   = myTexture.id;
    	    }

	        var myString = "[100[paint, paint, paint]]";
    		_myMaterial.requires.textures = myString;

    		addMaterialRequirement(_myMaterial, "option", "[10[yuv2rgb]]");
            _myMaterial.enabled = true;
        }
        
        var mySize = new Vector3f(_myMovie.width,_myMovie.height, 0);
        Protected.origin = Protected.getVector3f("origin", [0,0,0]);
        var myLowerLeft = new Vector3f(-Protected.origin.x,-Protected.origin.y,-Protected.origin.z);
        var myUpperRight = new Vector3f(mySize.x - Protected.origin.x, mySize.y - Protected.origin.y, mySize.z - Protected.origin.z);
                
        _myShape    = Modelling.createQuad(window.scene, _myMaterial.id, myLowerLeft, myUpperRight);
        _myShape.name = Public.name + "-shape";
        
        _myInitialSize = mySize;

        var myBody  = Modelling.createBody(Public.parent.innerSceneNode, _myShape.id);
        myBody.name = Public.name;
	
        var myInitialPlaymode = Protected.getString("playmode", "stop");
        _myMovie.playmode = myInitialPlaymode;
        _myMovie.loopcount = myLoopcount;
        
        var myInitialLoopcount = Protected.getString("loopcount", "1");
        _myMovie.loopcount = myInitialLoopcount;

        Base.realize(myBody);
    };
    
    // for audio synced video we need a reload, cause y60 looses sync on looping
    Public.reload = function() {
        var myTextures = [];
        if(_myMovie.targetpixelformat== "YUV420") {
            myTextures  = window.scene.textures.findAll("texture[@image='" + _myMovie.id + "']");
        } else {
            myTextures.push_back(window.scene.textures.find("texture[@image='" + _myMovie.id + "']"));
        }

        var myMovieNode = Node.createElement("movie");
        window.scene.images.appendChild(myMovieNode);
        myMovieNode.src   = _myMovie.src;
        myMovieNode.name  = _myMovie.name;
        myMovieNode.audio = _myMovie.audio;
        myMovieNode.maxcachesize = _myMovie.maxcachesize;
        myMovieNode.volume      = (_myMovie &&  "volume" in _myMovie) ? _myMovie.volume : 1.0;
        myMovieNode.loopcount   = _myMovie.loopcount;
        myMovieNode.resize      = "none";
        myMovieNode.decoderhint = _myMovie.decoderhint;
        myMovieNode.targetpixelformat = _myMovie.targetpixelformat;

        window.scene.loadMovieFrame(myMovieNode);
        _myMovie.parentNode.removeChild(_myMovie);
        _myMovie = myMovieNode;
		for (var i = 0; i < myTextures.length; i++) {
            myTextures[i].image = _myMovie.id;
			myTextures[i].image_index = i;
		}
    }

    function setupMovie(theFilename, theAudioFlag, theCacheSize, theTargetPixelFormat, theHint) {
        _myMovie = Node.createElement("movie");

        window.scene.images.appendChild(_myMovie);

        _myMovie.src = theFilename;
        _myMovie.name = Public.name + "-movie";
        _myMovie.resize = "none";
        _myMovie.loopcount = 1;
        _myMovie.playmode = "stop";
        _myMovie.audio = theAudioFlag
        _myMovie.maxcachesize = theCacheSize;
        _myMovie.targetpixelformat = theTargetPixelFormat;
        _myMovie.decoderhint = theHint;
        /* hmmmmm 
        // XXX: hmmm ...
        if (/.*\.mp4/.exec(theFilename)) {
            _myMovie.decoderhint = "FFMpegDecoder2";
        }*/

        window.scene.loadMovieFrame(_myMovie);
        _myInitialSize = getImageSize(_myMovie);
    };
    
    function internalResize(theNewSize, theBody) {
        if(_myInitialSize != theNewSize) {
            if(theBody == undefined) {
                dumpstack();
                exit(1);
            }
            var myXFactor = theNewSize.x/_myInitialSize.x;
            var myYFactor = theNewSize.y/_myInitialSize.y;
            theBody.scale = new Vector3f(myXFactor, myYFactor, 1);
        }
    };

};
