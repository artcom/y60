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
  Extensions to built-in **String**
    - trim()
    - ltrim()
    - rtrim()
    - checksum
*/

//////////////////////////////////////////////////////////////////////
// String methods and proprties
//////////////////////////////////////////////////////////////////////

makeExtendable(String);

/*
Will trim the following characters
  - " " (ASCII 32 (0x20)), an ordinary space.
  - "\t" (ASCII 9 (0x09)), a tab.
  - "\n" (ASCII 10 (0x0A)), a new line (line feed).
  - "\r" (ASCII 13 (0x0D)), a carriage return.
  - "\0" (ASCII 0 (0x00)), the NUL-byte.
  - "\x0B" (ASCII 11 (0x0B)), a vertical tab.

See:
  http://www.webtoolkit.info/javascript-trim.html
Additional Resources:
  http://blog.stevenlevithan.com/archives/faster-trim-javascript
Example:
  var test = "\n \x0B\t \r\r blah   ccc \n \x0B\t \r   vvv     kjkhj\n \t \n \x0B\t \r";
  print("!" + test.trim() + "!");
  //!blah   ccc 
  //
  //   vvv     kjkhj!
  
  print("!" + "XXXyyyXXX".trim(["X"]) + "!");
  //!yyy!
*/

String.method('trim', function(chars) {
    return this.ltrim(chars).rtrim(chars);
});

String.method('ltrim', function(chars) {
    chars = chars || "\\s";
    return this.replace(new RegExp("^[" + chars + "]+", "g"), "");
});

String.method('rtrim', function(chars) {
    chars = chars || "\\s";
    return this.replace(new RegExp("[" + chars + "]+$", "g"), "");
});

String.getter('checksum', function() {
    return checksumFromString(this);
});