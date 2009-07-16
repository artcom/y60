//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

// requires core

/*
  Extensions to built-in **Array**
    - indexOf()
    - lastIndexOf()
    - map()
    - forEach()
    - filter()
    - every()
    - some()
    - reduce()
    - reduceRight()
*/

//////////////////////////////////////////////////////////////////////
// Array Helpers
//////////////////////////////////////////////////////////////////////

//Check if two arrays' contents are the same
//returns true if they are, otherwise false
/*
var compareArrays = function(a,b) {
  //Check if the arrays are undefined/null
  if(!a || !b) {
    return false;
  }

  //first compare their lengths
  if(a.length === b.length) {
    //go thru all the vars
    for(var i = 0; i < a.length;i++) {
      //if the var is an array, we need to make a recursive check
      //otherwise we'll just compare the values
      if(typeof a[i] == 'object') {
        if(!compareArrays(a[i],b[i])){
          return false;
        }
      }
      else if(a[i] !== b[i]) {
        return false;
      }
    }
    return true;
  }
  else {
    return false;
  }
};
*/

//////////////////////////////////////////////////////////////////////
// Array methods and properties
//////////////////////////////////////////////////////////////////////

makeExtendable(Array);

/*
See:
  https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Global_Objects/Array/indexOf

Example:
  var array = [2, 5, 9];
  var index = array.indexOf(2);
  // index is 0
  index = array.indexOf(7);
  // index is -1
*/

Array.method('indexOf', function(elt /*, from*/) {
  var len  = this.length;
  var from = Number(arguments[1]) || 0;
  from = (from < 0)
       ? Math.ceil(from)
       : Math.floor(from);
  if (from < 0) {
    from += len;
  }
  for (; from < len; from++) {
    if (from in this && this[from] === elt) {
      return from;
    }
  }
  return -1;
});

/*
See: 
  https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Global_Objects/Array/lastIndexOf

Example:
  var array = [2, 5, 9, 2];
  var index = array.lastIndexOf(2);
  // index is 3
*/
Array.method('lastIndexOf', function(elt /*, from*/) {
  var len  = this.length;
  var from = Number(arguments[1]);
  if (isNaN(from)) {
    from = len - 1;
  } else {
    from = (from < 0)
         ? Math.ceil(from)
         : Math.floor(from);
    if (from < 0) {
      from += len;
    } else if (from >= len) {
      from = len - 1;
    }
  }
  
  for (; from > -1; from--) {
    if (from in this && this[from] === elt) {
      return from;
    }
  }
  return -1;
});

/**
See:
  https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Objects/Array/map

Example:

  var numbers = [1, 4, 9];
  var roots = numbers.map(Math.sqrt);
*/
Array.method('map', function(fun /*, thisp*/) {
  var len = this.length;
  if (typeof fun !== "function") {
    throw new TypeError();
  }
  
  var res   = new Array(len);
  var thisp = arguments[1];
  for (var i = 0; i < len; i++) {
    if (i in this) {
      res[i] = fun.call(thisp, this[i], i, this);
    }
  }
  return res;
});

/*
See:
  https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Global_Objects/Array/forEach
  
Example:
  function printElt(element, index, array) {
      print("[" + index + "] is " + element); // assumes print is already defined
  }
  [2, 5, 9].forEach(printElt);
*/
Array.method('forEach', function(fun /*, thisp*/) {
  var len = this.length;
  if (typeof fun !== "function") {
    throw new TypeError();
  }

  var thisp = arguments[1];
  for (var i = 0; i < len; i++) {
    if (i in this) {
      fun.call(thisp, this[i], i, this);
    }
  }
});

/*
See:
  https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Global_Objects/Array/filter
  
Example:
  function isBigEnough(element, index, array) {
    return (element >= 10);
  }
  var filtered = [12, 5, 8, 130, 44].filter(isBigEnough);
*/
Array.method('filter', function(fun /*, thisp*/) {
  var len = this.length;
  if (typeof fun !== "function") {
    throw new TypeError();
  }

  var res   = new Array();
  var thisp = arguments[1];
  for (var i = 0; i < len; i++) {
    if (i in this)
    {
      var val = this[i]; // in case fun mutates this
      if (fun.call(thisp, val, i, this)) {
        res.push(val);
      }
    }
  }
  return res;
});

/*
See:
  https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Global_Objects/Array/every
Example:
  function isBigEnough(element, index, array) {
    return (element >= 10);
  }
  var passed = [12, 5, 8, 130, 44].every(isBigEnough);
  // passed is false
  passed = [12, 54, 18, 130, 44].every(isBigEnough);
  // passed is true
*/
Array.method('every', function(fun /*, thisp*/) {
  var len = this.length;
  if (typeof fun !== "function") {
    throw new TypeError();
  }
  
  var thisp = arguments[1];
  for (var i = 0; i < len; i++) {
    if (i in this && !fun.call(thisp, this[i], i, this)) {
      return false;
    }
  }
  return true;
});
  
/*
See:
  https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Global_Objects/Array/some
Example:
  function isBigEnough(element, index, array) {
    return (element >= 10);
  }
  var passed = [2, 5, 8, 1, 4].some(isBigEnough);
  // passed is false
  passed = [12, 5, 8, 1, 4].some(isBigEnough);
  // passed is true
*/
Array.method('some', function(fun /*, thisp*/) {
  var len = this.length;
  if (typeof fun !== "function") {
    throw new TypeError();
  }
  
  var thisp = arguments[1];
  for (var i = 0; i < len; i++) {
    if (i in this && fun.call(thisp, this[i], i, this)) {
      return true;
    }
  }
  return false;
});

/*
See:
  https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Objects/Array/reduce
Example:
  var total = [0, 1, 2, 3].reduce(function(a, b){ return a + b; });
  // total == 6
*/
Array.method('reduce', function(fun /*, initial*/) {
  var len = this.length;
  if (typeof fun !== "function") {
    throw new TypeError();
  }
  
  // no value to return if no initial value and an empty array
  if (len === 0 && arguments.length === 1) {
    throw new TypeError();
  }
  
  var i = 0;
  if (arguments.length >= 2) {
    var rv = arguments[1];
  } else {
    do {
      if (i in this) {
        rv = this[i++];
        break;
      }
      
      // if array contains no values, no initial value to return
      if (++i >= len) {
        throw new TypeError();
      }
    } while (true);
  }
  
  for (; i < len; i++) {
    if (i in this) {
      rv = fun.call(null, rv, this[i], i, this);
    }
  }
  return rv;
});

/*
See:
  https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Objects/Array/reduceRight
Example:
  var flattened = [[0, 1], [2, 3], [4, 5]].reduceRight(function(a, b) {
    return a.concat(b);
  }, []);
  // flattened is [4, 5, 2, 3, 0, 1]
*/
Array.method('reduceRight', function(fun /*, initial*/) {
  var len = this.length;
  if (typeof fun != "function") {
    throw new TypeError();
  }

  // no value to return if no initial value, empty array
  if (len === 0 && arguments.length == 1) {
    throw new TypeError();
  }

  var i = len - 1;
  if (arguments.length >= 2) {
    var rv = arguments[1];
  } else {
    do {
      if (i in this) {
        rv = this[i--];
        break;
      }

      // if array contains no values, no initial value to return
      if (--i < 0) {
        throw new TypeError(); 
      }
    }
    while (true);
  }

  for (; i >= 0; i--) {
    if (i in this) {
      rv = fun.call(null, rv, this[i], i, this);
    }
  }
  return rv;
});