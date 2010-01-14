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

#include "CTScan.h"
#include "Mesh.h"

#include <y60/base/IDecoder.h>
#include <asl/base/PlugInManager.h>
#include <y60/base/DecoderManager.h>

#include <asl/base/UnitTest.h>
#include <asl/base/Time.h>

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
                ScenePtr myScene = myCTScan->polygonizeGlobal(myBox, float(i), 255, 1, false, true, myPackageManager);
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
                ScenePtr myScene = myCTScan->polygonizeGlobal(myBox, float(i), 255.0f, 1, false, true, myPackageManager);
                dom::NodePtr myShape = myScene->getShapesRoot()->childNode(0);
                ENSURE(myShape);
                Mesh myMesh(myShape);
                ENSURE(myMesh.isClosed());
            }
        }
};


class MyTestSuite : public UnitTestSuite {
    public:
        MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
        void setup() {
            UnitTestSuite::setup();
            addTest(new MarchTest);
        }
};

int main(int argc, char *argv[])
{
    MyTestSuite mySuite(argv[0], argc, argv);

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

