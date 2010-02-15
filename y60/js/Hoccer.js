var Hoccer = {};

Hoccer.station = function() {
    var that = {};

    that.init = function(theLongitude, theLatitude, theAccuracy) {
        that.longitude = theLongitude;
        that.latitude = theLatitude;
        that.accuracy = theAccuracy;
    }

    that.distribute = function(theFile) {
        print("distribute");
    }

    return that;
}();

