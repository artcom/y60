spark.RadarTouchMouseMapper = spark.Class("RadarTouchMouseMapper");

spark.RadarTouchMouseMapper.Constructor = function(Protected, theApp) {
    var Public = this;
    var Base   = {};

    this.Inherit(spark.EventDispatcher);

    var _myApp = theApp;
    var _myCursorId = null;
    var _myLastPosition = null;

    Public.parent = null;

    Public.pickWidget = function(theStageX, theStageY) {
        return _myApp.pickWidget(theStageX, theStageY);
    }

    Public.addEventListener(spark.CursorEvent.ENTER, function(theEvent) {
        if (_myCursorId === null) {
            _myCursorId = theEvent.cursor.id;
        }
    } );

    Public.addEventListener(spark.CursorEvent.MOVE, function(theEvent) {
        var myCursor = theEvent.cursor;
        if (myCursor.id == _myCursorId) {
            if (_myLastPosition === null) {
                _myApp.onMouseButton(LEFT_BUTTON, BUTTON_DOWN, myCursor.stageX, myCursor.stageY);
            }
            _myApp.onMouseMotion(myCursor.stageX, myCursor.stageY);
            _myLastPosition = [myCursor.stageX, myCursor.stageY];
        }
    } );

    Public.addEventListener(spark.CursorEvent.LEAVE, function(theEvent) {
        var myCursor = theEvent.cursor;
        if (myCursor.id == _myCursorId) {
            if (_myLastPosition) {
                _myApp.onMouseButton(LEFT_BUTTON, BUTTON_UP, _myLastPosition[0], _myLastPosition[1]);
                _myLastPosition = null;
            }
            _myCursorId = null;
        }
    } );

}
