/**
 * Container that switches between a bunch of children.
 */
spark.Switch = spark.ComponentClass("Switch");

spark.Switch.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Transform);

    var _myShown = null;

    Public.shown getter = function() {
        return _myShown;
    };

    Public.shown setter = function(theName) {
        _myShown = theName;

        var myWanted = Public.getChildByName(theName);

        var myChildren = Public.children;
        for(var i = 0; i < myChildren.length; i++) {
            var myChild = myChildren[i];
            myChild.visible = (myWanted != null) && (myChild == myWanted);
        }
    };
};
