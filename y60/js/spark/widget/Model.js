/*jslint plusplus: false*/
/*globals spark, window, Vector3f, Quaternionf, Modelling*/

// XXX: revisit. good idea though.
spark.Model = spark.ComponentClass("Model");

spark.Model.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Body);

    Base.realize = Public.realize;
    Public.realize = function () {
        var myY60Name = Protected.getString("rootName");
        var myY60DomObject = window.scene.dom.find("//*[@name='" + myY60Name + "']");
        var myPosition = new Vector3f(myY60DomObject.position);
        var myOrientation = new Quaternionf(myY60DomObject.orientation);
        var myScale = new Vector3f(myY60DomObject.scale);
        Base.realize(myY60DomObject);
        Public.position = myPosition;
        Public.orientation = myOrientation;
        Public.scale = myScale;
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
    };

    function propagateAlpha(theNode, theAlpha) {
        if (theNode.nodeName === "body") {
            Modelling.setAlpha(theNode, theAlpha);
        }
        for (var i = 0; i < theNode.childNodesLength(); i++) {
            propagateAlpha(theNode.childNode(i), theAlpha);
        }
    }

    Base.propagateAlpha = Public.propagateAlpha;
    Public.propagateAlpha = function () {
        Base.propagateAlpha();
        propagateAlpha(Public.sceneNode, Public.actualAlpha);
    };
};