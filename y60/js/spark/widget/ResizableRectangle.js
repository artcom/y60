/*jslint nomen:false*/
/*globals spark, Vector2f, Vector3f, product, difference, Modelling,
          window*/

/* used for addMaterialProperty */
use("BuildUtils.js")

spark.ResizableRectangle = spark.AbstractClass("ResizableRectangle");

spark.ResizableRectangle.Constructor = function (Protected) {
    var Public = this;
    var Base   = {};
    Public.Inherit(spark.Body);

    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myMaterial;
    var _myShape;
    var _myVertices;
    var _myUVCoords = null;
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function applySize() {
        if (_myVertices) {
            var o = Public.origin;
            _myVertices[0] = [-o.x, -o.y, -o.z];
            _myVertices[1] = [Public.width - o.x, -o.y, -o.z];
            _myVertices[2] = [-o.x, Public.height - o.y, -o.z];
            _myVertices[3] = [Public.width - o.x, Public.height - o.y, -o.z];
        }
    }

    ////////////////////
    // Public Methods //
    ////////////////////

    Public.Getter("size", function () {
        return new Vector2f(Public.width, Public.height);
    });

    Public.Setter("size", function (theSize) {
        // XXX this triggers applySize twice, modifying the SceneGraph twice
        // The spark.Property's internal value cannot be changed not triggering
        // the handle - which is architecturally fine but is a problem here.
        // Another way could be to use dirtyFlags but this has its own problems.
        // Main problem there would be that the dirtyFlag only gets cleared by
        // applying the change to the SceneGraph immediately before rendering
        // takes place making direct access to the scenegraph dangerous.
        // example: setter for size is called, width is modified and dirty flag is set
        // Then another part [1] accesses the shape directly and relies on the
        // vertex values of the vertices.
        // then the onPreRender gets called and the real changes from the size
        // setter gets applied and the dirty flag gets reset.
        // As can be seen this would lead to inconsistent values for [1]
        Public.width = theSize.x;
        Public.height = theSize.y;
    });

    
    Public.Property("width",  Number, 1, applySize, CALL_HANDLER_ON_CHANGE);
    Public.Property("height", Number, 1, applySize, CALL_HANDLER_ON_CHANGE);

    // ORIGIN
    Public.Getter("origin", function () {
        return new Vector3f(Public.originX, Public.originY, Public.originZ);
    });

    Public.Setter("origin", function (theValue) {
        // XXX this triggers applySize three times. See comment above^^
        Public.originX = theValue.x;
        Public.originY = theValue.y;
        Public.originZ = theValue.z;
    });

    Public.Property("originX", Number, 0.0, applySize, CALL_HANDLER_ON_CHANGE);
    Public.Property("originY", Number, 0.0, applySize, CALL_HANDLER_ON_CHANGE);
    Public.Property("originZ", Number, 0.0, applySize, CALL_HANDLER_ON_CHANGE);

    Base.realize = Public.realize;
    Public.realize = function (theMaterialOrImageOrShape) {
        if (theMaterialOrImageOrShape.nodeName === "shape") {
            _myShape = theMaterialOrImageOrShape;
            var myMateriaId = _myShape.childNode("primitives").childNode("elements").material;
            _myMaterial = _myShape.getElementById(myMateriaId);
        } else if (theMaterialOrImageOrShape.nodeName === "material") {
            _myMaterial = theMaterialOrImageOrShape;
        }
        // set the material to allow proper layering of transparencies
        // after rendering, the framebuffer will have the proper alpha
        _myMaterial.properties.blendfunction = "[src_alpha,one_minus_src_alpha,one,one_minus_src_alpha]";
        var myTextureUnit = _myMaterial.find("./textureunits/textureunit");
        if (!_myShape) {
            if (myTextureUnit) {
                var raster = myTextureUnit.$texture.$image.raster;
                Public.width = raster.width;
                Public.height = raster.height;
            }
            var mySize = new Vector3f(Public.width, Public.height, 0);
            var myOrigin = Protected.getString("origin", undefined);
            if (myOrigin === "center") {
                Public.origin = product(mySize, 0.5);
            } else {
                Public.origin = Protected.getVector3f("origin", Public.origin);
            }
            var myLowerLeft = product(Public.origin, -1);
            var myUpperRight = difference(mySize, Public.origin);
            _myShape = Modelling.createQuad(window.scene, _myMaterial.id,
                                            myLowerLeft, myUpperRight);
            // or more generic for later enhancements
            /*
            _myShape = Modelling.createPlane(window.scene, _myMaterial.id,
                       myLowerLeft, myUpperRight,
                       1,1);
            */
            _myShape.name = Public.name + "-shape";
        }
                
        if (myTextureUnit) {
            _myUVCoords = _myShape.find(".//*[@name='uvset']").firstChild.nodeValue;
        }

        var myTargetBuffers = Protected.getString("targetbuffers", undefined);
        if (myTargetBuffers !== undefined) {
            _myMaterial.properties.targetbuffers = myTargetBuffers;
        }
        var myDepthTest = Protected.getBoolean("depthtest", undefined);
        if (myDepthTest !== undefined) {
            var myDepthTestNode = _myMaterial.find(".//bool[@name='depthtest']");
            if (!myDepthTestNode) {
                addMaterialProperty(_myMaterial, "bool", "depthtest", myDepthTest);
            } else {
                myDepthTestNode.firstChild.nodeValue = myDepthTest;
            }
        }
        
        _myVertices = _myShape.find(".//*[@name='position']").firstChild.nodeValue;

        var myBody = Modelling.createBody(Public.parent.innerSceneNode, _myShape.id);
        if (Public.name) {
            myBody.name = Public.name;
        } else {
            myBody.name = "unnamed-spark-body";
        }

        Base.realize(myBody);
        applySize();
    };

    Protected.__defineGetter__("material", function () {
        return _myMaterial;
    });
    Protected.__defineSetter__("material", function (theMaterial) {
        _myMaterial = theMaterial;
        _myShape.childNode("primitives").childNode("elements").material = _myMaterial.id;
    });
    Protected.__defineGetter__("shape", function () {
        return _myShape;
    });
    Protected.__defineGetter__("vertices", function () {
        return _myVertices;
    });
    Protected.__defineGetter__("uvset", function () {
        return _myUVCoords;
    });
    
};
