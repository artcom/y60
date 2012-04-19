//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.

#include <stdexcept>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "ACMTextureCompressor.h"

#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#include <paintlib/Filter/plfiltercrop.h>
#include <paintlib/Filter/plfilterflip.h>
#include <paintlib/Filter/plfilterrotate.h>
#include <paintlib/Filter/plfilterresizebilinear.h>

#include <asl/base/Arguments.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/Time.h>

#include <iostream>


using namespace std;  // automatically added!
using namespace asl;



bool gAppend = false;
int  gStartFrame = 0;
int  gStartImage = 0;



int main( int argc, char *argv[] ) 
{
    asl::Arguments myArguments;
    asl::Arguments::AllowedOptionWithDocumentation myOptions[] = {
         {"--outfile",      "FILE", "write movie to FILE"},
         {"--img-dir",      "DIR", "search for input files in DIR. files on the command line will be ignored."},
         {"--compression",  "MODE", "set OpenGL texture compression mode to MODE (default: S3TC_DXT5)."},
         {"--xsize",        "PIXEL", "set movie-width to PIXEL."},
         {"--ysize",        "PIXEL", "set movie-height to PIXEL."},
         {"--xaspect",      "PIXEL", "set replay x-aspect ratio to PIXEL."},
         {"--yaspect",      "PIXEL", "set replay y-aspect ratio to PIXEL."},
         {"--fps",          "FPS", "set frames per second to FPS (default: 30.0Hz)."},
         {"--rotate90",     "", "rotate source image by 90°."},
         {"--pre-flip",     "", "flip source image upside down before applying rotation."},
         {"--post-flip",    "", "flip source image upside down after applying rotation."},
         {"--pre-xsize",    "PIXEL", "resize source image width to PIXEL before doing anything else."},
         {"--pre-ysize",    "PIXEL", "resize source image height to PIXEL before doing anything else."},
         {"--crop",         "", "crop the movie to xsize * ysize instead of scaling it."},
         {"--cropleft",     "PIXEL", "set left border to PIXEL."},
         {"--croptop",      "PIXEL", "set top border to PIXEL."},
         {"--cropright",    "PIXEL", "set right border to PIXEL."},
         {"--cropbottom",   "PIXEL", "set bottom border to PIXEL."},
         {"--append",      "", "append images at the end of an existing movie"},
         {"--start-frame",    "NUM", "specify the first frame in the movie that will be overridden"},
         {"--start-image",   "NUM", "skip the first NUM input images"},
         {"--num-images",   "NUM", "only add NUM images"},
         {"", ""} 
    };
    myArguments.addAllowedOptionsWithDocumentation(myOptions);
    myArguments.setShortDescription(myArguments.getProgramName() +
                " Valid OpenGL compression modes:\n GENERIC_RGB, GENERIC_RGBA, GENERIC_ALPHA, GENERIC_LUMINANCE,\n"
                + " GENERIC_LUMINANCE_ALPHA, GENERIC_INTENSITY, S3TC_DXT1, S3TC_DXT1A,\n S3TC_DXT3, S3TC_DXT5");
    
    if ( ! myArguments.parse( argc, argv )) {
        return 1;
    }
    vector<string> mySourceFiles;

    if ( myArguments.haveOption("--img-dir")) {
        string myImageDirectory = myArguments.getOptionArgument("--img-dir");

        vector<string> myFiles = getDirectoryEntries(myImageDirectory);
        int mySourceFileCount = myFiles.size();
        if (mySourceFileCount < 0) {
            string myErrorMsg = string("Can't find source images in directory '")
                                + myImageDirectory + "':";
            perror(myErrorMsg.c_str());
            return 1;
        } else {
            for (int i=0; i < mySourceFileCount; ++i) {
                mySourceFiles.push_back(string(myFiles[i]));
            }
            cerr << "Found " << mySourceFiles.size() << " files in directory '"
                 << myImageDirectory << "'." << endl;
        }
    } else if (myArguments.getCount() > 0) {
        cerr << "Got " << myArguments.getCount() << " files on the command line." << endl;
        for (int i=0; i < myArguments.getCount(); ++i) {
            mySourceFiles.push_back(string(myArguments.getArgument(i)));
        }
    } else {
        cerr << "### ERROR: No input files found." << endl;
        myArguments.printUsage();
        return 1;
    }

    // create context to let OpenGL do the work
    glutCreateWindow("Compressor");

    // initialize glew
	glewInit();

    TextureCompression::COMPRESSION_FORMAT theCompressionEnum;
    std::string theCompressionFormat = "S3TC_DXT5";

    try {
        std::string myTargetFileName;

        if (myArguments.haveOption("--outfile")) {
            myTargetFileName = myArguments.getOptionArgument("--outfile");
        } else {
            cerr << "### ERROR: No output file specified" << endl;
            return -1;
        }
        
        if (myArguments.haveOption("--compression")) {
            theCompressionFormat = myArguments.getOptionArgument("--compression");
        } else {
            cerr << "Using default compression type '" << theCompressionFormat << "'" << endl;
        }

        if ( ! TextureCompression::getEnum(
                    TextureCompression::getCompressionFormatSL(), 
                    theCompressionFormat.c_str(), 
                    theCompressionEnum) ) 
        {
            cerr << "### ERROR: Format '" << theCompressionFormat 
                << "' not supported!" << endl << endl;
            myArguments.printUsage();
        }

        try {
            PLAnyPicDecoder theDecoder;
            PLAnyBmp        mySourceBitMap;
            int             firstImage = 0;
            int             lastImage = mySourceFiles.size();

            
            cerr << "Info: Starting with image #" << firstImage << " [ "
                 << mySourceFiles[firstImage] << " ]\n";

            cerr << "Info: Processing " << (lastImage - firstImage) << " image(s)\n";

            cerr << "Info: Last image is image #" << (lastImage - 1) << " [ "
                 << mySourceFiles[lastImage - 1] << " ]\n";

            bool myCreateHeaderFlag  =true;
            if (fileExists(myTargetFileName)) {
                deleteFile(myTargetFileName);
            }
            asl::WriteableFile myTargetBlock(myTargetFileName, true);

            for (int i = firstImage; i < lastImage; ++i) {

                
                cerr << "Loading image : " << mySourceFiles[i];
                if (gAppend) {
                    cerr << " (APPENDING TO IMAGE)" << endl;
                } else {
                    cerr << endl;
                }
                
                if (myArguments.haveOption("--img-dir")) {
                    theDecoder.MakeBmpFromFile(
                            string(myArguments.getOptionArgument("--img-dir") 
                            + "/" + mySourceFiles[i]).c_str(), &mySourceBitMap);
                } else {
                    theDecoder.MakeBmpFromFile(
                            mySourceFiles[i].c_str(), 
                            &mySourceBitMap);
                }

                if (mySourceBitMap.GetBitsPerPixel() == 24) {
                    PLAnyBmp my32Bitmap;    
                    my32Bitmap.Create(mySourceBitMap.GetWidth(), mySourceBitMap.GetHeight(), PLPixelFormat::A8R8G8B8);

                    for (int y=0; y<mySourceBitMap.GetHeight(); y++) {
                        for (int x=0; x<mySourceBitMap.GetWidth(); x++) {
                            PLPixel24 SrcPixel = mySourceBitMap.GetPixel24(x,y);
                            my32Bitmap.SetPixel ( x, y, 
                                    PLPixel32 ( SrcPixel.GetR(), 
                                        SrcPixel.GetG(), 
                                        SrcPixel.GetB(), 
                                        255
                                        )
                                    );
                        }
                    }
                    mySourceBitMap = my32Bitmap;
                }
                int pre_x_size = mySourceBitMap.GetWidth();
                int pre_y_size = mySourceBitMap.GetHeight();
                if (myArguments.haveOption("--pre-xsize")) {
                    pre_x_size = asl::as<int>(myArguments.getOptionArgument("--pre-xsize"));
                }
                if (myArguments.haveOption("--pre-ysize")) {
                    pre_y_size = asl::as<int>(myArguments.getOptionArgument("--pre-ysize"));
                }
                if (pre_x_size !=  mySourceBitMap.GetWidth() ||
                        pre_y_size !=  mySourceBitMap.GetHeight())
                {
                    cerr << "pre-scaling source image to x (width) = " 
                        << pre_x_size << ", y (height) = " << pre_y_size << endl;
                    mySourceBitMap.ApplyFilter(PLFilterResizeBilinear(pre_x_size,pre_y_size));
                }
                
                if (myArguments.haveOption("--pre-flip")) {
                    cerr << "flipping source image upside down" << endl;
                    mySourceBitMap.ApplyFilter(PLFilterFlip());
                }
                if (myArguments.haveOption("--rotate90")) {
                    cerr << "rotating source image 90 degress clockwise" << endl;
                    mySourceBitMap.ApplyFilter(PLFilterRotate(PLFilterRotate::ninety));
                }
                if (myArguments.haveOption("--post-flip")) {
                    cerr << "flipping source image upside down" << endl;
                    mySourceBitMap.ApplyFilter(PLFilterFlip());
                }
                
                int now_xsize = mySourceBitMap.GetWidth();
                int now_ysize = mySourceBitMap.GetHeight();
                cerr << "source image has size " << now_xsize << " x " << now_ysize << endl;

                int cropleft = 0;
                int croptop = 0;
                int cropright = now_xsize;
                int cropbottom = now_ysize;

                if (myArguments.haveOption("--cropleft")) {
                    cropleft = asl::as<int>(myArguments.getOptionArgument("--cropleft"));
                    if (! (cropleft >= 0) && (cropleft < now_xsize)) {
                        cerr << "### ERROR: cropleft must be within 0 and " << now_xsize-1 
                            << " but is " << cropleft << endl;
                    }
                }
                if (myArguments.haveOption("--croptop")) {
                    croptop = asl::as<int>(myArguments.getOptionArgument("--croptop"));
                    if (! (croptop >= 0) && (croptop < now_ysize)) {
                        cerr << "### ERROR: croptop must be within 0 and " << now_ysize-1 
                            << " but is " << croptop << endl;
                    }
                }
                if (myArguments.haveOption("--cropright")) {
                    cropright = asl::as<int>(myArguments.getOptionArgument("--cropright"));
                    if (! (cropright > cropleft) && (cropright <= now_xsize)) {
                        cerr << "### ERROR: cropright must be within " << cropleft 
                            << " and " << now_xsize << " but is " << cropright << endl;
                    }
                }
                if (myArguments.haveOption("--cropbottom")) {
                    cropbottom = asl::as<int>(myArguments.getOptionArgument("--cropbottom"));
                    if (! (cropbottom > croptop) && (cropbottom <= now_ysize)) {
                        cerr << "### ERROR: cropbottom must be within " << croptop
                            << " and " << now_ysize << " but is " << cropbottom << endl;
                    }
                }

                bool scale = false;
                int xsize = 512;
                int ysize = 256;
                if (myArguments.haveOption("--xsize")) {
                    xsize = asl::as<int>(myArguments.getOptionArgument("--xsize"));
                    scale = true;
                }
                if (myArguments.haveOption("--ysize")) {
                    ysize = asl::as<int>(myArguments.getOptionArgument("--ysize"));
                    scale = true;
                }
                
                if (cropleft != 0 || croptop != 0 || cropright != now_xsize || cropbottom != now_ysize) {
                    cerr << "Extracting image from" 
                        << " left " << cropleft << " top " << croptop 
                        << " right " << cropright << " bottom" << cropbottom << endl;
                    mySourceBitMap.ApplyFilter(PLFilterCrop(cropleft,croptop,cropright,cropbottom));
                }
                if (myArguments.haveOption("--crop")) {
                    cerr << "Cropping image to size " << xsize << " x " << ysize << endl;
                    mySourceBitMap.ApplyFilter(PLFilterCrop(0,0,xsize,ysize));
                } else if (scale){
                    cerr << "Scaling image to size " << xsize << " x " << ysize << endl;
                    mySourceBitMap.ApplyFilter(PLFilterResizeBilinear(xsize,ysize));
                }

                int myXAspect = cropright - cropleft;
                if ( myArguments.haveOption("--xaspect")) {
                    myXAspect = asl::as<int>(myArguments.getOptionArgument("--xaspect"));
                }
                int myYAspect = cropbottom - croptop;
                if ( myArguments.haveOption("--yaspect")) {
                    myYAspect = asl::as<int>(myArguments.getOptionArgument("--yaspect"));
                }
                cerr << "Using x-aspect " << myXAspect << " y-aspect " << myYAspect << endl;

                float myFramesPerSecond = 30.0;
                if ( myArguments.haveOption("--fps")) {
                    myFramesPerSecond = asl::as<float>(myArguments.getOptionArgument("--fps"));
                }

                if (asl::powerOfTwo(mySourceBitMap.GetWidth()) 
                    || asl::powerOfTwo(mySourceBitMap.GetHeight())) {

                   
                   
                    if (mySourceBitMap.GetBitsPerPixel() == 32) {
                        cerr << "Creating compressor..." << endl;
                        TextureCompression::TextureCompressor compressor(theCompressionEnum, true);
                        cerr << "Compressing & storing..." << endl;
                        if (gStartFrame > 0) {
                            cerr << "INFO: possibly overriding old movie frame #" << (gStartFrame + i)
                                 << endl;
                        }
                        compressor.compressAndStoreFrame(myTargetBlock,
                                myTargetFileName,
                                &mySourceBitMap, 
                                gAppend ? (-1) : (gStartFrame + i),
                                myXAspect,
                                myYAspect,
                                0,          // the Orientation
                                myFramesPerSecond,
                                lastImage,
                                myCreateHeaderFlag);
                        myCreateHeaderFlag = false;
                        cerr << "O.k. Texture: " << myTargetFileName << " created." << endl;

                    } else {
                        cerr << "### ERROR: Input bitmap must be 24 or 32 bpp." << endl;
                    }
                } else {
                    cerr << "### ERROR: At least one side  must be power of two" << endl;
                }
            }
        }
        catch (const exception& e) {
            cerr << "### ERROR: Standard exception occured: " << endl 
                << "     " << e.what() << endl;
            return -1;
        }
        catch (const PLTextException& e) {
            cerr << "### ERROR: Paintlib exception occured." << endl
                 << "     " << static_cast<const char *>(e) << endl;
            return -1;
        }
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


//==============================================================================
//
// $Log: generateCompressedTex.cpp,v $
// Revision 1.2  2002/09/06 18:16:49  valentin
// merged linuxport_2nd_try branch into trunk
// -branch tag before merge : branch_before_merge_final
// -trunk tag before merge  : trunk_before_merge
//
//
// -Merge conflicts:
//  image/glutShowImage/Makefile
//  image/glutShowImage/glShowImage.C
//  loader/libPfIv/pfiv.c++
//  lso/script/vrfm/Intercom.h
//  tools/Pfconvert/pfconvert.c
//  vrap/libMover/HMD.c++
//
//  - compiled && testrun
//  - commit
//  - merged trunk tag : trunk_after_merge
//
// Revision 1.1.2.22  2002/08/30 16:26:37  martin
// more std-ization
//
// Revision 1.1.2.21  2002/04/26 17:31:14  david
// fixed sync problems and simplified some code
//
// Revision 1.1.2.20  2002/03/20 17:44:00  david
//  - fixed a bug in new --img-dir option code
//
// Revision 1.1.2.19  2002/03/13 18:08:06  david
//  - added --img-dir option
//  - extended --help option
//  - added --version option
//
//
//==============================================================================
