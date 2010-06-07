/*jslint*/
/*globals spark, Modelling*/

/**
 * Wrapper to Y60 bodies.
 *
 * Used to wrap everything that is a body in the scene.
 */
spark.Body = spark.AbstractClass("Body");

spark.Body.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Widget);

    Base.propagateAlpha = Public.propagateAlpha;
    Public.propagateAlpha = function () {
        Base.propagateAlpha();
        if (Public.sceneNode) {

            // XXX: why this condition!?
            // XXX: still unknown, but i converted it into an Error
            if (Public.sceneNode.nodeName !== "body") {
                throw new Error("instance of class Body has a non-body scene node");
            }

            Modelling.setAlpha(Public.sceneNode, Public.actualAlpha);
        }
    };
};