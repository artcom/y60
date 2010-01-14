
spark.enableProximatrix = function(theStage) {
    use("ASSManager.js");
    spark.proximatrix = new ASSManager(theStage);
};

spark.enableTuio = function() {
    plug("TUIOClient");
    TUIOClient.listenToUDP();
};

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

    var _myFocused = null;

    Public.focused getter = function() {
        return _myFocused;
    };

    var _myStagePosition = new Point2f();

    Public.stagePosition getter = function() {
        return _myStagePosition.clone();
    };

    Public.stageX getter = function() {
        return _myStagePosition.x;
    };

    Public.stageY getter = function() {
        return _myStagePosition.y;
    };

    Public.update = function(theFocused, theStagePosition) {
        _myFocused = theFocused;
        _myStagePosition = theStagePosition.clone();
    };

    Public.activate = function() {
        _myActive = true;
    };

    Public.deactivate = function() {
        _myActive = false;
    };

};

spark.CursorEvent = spark.Class("CursorEvent");

spark.CursorEvent.APPEAR = "cursor-appear";
spark.CursorEvent.VANISH = "cursor-vanish";
spark.CursorEvent.MOVE   = "cursor-move";
spark.CursorEvent.ENTER  = "cursor-enter";
spark.CursorEvent.LEAVE  = "cursor-leave";

spark.CursorEvent.Constructor = function(Protected, theType, theCursor) {
    var Public = this;

    this.Inherit(spark.Event, theType);

    var _myCursor = theCursor;

    Public.cursor getter = function() {
        return _myCursor;
    };

    Public.stageX getter = function() {
        return _myCursor.stageX;
    };

    Public.stageY getter = function() {
        return _myCursor.stageY;
    };

    Public.intensity getter = function() {
        return _myCursor.intensity;
    };
};
