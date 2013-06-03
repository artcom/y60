/*jslint*/
/*globals spark, Modelling*/

/**
 * Container wrapping a DOM transform.
 */
spark.External = spark.ComponentClass("External");

spark.External.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    var _d = {}
    _d.renderCallBack = null;
    Public.Inherit(spark.Widget);

    Base.realize = Public.realize;
    Public.realize = function () {
        var myExternal = Modelling.createExternal(window.scene,Public.parent.innerSceneNode, Public.name);
        Base.realize(myExternal);
	    Public.sceneNode.events = ["externalOnRender"]
	    Public.sceneNode.addEventListener("externalOnRender", Public, true, "onRenderCallback");

    };
    Public.onRenderCallback = function(theEvent) {
    	if (_d.renderCallBack) {
    		_d.renderCallBack();
    	}
    }
    Public.registerCallback = function(theCB) {
    	_d.renderCallBack = theCB;
    }

};
