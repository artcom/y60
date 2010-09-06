/*jslint nomen:false plusplus:false*/
/*globals spark*/

/**
 * Container that switches between a bunch of children.
 */
spark.Switch = spark.ComponentClass("Switch");

spark.Switch.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.Transform);

    /////////////////////
    // Private Members //
    /////////////////////

    var _myShown = null;

    ////////////////////
    // Public Methods //
    ////////////////////

    Public.__defineGetter__("shown", function () {
        return _myShown;
    });

    Public.__defineSetter__("shown", function (theName) {
        _myShown = theName;
        var myWanted = Public.getChildByName(theName);
        var myChildren = Public.children;
        for (var i = 0; i < myChildren.length; i++) {
            var myChild = myChildren[i];
            myChild.visible = (myWanted !== null) && (myChild === myWanted);
        }
    });
};