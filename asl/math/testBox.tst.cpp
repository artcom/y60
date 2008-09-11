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
//   $RCSfile: testBox.tst.cpp,v $
//
//   $Revision: 1.8 $
//
//   Description: unit test for box
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Box.h"
#include "linearAlgebra.h"
#include "intersection.h"
#include "numeric_functions.h"

#include <asl/base/UnitTest.h>
#include <iostream>

using namespace std;
using namespace asl;


template <class T>
class BoxTest : public TemplateUnitTest {
public:
    BoxTest(const char * theTemplateArgument) 
        : TemplateUnitTest("BoxTest",theTemplateArgument) {}

    void run() {
        testPoint2();
        testPoint3();
    }
    
    void testPoint2() {

        Point2<T> p0(T(0.55), T(0.6));
        Point2<T> p1(T(1.1), T(1.2));
        Point2<T> p2(T(2.3), T(2.5));
        Point2<T> p3(T(-1), T(-1.1));
        Vector2<T> v1(T(2.2), T(3.3));
       // Constructor tests
        {
            asl::Box2<T> b0;            
            asl::Box2<T> b1(p0, p1); 
            ENSURE(almostEqual(b1.getMin(), p0));           
            ENSURE(almostEqual(b1.getMax(), p1));           
            
            asl::Box2<T> b2(b1);            
            ENSURE(almostEqual(b1, b2));           
            
            asl::Box2<T> b3(0, 1, 2, 3);            
            ENSURE(almostEqual(b3.getMin(), Point2<T>(0, 1)));           
            ENSURE(almostEqual(b3.getMax(), Point2<T>(2, 3)));           
        }
        
        // Test get/set min / max / center
        {
            asl::Box2<T> b1(p0, p1);
            Point2<T>    myCenter = asPoint((asVector(p0) + asVector(p1)) / 2); 
            ENSURE(almostEqual(b1.getMin(), p0));           
            ENSURE(almostEqual(b1.getMax(), p1));  
            ENSURE(almostEqual(b1.getMinVector(), asVector(p0)));           
            ENSURE(almostEqual(b1.getMaxVector(), asVector(p1)));  
            ENSURE(almostEqual(b1.getCenter(), myCenter));  
            
            b1.setCenter(p3);
            ENSURE(almostEqual(b1.getCenter(),p3));
            b1.setSize(v1);
            ENSURE(almostEqual(b1.getSize(),v1));
       }
        
        // test extendBy
        {
            asl::Box2<T> b1;            
            b1.makeEmpty();
            b1.extendBy(p0);
            ENSURE(almostEqual(b1.getMin(), p0));           
            ENSURE(almostEqual(b1.getMax(), p0));  
            b1.extendBy(p1);
            ENSURE(almostEqual(b1.getMin(), p0));           
            ENSURE(almostEqual(b1.getMax(), p1));  
            b1.extendBy(p2);
            b1.extendBy(p3);
            ENSURE(almostEqual(b1.getMin(), p3));           
            ENSURE(almostEqual(b1.getMax(), p2));  
            b1.extendBy(b1);            
            ENSURE(almostEqual(b1, b1));  
            b1.makeEmpty();
            asl::Box2<T> b2(p1, p2);
            b1.extendBy(b2);
            ENSURE(almostEqual(b1, b2));              
            b1.makeEmpty();
            b1.extendBy(p1);
            b1.extendBy(b1);
            ENSURE(b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(!b1.hasSize());
            ENSURE(!b1.hasArea());
            b1.extendBy(b2);
            ENSURE(!b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(b1.hasSize());
            ENSURE(b1.hasArea());
        }

        // test contains
        {
            asl::Box2<T> b1(p0, p2);            
            ENSURE(b1.contains(p1));
            b1.makeEmpty();
            ENSURE(!b1.contains(p1));
        }  
        // test intersect
        {
            asl::Box2<T> myFull;
            myFull.makeFull();
            asl::Box2<T> myEmpty;
            myEmpty.makeEmpty();
            
            asl::Box2<T> myBox;
            myBox.makeEmpty();
            myBox.intersect(myFull);
            ENSURE(myBox.isEmpty());
            myBox.makeFull();
            myBox.intersect(myEmpty);
            ENSURE(myBox.isEmpty());

            asl::Box2<T> b1(p3, p1);
            asl::Box2<T> b2(p0, p2);
            myBox = b1;
            myBox.intersect(myEmpty);
            ENSURE(myBox.isEmpty());
            myBox = b1;
            myBox.intersect(myFull);
            ENSURE(myBox == b1);

            myBox = b1;
            myBox.intersect(b2);
            ENSURE(myBox == Box2<T>(p0, p1));
        }
        
        // Test isEmpty, hasPosition, hasSize, hasArea
        {
            Point2<T> p1(1, 1);
            Point2<T> p2(1, 2);
            Point2<T> p3(2, 2);
            
            asl::Box2<T> b1;
            b1.makeEmpty();
            ENSURE(b1.isEmpty());
            ENSURE(!b1.hasPosition());
            ENSURE(!b1.hasSize());
            ENSURE(!b1.hasArea());
            b1.extendBy(p1);
            ENSURE(b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(!b1.hasSize());
            ENSURE(!b1.hasArea());
            b1.extendBy(p1);
            ENSURE(b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(!b1.hasSize());
            ENSURE(!b1.hasArea());
            b1.extendBy(p2);
            ENSURE(b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(b1.hasSize());
            ENSURE(!b1.hasArea());
            b1.extendBy(p3);
            ENSURE(!b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(b1.hasSize());
            ENSURE(b1.hasArea());
        }        
    }
    
    void testPoint3() {                                
        Point3<T> p0(T(0.55), T(0.6), T(0.7));
        Point3<T> p1(T(1.1), T(1.2), T(1.4));
        Point3<T> p2(T(2.3), T(2.5), T(2.9));
        Point3<T> p3(T(-1), T(-1.1), T(-1.2));
        Vector3<T> v1(T(2.2), T(3.3), T(5.5));
       
        // Constructor tests
        {
            asl::Box3<T> b0;            
            asl::Box3<T> b1(p0, p1); 
            ENSURE(almostEqual(b1.getMin(), p0));           
            ENSURE(almostEqual(b1.getMax(), p1));           
            
            asl::Box3<T> b2(b1);            
            ENSURE(almostEqual(b1, b2));           
            
            asl::Box3<T> b3(0, 1, 2, 3, 4, 5);            
            ENSURE(almostEqual(b3.getMin(), Point3<T>(0, 1, 2)));           
            ENSURE(almostEqual(b3.getMax(), Point3<T>(3, 4, 5))); 
        }
        
        // Test get/set min / max / center / size
        {
            asl::Box3<T> b1(p0, p1);
            Point3<T>    myCenter = asPoint((asVector(p0) + asVector(p1)) / 2); 
            ENSURE(almostEqual(b1.getMin(), p0));           
            ENSURE(almostEqual(b1.getMax(), p1));  
            ENSURE(almostEqual(b1.getMinVector(), asVector(p0)));           
            ENSURE(almostEqual(b1.getMaxVector(), asVector(p1)));  
            ENSURE(almostEqual(b1.getCenter(), myCenter));  
            
            b1.setCenter(p3);
            ENSURE(almostEqual(b1.getCenter(),p3));
            b1.setSize(v1);
            ENSURE(almostEqual(b1.getSize(),v1));
        }
                
        // test extendBy
        {
            asl::Box3<T> b1;            
            b1.makeEmpty();
            b1.extendBy(p0);
            ENSURE(almostEqual(b1.getMin(), p0));           
            ENSURE(almostEqual(b1.getMax(), p0));  
            b1.extendBy(p1);
            ENSURE(almostEqual(b1.getMin(), p0));           
            ENSURE(almostEqual(b1.getMax(), p1));  
            b1.extendBy(p2);
            b1.extendBy(p3);
            ENSURE(almostEqual(b1.getMin(), p3));           
            ENSURE(almostEqual(b1.getMax(), p2));  
            b1.extendBy(b1);            
            ENSURE(almostEqual(b1, b1));  
            b1.makeEmpty();
            asl::Box3<T> b2(p1, p2);
            b1.extendBy(b2);
            ENSURE(almostEqual(b1, b2));  

            b1.makeEmpty();
            b1.extendBy(p1);
            b1.extendBy(b1);
            ENSURE(b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(!b1.hasSize());
            ENSURE(!b1.hasArea());
            ENSURE(!b1.hasVolume());
            b1.extendBy(b2);
            ENSURE(!b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(b1.hasSize());
            ENSURE(b1.hasArea());
            ENSURE(b1.hasVolume());
        }
        
        // Test contains
        {
            asl::Box3<T> b1(p0, p2);
            ENSURE(b1.contains(p1));
            b1.makeEmpty();
            ENSURE(!b1.contains(p1));       
        }        
        
        // Test matrix multiply
        {
            asl::Box3<T> b1(p0, p2);
            asl::Matrix4<T> myMatrix;
            myMatrix.makeIdentity();
            asl::Box3<T> b2 = b1 * myMatrix;
            ENSURE(almostEqual(b1, b2));  
            myMatrix.makeScaling(asl::Vector3<T>(2,2,2));
            b2 = b1 * myMatrix;
            ENSURE(almostEqual(b1 * 2, b2));  
        }
        
        // Test isEmpty, hasPosition, hasSize, hasArea, hasVolume
        {
            Point3<T> p1(1, 1, 1);
            Point3<T> p2(1, 1, 2);
            Point3<T> p3(1, 2, 2);
            Point3<T> p4(2, 2, 2);
            
            asl::Box3<T> b1;
            b1.makeEmpty();
            ENSURE(b1.isEmpty());
            ENSURE(!b1.hasPosition());
            ENSURE(!b1.hasSize());
            ENSURE(!b1.hasArea());
            ENSURE(!b1.hasVolume());
            b1.extendBy(p1);
            ENSURE(b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(!b1.hasSize());
            ENSURE(!b1.hasArea());
            ENSURE(!b1.hasVolume());
            b1.extendBy(p1);
            ENSURE(b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(!b1.hasSize());
            ENSURE(!b1.hasArea());
            ENSURE(!b1.hasVolume());
            b1.extendBy(p2);
            ENSURE(b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(b1.hasSize());
            ENSURE(!b1.hasArea());
            ENSURE(!b1.hasVolume());
            b1.extendBy(p3);
            ENSURE(b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(b1.hasSize());
            ENSURE(b1.hasArea());
            ENSURE(!b1.hasVolume());
            b1.extendBy(p4);
            ENSURE(!b1.isEmpty());
            ENSURE(b1.hasPosition());
            ENSURE(b1.hasSize());
            ENSURE(b1.hasArea());
            ENSURE(b1.hasVolume());
        }
        // Test intersects
        {
            asl::Box3<T> b1(-4,-4,-4,4,4,4);
            asl::Box3<T> b2(-2,-2,-8,2,2,8);
            asl::Box3<T> b3(4,4,4,5,5,5);
            
            ENSURE(b1.touches(b2));
            ENSURE(b2.touches(b1));
            ENSURE(b1.intersects(b2));
            ENSURE(b2.intersects(b1));
            asl::Box3<T> b12 = b1;
            b12.intersect(b2);
            ENSURE(!b12.isEmpty());
            DPRINT(b12);

            ENSURE(b1.touches(b3));
            ENSURE(b3.touches(b1));
            ENSURE(!b1.intersects(b3));
            ENSURE(!b3.intersects(b1));
            asl::Box3<T> b13 = b1;
            b13.intersect(b3);
            ENSURE(b13.hasPosition());
            ENSURE(b13.isEmpty());

      }
    }
     
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new BoxTest<float>("<float>"));
        addTest(new BoxTest<double>("<double>"));
    }
};

int main(int argc, char *argv[]) {
    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();
	std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << endl;
    return mySuite.returnStatus();
}
