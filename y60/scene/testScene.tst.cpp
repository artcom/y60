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

#include "Scene.h"
#include "Body.h"
#include "TextureManager.h"

#include <y60/base/Y60xsd.h>
#include <asl/base/file_functions.h>
#include <asl/base/UnitTest.h>
#include <asl/dom/Nodes.h>
#include <asl/dom/Schema.h>

#include <string>
#include <iostream>

using namespace asl;
using namespace std;
using namespace y60;
using namespace dom;

class SceneUnitTest : public UnitTest {
public:
    SceneUnitTest() : UnitTest("SceneUnitTest") {  }
    void run() {
        ENSURE(1 + 1 == 2);
        // perform more test here
    }
};

class SceneY60SchemaUnitTest : public UnitTest {
public:
    SceneY60SchemaUnitTest() : UnitTest("SceneY60SchemaUnitTest") {  }
    void run() {

        {
            DTITLE("Starting Y60 Schema tests");
            PackageManagerPtr myPackageManager(new PackageManager());
            ScenePtr myScene = Scene::createStubs(myPackageManager);

            ENSURE(myScene->getWorldRoot());
            NodePtr myWorldNode = myScene->getWorldRoot();
            TransformHierarchyFacadePtr myWorld = myWorldNode->getFacade<TransformHierarchyFacade>();
            ENSURE(myWorld);

            NodePtr myTopGroupNode = myWorldNode->appendChild(NodePtr(new dom::Element("transform")));
            ENSURE(myTopGroupNode);
            TransformHierarchyFacadePtr myTopGroup = myTopGroupNode->getFacade<TransformHierarchyFacade>();
            ENSURE(myTopGroup);

            ENSURE(myTopGroup->get<ScaleTag>() == Vector3f(1,1,1));
            ENSURE(myTopGroup->get<OrientationTag>() == Vector4f(0,0,0,1));
            ENSURE(myTopGroup->get<PositionTag>() == Vector3f(0,0,0));

            ENSURE(myTopGroup->isDirty<LocalMatrixTag>());
            ENSURE(myTopGroup->get<LocalMatrixTag>() == Matrix4f::Identity());
            ENSURE(!myTopGroup->isDirty<LocalMatrixTag>());
            ENSURE(myTopGroup->get<LocalMatrixTag>() == Matrix4f::Identity());

            ENSURE(myWorld->isDirty<GlobalMatrixTag>());
            ENSURE(myTopGroup->isDirty<GlobalMatrixTag>());
            ENSURE(myTopGroup->get<GlobalMatrixTag>() == Matrix4f::Identity());
            ENSURE(!myTopGroup->isDirty<GlobalMatrixTag>());
            ENSURE(!myWorld->isDirty<GlobalMatrixTag>());
            ENSURE(!myWorld->isDirty<LocalMatrixTag>());

            {
                Matrix4f myMatrix;
                myMatrix.makeTranslating(Vector3f(1,0,0));
                ENSURE(myTopGroup->set<PositionTag>(Vector3f(1,0,0)) == Vector3f(1,0,0));
                ENSURE(myTopGroup->get<PositionTag>() == Vector3f(1,0,0));
                ENSURE(myTopGroup->isDirty<LocalMatrixTag>());
                ENSURE(myTopGroup->isDirty<GlobalMatrixTag>());

                ENSURE(myTopGroup->get<GlobalMatrixTag>() == myMatrix);
                ENSURE(myTopGroup->get<InverseGlobalMatrixTag>() == asl::inverse(myMatrix));
                ENSURE(!myTopGroup->isDirty<LocalMatrixTag>());
                ENSURE(!myTopGroup->isDirty<GlobalMatrixTag>());
            }
            {
                ENSURE(myWorld->set<PositionTag>(Vector3f(1,0,0)) == Vector3f(1,0,0));
                ENSURE(myWorld->isDirty<LocalMatrixTag>());
                ENSURE(myWorld->isDirty<GlobalMatrixTag>());
                ENSURE(myWorld->isDirty<InverseGlobalMatrixTag>());
                ENSURE(!myTopGroup->isDirty<LocalMatrixTag>());
                ENSURE(myTopGroup->isDirty<GlobalMatrixTag>());
                ENSURE(myTopGroup->isDirty<InverseGlobalMatrixTag>());
                ENSURE(myWorld->get<InverseGlobalMatrixTag>() == asl::inverse(myWorld->get<GlobalMatrixTag>()));

                Matrix4f myMatrix;
                myMatrix.makeTranslating(Vector3f(2,0,0));
                ENSURE(myTopGroup->get<GlobalMatrixTag>() == myMatrix)

                ENSURE(!myTopGroup->isDirty<LocalMatrixTag>());
                ENSURE(!myTopGroup->isDirty<GlobalMatrixTag>());
                ENSURE(!myWorld->isDirty<LocalMatrixTag>());
                ENSURE(!myWorld->isDirty<GlobalMatrixTag>());
            }

            // test material schema
            NodePtr myMaterialNode = myScene->getMaterialsRoot()->appendChild(NodePtr(new dom::Element("material")));
            MaterialBasePtr myMaterialFacade = myMaterialNode->getFacade<MaterialBase>();
            myMaterialFacade->set<EnabledTag>(false);
            myMaterialNode->appendChild(NodePtr(new dom::Element("requires")));
            MaterialRequirementFacadePtr myReqFacade = myMaterialFacade->getChild<MaterialRequirementTag>();
            myReqFacade->getNode().appendChild(NodePtr(new dom::Node("<feature name='option'>[10[foo]]</feature>"))->firstChild());
            myMaterialFacade->set<EnabledTag>(true);
            


            // Test bounding box handling
            {

            }
        }
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new SceneUnitTest);
        addTest(new SceneY60SchemaUnitTest);
    }
};


int main(int argc, char *argv[]) {
    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();
    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}
