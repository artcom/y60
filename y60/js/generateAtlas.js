/*jslint plusplus:false*/
/*globals print, Logger, TextureAtlas, exit*/

var myAtlasName      = null;
var myAtlasTextures  = {};
var myBorderFlag     = true;
var myPowerOfTwoFlag = false;

var HELP_ARGUMENT = "help";

function usage() {
    print("Texture Atlas Generator Help:");
    print("arguments:\n\thelp: this help");
    print("\tatlasName=\"<theAtlasDefinitionFile>\"");
    print("\ttextures=[<textureName1>,<texturePath1>],[<textureName2>,<texturePath2>]...");
    print("\tborder=true|false default: true (optional)");
    print("\tpowerof=true|false default: false (optional)");
}

var ARGUMENTS_PARTS = {
    'atlasName' : function (theRawValue) {
        myAtlasName = theRawValue;
    },
    "textures" : function (theRawValue) {
        theRawValue = theRawValue.substr(1, theRawValue.length - 2);
        var myTextures = theRawValue.split("],[");
        for (var i = 0; i < myTextures.length; i++) {
            var myRawTexture = myTextures[i].split(",");
            if (myRawTexture[0] in myAtlasTextures) {
                Logger.warning("Texture '" + myRawTexture[0] + "' was already specified with value '" +
                               myAtlasTextures[myRawTexture[0]] + "'! Ignoring new value '" + myRawTexture[1] + "'");
            } else {
                myAtlasTextures[myRawTexture[0]] = myRawTexture[1];
            }
        }
    },
    'help' : function () {
        usage();
        exit(0);
    },
    'border' : function (theRawBorderFlag) {
        if (theRawBorderFlag === 'false') {
            myBorderFlag = false;
        } else if (theRawBorderFlag === 'true') {
            myBorderFlag = true;
        } else {
            Logger.warning("valid value for border are 'true' or 'false'. Ignoring supplied value '" + theRawBorderFlag + "'. Using default: '" + myBorderFlag.toString() + "'");
        }
    },
    'poweroftwo' : function (theRawBorderFlag) {
        if (theRawBorderFlag === 'false') {
            myPowerOfTwoFlag = false;
        } else if (theRawBorderFlag === 'true') {
            myPowerOfTwoFlag = true;
        } else {
            Logger.warning("valid value for poweroftwo are 'true' or 'false'. Ignoring supplied value '" + theRawBorderFlag + "'. Using default: '" + myPowerOfTwoFlag.toString() + "'");
        }
    }
};

function parseArguments(theArguments) {
    var rawArguments = {};
    for (var i = 0; i < theArguments.length; i++) {
        var myArgumentParts = theArguments[i].split("=");
        var rawKey   = myArgumentParts[0];
        var rawValue = (myArgumentParts.length === 2) ? myArgumentParts[1] : null;
        if (rawKey in ARGUMENTS_PARTS) {
            rawArguments[rawKey] = rawValue;
        } else {
            Logger.warning("Unknown argument:'" + rawKey + "' with value:'" + rawValue + "'");
        }
    }
    
    if (HELP_ARGUMENT in rawArguments) {
        return {'help' : null};
    }
    return rawArguments;
}

function evaluateArguments(theParsedArguments) {
    for (var key in theParsedArguments) {
        ARGUMENTS_PARTS[key](theParsedArguments[key]);
    }
}

function main() {
    print("building Atlas: '" + myAtlasName + "':");
    print("\tborder: " + myBorderFlag.toString());
    print("\tpoweroftwo: " + myPowerOfTwoFlag.toString());
    // print texture info?
    if (myAtlasName) {
        var myNewTextureAtlas = new TextureAtlas(myAtlasTextures, myBorderFlag, myPowerOfTwoFlag);
        myNewTextureAtlas.saveToFile(myAtlasName);
    } else {
        Logger.error("could not build Atlas '" + myAtlasName + "'");
        usage();
    }
}

evaluateArguments(parseArguments(arguments));
main();