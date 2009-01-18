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

#include <asl/base/Arguments.h>
#include <asl/base/MappedBlock.h>

#include <y60/image/ImageLoader.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planybmp.h>
#include <paintlib/plpngenc.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)


using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) // x

////////////////////////////////////////////////////////////////////////
//
// For file format specification see som/Movie.h
//
////////////////////////////////////////////////////////////////////////


void printVersion();

asl::Arguments::AllowedOption myOptions[] = {
                                             {"--outfile",      "%s"},
                                             {"--front",        "%s"},
                                             {"--back",         "%s"},
                                             {"--left",         "%s"},
                                             {"--right",        "%s"},
                                             {"--top",          "%s"},
                                             {"--bottom",       "%s"},
                                             {"--size",         "%s"},
                                             {"--version",      ""  },
                                             {"--help",         ""  },
                                             {"", ""}
                                            };
asl::Arguments myArguments(myOptions);

template <class TX>
bool
getParameter(const string & theParamName, const asl::Arguments & theArguments, TX & theResult) {
    if (myArguments.haveOption(string("--")+theParamName)) {
        theResult = asl::as<TX>(myArguments.getOptionArgument(string("--")+theParamName));
        return true;
    }
    return false;
}

void
printHelp() {
    // TODO: Please help to maintain this function
    myArguments.printUsage();
    cout << "Command line options:" << endl
         << "  --outfile       FILE   write movie to FILE" << endl
         << "  --front         FILE   cubemaps front map." << endl
         << "  --back          FILE   cubemaps back map." << endl
         << "  --left          FILE   cubemaps left map." << endl
         << "  --right         FILE   cubemaps rught map." << endl
         << "  --top           FILE   cubemaps top map." << endl
         << "  --bottom        FILE   cubemaps bottom map." << endl
         << "  --size          PIXEL  set image size to PIXELxPIXEL." << endl
         << "  --version              print version information and exit" << endl
         << "  --help                 print this help text and exit" << endl
         << endl;
}

void copyImageData(asl::Ptr<ImageLoader> theSource, PLBmp & theDestination, unsigned int theOffset) {
        PLBYTE ** mySrcLines = theSource->GetLineArray();
        PLBYTE ** myDstLines = theDestination.GetLineArray();

        unsigned myFaceHeight = theSource->GetHeight();
        long myLineSize = theDestination.GetBytesPerLine();
        for (unsigned y = 0; y < myFaceHeight; ++y) {
            memcpy(myDstLines[y] + theOffset, mySrcLines[y], myLineSize);
        }
}

void createCubeMap(const std::string & theFace, PLBmp& theTargetBitmap, bool doScaling, unsigned int theSize) {
    string myFileName;
    string myArgument("--" + theFace);
    if (myArguments.haveOption(myArgument)) {
        myFileName = myArguments.getOptionArgument(myArgument);
    } else {
        throw asl::Exception(string("ERROR: No file specified for face ") + theFace );
    }
    asl::Ptr<ImageLoader> myFace(new ImageLoader(asl::Ptr<asl::ReadableBlockHandle>(new asl::AnyReadableBlockHandle(asl::Ptr<ReadableBlock>(new asl::ConstMappedBlock(myFileName)), myFileName)), myFileName));

    if (doScaling && theSize!=static_cast<unsigned int>(myFace->GetWidth())){
        cout << "Scaling image to size " << theSize << " x " << theSize << endl;
        myFace->ApplyFilter(PLFilterResizeBilinear(theSize,theSize));
    }

    unsigned myTargetWidth  = myFace->GetWidth();
    unsigned myTargetHeight = myFace->GetHeight() * 6;
    theTargetBitmap.Create( myTargetWidth, myTargetHeight, myFace->GetPixelFormat());

    cout << "Added face: " << theFace << " from file: " << myFileName << endl;
    copyImageData(myFace,  theTargetBitmap, 0);
}

void addCubeMapImage(const std::string & theFace, PLBmp & theTargetBitmap, bool doScaling, unsigned int theSize) {

    static int myCounter = 0;
    myCounter++;
    string myFileName;
    string myArgument("--" + theFace);
    if (myArguments.haveOption(myArgument)) {
        myFileName = myArguments.getOptionArgument(myArgument);
    } else {
        throw asl::Exception(string("ERROR: No file specified for face ") + theFace );
    }
    asl::Ptr<ImageLoader> myFace(new ImageLoader(myFileName));
    if (doScaling && theSize!=static_cast<unsigned int>(myFace->GetWidth())){
        cout << "Scaling image to size " << theSize << " x " << theSize << endl;
        myFace->ApplyFilter(PLFilterResizeBilinear(theSize,theSize));
    }

    cout << "Added face: " << theFace << " from file: " << myFileName << endl;
    copyImageData(myFace,  theTargetBitmap, (myFace->getRaster()->pixels().size()) * myCounter);

}



int main( int argc, char *argv[])  {
    cout << getFilenamePart(argv[0]) << " copyright (c) 2001-2004 ART+COM AG" << endl;
    string myArgDesc = string("See '") + string(getFilenamePart(argv[0])) +
                              " --help' for more information.";
    myArguments.setShortDescription(myArgDesc.c_str());

    if (!myArguments.parse( argc, argv )) {
        return 1;
    }
    if (myArguments.haveOption("--help")) {
        printHelp();
        return 0;
    }
    if (myArguments.haveOption("--version")) {
        printVersion();
        return 0;
    }

    try {
        string myTargetFileName;
        if (myArguments.haveOption("--outfile")) {
            myTargetFileName = myArguments.getOptionArgument("--outfile");
        } else {
            cerr << "### ERROR: No output file specified" << endl;
            return -1;
        }


        unsigned int mySize;
        bool doScaling = false;
        if (getParameter("size",myArguments, mySize)) {
            doScaling = true;
            cout << "SCALE image to " << mySize << endl;
        }

        PLAnyBmp theTargetBitmap;
        createCubeMap( "front", theTargetBitmap, doScaling, mySize);
        addCubeMapImage( "right", theTargetBitmap, doScaling, mySize);
        addCubeMapImage( "back", theTargetBitmap, doScaling, mySize);
        addCubeMapImage( "left", theTargetBitmap, doScaling, mySize);
        addCubeMapImage( "top", theTargetBitmap, doScaling, mySize);
        addCubeMapImage( "bottom", theTargetBitmap, doScaling, mySize);

        PLPNGEncoder myEncoder;
        myEncoder.MakeFileFromBmp(myTargetFileName.c_str(), &theTargetBitmap);

        cerr << "----------------------------------------------------" << endl;
        cerr << "Generate cubemap done: " << myTargetFileName << endl;
        cerr << "----------------------------------------------------" << endl;
    }
    catch (asl::Exception & e) {
        cerr << "### ERROR: " << e << endl;
        return -1;
    }
    catch (...) {
        cerr << "### ERROR: Unknown exception occured." << endl;
        return -1;
    }
    return 0;
}


void
printVersion() {
    cout << "CVS $Revision: 1.8 $ $Date: 2005/03/24 23:36:01 $." << endl;
    cout << "Build at " << __DATE__ << " " << __TIME__ << "." << endl;
}
