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
//    $RCSfile: Playlist.js,v $
//     $Author: thomas $
//   $Revision: 1.18 $
//       $Date: 2005/04/26 12:05:00 $
//
//=============================================================================

const AUDIO_MEDIA   = "AUDIO";
const VIDEO_MEDIA   = "VIDEO";
const CAPTURE_MEDIA = "CAPTURE";
const IMAGE_MEDIA   = "IMAGE";
const MODEL_MEDIA   = "MODEL";
const NO_MEDIA      = "NO MEDIA";

const ASX_REGEXP       = /<entry.*?>(?:.*?<title.*?>\s*(.*?)\s*<\/title>)?.*?<ref.+?href\s*=\s*"(.+?)"/gi;
const ASF_REGEXP       = /Ref\d+=(.*?)\n/gi;
const RM_REGEXP        = /^\s*(.*?)(?:\s*--stop--.*)/i;
const PLS_REGEXP       = /file\d+=(.*?)\n(?:title\d+=(.*?))\n/gi;
const SMIL_REGEXP      = /<audio.+?src\s*=\s*"(.+?)"/gi;
const M3U_REGEXP       = /#EXTINF:\d+,(.*?)\n(.*?)\n/gi;
const EMBEDDED_REGEXP  = /<object.*?>.*?<embed.+?type\s*?=\s*?"(?:.+?)".+?src\s*=\s*"(.+?)"/gi;

const MAX_REDIRECTION_DEPTH = 5;

const MimeTypes = {
    HTML: 1,
    XML:  2,
    ASX:  3,
    WAX:  4,
    WVX:  5,
    WMX:  6,
    WM:   7,
    WMV:  8,
    WMA:  9,
    ASF:  10,
    //RAM:  11,
    RA:   12,
    //RM:   13,
    //RMM:  14,
    RV:   15,
    PLS:  16,
    M3U:  17,
    SMIL: 18,
    MP3: 19,
    TXT: 20
};

// Constructor
function Playlist() {
    this.Constructor(this);
}

Playlist.prototype.Constructor = function(self) {

    var _myPlaylist = [];
    var _myPlaylistIndex = null;
    var _myRecursiveFlag = false;
    var _myRedirectCount = 0;

    self.setRecursiveFlag = function(theFlag) {
        _myRecursiveFlag = theFlag;
    };

    self.addEntry = function(theUrl, theTitle, theMediaType) {

        //print("addEntry:" + theUrl);
        if (isDirectory(theUrl)) {
            //handle directories
	        var myDirList = getDirectoryEntries(theUrl);
	        if (myDirList == null) {
	            //print("### Playlist : Directory '" + theUrl + "' is null");
	            return;
	        }
            myDirList.sort();
            for (var i = 0; i < myDirList.length; ++i) {
                var myAbsoluteEntry = theUrl + '/' + myDirList[i];
                if (isDirectory(myAbsoluteEntry)) {
                    if (_myRecursiveFlag) {
                        self.addEntry(myAbsoluteEntry, theTitle, theMediaType);
                    }
                    //print("skip " + myAbsoluteEntry);
                } else {
                    self.addEntry(myAbsoluteEntry, theTitle, theMediaType);
                }
            }
        }
        else {
            if (theUrl.search(/^mms:\/\//i) != -1 ||
                theUrl.search(/^rtsp:\/\//i) != -1 )
            {
                //streaming protocol detected
                pushEntry(theUrl, theTitle, theMediaType);
            } else {
                //look for meta files to parse
                switch (getMimeType(theUrl)) {
                    case MimeTypes.HTML:
                    case MimeTypes.ASX:
                    case MimeTypes.WAX:
                    case MimeTypes.WVX:
                    case MimeTypes.WMX:
                    case MimeTypes.RA:
                    case MimeTypes.PLS:
                    case MimeTypes.M3U:
                    case MimeTypes.SMIL:
                        retrievePlaylist(theUrl);
                    break;
                    default:
                        //handle media files
                        if (theUrl.search(/^http:\/\/.*\?/i) != -1)
                        {
                            // handle special case of cgi-style parameters e.g.:
                            //   http://myServer.com/redirect.lsc?stream=livestream.wma
                            retrievePlaylist(theUrl);
                        } else {
                            pushEntry(theUrl, theTitle, theMediaType);
                        }
                }
            }
        }
    };

    self.getNumEntries = function() {
        return _myPlaylist.length;
    };

    self.getEntry = function(theIndex) {
        if (theIndex < 0 || theIndex >= _myPlaylist.length) {
            return null;
        }
        _myPlaylistIndex = theIndex;
        return _myPlaylist[theIndex];
    };

    self.getMediaHintFromURL = function(theUrl) {
        //print("####:"+ theUrl);
        if (theUrl.search(/^dshow:\/\//i) != -1 ||
            theUrl.search(/^video:\/\//i) != -1) {
            return CAPTURE_MEDIA;
        } else if (theUrl.search(/\.mp3$/i)  != -1 ||
                   theUrl.search(/\.mp2$/i)  != -1 ||
                   theUrl.search(/\.ogg$/i)  != -1 ||
                   theUrl.search(/\.flac$/i)  != -1 ||
                   theUrl.search(/\.mpc$/i)  != -1 ||
                   theUrl.search(/\.ape$/i)  != -1 ||
                   theUrl.search(/\.ac3$/i)  != -1 ||
                   theUrl.search(/\.aac$/i)  != -1 ||
                   theUrl.search(/\.wma$/i)  != -1 ||
                   theUrl.search(/\.aif$/i)  != -1 ||
                   theUrl.search(/\.aiff$/i)  != -1 ||
                   theUrl.search(/\.rm$/i)   != -1 ||
                   theUrl.search(/\.ra$/i)   != -1 ||
                   theUrl.search(/\.wav$/i)  != -1 ||
                   theUrl.search(/\.asf$/i)  != -1/*||
                   theUrl.search(/^mms:\/\//i) != -1*/ )
                   //XXX default fallback for mms://server/dir/
        {
            return AUDIO_MEDIA;
        } else if (theUrl.search(/\.m60$/i)  != -1 ||
                   theUrl.search(/\.vob$/i)  != -1 ||
                   theUrl.search(/\.mp4$/i)  != -1 ||
                   theUrl.search(/\.m4a$/i)  != -1 ||
                   theUrl.search(/\.mkv$/i)  != -1 ||
                   theUrl.search(/\.flv$/i)  != -1 ||
                   theUrl.search(/\.mpg$/i)  != -1 ||
                   theUrl.search(/\.mpeg$/i) != -1 ||
                   theUrl.search(/\.divx$/i) != -1 ||
                   theUrl.search(/\.avi$/i)  != -1 ||
                   theUrl.search(/\.wmv$/i)  != -1 ||
                   theUrl.search(/\.mov$/i)  != -1 ||
                   theUrl.search(/\.m4v$/i)  != -1 ||
                   theUrl.search(/\.mts$/i)  != -1 ||
                   theUrl.search(/\.webm$/i) != -1 ||
                   theUrl.search(/\.ogm$/i)  != -1 ||
                   theUrl.search(/\.m2v$/i)  != -1)
        {
            return VIDEO_MEDIA;
        } else if (theUrl.search(/\.x60$/i) != -1 ||
                   theUrl.search(/\.b60$/i) != -1 ||
                   theUrl.search(/\.stl$/i) != -1 ||
                   theUrl.search(/\.sta$/i) != -1)
        {
            return MODEL_MEDIA;
        } else {
            // one more chance for http audio
            if (theUrl.search(/:\/\//) != -1) {
                return AUDIO_MEDIA;
            } else if ( theUrl.search(/\.bmp$/i)  != -1 ||
                        theUrl.search(/\.gif$/i)  != -1 ||
                        theUrl.search(/\.iff$/i)  != -1 ||
                        theUrl.search(/\.jpg$/i)  != -1 ||
                        theUrl.search(/\.jpeg$/i) != -1 ||
                        theUrl.search(/\.pcx$/i)  != -1 ||
                        theUrl.search(/\.pgm$/i)  != -1 ||
                        theUrl.search(/\.pict$/i) != -1 ||
                        theUrl.search(/\.png$/i)  != -1 ||
                        theUrl.search(/\.rgb$/i)  != -1 ||
                        theUrl.search(/\.psd$/i)  != -1 ||
                        theUrl.search(/\.sgi$/i)  != -1 ||
                        theUrl.search(/\.tga$/i)  != -1 ||
                        theUrl.search(/\.tiff$/i) != -1 ||
                        theUrl.search(/\.i60$/i) != -1 ||
                        theUrl.search(/\.tif$/i) != -1 ||
                        theUrl.search(/\.wmf$/i)  != -1)
            {
                return IMAGE_MEDIA;
            } else {
                return NO_MEDIA;
            }
        }
    };

    self.getVideoDecoderHintFromURL = function(theUrl, theSeekableFlag) {
        if (theSeekableFlag == undefined) {
            theSeekableFlag = false;
        }
        var myDecoderHint = "";
        if (theUrl.search(/\.mpg$/i)  != -1 ||
            theUrl.search(/\.mp4$/i)  != -1 ||
            theUrl.search(/\.m4a$/i)  != -1 ||
            theUrl.search(/\.vob$/i)  != -1 ||
            theUrl.search(/\.mkv$/i)  != -1 ||
            theUrl.search(/\.flv$/i)  != -1 ||
            theUrl.search(/\.mpeg$/i) != -1 ||
            theUrl.search(/\.divx$/i) != -1 ||
            theUrl.search(/\.avi$/i)  != -1 ||
            theUrl.search(/\.wmv$/i)  != -1 ||
            theUrl.search(/\.mov$/i)  != -1 ||
            theUrl.search(/\.m4v$/i)  != -1 ||
            theUrl.search(/\.mts$/i)  != -1 ||
            theUrl.search(/\.m2v$/i)  != -1 ) {
            if (theSeekableFlag) {
                myDecoderHint = "FFMpegDecoder2";
            } else {
                myDecoderHint = "FFMpegDecoder2";
            }
        }
        if (theUrl.search(/\.mov$/i)  != -1) {
            if (operatingSystem() == "LINUX" || theSeekableFlag) {
                myDecoderHint = "FFMpegDecoder2";
            } else {
                myDecoderHint = "FFMpegDecoder2"; //"QuicktimeDecoder";
            }
        }
        if (theUrl.search(/\.wmv$/i)  != -1 ) {
            if (operatingSystem() == "LINUX") {
                myDecoderHint = "FFMpegDecoder2";
            } else {
                myDecoderHint = "FFMpegDecoder2"; //"WMVDecoder";
            }
        }
        return myDecoderHint;
    };

    function pushEntry(theUrl, theTitle, theMediaHint) {
        if (theTitle == undefined) {
            theTitle = theUrl;
        }
        if (theMediaHint == undefined) {
            theMediaHint = self.getMediaHintFromURL(theUrl);
        }
        var myEntryNodeString = '<Entry href="' + urlEncode(theUrl) + '" title="' + urlEncode(theTitle) + '" mediaType="' + theMediaHint + '"/>';
        var myEntryNode = new Node(myEntryNodeString);
        if (self.getMediaHintFromURL(theUrl) == NO_MEDIA) {
            print("Skipping '" + theUrl + "' as it is not known as supported media-file.");
        } else {
            _myPlaylist.push(myEntryNode.firstChild);
        }
    };

    //theString can be a URL, a file, a file extension or a MIME type string
    function getMimeType(theString) {
        var myString = theString.toLowerCase();
        var myExtRegExp = /\.(.{2,4})$/;
        var myMimeRegExp = /^(.+?\/[^;]+)/; //ambiguous: test URL first!

        if (myExtRegExp.test(myString)) {
            //retrieve the extension from URL/file
            myString = RegExp.$1;
        } else if (myMimeRegExp.test(myString)) {
            //retrieve the MIME type
            myString = RegExp.$1;
        } else if (myString.length > 4) {
            //neither extension nor MIME
            print("### Playlist : Could not retrieve extension or MIME type from '" + theString + "'.");
            return -1;
        }
        //===Windows Media===
        if (myString == "wax" ||
            myString == "audio/x-ms-wax") {
            return MimeTypes.WAX;
        }
        if (myString == "wvx" ||
            myString == "video/x-ms-wvx") {
            return MimeTypes.WVX;
        }
        if (myString == "wmx" ||
            myString == "video/x-ms-wmx") {
            return MimeTypes.WMX;
        }
        if (myString == "asx" ||
            myString == "asf" ||
            myString == "nsc" ||
            myString == "video/x-ms-asf") {
            return MimeTypes.ASX;
        }
        if (myString == "wm" ||
            myString == "video/x-ms-wm") {
            return MimeTypes.WM;
        }
        if (myString == "wmv" ||
            myString == "video/x-ms-wmv") {
            return MimeTypes.WMV;
        }
        if (myString == "wma" ||
            myString == "audio/x-ms-wma") {
            return MimeTypes.WMA;
        }
        //===Real Networks===
        if (myString == "ram" ||
            myString == "ra" ||
            myString == "rm" ||
            myString == "rmm" ||
            myString == "application/x-pn-realaudio" ||
            myString == "audio/x-pn-realaudio" ||
            myString == "audio/x-pn-realaudio-plugin" ||
            myString == "audio/vnd.rn-realaudio" ||
            myString == "audio/x-realaudio" ||
            myString == "application/vnd.rn-realmedia") {
            return MimeTypes.RA;
        }
        //===M3U===
        if (myString == "m3u" ||
            myString == "audio/x-mpegurl") {
            return MimeTypes.M3U;
        }
        //===PLS===
        if (myString == "pls" ||
            myString == "audio/x-pls" ||
            myString == "audio/x-scpls") {
            return MimeTypes.PLS;
        }
        if (myString == "audio/mpeg") {
            return MimeTypes.MP3;
        }
        //===SMIL===
        if (myString == "smil" ||
            myString == "application/smil") {
            return MimeTypes.SMIL;
        }

        if (myString == "htm" ||
            myString == "html" ||
            myString == "text/html") {
            return MimeTypes.HTML;
        }
        if (myString == "text/plain") {
            return MimeTypes.TXT;
        }
        //print("###WARNING: Unsupported MIME type ('"+myString+"').");
        return -1;
    };

    function retrievePlaylist(theUrl) {
        var myPlaylistNode = null;
        var myResponse = "";
        var myMimeType = "";
        var myRegExp = null;

        if (theUrl.search(/:\/\//) != -1) {
            //handle URLs
            self.myRequestManager = new RequestManager();
            self.myRequest = new Request(theUrl); //"Mozilla/5.0" or "Windows-Media-Player/10.00.00.3646"
            self.myRequest.onDone = function() {};
            self.myRequest.onError = function(theCode) {
                print("### Playlist: CURL error! Error code: "+theCode);
                return;
            };
            self.myRequest.setTimeoutParams(10, 120);
            self.myRequestManager.performRequest(self.myRequest);
            while(self.myRequestManager.activeCount) {
                self.myRequestManager.handleRequests();
            }
            print("Requested URL: "+self.myRequest.URL);

            if ((self.myRequest.responseCode == 301 ||
                 self.myRequest.responseCode == 302 ||
                 self.myRequest.responseCode == 303 ||
                 self.myRequest.responseCode == 307) &&
                 _myRedirectCount <= MAX_REDIRECTION_DEPTH)
            {
                //handle HTTP redirections (3xx)
                _myRedirectCount++;
                var myLocation = self.myRequest.getResponseHeader("Location");
                print("### Playlist: server response code: " + self.myRequest.responseCode + ".");
                print("              Following to location '"+myLocation+"'.");
                retrievePlaylist(myLocation);
                return;
            }
            _myRedirectCount = 0;
            if (self.myRequest.responseCode != 200) {
                print("### Playlist: Error from server: code=" + self.myRequest.responseCode);
                return;
            }
            myResponse = self.myRequest.responseString;
            myMimeType = getMimeType(self.myRequest.getResponseHeader("Content-Type"));
            if (myMimeType == MimeTypes.TXT) {
                myMimeType = getMimeType(theUrl);
            }
            //print("ICE '" + self.myRequest.getResponseHeader("x-audiocast-url")+"'");
        } else {
            //read meta file
            myResponse = readFileAsString(theUrl);
            myMimeType = getMimeType(theUrl);
        }

        switch (myMimeType) {
            case MimeTypes.ASX:
            case MimeTypes.WAX:
            case MimeTypes.WVX:
            case MimeTypes.WMX:
            case MimeTypes.ASF:
            //unfortunately, ASX and ASF both use the video/x-ms-asf MIME type,
            //thus we need to double check
            //1. ASF version
            myResponse = myResponse.replace(/\r/g, "");
            myRegExp = new RegExp(ASF_REGEXP);
            while (myRegExp.test(myResponse)) {
                print("  adding: "+RegExp.$1);
                //pushEntry to prevent recursion
                //[JB] Trust me on this one. I've seen horses puke, too!
                pushEntry(RegExp.$1, "", AUDIO_MEDIA);
            }
            //2. ASX version
            myResponse = myResponse.replace(/\n/g, "");
            myRegExp = new RegExp(ASX_REGEXP);
            while (myRegExp.test(myResponse)) {
                print("  adding: "+RegExp.$2+" ('"+RegExp.$1+"')");
                self.addEntry(RegExp.$2, RegExp.$1, AUDIO_MEDIA);
            }
            break;

            case MimeTypes.RA:
            myResponse = myResponse.replace(/\r|\n/g, "");
            myRegExp = new RegExp(RM_REGEXP);
            if (myRegExp.test(myResponse)) {
                print("  adding: "+RegExp.$1);
                self.addEntry(RegExp.$1, "", AUDIO_MEDIA);
            }
            break;

            case MimeTypes.PLS:
            myResponse = myResponse.replace(/\r/g, "");
            myRegExp = /NumberOfEntries=(\d+)/i;
            if(myRegExp.test(myResponse)) {
                print("  # of entries: "+RegExp.$1);
            }
            myRegExp = new RegExp(PLS_REGEXP);
            while (myRegExp.test(myResponse)) {
                print("  adding: "+RegExp.$1+" ('"+RegExp.$2+"')");
                self.addEntry(RegExp.$1, RegExp.$2, AUDIO_MEDIA);
            }
            break;

            case MimeTypes.M3U:
            if (myResponse.search(/^#EXTM3U/) != -1) {
                //Extended M3U
            print("M3U_EX");
                myRegExp = new RegExp(M3U_REGEXP);
                while (myRegExp.test(myResponse)) {
                print("  adding: "+RegExp.$2+" ('"+RegExp.$1+"')");
                    self.addEntry(RegExp.$2, RegExp.$1, AUDIO_MEDIA);
                }
            } else {
                //Simple M3U
                myResponse = myResponse.replace(/\r/g, "");
                var myEntries = myResponse.split("\n");
                for (var i=0; i < myEntries.length; ++i) {
                    print(i+": '"+myEntries[i]+"'");
                    self.addEntry(myEntries[i], "", AUDIO_MEDIA);
                }
            }
            break;

            case MimeTypes.SMIL:
            myResponse = myResponse.replace(/\r|\n/g, "");
            myRegExp = new RegExp(SMIL_REGEXP);
            while (myRegExp.test(myResponse)) {
                print("  adding: "+RegExp.$1);
                self.addEntry(RegExp.$1, "", AUDIO_MEDIA);
            }
            break;

            case MimeTypes.MP3:
                print("MP3 stream");
                self.addEntry(myResponse, "", AUDIO_MEDIA);
            break;

            case MimeTypes.HTML:
            //Try to find embedded player:
            myResponse = myResponse.replace(/\r|\n/g, "");
            myRegExp = new RegExp(EMBEDDED_REGEXP);
            while (myRegExp.test(myResponse)) {
                print("  entry: "+RegExp.$1);
                self.addEntry(RegExp.$1, "", AUDIO_MEDIA);
            }
            break;

            default:
            print("### Playlist : MIME type not supported.");
            return;
        }
        //return;
    };

};
