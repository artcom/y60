/*jslint */
/*globals spark, window*/

/**
 * Wrapper to Y60 worlds.
 *
 * This gets the world of the one and only SceneViewer
 * and wraps it as a spark component.
 *
 * Bottom line: use this as the toplevel component of your app.
 */
spark.World = spark.ComponentClass("World");

spark.World.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Stage);

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize(window.scene.world);
    };
};
