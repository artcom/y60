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

/**
 * @fileoverview This file is to be used for testing the JSDoc parser
 * {@link http://sourceforge.net/projects/jsdoc JSDoc} parser
 *
 * @author Christian Hardenberg
 * @version 0.1
 */

/**
 * Example for constructor description
 * @class A shape base class, you cannot do without
 * @param {string} theName Specifies the name of the shape
 * @param {id} theId Specifies the id of the shape
 */
function Shape(theName, theId) {
    /**
     * @private
     */
    this.init = function(theName, theId) {
        _myName = theName;
        this.id = theId;
    }

    /**
     * Gets the shape name
     * @return {string} The shape name
     */
    this.getName = function() {
        return _myName;
    }

    /**
     * Sets the shape name
     * @param {string} theName Specifies the shape name
     * @deprecated You should no use this member function
     */
    this.setName = function(theName) {
        _myName = theName;
    }

    /**
     * Example for a class base property
     * @type unsigned
     */
    this.id = null;

    // ++++++++++++++++++++++++ Private members ++++++++++++++++++++++++

    var _myName;

    if (arguments.length > 0) {
        this.init(theName, theId);
    }
}

/**
 * Example for constructor description
 * @class A very usefull sqare class
 * @param {float} theSize Specifies the size of the shape
 * @param {id} theId Specifies the id of the shape
 * @throws SizeToBigException This exception is almost never thrown
 */
function Square(theSize, theId) {
    /**
     * @private
     */
    this.init = function(theSize, theId) {
        Square.prototype.init("Square", theId);
        _mySize = theSize;
    }

    /**
     * Gets the square size
     * @return {unsigend} The square size
     */
    this.getSize = function () {
        return _mySize;
    }

    /**
     * Sets the square size
     * @param {float} theSize Specifies the square size
     */
    this.setSize = function(theSize) {
        _mySize = theSize;
    }

    /**
     * Gets the square area
     * @return {float} The square area
     */
    this.getArea = function() {
        return _mySize * _mySize;
    }

    /**
     * Example for a class property
     * @type unsigned
     */
    this.speed = 0;

    // ++++++++++++++++++++++++ Private members ++++++++++++++++++++++++

    var _mySize;

    if (arguments.length > 0) {
        this.init(theSize, theId);
    }
}

Square.prototype = new Shape();


/**
 * Tests if the argument is true, exits if the test fails
 * @param {string} theTest A line of code to be tested for trueness
 */
function ENSURE(theTest) {
    print(">>> " + theTest);
    var myResult = eval(theTest);
    if (!myResult) {
        print("Test failed!");
        exit(1);
    }
}

// Testing inheritance
var myShape = new Shape("Shape", "id0");
ENSURE('myShape.getName() == "Shape"');
myShape.setName("myShape");
ENSURE('myShape.getName() == "myShape"');
ENSURE('myShape.id == "id0"');

var mySquare = new Square(2, "id1");
ENSURE('mySquare.getSize() == 2');
ENSURE('mySquare.getArea() == 4');
mySquare.setSize(1);
ENSURE('mySquare.getSize() == 1');
ENSURE('mySquare.getArea() == 1');
ENSURE('mySquare.speed == 0');

// Base class access
ENSURE('mySquare.getName() == "Square"');
mySquare.setName("mySquare");
ENSURE('mySquare.getName() == "mySquare"');
ENSURE('mySquare.id == "id1"');

print("All tests succeeded!");
