//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef KSAnimationSampler_INCLUDED
#define KSAnimationSampler_INCLUDED

#include <asl/base/Ptr.h>
#include <asl/math/Vector234.h>

#include <iostream>

#include <maya/MString.h> 
#include <maya/MSyntax.h> 
#include <maya/MArgDatabase.h> 
#include <maya/MSelectionList.h> 
#include <maya/MFnDependencyNode.h> 
#include <maya/MArgList.h> 
#include <maya/MPxCommand.h> 
#include <maya/MFnDagNode.h> 

#include <vector>

typedef std::vector<double> ValueList;
typedef asl::Ptr<ValueList> ValueListPtr;

class KSAnimationSampler : public MPxCommand {
    public:
        KSAnimationSampler();
        MStatus doIt( const MArgList & theArgs );

        static void * creator();
        static MSyntax newSyntax();

    private:
        void parseArgs(const MArgList & theArgs, MSelectionList & theNodes);

        void collectPositions(const MSelectionList & theNodes,
                std::vector<double> & theYPositions, std::vector<bool> & theOutOfBoundsList,
                std::vector<bool> & theCompletelyOutOfBoundsList);
        
        ValueListPtr deriveVelocities( const ValueList & currentPositions,
                                       const ValueList & previousPositions,
                                       std::vector<bool> & theVExceededList  );
        void checkAccelerations( const ValueList & currentV, const ValueList & previousV,
                                 std::vector<bool> & theAExceededList );

        void saveKSAnimation( const std::vector<ValueListPtr> & thePositions,
                              const std::vector<ValueListPtr> & theVelocities );


        double _myStartTime;
        double _myEndTime;
        double _myVMax;
        double _myAMax;
        std::string _myFilename;

        double _myGridSpacing;
        double _myFramerate;
        asl::Vector2i _myResolution;

        bool _myIgnoreVFlag;
        bool _myIgnoreAFlag;
        bool _myIgnoreOutOfBounds;
};

#endif // KSAnimationSampler_INCLUDED
