
if("sparkProximatrix" in this) {
    spark.proximatrix = plug("ASSEventSource");
}

spark.Cursor = spark.Class("Cursor");

spark.Cursor.Constructor = function(Protected, theId) {
    var Public = this;

    var _myId = theId;
    
    Public.id getter = function() {
        return _myId;
    };
    
    var _myActive = false;
    
    Public.active getter = function() {
        return _myActive;
    };
    
    var _myFocused;
    
    Public.focused getter = function() {
        return _myFocused;
    };
    
    var _myIntensity;
    
    Public.intensity getter = function() {
        return _myIntensity;
    };
    
    var _myStageX;
    
    Public.stageX getter = function() {
        return _myStageX;
    };
    
    var _myStageY;
    
    Public.stageY getter = function() {
        return _myStageY;
    };
    
    Public.add = function(theProperties, theFocused) {
        _myActive = true;
    };

    Public.move = function(theProperties, theFocused) {
    };
    
    Public.remove = function() {
        _myActive = false;
    };
    
};

spark.CursorEvent = spark.Class("CursorEvent");

spark.CursorEvent.ADD    = "cursor-add";
spark.CursorEvent.REMOVE = "cursor-remove";
spark.CursorEvent.MOVE   = "cursor-move";
spark.CursorEvent.ENTER  = "cursor-enter";
spark.CursorEvent.LEAVE  = "cursor-leave";

spark.CursorEvent.Constructor = function(Protected, theType, theCursor) {
    var Public = this;
    
    this.Inherit(spark.Event, theType);
    
    var _myCursor = theProperties.cursor;
    
    Public.cursor getter = function() {
        return _myCursor;
    };
    
    Public.stageX getter = function() {
        return _myCursor.stageX;
    };
    
    Public.stageY getter = function() {
        return _myCursor.stageY;
    };
};
