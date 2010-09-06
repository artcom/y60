/*jslint*/
/*globals spark, Modelling*/

/**
 * Container wrapping a DOM transform.
 */
spark.Transform = spark.ComponentClass("Transform");

spark.Transform.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.Widget);

    Base.realize = Public.realize;
    Public.realize = function () {
        var myTransform = Modelling.createTransform(Public.parent.innerSceneNode, Public.name);
        Base.realize(myTransform);
    };
};