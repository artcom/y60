//=============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "CTScan.h"
#include "Mesh.h"

#include <y60/IDecoder.h>
#include <asl/PlugInManager.h>
#include <y60/DecoderManager.h>

#include <asl/UnitTest.h>
#include <asl/Time.h>

using namespace std;
using namespace asl;
using namespace y60;

class MarchTest : public UnitTest {
    public:
        MarchTest() : UnitTest("MarchTest") {  }

        void createSlices(asl::Point3i theSize, std::vector<dom::ResizeableRasterPtr> & theSlices ) {
            theSlices.clear();
            for (int z = 0; z < theSize[2]; ++z) {
                dom::ResizeableRasterPtr mySlice = dynamic_cast_Ptr<dom::ResizeableRaster>(
                        createRasterValue(y60::GRAY, theSize[0], theSize[1]));
                theSlices.push_back(mySlice);
            }
        }

        void createSphere(int theSize, std::vector<dom::ResizeableRasterPtr> & theSlices ) {
            createSlices(Point3i(theSize, theSize, theSize), theSlices);
            Vector3f myCenter(float(theSize)/2.0f, float(theSize)/2.0f, float(theSize)/2.0f);
            for (int z = 0; z < theSize; ++z) {
                asl::WriteableBlock & myPixels = theSlices[z]->pixels(); 
                for (int y = 0; y < theSize; ++y) {
                    for (int x = 0; x < theSize; ++x) {
                        float myDistance = magnitude(difference(Vector3f(x,y,z), myCenter));
                        myPixels[y*theSize+x] = Unsigned8((1.0f-clamp(myDistance*2.5f/ (theSize), 0.0f, 1.0f))*255); 
                    }
                }
            }
        }
            
        void createWave(int theSize, asl::Box3i theBox, std::vector<dom::ResizeableRasterPtr> & theSlices ) {
            createSlices(Point3i(theSize, theSize, theSize), theSlices);
            Vector3f myCenter(float(theSize)/2.0f, float(theSize)/2.0f, float(theSize)/2.0f);
            for (int z = 0; z < theSize; ++z) {
                asl::WriteableBlock & myPixels = theSlices[z]->pixels(); 
                for (int y = 0; y < theSize; ++y) {
                    for (int x = 0; x < theSize; ++x) {
                        float myValue = sin(float(x+y/5)*20/theSize)+
                                        sin(float(y)*20/theSize)/10;
                        bool isNotOnBorder = (z > theBox.getMin()[2]) && (z < theBox.getMax()[2]) &&
                                      (y > theBox.getMin()[1]) && (y < theBox.getMax()[1]) &&
                                      (x > theBox.getMin()[0]) && (x < theBox.getMax()[0]);

                        isNotOnBorder = true;
                        
                        float myGrayValue = clamp( (-(float(z)/theSize) + (myValue/4+0.5f))*255, 0.0f, 255.0f);
                        
  
                        myPixels[y*theSize+x] = isNotOnBorder ? Unsigned8(myGrayValue) : 0; 
                    }
                }
            }
        }
            
        void run() {
            testSphere();
            // testWaves();
        }
        
        void testWaves() {
            int mySize = 64;
            PackageManagerPtr myPackageManager = PackageManagerPtr(new PackageManager());
            std::vector<dom::ResizeableRasterPtr> mySlices;
            Box3i myBox(Point3i(23,9,4), Point3i(24,10,6));
            createWave(mySize, myBox, mySlices);
            CTScanPtr myCTScan = CTScanPtr(new CTScan());

            myCTScan->setSlices(mySlices);
            myCTScan->setVoxelSize(Vector3f(1.0f,1.0f,1.0f));

            // for (int i = 0; i < 256; ++i) {
                int i = 170;
                ScenePtr myScene = myCTScan->polygonize(myBox, float(i), 255, 1, true, myPackageManager); 
                dom::NodePtr myShape = myScene->getShapesRoot()->childNode(0);
                ENSURE(myShape);
                Mesh myMesh(myShape);
                ENSURE(myMesh.isClosed());
                //if (!myMesh.isClosed()) {
                //    myScene->save(as_string(i)+"_test.x60", false);
                //}
                // myScene->save(as_string(i)+"_test.x60", false);
            //}
        }
        void testSphere() {
            int mySize = 64;
            PackageManagerPtr myPackageManager = PackageManagerPtr(new PackageManager());
            std::vector<dom::ResizeableRasterPtr> mySlices;
            createSphere(mySize, mySlices);
            CTScanPtr myCTScan = CTScanPtr(new CTScan());

            myCTScan->setSlices(mySlices);
            myCTScan->setVoxelSize(Vector3f(1.0f,1.0f,1.0f));
            Box3i myBox(Point3i(0,0,0), Point3i(mySize,mySize,mySize));

            for (int i = 0; i < 256; ++i) {
                ScenePtr myScene = myCTScan->polygonize(myBox, float(i), 255, 1, true, myPackageManager); 
                dom::NodePtr myShape = myScene->getShapesRoot()->childNode(0);
                ENSURE(myShape);
                Mesh myMesh(myShape);
                ENSURE(myMesh.isClosed());
            }
        }
};


class MyTestSuite : public UnitTestSuite {
    public:
        MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
        void setup() {
            UnitTestSuite::setup();
            addTest(new MarchTest);
        }
};

int main(int argc, char *argv[])
{
    MyTestSuite mySuite(argv[0]);

    try {
        mySuite.run();
    } catch (const asl::Exception & ex) {
        cerr << "Exception during test execution: " << ex << endl;
        return -1;
    }

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}

