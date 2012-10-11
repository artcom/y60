/*jslint nomen: false*/
/*global print, use, UnitTest, UnitTestSuite, __FILE__, exit, Namespace,
         ENSURE, ENSURE_EQUAL*/

use("UnitTest.js");
use("spark/spark.js");

var global = this;

// Namespace

function NamespaceUnitTest() {
    this.Constructor(this, "NamespaceUnitTest");
}

NamespaceUnitTest.prototype.Constructor = function (obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);
    
    function testBasics() {
        var myNamespace = Namespace("test");
        ENSURE_EQUAL("test", myNamespace.name, "namespace name");
        ENSURE((myNamespace.AbstractClass !== undefined), "namespace contains AbstractClass");
        ENSURE((myNamespace.Class !== undefined), "namespace contains Class");
    }
    
    obj.run = function () {
        testBasics();
    };
};

// AbstractClass

function AbstractClassUnitTest() {
    this.Constructor(this, "AbstractClassUnitTest");
}

AbstractClassUnitTest.prototype.Constructor = function (obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);
    
    function testBasics() {
        obj.myNamespace = Namespace("test");
        obj.myNamespace.A = obj.myNamespace.AbstractClass("A");
        ENSURE_EQUAL("A", obj.myNamespace.A._className_, "Abstract class' name is set");
    }
    
    obj.run = function () {
        testBasics();
    };
};

// Class

function ClassUnitTest() {
    this.Constructor(this, "ClassUnitTest");
}

ClassUnitTest.prototype.Constructor = function (obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);
    
    function testBasics() {
        obj.myNamespace = Namespace("test");
        
        //////////////////
        // Simple Class //
        //////////////////
        
        obj.myNamespace.myClass = obj.myNamespace.Class("myClass");
        obj.myNamespace.myClass.Constructor = function (Protected) {
            this.foo = "bar";
        };
        var myInstance = new obj.myNamespace.myClass();
        ENSURE_EQUAL("myClass", myInstance._className_, "classname is properly set");
        ENSURE_EQUAL(obj.myNamespace.myClass, myInstance._classes_.myClass, "_classes_ contains self via Namespace");
        ENSURE_EQUAL(obj.myNamespace.myClass, myInstance._class_, "we are _class_");
        ENSURE_EQUAL(0, myInstance._properties_.length, "No Properties");
        ENSURE_EQUAL(0, myInstance._signals_.length, "No Signals");
        /*var protected_props_count = 0;
        for (var prop in myInstance._protected_) {
            if (myInstance.hasOwnProperty(prop)) {
                protected_props_count += 1;
            }
        }
        ENSURE_EQUAL(0, protected_props_count, "_protected_ is empty");*/
        ENSURE_EQUAL("bar", myInstance.foo, "has custom member");
        
        ///////////////
        // Inherited //
        ///////////////
        
        obj.testContext = {};
        obj.myNamespace.myInheritedClass = obj.myNamespace.Class("myInheritedClass");
        obj.myNamespace.myInheritedClass.Constructor = function (Protected) {
            var Public = this;
            
            this.Inherit(obj.myNamespace.myClass);
            
            this.additional = "+++";
            
            // Property
            this.Property("alpha", Number, 1.0, function (theValue) {
                Public.additional = theValue * theValue;
            });
            
            // Signal
            this.Signal("volcano");
            this.volcano.call(function (x) {
                this.bla = x;
            }, obj.testContext);
            this.volcano.call(function (x) {
                this.additional = x;
            }, Public);
            this.volcano.call(function (x) {
                this.foo = x;
            }, this);
            this.volcano.call(function (x) {
                this.bar = x;
            });
            
            // Getter/Setter manually
            this.Getter("rhabarber", function () {
                return 42;
            });
            this.Setter("rhabarber", function (theValue) {
                this.foo = theValue;
            });
        };
        
        var myInheritedInstance = new obj.myNamespace.myInheritedClass();
        ENSURE_EQUAL("bar", myInheritedInstance.foo, "has custom member from base class");
        ENSURE_EQUAL("+++", myInheritedInstance.additional, "has custom member");
        ENSURE_EQUAL(obj.myNamespace.myClass, myInheritedInstance._classes_.myClass, "_classes_ contains superclass via Namespace");
        ENSURE_EQUAL(obj.myNamespace.myInheritedClass, myInheritedInstance._classes_.myInheritedClass, "_classes_ contains self via Namespace");
        
        // Property related...
        ENSURE_EQUAL(1, myInheritedInstance._properties_.length, "1 Property");
        ENSURE_EQUAL(1.0, myInheritedInstance.alpha, "default value of property");
        myInheritedInstance.alpha = 2.0;
        ENSURE_EQUAL(2.0, myInheritedInstance.alpha, "setter/getter of property");
        ENSURE_EQUAL(4.0, myInheritedInstance.additional, "property setter handler");
        ENSURE_EQUAL("2", myInheritedInstance._properties_[0].getAsString(), "get property as String");
        myInheritedInstance._properties_[0].setFromString("5");
        ENSURE_EQUAL("alpha", myInheritedInstance._properties_[0].name, "Name of Property in registry");
        ENSURE_EQUAL(5.0, myInheritedInstance.alpha, "read after setFromString");
        ENSURE_EQUAL(25.0, myInheritedInstance.additional, "handle called after setFromString");
        
        // Signal related...
        myInheritedInstance.volcano.signal("foo");
        ENSURE_EQUAL("foo", myInheritedInstance.additional, "handler with context of self via 'Public' called");
        ENSURE_EQUAL("foo", myInheritedInstance.foo, "handler with context of self via 'this' called");
        ENSURE_EQUAL("foo", obj.testContext.bla, "handler with some other context");
        ENSURE_EQUAL("foo", global.bar, "undefined context bleeds into globals");
        
        // Getter/Setter
        ENSURE_EQUAL(42, myInheritedInstance.rhabarber, "manual getter");
        myInheritedInstance.rhabarber = "kuchenhgabel";
        ENSURE_EQUAL("kuchenhgabel", myInheritedInstance.foo, "manual setter");
    }
    
    obj.run = function () {
        testBasics();
    };
};

// testSuite

try {
    // if (operatingSystem() === 'OSX') {}
    var myScriptPathSegments = __FILE__().split("/");
    var mySuiteName = myScriptPathSegments[myScriptPathSegments.length - 1];
    var mySuite = new UnitTestSuite(mySuiteName);
    
    mySuite.addTest(new NamespaceUnitTest());
    mySuite.addTest(new AbstractClassUnitTest());
    mySuite.addTest(new ClassUnitTest());
    mySuite.run();
    
    print(">> Finished test suite '" + mySuiteName + "', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}