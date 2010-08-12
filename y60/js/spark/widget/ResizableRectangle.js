spark.ResizableRectangle = spark.AbstractClass("ResizableRectangle");

spark.ResizableRectangle.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Body);

    Public.Getter("size", function() {
        return new Vector2f(Public.width, Public.height);
    });

    Public.Setter("size", function(s) {
        Public.width = s.x;
        Public.height = s.y;
    });

    Public.SetterOverride("origin", applyOrigin);

    this.Property("width",  Number, 1, applySize);
    this.Property("height", Number, 1, applySize);

    function applySize() {
        if(_myVertices) {
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

    var _myMaterial;
    var _myShape;
    var _myVertices;

    Base.realize = Public.realize;
    Public.realize = function(theMaterialOrImageOrShape) {
        if (theMaterialOrImageOrShape.nodeName == "shape") {
            _myShape = theMaterialOrImageOrShape;
            var myMateriaId = _myShape
                . childNode("primitives")
                . childNode("elements").material;
            _myMaterial = _myShape.getElementById(myMateriaId);
        } else if (theMaterialOrImageOrShape.nodeName == "material") {
            _myMaterial = theMaterialOrImageOrShape;
        }
        _myMaterial.properties.blendfunction = "[src_alpha,one_minus_src_alpha,one,one_minus_src_alpha]";
        if ( ! _myShape) {
            var tu = _myMaterial.find("./textureunits/textureunit");
            if (tu) {
               var raster = tu.$texture.$image.raster;
               Public.width = raster.width;
               Public.height = raster.height;
            }

            var mySize = new Vector3f(Public.width, Public.height, 0);

            Public.origin = Protected.getVector3f("origin", Public.origin);
            var myLowerLeft = product( Public.origin, -1);
            var myUpperRight = difference( mySize, Public.origin);

            _myShape = Modelling.createQuad(window.scene, _myMaterial.id,
                    myLowerLeft, myUpperRight);
            _myShape.name = Public.name + "-shape";
        }

        _myVertices = _myShape.find(".//*[@name='position']").firstChild.nodeValue;

        var myBody  = Modelling.createBody(Public.parent.innerSceneNode, _myShape.id);
        if(Public.name) {
            myBody.name = Public.name;
        } else {
            myBody.name = "unnamed-spark-body";
        }

        Base.realize(myBody);

        applySize();
    };

    Protected.material getter = function() { return _myMaterial; }
    Protected.shape    getter = function() { return _myShape; }
    Protected.vertices getter = function() { return _myVertices; }
}
