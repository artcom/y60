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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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
        if (theKeywords) {
            myRegexList.push({ regex: new RegExp(getKeywords(theKeywords), 'gm'), css: theStyle });
        }
    }

    const myKeywords = 'abstract boolean break byte case catch char class const continue debugger ' +
                       'default delete do double else enum export extends false final finally float ' +
                       'for function goto if implements import in instanceof int interface long native ' +
                       'new null package private protected public return short static super switch ' +
                       'synchronized this throw throws transient true try typeof var void volatile while with';

    var myRegexList = [
        { regex: new RegExp('//.*$', 'gm'),                         css: 'comment' },           // one line comments
        { regex: new RegExp('/\\*[\\s\\S]*?\\*/', 'g'),             css: 'comment' },           // multiline comments
        { regex: new RegExp('"(?:[^"\n]|[\"])*?"', 'g'),            css: 'string' },            // double quoted strings
        { regex: new RegExp("'(?:[^'\n]|[\'])*?'", 'g'),            css: 'string' },            // single quoted strings
        { regex: new RegExp('([0-9]*\\.?[0-9]+)', 'g'),             css: 'number' },            // numbers
        { regex: new RegExp(getKeywords(myKeywords), 'gm'),         css: 'keyword' }            // keywords
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
        //theWord = theWord.replace(/&/g, '&amp;');
        theWord = theWord.replace(/ /g, '&nbsp;');
        //theWord = theWord.replace(/</g, '&lt;');
        theWord = theWord.replace(/\n/gm, '<br/>');

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
