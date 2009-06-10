/**
 * Wrapper to Y60 bodies.
 * 
 * Used to wrap everything that is a body in the scene.
 */
spark.Body = spark.AbstractClass("Body");

spark.Body.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Widget);

    Base.propagateAlpha = Public.propagateAlpha;
    Public.propagateAlpha = function() {
        Base.propagateAlpha();
        
        // XXX: why this condition!?
        if (Public.sceneNode.nodeName == "body") {
            Modelling.setAlpha(Public.sceneNode, Public.actualAlpha);
        }
    };
};
