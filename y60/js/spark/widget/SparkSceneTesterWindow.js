use("SceneTester.js");

spark.SparkSceneTesterWindow = spark.ComponentClass("SparkSceneTesterWindow");

spark.SparkSceneTesterWindow.Constructor = function (Protected) {
    var Public = this;
    var Base = {};
    var SceneTesterBase = {};
    
    /*
    SceneTester is a SceneViewer
    Window is also a Sceneviewer
    
    so overwriting the local scope can destroy functionality and
    we have to preserve this manually. Luckily this seems to only apply to
    onPostRender
    */
    SceneTester.prototype.Constructor(Public, []);
    SceneTesterBase.onPostRender = Public.onPostRender;
    
    Public.Inherit(spark.Window);
    
    Base.onPostRender = Public.onPostRender;
    Public.onPostRender = function() {
        Base.onPostRender();
        SceneTesterBase.onPostRender();
    };
};