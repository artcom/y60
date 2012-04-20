//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "KSAnimationSampler.h"

#include <asl/base/Logger.h>
#include <asl/dom/Nodes.h>
#include <y60/base/DataTypes.h>

#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include <maya/MVector.h>
#include <maya/MTime.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MComputation.h>

#include <sstream>
#include <fstream>

using namespace asl;
using namespace std;
using namespace dom;

static const double BOTTOM_OFFSET = 2.0;

static const char * DUMMY_START_TIME("-d");
static const char * DUMMY_START_TIME_L("-dstartT");

static const char * START_TIME("-s");
static const char * START_TIME_L("-startT");

static const char * END_TIME("-e");
static const char * END_TIME_L("-endT");

static const char * VMAX("-vm");
static const char * VMAX_L("-vmax");

static const char * AMAX("-am");
static const char * AMAX_L("-amax");

static const char * IGNORE_V("-iv");
static const char * IGNORE_V_L("-ignoreV");

static const char * IGNORE_A("-ia");
static const char * IGNORE_A_L("-ignoreA");

static const char * SAVE("-f");
static const char * SAVE_L("-file");

static const char * ourSchema =
        "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>\n"
        "   <xs:simpleType name='VectorOfVector2f'>\n"
        "       <xs:restriction base='xs:string' />\n"
        "   </xs:simpleType>\n"
        "   <xs:simpleType name='Vector2f'>\n"
        "       <xs:restriction base='xs:string' />\n"
        "   </xs:simpleType>\n"
        "   <xs:simpleType name='Vector3f'>\n"
        "       <xs:restriction base='xs:string' />\n"
        "   </xs:simpleType>\n"
        "   <xs:simpleType name='Vector2i'>\n"
        "       <xs:restriction base='xs:string' />\n"
        "   </xs:simpleType>\n"
        "   <xs:element name='vectorofvector2f'>\n"
        "              <xs:complexType>\n"
        "                  <xs:simpleContent>\n"
        "             <xs:extension base='VectorOfVector2f'>\n"
        "                  <xs:attribute name='name' type='xs:string'/>/n"
        "             </xs:extension>\n"
        "         </xs:simpleContent>\n"
        "      </xs:complexType>\n"
        "   </xs:element>\n"
        "      <xs:element name='ParticleGrid'>\n"
        "              <xs:complexType>\n"
        "                      <xs:sequence>\n"
        "                              <xs:element ref='MotionData'/>\n"
        "                      </xs:sequence>\n"
        "                      <xs:attribute name='framerate' type='xs:unsignedInt'/>\n"
        "                      <xs:attribute name='spacing' type='xs:float'/>\n"
        "                      <xs:attribute name='framecount' type='xs:unsignedInt'/>\n"
        "                      <xs:attribute name='resolution' type='Vector2i'/>\n"
        "                      <xs:attribute name='origin' type='Vector3f'/>\n"
        "              </xs:complexType>\n"
        "      </xs:element>\n"
        "  <xs:element name='MotionData'>\n"
        "          <xs:complexType>\n"
        "                  <xs:sequence minOccurs='0' maxOccurs='unbounded'>\n"
        "                          <xs:element ref='vectorofvector2f' />\n"
        "                  </xs:sequence>\n"
        "                  <xs:attribute name='id' type='xs:string' />\n"
        "                      <xs:attribute name='gridpos' type='Vector2i'/>\n"
        "          </xs:complexType>\n"
        "  </xs:element>\n"
        "</xs:schema>\n";


KSAnimationSampler::KSAnimationSampler() :
    _myGridSpacing( 9.0 ),
    _myFramerate( 5.0 ),
    _myResolution( 42,17 ),
    _myIgnoreVFlag( false ),
    _myIgnoreAFlag( false ),
    _myIgnoreOutOfBounds( false )
{}

MStatus
KSAnimationSampler::doIt( const MArgList & theArgs ) {
    MStatus myStatus = MS::kFailure;
    bool isInterrupted = false;
    MComputation myComputation;
    myComputation.beginComputation();
    try {

        MSelectionList myNodes;
        parseArgs( theArgs, myNodes);

        vector< ValueListPtr > myPositions;
        vector< ValueListPtr > myVelocities;

        unsigned myNumSpheres = _myResolution[0] * _myResolution[1];
        vector<bool> myExceededVList(myNodes.length(), false);
        vector<bool> myExceededAList(myNodes.length(), false);
        vector<bool> myOutOfBoundsList(myNodes.length(), false);
        vector<bool> myCompletelyOutOfBoundsList(myNodes.length(), false);

        double dt = 1.0 / _myFramerate;

        int mySamplesPerFrame = 0;
        for (double t = _myStartTime; t <= _myEndTime + dt; t += dt) {
            isInterrupted = myComputation.isInterruptRequested();
            if (isInterrupted) {
                MGlobal::displayInfo("Interrupted.");
                break;
            }
            MGlobal::viewFrame( MTime( t, MTime::kSeconds ));
            ValueListPtr myCurrentPositions( new ValueList() );
            collectPositions( myNodes, * myCurrentPositions, myOutOfBoundsList,
                    myCompletelyOutOfBoundsList);

            if (myPositions.empty()) {
                mySamplesPerFrame = myCurrentPositions->size();
                myVelocities.push_back( ValueListPtr( new ValueList( myCurrentPositions->size(), 0.0)));
            } else {
                ValueListPtr myV = deriveVelocities( * myCurrentPositions, * myPositions.back(),
                        myExceededVList );
                checkAccelerations( * myV, * myVelocities.back(), myExceededAList);
                myVelocities.push_back( myV );
            }
            myPositions.push_back( myCurrentPositions );
        }

        bool isAnimOk = true;
        if ( ! isInterrupted) {
            myVelocities.pop_back();
            ValueListPtr myLastV = ValueListPtr( new ValueList( mySamplesPerFrame, 0.0));
            checkAccelerations( * myLastV, * myVelocities.back(), myExceededAList);
            myVelocities.push_back( myLastV );


            MSelectionList myBadSpheres;
            unsigned myNodeCount = myNodes.length();
            if ( ! _myIgnoreAFlag || ! _myIgnoreVFlag) {
                MGlobal::clearSelectionList();
                for (unsigned i = 0; i < myNodeCount; ++i) {
                    isInterrupted = myComputation.isInterruptRequested();
                    if (isInterrupted) {
                        MGlobal::displayInfo("Interrupted.");
                        break;
                    }
                    MObject myNode;
                    myStatus = myNodes.getDependNode( i, myNode );
                    if (myExceededVList[i] && ! _myIgnoreVFlag) {
                        isAnimOk = false;

                        MFnDagNode myDagNode( myNode );
                        MDagPath myPath;
                        myDagNode.getPath( myPath);
                        myBadSpheres.add( myPath);

                        MGlobal::displayWarning(MString("Velocity exceeded for object ") + myDagNode.name());
                    }
                    if (myExceededAList[i] && ! _myIgnoreAFlag) {
                        isAnimOk = false;

                        MFnDagNode myDagNode( myNode );
                        MDagPath myPath;
                        myDagNode.getPath( myPath );
                        myBadSpheres.add( myPath );

                        MGlobal::displayWarning(MString("Acceleration exceeded for object ") + myDagNode.name());
                    }
                    if (myOutOfBoundsList[i] && ! _myIgnoreOutOfBounds) {
                        MFnDagNode myDagNode( myNode );
                        MDagPath myPath;
                        myDagNode.getPath( myPath );
                        myBadSpheres.add( myPath );

                        MGlobal::displayWarning(MString("Position is out of bounds ") + myDagNode.name());
                    }
                    if (myCompletelyOutOfBoundsList[i] && ! _myIgnoreOutOfBounds) {
                        isAnimOk = false;

                        MFnDagNode myDagNode( myNode );
                        MDagPath myPath;
                        myDagNode.getPath( myPath );
                        myBadSpheres.add( myPath );

                        MGlobal::displayWarning(MString("######## ERROR: Position is completely out of bounds. ##########") + myDagNode.name());
                    }
                }
                //MGlobal::selectCommand( myBadSpheres, MGlobal::kReplaceList );
                if ( MGlobal::setActiveSelectionList( myBadSpheres, MGlobal::kReplaceList ) != MS::kSuccess ) {;
                    MGlobal::displayError("Failed to set selection list.");
                }

            }

        }

        if ( ! isInterrupted) {
            if ( ! _myFilename.empty() ) {
                // XXX
                if ( myNumSpheres > myNodes.length() ) {
                    ostringstream s;
                    s << "Spherecount does not match selection. Expected " << myNumSpheres
                      <<  " objects, but got " << myNodes.length() << ". Did you select all spheres?";
                    MGlobal::displayWarning(MString(s.str().c_str()));
                }
                if ( isAnimOk ) {
                    saveKSAnimation(myPositions, myVelocities);
                } else {
                    MGlobal::displayError(MString("Sequence contains bogus values. NOT SAVED."));
                }
            }
        }

        myStatus = MS::kSuccess;
    } catch (const std::exception & ex) {
        myStatus = MS::kFailure;
        cerr << "std::exception " << ex.what() << endl;
    } catch (const asl::Exception & ex) {
        myStatus = MS::kFailure;
        cerr << "asl::Exception" << ex << endl;
    } catch (...) {
        myStatus = MS::kFailure;
        cerr << "Unknown exception" << endl;
    }
    myComputation.endComputation();
    return myStatus;
}

ValueListPtr
KSAnimationSampler::deriveVelocities( const ValueList & currentPositions,
                                      const ValueList & previousPositions,
                                      vector<bool> & theVExceededList)
{
    ValueListPtr myVelocities( new ValueList() );
    for (unsigned i = 0; i < currentPositions.size(); ++i) {
        double myV = (currentPositions[i] - previousPositions[i]) * _myFramerate;
        if (myV > _myVMax) {
            theVExceededList[ i ] = true;
        }
        myVelocities->push_back( myV );
    }
    return myVelocities;
}

void
KSAnimationSampler::checkAccelerations( const ValueList & currentV,
                                        const ValueList & previousV,
                                        vector<bool> & theAExceededList)
{
    for (unsigned i = 0; i < currentV.size(); ++i) {
        double myA = fabs(currentV[i] - previousV[i]) * _myFramerate;
        if ( myA > _myAMax) {
            theAExceededList[ i ] = true;
        }
    }
}

void
KSAnimationSampler::collectPositions(const MSelectionList & theNodes,
        std::vector<double> & theYPositions, std::vector<bool> & theOutOfBoundsList,
        std::vector<bool> & theCompletelyOutOfBoundsList)
{
    unsigned myNodeCount = theNodes.length();
    MStatus myStatus;
    for (unsigned i = 0; i < myNodeCount; ++i) {
        MObject myNode;
        myStatus = theNodes.getDependNode( i, myNode );
        if (myStatus != MS::kSuccess) {
            cerr << "Failed to get depend node." << endl;
            //return myStatus;
            return;
        }

        MFnDagNode myDAGNode( myNode );
        double y;
        myDAGNode.findPlug("translateY").getValue( y );
        y += BOTTOM_OFFSET;
        if (y < BOTTOM_OFFSET || y > (300 - BOTTOM_OFFSET)) {
            MGlobal::displayWarning(MString("Position out of bounds for object ") + myDAGNode.name());
            theOutOfBoundsList[i] = true;
        }
        if (y < 0.0 || y > 300) {
            MGlobal::displayWarning(MString("Position completely out of bounds for object ") + myDAGNode.name());
            theCompletelyOutOfBoundsList[i] = true;
        }
        theYPositions.push_back( y );
    }
}

void
KSAnimationSampler::saveKSAnimation( const vector<ValueListPtr> & thePositions,
                                     const vector<ValueListPtr> & theVelocities )
{
    dom::DocumentPtr myDoc( new dom::Document() );
    myDoc->setValueFactory( asl::Ptr<dom::ValueFactory>( new dom::ValueFactory()));
    dom::registerStandardTypes( * myDoc->getValueFactory() );
    y60::registerSomTypes( * myDoc->getValueFactory() );
    myDoc->addSchema( dom::Node( ourSchema ), "");

    unsigned myFrameCount = thePositions.size();
    // XXX
    unsigned mySphereCount = _myResolution[0] * _myResolution[1];

    dom::NodePtr myGridNode( new Element("ParticleGrid") );
    myDoc->appendChild( myGridNode );
    myGridNode->appendAttribute("spacing", _myGridSpacing);
    myGridNode->appendAttribute("framerate", _myFramerate );
    myGridNode->appendAttribute("resolution", _myResolution );
    myGridNode->appendAttribute("framecount", myFrameCount);
    myGridNode->appendAttribute("origin", Vector3f(0,0,0));

    for (unsigned i = 0; i < mySphereCount; ++i) {
        unsigned y = (_myResolution[1] - 1) - (i % _myResolution[1]);
        unsigned x = i / _myResolution[1];
        dom::NodePtr myMotionData( new Element("MotionData") );
        myGridNode->appendChild( myMotionData );
        myMotionData->appendAttribute("gridpos", Vector2i(x, y));
        dom::NodePtr myDataNode( new Element("vectorofvector2f"));
        myMotionData->appendChild( myDataNode );

        myDataNode->appendChild(dom::Text(dom::ValuePtr(new dom::VectorValue<std::vector<Vector2f>,
                dom::MakeResizeableVector>(0))));

        dom::NodePtr myTextChild = myDataNode->childNode("#text");
        std::vector<Vector2f> & myData = (*myTextChild->nodeValuePtrOpen<std::vector<Vector2f> >());
        myData.resize( myFrameCount );

        for (unsigned j = 0; j < myFrameCount; ++j ) {
            float myP = static_cast<float>((*thePositions[j])[i]);
            if ( fabs(myP) < 0.001) {
                myP = 0.0;
            }
            float myV = static_cast<float>((*theVelocities[j])[i]);
            if ( fabs(myV) < 0.001) {
                myV = 0.0;
            }
            myData[ j ] = Vector2f( myP, myV );
        }
        myTextChild->dom::Node::nodeValuePtrClose<std::vector<Vector2f> >();
    }

    std::ofstream myOutFile( _myFilename.c_str(), std::ios::binary);
    if ( ! myOutFile ) {
        AC_ERROR << "Failed to open file '" << _myFilename << "' for writing";
        return;
    }
    myOutFile << * myDoc;
}

void *
KSAnimationSampler::creator() {
    return new KSAnimationSampler();
}

MSyntax
KSAnimationSampler::newSyntax() {
    MSyntax mySyntax;

    mySyntax.useSelectionAsDefault( true );
    mySyntax.setObjectType( MSyntax::kSelectionList, 1 );

    mySyntax.addFlag( DUMMY_START_TIME, DUMMY_START_TIME_L, MSyntax::kTime);
    mySyntax.addFlag( START_TIME, START_TIME_L, MSyntax::kTime);
    mySyntax.addFlag( END_TIME, END_TIME_L, MSyntax::kTime);
    mySyntax.addFlag( VMAX, VMAX_L, MSyntax::kDouble);
    mySyntax.addFlag( AMAX, AMAX_L, MSyntax::kDouble);
    mySyntax.addFlag( IGNORE_V, IGNORE_V_L, MSyntax::kNoArg);
    mySyntax.addFlag( IGNORE_A, IGNORE_A_L, MSyntax::kNoArg);
    mySyntax.addFlag( SAVE, SAVE_L, MSyntax::kString);

    return mySyntax;
}

void
KSAnimationSampler::parseArgs(const MArgList & theArgs, MSelectionList & theSelectionList) {
    MArgDatabase myParser( newSyntax(), theArgs );
    myParser.getObjects( theSelectionList );

    if (myParser.isFlagSet( END_TIME )) {
        _myEndTime = myParser.flagArgumentMTime( END_TIME, 0 ).as( MTime::kSeconds );
    } else {
        _myEndTime = 0.0;
    }

    if (myParser.isFlagSet( DUMMY_START_TIME )) {
        _myStartTime = myParser.flagArgumentMTime( DUMMY_START_TIME, 0 ).as( MTime::kSeconds );
    } else {
        _myStartTime = 0.0;
    }

    if (myParser.isFlagSet( START_TIME )) {
        _myStartTime = myParser.flagArgumentMTime( START_TIME, 0 ).as( MTime::kSeconds );
    } else {
        _myStartTime = 0.0;
    }

    if (myParser.isFlagSet( VMAX)) {
        _myVMax = myParser.flagArgumentDouble( VMAX, 0 );
    } else {
        _myVMax = 100.0;
    }

    if (myParser.isFlagSet( AMAX)) {
        _myAMax = myParser.flagArgumentDouble( AMAX, 0 );
    } else {
        _myAMax = 100.0;
    }

    _myIgnoreVFlag = myParser.isFlagSet( IGNORE_V);
    _myIgnoreAFlag = myParser.isFlagSet( IGNORE_A);

    if (myParser.isFlagSet( SAVE)) {
        MString str = myParser.flagArgumentString( SAVE, 0 );
        _myFilename = str.asChar();
    }

}

