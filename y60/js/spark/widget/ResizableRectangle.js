/*jslint nomen:false*/
/*globals spark, Vector2f, Vector3f, product, difference, Modelling,
          window*/

spark.ResizableRectangle = spark.AbstractClass("ResizableRectangle");

spark.ResizableRectangle.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    this.Inherit(spark.Body);

    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myMaterial;
    var _myShape;
    var _myVertices;
    
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

    function applyOrigin(o, theBaseSetter) {
        theBaseSetter(o);
        applySize();
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////

    Public.Getter("size", function () {
        return new Vector2f(Public.width, Public.height);
    });

    Public.Setter("size", function (theSize) {
        Public.width = theSize.x;
        Public.height = theSize.y;
    });

    Public.SetterOverride("origin", applyOrigin);

    this.Property("width",  Number, 1, applySize);
    this.Property("height", Number, 1, applySize);

    Base.realize = Public.realize;
    Public.realize = function (theMaterialOrImageOrShape) {
        if (theMaterialOrImageOrShape.nodeName === "shape") {
            _myShape = theMaterialOrImageOrShape;
            var myMateriaId = _myShape
                . childNode("primitives")
                . childNode("elements").material;
            _myMaterial = _myShape.getElementById(myMateriaId);
        } else if (theMaterialOrImageOrShape.nodeName === "material") {
            _myMaterial = theMaterialOrImageOrShape;
        }
        // set the material to allow proper layering of transparencies
        // after rendering, the framebuffer will have the proper alpha
        _myMaterial.properties.blendfunction = "[src_alpha,one_minus_src_alpha,one,one_minus_src_alpha]";
        if (!_myShape) {
            var tu = _myMaterial.find("./textureunits/textureunit");
            if (tu) {
                var raster = tu.$texture.$image.raster;
                Public.width = raster.width;
                Public.height = raster.height;
            }
            var mySize = new Vector3f(Public.width, Public.height, 0);
            Public.origin = Protected.getVector3f("origin", Public.origin);
            var myLowerLeft = product(Public.origin, -1);
            var myUpperRight = difference(mySize, Public.origin);
            _myShape = Modelling.createQuad(window.scene, _myMaterial.id,
                    myLowerLeft, myUpperRight);
            _myShape.name = Public.name + "-shape";
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
    Protected.__defineGetter__("shape", function () {
        return _myShape;
    });
    Protected.__defineGetter__("vertices", function () {
        return _myVertices;
    });
};