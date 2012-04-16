//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

//#include <asl/Exception.h>
//#include <asl/numeric_functions.h>

#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfCRgbaFile.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImathBox.h>

#include <maya/MString.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MArgList.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <math.h>


#include "KSAnimationSampler.h"

//using namespace asl;
using namespace std;
using namespace Imf;

static const std::string DIGITS("0123456789");

static const char * MIN_U("-mu");
static const char * MIN_U_L("-minU");
static const char * MIN_V("-mv");
static const char * MIN_V_L("-minV");

static const char * MAX_U("-xu");
static const char * MAX_U_L("-maxU");
static const char * MAX_V("-xv");
static const char * MAX_V_L("-maxV");

static const char * SAMPLES_U("-su");
static const char * SAMPLES_U_L("-samplesU");
static const char * SAMPLES_V("-sv");
static const char * SAMPLES_V_L("-samplesV");

static const char * SCALE("-sc");
static const char * SCALE_L("-scale");

class ColorAtPointHDR : public MPxCommand {
    public:
        MStatus        doIt( const MArgList& args );
        static void*   creator();

        static MSyntax newSyntax();
    private:

        void parseArgs(const MArgList & theArgs,
                MObject & theTextureNode,
                double & theMinU, double & theMaxU, double & theMinV, double & theMaxV,
                long & theSamplesU, long & theSamplesV,
                double & theScale);

        MStatus composeFilename( MObject theTextureNode, std::string & theFilename );
        void sampleTexture(const std::string & theFilename,
                const double & theMinU, const double & theMaxU,
                const double & theMinV, const double & theMaxV,
                const long & theSamplesU, const long & theSamplesV,
                const double & theScale);
};

#if WIN32
#   define DLL_EXPORT __declspec(dllexport)
#else
#   define DLL_EXPORT
#endif

DLL_EXPORT
MStatus
initializePlugin( MObject obj ) {
    MFnPlugin plugin( obj, "ART+COM", "1.0", "Any" );
    plugin.registerCommand( "colorAtPointHDR",
                            ColorAtPointHDR::creator,
                            ColorAtPointHDR::newSyntax);
    plugin.registerCommand( "sampleKSAnimation",
                            KSAnimationSampler::creator,
                            KSAnimationSampler::newSyntax );
    return MS::kSuccess;
}

DLL_EXPORT
MStatus
uninitializePlugin( MObject obj ) {
    MFnPlugin plugin( obj );
    plugin.deregisterCommand( "colorAtPointHDR" );
    plugin.deregisterCommand( "sampleKSAnimation" );
    return MS::kSuccess;
}


MStatus
ColorAtPointHDR::doIt( const MArgList& args ) {
    try {
        MObject myObject;
        double myMinU, myMaxU, myMinV, myMaxV;
        long mySamplesU, mySamplesV;
        double myScale;
        parseArgs( args, myObject, myMinU, myMaxU, myMinV, myMaxV,
                mySamplesU, mySamplesV, myScale);
        std::string myFilename;
        MStatus myStatus = composeFilename( myObject, myFilename );
        //cout << "Sampling " << myFilename << endl;

        sampleTexture( myFilename , myMinU, myMaxU, myMinV, myMaxV,
                mySamplesU, mySamplesV, myScale);
    } catch (const std::exception & ex) {
        cerr << "std::exception: " << ex.what();
        return MS::kFailure;
    } catch (...) {
        cerr << "EXCEPTION" << endl;
    }

    return MS::kSuccess;
}

void *
ColorAtPointHDR::creator() {
    return new ColorAtPointHDR;
}

MSyntax
ColorAtPointHDR::newSyntax() {
    MSyntax mySyntax;
    mySyntax.addArg(MSyntax::kSelectionItem);
    mySyntax.addFlag(MIN_U, MIN_U_L, MSyntax::kDouble);
    mySyntax.addFlag(MIN_V, MIN_V_L, MSyntax::kDouble);
    mySyntax.addFlag(MAX_U, MAX_U_L, MSyntax::kDouble);
    mySyntax.addFlag(MAX_V, MAX_V_L, MSyntax::kDouble);
    mySyntax.addFlag(SAMPLES_U, SAMPLES_U_L, MSyntax::kLong);
    mySyntax.addFlag(SAMPLES_V, SAMPLES_V_L, MSyntax::kLong);
    mySyntax.addFlag(SCALE, SCALE_L, MSyntax::kDouble);
    return mySyntax;
}


MStatus
ColorAtPointHDR::composeFilename( MObject theTextureNode, std::string & theFilename ) {
    MString myFilename;
    MFnDependencyNode myTextureNode( theTextureNode );
    MStatus myStatus = myTextureNode.findPlug( "fileTextureName").getValue( myFilename );

    bool myUseFrameExtensionFlag;
    myStatus = myTextureNode.findPlug("useFrameExtension").getValue( myUseFrameExtensionFlag );

    if ( ! myUseFrameExtensionFlag ) {
        theFilename = myFilename.asChar();
    } else {

        int myFrameNo;
        myStatus = myTextureNode.findPlug("frameExtension").getValue( myFrameNo );
        if (myStatus != MS::kSuccess) {
            cerr << "Failed to get texture filename." << endl;
            return myStatus;
        }

        std::string myString( myFilename.asChar() );
        string::size_type myLastDigit = myString.find_last_of( DIGITS );
        if (myLastDigit == string::npos ) {
            cerr << "Failed to find frame number in filename" << endl;
        }
        string::size_type myFirstDigit = myString.find_last_not_of( DIGITS, myLastDigit -1 );

        std::string myHead = myString.substr( 0, myFirstDigit + 1 );
        std::string myTail = myString.substr( myLastDigit + 1);

        ostringstream s;
        s << myHead << setw( myLastDigit - myFirstDigit ) << setfill('0')
          << myFrameNo << myTail;

        theFilename = s.str();

        //cerr << "frame: " << theFilename << endl;
    }

    return MS::kSuccess;
}

void
ColorAtPointHDR::sampleTexture( const std::string & theFilename,
                const double & theMinU, const double & theMaxU,
                const double & theMinV, const double & theMaxV,
                const long & theSamplesU, const long & theSamplesV,
                const double & theScale )
{
    RgbaInputFile myFile( theFilename.c_str() );

    Imath::Box2i dw = myFile.dataWindow();

    int myWidth = dw.max.x - dw.min.x + 1;
    int myHeight = dw.max.y - dw.min.y + 1;

    Array2D<Rgba> myPixels;
    myPixels.resizeErase( myHeight, myWidth);

    myFile.setFrameBuffer( & myPixels[0][0] - dw.min.x - dw.min.y * myWidth, 1, myWidth);
    myFile.readPixels( dw.min.y, dw.max.y );

    clearResult();


    double du = (theMaxU - theMinU) / (theSamplesU - 1);
    double dv = (theMaxV - theMinV) / (theSamplesV - 1);


    //cout << "w: " << myWidth << " h: " << myHeight << endl;

    for (int myUSteps = 0; myUSteps < theSamplesU; ++myUSteps) {
        double myU = (theMinU + myUSteps * du) * (myWidth - 1);
        int myLowerU = int( floor( myU + 0.5 ));
        //cerr << "u: " << myU << " int u: " << myLowerU << endl;
        int myUpperU = min( myLowerU + 1, (myWidth - 1) ); // XXX
        double myUWeight = myU - myLowerU;

        for (int myVSteps = 0; myVSteps < theSamplesV; ++myVSteps) {
            double myV = (theMinV + myVSteps * dv) * (myHeight - 1);
            myV = (myHeight -1) - myV;
            int myLowerV = int( floor( myV  + 0.5));
            int myUpperV = min( myLowerV + 1, (myHeight - 1 )); // XXX
            double myVWeight = myV - myLowerV;

            float mySample = myPixels[myLowerV][myLowerU].r;

            //cout << "u: " << myLowerU << " v: " << myLowerV
            //     << " sample: " << mySample << endl;

            appendToResult(double( mySample ) * theScale);
        }
    }
}

void
ColorAtPointHDR::parseArgs(const MArgList & theArgs,
        MObject & theTextureNode,
        double & theMinU, double & theMaxU, double & theMinV, double & theMaxV,
        long & theSamplesU, long & theSamplesV,
        double & theScale)
{
    MSelectionList mySelectionList;
    MArgDatabase myParser( newSyntax(), theArgs);
    MStatus myStatus = myParser.getCommandArgument(0, mySelectionList );
    if (myStatus != MS::kSuccess) {
        //throw std::exception("Failed to get arg.");
    }
    myStatus = mySelectionList.getDependNode( 0, theTextureNode );
    if (myStatus != MS::kSuccess) {
        //throw std::exception("Failed to get depend node.");
    }

    if (myParser.isFlagSet( MIN_U )) {
        theMinU = myParser.flagArgumentDouble( MIN_U, 0 );
    } else {
        theMinU = 0.0;
    }
    if (myParser.isFlagSet( MIN_V )) {
        theMinV = myParser.flagArgumentDouble( MIN_V, 0 );
    } else {
        theMinV = 0.0;
    }
    if (myParser.isFlagSet( MAX_U )) {
        theMaxU = myParser.flagArgumentDouble( MAX_U, 0 );
    } else {
        theMaxU = 1.0;
    }
    if (myParser.isFlagSet( MAX_V )) {
        theMaxV = myParser.flagArgumentDouble( MAX_V, 0 );
    } else {
        theMaxV = 1.0;
    }
    if (myParser.isFlagSet( SAMPLES_U )) {
        theSamplesU = myParser.flagArgumentInt( SAMPLES_U, 0 );
    } else {
        theSamplesU = 1;
    }
    if (myParser.isFlagSet( SAMPLES_V )) {
        theSamplesV = myParser.flagArgumentInt( SAMPLES_V, 0 );
    } else {
        theSamplesV = 1;
    }
    if (myParser.isFlagSet( SCALE )) {
        theScale = myParser.flagArgumentDouble( SCALE, 0 );
    } else {
        theScale = 1.0;
    }
}
