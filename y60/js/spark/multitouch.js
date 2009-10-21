
if("sparkProximatrix" in this) {
    Logger.info("SPARK proximatrix support enabled");

    use("ASSManager.js");
    
    spark.enableProximatrix = function(theStage) {
        spark.proximatrix = new ASSManager(theStage);
    };
}



if("sparkDSASensoric" in this) {
    Logger.info("DSA sensoric support enabled");

    spark.enableDSASensoric = function(theSettingsFile) {
        var myDSAConfig = new Node();
        Logger.info("using DSA sensoric settings " + theSettingsFile);
        myDSAConfig.parseFile(theSettingsFile);
        plug("DSADriver", myDSAConfig);
    };
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
    
    var _myFocused = null;
    
    Public.focused getter = function() {
        return _myFocused;
    };
    
    var _myIntensity = 0.0;
    
    Public.intensity getter = function() {
        return _myIntensity;
    };
    
    var _myStageX = 0.0;
    
    Public.stageX getter = function() {
        return _myStageX;
    };
    
    var _myStageY = 0.0;
    
    Public.stageY getter = function() {
        return _myStageY;
    };
    
    Public.update = function(theProperties, theFocused) {
        _myFocused = theFocused;
        updateProperties(theProperties);
    };
    
    Public.activate = function() {
        _myActive = true;
    };

    Public.deactivate = function() {
        _myActive = false;
    };
    
    function updateProperties(theProperties) {
        _myStageX = theProperties.position3D.x;
        _myStageY = theProperties.position3D.y;
        _myIntensity = theProperties.intensity;
    };
    
};

spark.CursorEvent = spark.Class("CursorEvent");

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
