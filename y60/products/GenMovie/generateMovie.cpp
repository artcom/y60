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

#include <y60/image/ImageLoader.h>
#include <y60/video/MovieEncoding.h>
#include <y60/video/M60Header.h>

#include <asl/base/Arguments.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/MappedBlock.h>
#include <asl/math/Matrix4.h>
#include <asl/base/RunLengthEncoder.h>

#include <asl/dom/Nodes.h>
#include <set>
#include <algorithm>
#include <iostream>
#include <string>
#include <ctype.h>

using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) // x

////////////////////////////////////////////////////////////////////////
//
// For file format specification see som/M60Decoder.cpp
//
////////////////////////////////////////////////////////////////////////

asl::Ptr<ImageLoader> ourPreviousFrame;
float            ourFramesPerSecond = 25.0f;

string
getTargetFilename(const string & theTargetDir, const string & theSourceFileName) {

    return theTargetDir + theDirectorySeparator +
            theSourceFileName.substr(0,theSourceFileName.find_last_of("."))+".dxt";
}

void writeHeader(asl::WriteableStream & theTargetBlock, MovieEncoding theCompression,
    asl::Ptr<ImageLoader> theFirstImage, unsigned int theTotalFrameCount)
{
    asl::Matrix4f myImageMatrix = theFirstImage->getImageMatrix();
    asl::Vector4f mySize(float(theFirstImage->GetWidth()), float(theFirstImage->GetHeight()), 0, 0);
    mySize = asl::product(mySize, myImageMatrix);

    M60Header myHeader;
    myHeader.framecount   = theTotalFrameCount;
    myHeader.fps          = ourFramesPerSecond;
    myHeader.width        = unsigned(mySize[0]);
    myHeader.height       = unsigned(mySize[1]);
    myHeader.framewidth   = theFirstImage->GetWidth();
    myHeader.frameheight  = theFirstImage->GetHeight();
    myHeader.compression  = unsigned(theCompression);
    myHeader.pixelformat  = unsigned(theFirstImage->getEncoding());

    theTargetBlock.appendData(myHeader);
}

void
appendFrame(const string & theSourceFile, asl::WriteableStream & theTargetBlock,
            MovieEncoding theMovieEncoding, const std::string & theResizeMode, unsigned int theTotalFrameCount)
{
    if (!fileExists(theSourceFile) || isDirectory(theSourceFile)) {
        cerr << "Skipping image : " << theSourceFile << endl;
        return;
    }
    cerr << "Loading image : " << theSourceFile << endl;
    asl::Ptr<ImageLoader> myFrame(new ImageLoader(
                    asl::Ptr<asl::ReadableBlockHandle>(new asl::AnyReadableBlockHandle(asl::Ptr<ReadableBlock>(new asl::ConstMappedBlock(theSourceFile)), theSourceFile)), theSourceFile));

    if (theResizeMode != "none" ) {
        myFrame->ensurePowerOfTwo(theResizeMode);
    }

    if (!ourPreviousFrame) {
        cerr << "Writing header: encoding: " << theMovieEncoding << ", framewidth: " << myFrame->GetWidth() << ", frameheight: " <<
            myFrame->GetHeight() << endl;
        writeHeader(theTargetBlock, theMovieEncoding, myFrame, theTotalFrameCount);
    } else {
        // Check if current frame matches first frame
        if (myFrame->GetWidth() != ourPreviousFrame->GetWidth()) {
            throw asl::Exception(string("Frame ") + theSourceFile + " has width: " +
                as_string(myFrame->GetWidth()) + " previous frame had: " +
                as_string(ourPreviousFrame->GetWidth()));
        }
        if (myFrame->GetHeight() != ourPreviousFrame->GetHeight()) {
            throw asl::Exception(string("Frame ") + theSourceFile + " has height: " +
                as_string(myFrame->GetHeight()) + " previous frame had: " +
                as_string(ourPreviousFrame->GetHeight()));
        }
        if (myFrame->getEncoding() != ourPreviousFrame->getEncoding()) {
            throw asl::Exception(string("Frame ") + theSourceFile + " has encoding: " +
                as_string(myFrame->getEncoding()) + " previous frame had: " +
                as_string(ourPreviousFrame->getEncoding()));
        }
        if (myFrame->getHeaderSize() != ourPreviousFrame->getHeaderSize()) {
            throw asl::Exception(string("Frame ") + theSourceFile + " has header size: " +
                as_string(myFrame->getHeaderSize()) + " previous frame had: " +
                as_string(ourPreviousFrame->getHeaderSize()));
        }
        if (myFrame->getRaster()->pixels().size() != ourPreviousFrame->getRaster()->pixels().size()) {
            throw asl::Exception(string("Frame ") + theSourceFile + " has different size than previous frame");
        }
    }

    dom::ValuePtr myNewFrame;
    if (ourPreviousFrame && (theMovieEncoding == MOVIE_DIFF || theMovieEncoding == MOVIE_DRLE)) {
        myNewFrame = dom::ValuePtr(myFrame->getData()->clone(0));
        raster_cast(myNewFrame)->sub(*ourPreviousFrame->getData());
    } else {
        myNewFrame = myFrame->getData();
    }
    if (theMovieEncoding == MOVIE_RLE || theMovieEncoding == MOVIE_DRLE) {
        DB(
            cerr << "  Appending compressed frame with size: " << myNewFrame->accessReadableBlock().size() << endl;
            cerr << "  at fileposition: " << theTargetBlock.size() << endl;
        )
        theTargetBlock.appendUnsigned32(myNewFrame->accessReadableBlock().size());
        theTargetBlock.append(myNewFrame->accessReadableBlock());
    } else {
        DB(
            cerr << "  Appending uncompressed frame with size: " << raster_cast(myNewFrame)->pixels().size() << endl;
            cerr << "  at fileposition: " << theTargetBlock.size() << endl;
        )
        theTargetBlock.appendUnsigned32(raster_cast(myNewFrame)->pixels().size());
        theTargetBlock.append(raster_cast(myNewFrame)->pixels());
    }

    ourPreviousFrame = myFrame;
}

int main( int argc, char *argv[])  {

    asl::Arguments myArguments;

    asl::Arguments::AllowedOptionWithDocumentation myOptions[] = {
         {"--outfile",      "FILE", "write movie to FILE"},
         {"--source-dir",   "DIR", "search for input files in DIR. files on the command line will be ignored."},
         {"--encoding",     "MODE", "set movie encoding mode to MODE (default: DRLE)."},
         {"--resize",       "MODE", "set frame resize mode in case of non power of two dimensions to MODE (default: scale) [none, pad]."},
         {"--fps",          "FLOAT", "Set the playback speed in frames per second (default is 25)"},
         {"", ""}
    };
    myArguments.addAllowedOptionsWithDocumentation(myOptions);

    myArguments.setShortDescription("Valid encoding modes:\n RLE, DIFF, DRLE\n");

    if (!myArguments.parse( argc, argv )) {
        return 1;
    }
    if (myArguments.haveOption("--fps")) {
        ourFramesPerSecond = asl::as<float>(myArguments.getOptionArgument("--fps"));
    }

    dom::NodePtr myXmlConfigDocument(new dom::Document());
    dom::NodePtr myXmlConfig;
    set<string> mySourceFiles;

    try {
        if ( myArguments.haveOption("--source-dir")) {
            string mySourceDirectory = myArguments.getOptionArgument("--source-dir");
            DIR * myDir = opendir(mySourceDirectory.c_str());
            dirent * myDirEntry = 0;
            while ( 0 != (myDirEntry = readdir(myDir))) {
                string myFilename(myDirEntry->d_name);
                string myPathname = mySourceDirectory + theDirectorySeparator + myFilename;
                if (isDirectory(myPathname)) {
                    cerr << "Skipping directory " << myFilename << endl;
                    continue;
                }
                if (myFilename[0] != '.') {
                    mySourceFiles.insert(myPathname);
                }
            }
            if (mySourceFiles.size() == 0) {
                string myErrorMsg = string("Can't find source images in directory '")
                    + mySourceDirectory + "':";
                perror(myErrorMsg.c_str());
                return 1;
            } else {
                cout << "Found " << mySourceFiles.size() << " files in directory '"
                    << mySourceDirectory << "'." << endl;
            }
        } else if (myArguments.getCount() > 0) {
            cout << "Got " << myArguments.getCount() << " files on the command line." << endl;
            for (int i=0; i < myArguments.getCount(); ++i) {
                mySourceFiles.insert(string(myArguments.getArgument(i)));
            }
        } else {
            cerr << "### ERROR: No input files found." << endl;
            myArguments.printUsage();
            return 1;
        }

        string myTargetFileName;
        if (myArguments.haveOption("--outfile")) {
            myTargetFileName = myArguments.getOptionArgument("--outfile");
        } else {
            cerr << "### ERROR: No output file specified" << endl;
            myArguments.printUsage();
            return -1;
        }

        MovieEncoding myMovieEncoding = MOVIE_DRLE;
        std::string theEncodingFormat = "DRLE";
        if (myArguments.haveOption("--encoding")) {
            theEncodingFormat = myArguments.getOptionArgument("--encoding");
        } else {
            cout << "Using default encoding type '" << theEncodingFormat << "'" << endl;
        }
        try {
            myMovieEncoding = MovieEncoding(getEnumFromString(theEncodingFormat.c_str(), MovieEncodingString));
        } catch (ParseException & ex) {
            cerr << "### ERROR: Format '" << theEncodingFormat
                << "' not supported!" << endl << endl << ex << endl;
            myArguments.printUsage();
            return 1;
        }

        std::string myResizeMode = IMAGE_RESIZE_SCALE;
        if (myArguments.haveOption("--resize")) {
            myResizeMode = myArguments.getOptionArgument("--resize");

           string::iterator myIter = myResizeMode.begin();
           string::iterator myEnd  = myResizeMode.end();
           for (;myIter!=myEnd;++myIter) {
               *myIter = static_cast<char>(tolower(*myIter));
           }
        } else {
            cout << "Using default resize mode '" << myResizeMode << "'" << endl;
        }

        if (myResizeMode != IMAGE_RESIZE_SCALE &&
            myResizeMode != "none" &&
            myResizeMode != IMAGE_RESIZE_PAD ) {
                cerr << "### ERROR: Resizemode '" << myResizeMode
                    << "' not supported!" << endl << endl << endl;
                myArguments.printUsage();
                return 1;
        }

        if (mySourceFiles.size() == 0) {
            cerr << "### WARNING: No images to process!" << endl;
            return 0;
        }

        cout << "Info: Starting with image " << *(mySourceFiles.begin()) << endl;
        cout << "Info: Processing " << mySourceFiles.size() << " image(s)" << endl;
        cout << "Info: Last image is image " << *(--(mySourceFiles.end())) << endl;

        cout << "creating target file " << myTargetFileName << endl;
#ifdef USE_MAPPED_BLOCK_WRITE
        asl::MappedBlock myTargetBlock(myTargetFileName.c_str());
#else
        asl::WriteableFile myTargetBlock(myTargetFileName.c_str());
#endif
        unsigned long myFrameCount = 0;

        vector<string> myBrokenFrames;

        for (set<string>::iterator myIt = mySourceFiles.begin(); myIt != mySourceFiles.end(); ++myIt) {
            int myRetryCount = 0;
            bool myOKFlag = false;

            do {
                myOKFlag = false;
                try {
                    appendFrame(*myIt, myTargetBlock, myMovieEncoding, myResizeMode, mySourceFiles.size());
                    myOKFlag = true;
                } catch (const asl::Exception & e) {
                    cerr << "### ERROR: Exception occured while processing " << *myIt << endl << e << endl;
                } catch (const exception & e) {
                    cerr << "### ERROR: Exception occured while processing " << *myIt << endl << e.what() << endl;
                } catch (...) {
                    cerr << "### ERROR: Unknown exception occured while processing " << *myIt << endl;
                }
                ++myRetryCount;
            } while (myRetryCount < 3 && !myOKFlag);
            if (!myOKFlag) {
                cerr << "### ERROR: Skipping file " << *myIt << " after " << myRetryCount
                    << " retrys" << endl;
                myBrokenFrames.push_back(*myIt);
            } else {
                myFrameCount++;
            }
        }

        // Write framecount into header
        //M60Header * myHeader = reinterpret_cast<M60Header *>(myTargetBlock.begin());
        //myHeader->framecount = myFrameCount;

        cerr << "----------------------------------------------------" << endl;
        cerr << "Generate movie done. Framecount: " << myFrameCount << endl;
        if (myBrokenFrames.size()) {
            cerr << "Broken frames:" << endl;
            for (unsigned i = 0; i < myBrokenFrames.size(); ++i) {
                cerr << "    " << myBrokenFrames[i] << endl;
            }
        }
        cerr << "----------------------------------------------------" << endl;

        // Force destruction of ourPreviousFrame before main exits, otherwise there is
        // no logger left to log the imageloader destruction.
        ourPreviousFrame = asl::Ptr<ImageLoader>();
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
