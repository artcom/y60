//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: create_documentation.js,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/28 17:12:57 $
//
//
//=============================================================================

function SyntaxHighlighter() {
    this.Constructor(this);
}

SyntaxHighlighter.prototype.Constructor = function(Public) {

    Public.highlight = function(theCode) {
        var myResult = "";
        var myMatches = [];
    	for (var i = 0; i < myRegexList.length; ++i) {
    		myMatches = myMatches.concat(getMatches(theCode, myRegexList[i].regex, myRegexList[i].css));
    	}

        myMatches = myMatches.sort(sortCallback);

        for (var i = 0; i < myMatches.length; i++) {
		    if (isInside(myMatches, myMatches[i])) {
			    myMatches[i] = null;
			}
		}

    	if (myMatches.length) {
    	    var myPos = 0;
    	    for (var i = 0; i < myMatches.length; ++i) {
        		var myMatch = myMatches[i];
        		if (myMatch) {
        		    myResult += addWord(theCode.substring(myPos, myMatch.index), null);
        		    myResult += addWord(myMatch.value, myMatch.css);
        		    myPos = myMatch.index + myMatch.value.length;
        		}
    	    }
    	    myResult += addWord(theCode.substring(myPos), null);
    	} else {
    	    myResult = addWord(theCode, null);
    	}

    	return myResult;
    }

    Public.addKeywords = function(theKeywords, theStyle) {
        myRegexList.push({ regex: new RegExp(getKeywords(theKeywords), 'gm'), css: theStyle });
    }

	const myKeywords = 'abstract boolean break byte case catch char class const continue debugger ' +
					   'default delete do double else enum export extends false final finally float ' +
					   'for function goto if implements import in instanceof int interface long native ' +
					   'new null package private protected public return short static super switch ' +
					   'synchronized this throw throws transient true try typeof var void volatile while with';

	var myRegexList = [
		{ regex: new RegExp('//.*$', 'gm'),							css: 'comment' },			// one line comments
		{ regex: new RegExp('/\\*[\\s\\S]*?\\*/', 'g'),				css: 'comment' },			// multiline comments
		{ regex: new RegExp('"(?:[^"\n]|[\"])*?"', 'g'),			css: 'string' },			// double quoted strings
		{ regex: new RegExp("'(?:[^'\n]|[\'])*?'", 'g'),			css: 'string' },			// single quoted strings
	    { regex: new RegExp('([0-9]*\\.?[0-9]+)', 'g'),		        css: 'number' },			// numbers
		{ regex: new RegExp(getKeywords(myKeywords), 'gm'),		    css: 'keyword' }			// keywords
    ];

    function getKeywords(theString) {
    	return '\\b' + theString.replace(/ /g, '\\b|\\b') + '\\b';
    }

    // gets a list of all matches for a given regular expression
    function getMatches(theCode, theRegex, theCss) {
    	var myMatch = null;
        var myResult = [];

        while ((myMatch = theRegex.exec(theCode)) != null) {
    		myResult.push({value: myMatch[0], index: myMatch.index, css: theCss, length: myMatch[0].length });
    	}
    	return myResult;
    }

    function addWord(theWord, theCSS) {
    	var myResult = null;

        // Create html entities
    	theWord = theWord.replace(/&/g, '&amp;');
    	theWord = theWord.replace(/ /g, '&nbsp;');
    	theWord = theWord.replace(/</g, '&lt;');
    	theWord = theWord.replace(/\n/gm, '&nbsp;<br>');

        // Add css class
    	if (theCSS) {
    		myResult = "<span class='" + theCSS + "'>" + theWord + "</span>";
    	} else {
    	    myResult = theWord;
    	}

    	return myResult;
    }

    function sortCallback(m1, m2) {
    	// sort matches by index first
    	if(m1.index < m2.index) {
    		return -1;
    	} else if(m1.index > m2.index) {
    		return 1;
    	} else {
    		// if index is the same, sort by length
    		if(m1.length < m2.length) {
    			return -1;
    		} else if(m1.length > m2.length) {
    			return 1;
    		}
    	}
    	return 0;
    }

    function isInside(theMatches, theMatch) { 
    	if (theMatch == null || theMatch.length == 0) {
    		return;
    	}
    	
    	for (var i = 0; i < theMatches.length; i++) {
    		var myMatch = theMatches[i];
    		
    		if (myMatch == null) {
    			continue;
    		}
    		
    		if ((theMatch.index > myMatch.index) &&
    		    (theMatch.index <= myMatch.index + myMatch.length))
            {
    			return true;
    		}
    	}
    	
    	return false;
    }

}