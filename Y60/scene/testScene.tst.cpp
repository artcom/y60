/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testScene.tst.cpp,v $
//
//   $Revision: 1.23 $
//
// Description: unit test template file - change Scene to whatever
//              you want to test and add the apprpriate tests.
//
//
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
