/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("Y60JSSL.js");

function SunPositioner() {
    this.Constructor(this);
}
SunPositioner.prototype.Constructor = function(self) {

    const DEFAULT_MONTH    = "Jun";
    const DEFAULT_DAY      = 1;
    const DEFAULT_HOUR     = 10;
    const DEFAULT_MINUTE   = 0;
    const DEFAULT_SECONDS  = 0;
    const DEFAULT_TIMEZONE = 0;

    self._myLongitude         = 13; //defaults to berlin
    self._myLatitude          = 52;

    var _useCurrentLocalTime = false;
    var _myDeclination       = 0.0;
    var _myAltitude          = 0.0;
    var _myAzimuth           = 0.0;
    var _myLocalTimeStr      = "00:00";
    var _myEquationOfTime    = "00:00";
    var _mySolarTime         = "00:00";
    var _mySunriseTime       = "00:00";
    var _mySunsetTime        = "00:00";

    var _myMinuteOffset      = 0;

    // public methods
    self.rise = function(theMinuteOffset) {
        if (_myLocalTimeStr == _mySunsetTime) {
            print("### Warning, it is dusk, cannot rise the sun anymore!");
            return;
        }
        _myMinuteOffset += theMinuteOffset;
        self.calculateCurrentPosition();
    }

    self.set = function(theMinuteOffset) {
        if (_myLocalTimeStr == _mySunriseTime) {
            print("### Warning, it is dawn, cannot set the sun anymore!");
            return;
        }
        _myMinuteOffset -= theMinuteOffset;
        self.calculateCurrentPosition();
    }

    self.reset= function(theMinuteOffset) {
        _myMinuteOffset = 0;
        self.calculateCurrentPosition();
    }

    // Sets the sun position between dusk and dawn
    // thePercentage = 0.0 -> sunrise
    // thePercentage = 1.0 -> sunset
    self.setRelative = function(thePercentage) {
        var mySunrise = _mySunriseTime.split(":");
        var mySunset  = _mySunsetTime.split(":");
        var mySunriseHour = Number(mySunrise[0]) + mySunrise[1] / 60;
        var mySunsetHour  = Number(mySunset[0]) + mySunset[1] / 60;
        var myTime = mySunriseHour + (mySunsetHour - mySunriseHour) * thePercentage;

        var myHour    = Math.floor(myTime);
        var myMinutes = (myTime - myHour) * 60;

        calculatePosition(myHour, myMinutes, 0, DEFAULT_DAY,
                          DEFAULT_MONTH, DEFAULT_TIMEZONE);
    }

    self.calculateCurrentPosition = function(theLatitude, theLongitude) {
        var myMinuteOffset = _myMinuteOffset;
        var myHourOffset = 0;
        if (Math.abs(_myMinuteOffset) > 60 ) {
            if (_myMinuteOffset > 0 ) {
                myHourOffset   = Math.floor(_myMinuteOffset / 60);
            } else {
                myHourOffset   = -Math.floor(-_myMinuteOffset / 60);
            }
            myMinuteOffset = _myMinuteOffset % 60;
        }

        var myHour        = DEFAULT_HOUR;
        var myMinutes     = DEFAULT_MINUTE;
        var mySeconds     = DEFAULT_SECONDS;
        var myTimeZone    = DEFAULT_TIMEZONE;
        var myDay         = DEFAULT_DAY;
        var myMonth       = DEFAULT_MONTH;

        if (_useCurrentLocalTime) {
            var myLocalTime   = new Date();
            myHour        = myLocalTime.getHours()   + myHourOffset;
            myHour = myHour % 24;
            myMinutes     = myLocalTime.getMinutes() + myMinuteOffset;
            mySeconds     = myLocalTime.getSeconds();

            myTimeZone    = Math.floor(-myLocalTime.getTimezoneOffset() / 60.0);

            myDay         = myLocalTime.getDate();
            var myMonthTable  = ["Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
            myMonth       = myMonthTable[myLocalTime.getMonth()];
        } else {
            myMinutes += myMinuteOffset;
            myHour    += myHourOffset;
        }

        calculatePosition(myHour, myMinutes, mySeconds, myDay,
                          myMonth, myTimeZone,
                          theLatitude, theLongitude);
    }

    function calculatePosition(theHours, theMinutes, theSeconds,
                               theDay, theMonth, theTimeZone,
                               theLatitude, theLongitude)
    {
        // theTimeZone = -12 - 13 // 0 = GMT, positiv towards east, negativ toward west
        if (theLatitude != null && theLongitude != null) {
            self._myLongitude = theLongitude;
            self._myLatitude  = theLatitude;
        }
        var myMonthMap = new Array();
        myMonthMap["Jan"] = 0;
        myMonthMap["Feb"] = 31;
        myMonthMap["Mar"] = 59;
        myMonthMap["Apr"] = 90;
        myMonthMap["May"] = 120;
        myMonthMap["Jun"] = 151;
        myMonthMap["Jul"] = 181;
        myMonthMap["Aug"] = 212;
        myMonthMap["Sep"] = 243;
        myMonthMap["Oct"] = 273;
        myMonthMap["Nov"] = 304;
        myMonthMap["Dec"] = 334;

        if (theDay < 0 || theDay > 31 || myMonthMap[theMonth] == null ) {
            print("### Error: Wrong dateformat. See Function definitions for right format!")
            return;
        }

        // Get location data.
        var myLatitude = radFromDeg(self._myLatitude);
        var myTimeZone = radFromDeg(theTimeZone * 15);
        var myLongitude = radFromDeg(self._myLongitude);

        // Get julian date.
        var myJulianDate = myMonthMap[theMonth] + theDay;

        ////////////////////////////////////////////////////////////
        // CALCULATE SOLAR VALUES
        ////////////////////////////////////////////////////////////
        var myLocalTime = theHours + (theMinutes / 60) + (theSeconds / 3600);


        // Calculate solar declination as per Carruthers et al.
        var myT = 2 * Math.PI * ((myJulianDate - 1) / 365.0);

        var myDeclination = (0.322003
                - 22.984 * Math.cos(myT)
                - 0.357898 * Math.cos(2*myT)
                - 0.14398 * Math.cos(3*myT)
                + 3.94638 * Math.sin(myT)
                + 0.019334 * Math.sin(2*myT)
                + 0.05928 * Math.sin(3*myT)
                );

        // Convert degrees to radians.
        if (myDeclination > 89.9) myDeclination = 89.9;
        if (myDeclination < -89.9) myDeclination = -89.9;

        // Convert to radians.
        myDeclination = radFromDeg(myDeclination);

        // Calculate the equation of time as per Carruthers et al.
        myT = radFromDeg(279.134 + 0.985647 * myJulianDate);

        var myEquation = (5.0323
                - 100.976 * Math.sin(myT)
                + 595.275 * Math.sin(2*myT)
                + 3.6858 * Math.sin(3*myT)
                - 12.47 * Math.sin(4*myT)
                - 430.847 * Math.cos(myT)
                + 12.5024 * Math.cos(2*myT)
                + 18.25 * Math.cos(3*myT)
                );

        // Convert seconds to hours.
        myEquation = myEquation / 3600.00;

        // Calculate difference (in minutes) from reference longitude.
        var myDifference = (degFromRad(myLongitude - myTimeZone) * 4) / 60.0;

        // Convert solar noon to local noon.
        var myLocalNoon = 12.0 - myEquation - myDifference;

        // Calculate angle normal to meridian plane.
        if (myLatitude > (0.99 * (Math.PI/2.0))) myLatitude = (0.99 * (Math.PI/2.0));
        if (myLatitude < -(0.99 * (Math.PI/2.0))) myLatitude = -(0.99 * (Math.PI/2.0));

        var myTest = -Math.tan(myLatitude) * Math.tan(myDeclination);

        if (myTest < -1) myT = Math.acos(-1.0) / (15 * (Math.PI / 180.0));
        else if (myTest > 1) myT = acos(1.0) / (15 * (Math.PI / 180.0));
        else myT = Math.acos(-Math.tan(myLatitude) * Math.tan(myDeclination)) /
                  (15 * (Math.PI / 180.0));

        // Sunrise and sunset.
        var mySunrise = myLocalNoon - myT;
        var mySunset  = myLocalNoon + myT;

        // Check validity of local time.
        if (myLocalTime > mySunset) {
            myLocalTime = mySunset;
        }
        if (myLocalTime < mySunrise) {
            myLocalTime = mySunrise;
        }
        if (myLocalTime > 24.0) {
            myLocalTime = 24.0;
        }
        if (myLocalTime < 0.0) {
            myLocalTime = 0.0;
        }

        // Caculate solar time.
        var mySolarTime = myLocalTime + myEquation + myDifference;

        // Calculate hour angle.
        var myHourAngle = (15 * (mySolarTime - 12)) * (Math.PI/180.0);

        // Calculate current altitude.
        myT = (Math.sin(myDeclination) * Math.sin(myLatitude)) +
              (Math.cos(myDeclination) * Math.cos(myLatitude) * Math.cos(myHourAngle));
        var myAltitude = Math.asin(myT);

        // Calculate current azimuth.
        myT = (Math.cos(myLatitude) * Math.sin(myDeclination)) -
              (Math.cos(myDeclination) * Math.sin(myLatitude) * Math.cos(myHourAngle));
        var myAzimuth = Math.acos(myT / Math.cos(myAltitude));

        // Update local time.

        myT = Math.floor(myLocalTime);
        var myM = Math.floor((myLocalTime - myT) * 60.0);

        var myMinute;
        var myHour;
        if (myM < 10) {
            myMinute = "0" + myM;
        } else {
            myMinute = myM;
        }
        if (myT < 10) {
            myHour = "0" + myT;
        } else {
            myHour = myT;
        }
        _myLocalTimeStr = myHour + ":" + myMinute

        // Output declination.
        _myDeclination = Math.round(degFromRad(myDeclination) * 1000) / 1000.0;

        // Output equation of time.
        _myEquationOfTime = Math.round((myEquation * 60.0) * 1000) / 1000.0;

        // Output altitude value.
        _myAltitude = Math.round(degFromRad(myAltitude) * 100) / 100.0;

        // Output aziumth value.
        _myAzimuth = Math.round(degFromRad(myAzimuth) * 100) / 100.0;

        // Output solar time.
        myT = Math.floor(mySolarTime);
        myM = Math.floor((mySolarTime - myT) * 60.0);
        if (myM < 10) {
            myMinute = "0" + myM;
        } else {
            myMinute = myM;
        }
        if (myT < 10) {
            myHour = "0" + myT;
        } else {
            myHour = myT;
        }
        _mySolarTime = myHour + ":" + myMinute;
        // NOON
        if (mySolarTime > 12 ) {
            _myAzimuth = 360.0 - _myAzimuth;
        }

        // Output sunrise time.
        myT = Math.floor(mySunrise);
        myM = Math.floor((mySunrise - myT) * 60.0);
        if (myM < 10) {
            myMinute = "0" + myM;
        } else {
            myMinute = myM;
        }
        if (myT < 10) {
            myHour = "0" + myT;
        } else {
            myHour = myT;
        }
        _mySunriseTime = myHour + ":" + myMinute;

        // Output sunset time.
        myT = Math.floor(mySunset);
        myM = Math.floor((mySunset - myT) * 60.0);
        if (myM < 10) {
            myMinute = "0" + myM;
        } else {
            myMinute = myM;
        }
        if (myT < 10) {
            myHour = "0" + myT;
        } else {
            myHour = myT;
        }
        _mySunsetTime = myHour + ":" + myMinute;
    }

    self.getRotation = function() {
        return new Vector3f(-radFromDeg(_myAltitude), radFromDeg(_myAzimuth), 0.0);
    }

    self.getDirection = function() {
        return new Vector3f(Math.sin(radFromDeg(_myAzimuth)),
                            Math.sin(radFromDeg(_myAltitude)),
                            -Math.cos(radFromDeg(_myAzimuth)));
    }

    self.dump = function() {
        print("LocalTime " + _myLocalTimeStr);
        print("Declination: " + _myDeclination);
        print("Equation of time: " + _myEquationOfTime);
        print("Altitude: " + _myAltitude);
        print("Azimuth: " + (_myAzimuth > 180 ? _myAzimuth - 360 : _myAzimuth));
        print("SolarTime " + _mySolarTime);
        print("Sunrise time " + _mySunriseTime);
        print("Sunset time " + _mySunsetTime);
        print("-------------------------------------");
    }

    self.getCurrentTime = function() {
        return _myLocalTimeStr;
    }
}

//var theLocalSunPosition = new SunPositioner();
//theLocalSunPosition.calculateCurrentPosition(52.0, 13.0, 1);
//theLocalSunPosition.dump()
//var myLightDir = theLocalSunPosition.getDirection();

//var theSunPosition = new SunPositioner();
//theSunPosition.calculatePosition(17,5,1,18,"Sep", +2, 52.0, 13);
//theSunPosition.dump();

