//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/*
  Provides some helper functions for basic JavaScript.
  
  All extension mechanisms are safe and do not collide with future versions of
  JavaScript.
  
  Helper functions provided:
    - typeOf(o)
        Alternative to built-in typeof keyword
        
    - checkObjectHasAttribute(o, v)
        Wrapper around hasOwnProperty - work in progress
        
    - isEmpty(o)
        returns true if o is an object containing no enumerable members
        
    - makeExtendable(o, v)
        Basic extension initialization for basic types. Provides "class" methods
        for creating
          method()
          getter()
          setter()
*/

//////////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////////

/*
See:
  http://javascript.crockford.com/prototypal.html
*/
/*
if (typeof Object.create !== 'function') {
    Object.create = function (o) {
        function F() {}
        F.prototype = o;
        return new F();
    };
}
*/

/**
 Checks the type of a given object.

 @param obj the object to check.
 @returns one of; "boolean", "number", "string", "object",
  "function", or "null".
 
 See:
   http://javascript.crockford.com/remedial.html
 
 Fixes this:
  Object    'object'
  Array     'object'
  Function  'function'
  String    'string'
  Number    'number'
  Boolean   'boolean'
  null      'object'
  undefined 'undefined'

  TODO: Add y60 specific native types like Vector or Node
*/
function typeOf(value) {
    var s = typeof value;
    if (s === 'object') {
        if (value) {
            if (typeof value.length === 'number' &&
                    !(value.propertyIsEnumerable('length')) &&
                    typeof value.splice === 'function') {
                s = 'array';
            }
        } else {
            s = 'null';
        }
    }
    return s;
}

/*
  Supplies a safer way to query an object about the presence of an attribute
  Main reason is, that getters would be executed by hasOwnProperty, which can lead to 
  unwanted behaviour, esp. in our y60 wrapped objects.
  
  Follows the Prototype-chain (can be switched off)
*/
function checkObjectHasAttribute(theObject, theAttributeName, theDescentFlag) {
    var myDescentFlag = theDescentFlag || true;
    var testSelf = theObject.__lookupGetter__(theAttributeName) ||
             theObject.__lookupSetter__(theAttributeName) ||
             theObject.hasOwnProperty(theAttributeName);
    var testPrototype = false;
    if (myDescentFlag && ("prototype" in theObject)) {
        testPrototype = checkObjectHasAttribute(theObject.prototype,
                                                theAttributeName);
    }
    return testSelf || testPrototype;
};

/*
  returns true if o is an object containing no enumerable members
*/
function isEmpty(o) {
    var i, v;
    if (typeOf(o) === 'object') {
        for (i in o) {
            v = o[i];
            if (v !== undefined && typeOf(v) !== 'function') {
                return false;
            }
        }
    }
    return true;
}

/*
  Adds "class" methods
  
    method
    getter
    setter

  to the given object.
  
  Example:
    
    // make Vector2f easily extendable
    makeExtendable(Vector2f);
    
    // extends the Basic object Type Vector2f with a getter on the property size
    Vector2f.getter('size', function() {
        return 2;
    });
    
    var myVector = new Vector2f([10,10])
    print(myVector.size);
    // 2
*/
function makeExtendable(theObjectType) {
    if (!checkObjectHasAttribute(theObjectType, "method")) {
        theObjectType.method = function(name, func) {
            if ( !checkObjectHasAttribute(theObjectType, name)) {
                theObjectType.prototype[name] = func;
            } else {
                Logger.warning("redefinition locked: <" + name + "> on " + theObjectType.name + " -> Already defined!");
            }
        };
    } else {
        Logger.warning("Object >" + theObjectType.name + "< is already extendable with methods");
    }
    
    if (!checkObjectHasAttribute(theObjectType, "getter")) {
        theObjectType.getter = function(name, func) {
            if ( !checkObjectHasAttribute(theObjectType, name)) {
                theObjectType.prototype.__defineGetter__(name, func);
            } else {
                Logger.warning("redefinition locked: <" + name + "> on " + theObjectType.name + " -> Already defined!");
            }
        };
    } else {
        Logger.warning("Object >" + theObjectType.name + "< is already extendable with getters");
    }

    if (!checkObjectHasAttribute(theObjectType, "setter")) {
        theObjectType.setter = function(name, func) {
            if ( !checkObjectHasAttribute(theObjectType, name)) {
                theObjectType.prototype.__defineSetter__(name, func);
            } else {
                Logger.warning("redefinition locked: <" + name + "> on " + theObjectType.name + " -> Already defined!");
            }
        };
    } else {
        Logger.warning("Object >" + theObjectType.name + "< is already extendable with setters");
    }
}

/*
 * TODO: Add a better way to compare objects.
 * */

// http://ejohn.org/blog/javascript-getters-and-setters/
// Helper method for extending one object with another
/*
function extend(a,b) {
    for ( var i in b ) {
        var g = b.__lookupGetter__(i), s = b.__lookupSetter__(i);
        
        if ( g || s ) {
            if ( g ) {
                a.__defineGetter__(i, g);
            }
            if ( s ) {
                a.__defineSetter__(i, s);
            }
         } else {
             a[i] = b[i];
         }
    }
    return a;
}
*/

//////////////////////////////////////////////////////////////////////
// Introspection
//////////////////////////////////////////////////////////////////////

/*
TODO: Make this more useful
function listProperties(obj) {
   var propList = "";
   for(var propName in obj) {
      if(typeOf(obj[propName]) !== "undefined") {
         propList += (propName + ", ");
      }
   }
   print(propList);
};
*/