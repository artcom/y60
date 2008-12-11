/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSnumeric: test_linearAlgebra.tst.cpp,v $
//
//   $Revision: 1.5 $
//
// Description: unit test for linear algebra
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "contact.h"

#include <asl/base/UnitTest.h>
#include <iostream>

using namespace std;
using namespace asl;

// #define GRID_Z_TEST

template<class Number>
class SweptSpherePointContact : public TemplateUnitTest {
    public:
        SweptSpherePointContact(const char * theTemplateArgument) 
            : TemplateUnitTest("SweptSpherePointContact",theTemplateArgument) {}

        void run() {
          
            Sphere<Number> mySphere(Point3<Number>(1,2,3), 0.5);
            Vector3<Number> myMotion(5, 0, 0);

            {
                Number firstContact;
                Number lastContact;
                Point3<Number> myVertex(1.0,0.0,3.0);
                
                int myContacts = contacts(mySphere, myMotion, myVertex, firstContact, lastContact);
                ENSURE_MSG(myContacts == 0, "Vertex is too far away");

            }

            {
                Number firstContact;
                Number lastContact;
                Point3<Number> myVertex(0.0,2.0,3.0);
                
                int myContacts = contacts(mySphere, myMotion, myVertex, firstContact, lastContact);
                DPRINT(myContacts);
                DPRINT(firstContact);
                DPRINT(lastContact);
                ENSURE_MSG(myContacts == 2, "Vertex is on the axis...");
                int myInBounds = clipContacts(firstContact, lastContact);
                ENSURE_MSG(myInBounds == 0, "   ...but before the first sphere");
            }

            {
                Number firstContact;
                Number lastContact;
                Point3<Number> myVertex(0.5,2.0,3.0);
                
                int myContacts = contacts(mySphere, myMotion, myVertex, firstContact, lastContact);
                DPRINT(myContacts);
                DPRINT(firstContact);
                DPRINT(lastContact);
                ENSURE_MSG(myContacts == 2, "Vertex is on the axis...");
                int myInBounds = clipContacts(firstContact, lastContact);
                ENSURE_MSG(myInBounds == 1, "   ...and touching the first sphere");
                ENSURE(almostEqual(firstContact, Number(0.0)));
            }
     
            {
                Number firstContact;
                Number lastContact;
                Point3<Number> myVertex(6.5,2.0,3.0);
                
                int myContacts = contacts(mySphere, myMotion, myVertex, firstContact, lastContact);
                DPRINT(myContacts);
                DPRINT(firstContact);
                DPRINT(lastContact);
                ENSURE_MSG(myContacts == 2, "Vertex is on the axis...");
                int myInBounds = clipContacts(firstContact, lastContact);
                ENSURE_MSG(myInBounds == 1, "   ...and is touching the last sphere");
                ENSURE(almostEqual(firstContact, Number(1.0)));
            }

            {
                Number firstContact;
                Number lastContact;
                Point3<Number> myVertex(Number(3.0),Number(2.2),Number(3.0));
                
                int myContacts = contacts(mySphere, myMotion, myVertex, firstContact, lastContact);
                DPRINT(myContacts);
                DPRINT(firstContact);
                DPRINT(lastContact);
                ENSURE_MSG(myContacts == 2, "Vertex is close enough to the axis...");
                int myInBounds = clipContacts(firstContact, lastContact);
                ENSURE_MSG(myInBounds == 2, "   ...and is touched twice by the sphere");
            }

            {
                Number firstContact;
                Number lastContact;
                Point3<Number> myVertex(Number(1.1),Number(2.1),3);
                Vector3<Number> myTinyMotion(Number(0.1), 0.0, 0.0);
                
                int myContacts = contacts(mySphere, myTinyMotion, myVertex, firstContact, lastContact);
                DPRINT(myContacts);
                DPRINT(firstContact);
                DPRINT(lastContact);
                ENSURE_MSG(myContacts == 2, "Vertex get swept completely by the sphere");
                int myInBounds = clipContacts(firstContact, lastContact);
                ENSURE_MSG(myInBounds == 2, "   ...and is completely encompassed by the sphere");
                ENSURE(firstContact == Number(0));
                ENSURE(lastContact == Number(1));
            }

        }
};

template<class Number>
class SweptSphereEdgeContact : public TemplateUnitTest {
    public:
        SweptSphereEdgeContact(const char * theTemplateArgument) 
            : TemplateUnitTest("SweptSphereEdgeContact",theTemplateArgument) {}

        void run() {
            Sphere<Number> mySphere(Point3<Number>(1, 2, 3), Number(0.5));
            Vector3<Number> myMotion(5, 0, 0);

            {
                // Segment to far away
                Number firstContact;
                Number lastContact;
                Number lineFirstContact;
                Number lineLastContact;
                LineSegment<Number> mySegment(Point3<Number>(1.0,0.0,3.0), Point3<Number>(1.0,-1.0,3.0));
                
                int myContacts = contacts(mySphere, myMotion, mySegment, firstContact, lastContact, lineFirstContact, lineLastContact);
                DPRINT(myContacts);
                DPRINT(firstContact);
                DPRINT(lastContact);
                ENSURE_MSG(myContacts == 0, "Segment is too far away");

            }
            
            {
                // segment crosses the path completely (perpendicular)
                Number firstContact;
                Number lastContact;
                Number lineFirstContact;
                Number lineLastContact;
                LineSegment<Number> mySegment(Point3<Number>(3.0,4.0,3.0), Point3<Number>(3.0,0.0,3.0));
                
                int myContacts = contacts(mySphere, myMotion, mySegment, firstContact, lastContact, lineFirstContact, lineLastContact);
                DPRINT(myContacts);
                DPRINT(firstContact);
                DPRINT(lastContact);
                ENSURE_MSG(myContacts == 2, "Segement gets swept completely by sphere");
                int myInBounds = clipContacts(firstContact, lastContact);
                ENSURE_MSG(myInBounds == 2, "   ...and extends beyond the swept sphere in both directions");
                // the contact points should be (2+0.5)/5 and (2-0.5)/5 
                ENSURE(almostEqual(firstContact,Number(0.3)));
                ENSURE(almostEqual(lastContact,Number(0.5)));
            }

            {
                // segment parallel to sweep and extends beyond the sweep in both directions
                Number firstContact;
                Number lastContact;
                Number lineFirstContact;
                Number lineLastContact;
                LineSegment<Number> mySegment(Point3<Number>(0.0,2.0,3.0), Point3<Number>(7.0,2.0,3.0));
                
                int myContacts = contacts(mySphere, myMotion, mySegment, firstContact, lastContact, lineFirstContact, lineLastContact);
                DPRINT(myContacts);
                DPRINT(firstContact);
                DPRINT(lastContact);
                ENSURE_MSG(myContacts == 2, "Vertex contact - no edge contact");
                ENSURE(firstContact == 0);
                ENSURE(lastContact == 1);
            }
            if (false) {
                // segment parallel to sweep and extends beyond the sweep in one direction
                Number firstContact;
                Number lastContact;
                Number lineFirstContact;
                Number lineLastContact;
                LineSegment<Number> mySegment(Point3<Number>(0.0,2.0,3.0), Point3<Number>(4.0,2.0,3.0));
                
                int myContacts = contacts(mySphere, myMotion, mySegment, firstContact, lastContact, lineFirstContact, lineLastContact);
                DPRINT(myContacts);
                DPRINT(firstContact);
                DPRINT(lastContact);
                ENSURE_MSG(myContacts == 2, "Vertex contact - no edge contact");
                ENSURE(firstContact == 0);
                ENSURE(lastContact == 0.5);
            }
        }
};


template<class Number>
class SweptSphereTriangleContact : public TemplateUnitTest {
    public:
        SweptSphereTriangleContact(const char * theTemplateArgument) 
            : TemplateUnitTest("SweptSphereTriangleContact",theTemplateArgument) {}

        void gridTest_Z(const Point3<Number> & theContactSphereOrigin, const Triangle<Number> & theTriangle,
                        Number theRadius, const Point3<Number> & theContactPoint) {
          
            Sphere<Number> mySphere(theContactSphereOrigin, theRadius);
            mySphere.center[2] -= theRadius*2;
            Vector3<Number>myMotion(0,0,theRadius*4);
            Vector3<Number> myNormal = theTriangle.normal();
            Vector3<Vector3<Number> > myNormals(myNormal, myNormal, myNormal);
            SweptSphereContact<Number> myMinContact;
            SweptSphereContact<Number> myMaxContact;
            int myContacts = contacts(mySphere, myMotion, theTriangle, &myNormals, myMinContact, myMaxContact);
            DPRINT("============== Z-Test on:");
            DPRINT(theContactSphereOrigin);
            DPRINT(theContactPoint);
            DPRINT(myMinContact.t);
            DPRINT(myMaxContact.t);
            DPRINT(myMinContact.contactPoint);
            DPRINT(myMaxContact.contactPoint);
            ENSURE_MSG(myContacts==1, "grid_Z test: one contact");
            DPRINT(myMinContact.contactPoint);
            DPRINT(theContactPoint);
            ENSURE_MSG(almostEqual(myMinContact.contactPoint, theContactPoint), "grid_Z test: Contact Point ok");
        }

        void gridTest() {
            Number z(3);
            Triangle<Number> myTriangle(Point3<Number>(3,3,z), Point3<Number>(6,3,z), Point3<Number>(6,6,z));
            // setAbortOnFailure(true);
            for (int y=0; y < 10; ++y) {
                for (int x=0; x < 10; ++x) {
                    for (int deltaX=0; deltaX+x < 10; ++deltaX) {
                        int lastX = x+deltaX;

                        SweptSphereContact<Number> myMinContact;
                        SweptSphereContact<Number> myMaxContact;
                        Sphere<Number> mySphere(Point3<Number>(x,y,z), 2);
                        Vector3<Number>myMotion(deltaX,0,0);
                        DPRINT(x);
                        DPRINT(y);
                        DPRINT(deltaX);
                        if (x==4 && y==2 && deltaX==5) {
                            z = z; // place breakpoint here
                        }
                        int myContacts = contacts<Number>(mySphere, myMotion, myTriangle, 0, myMinContact, myMaxContact);
                        DPRINT(myContacts);
                        if (lastX <=1) {
                            if (y==3 && lastX==1) {
                                ENSURE_MSG(myContacts==1, "one contact");
                                ENSURE_MSG(myMinContact.contactType == VERTEX_CONTACT, "min hit lower-right vertex");
                            } else {
                                ENSURE_MSG(myContacts==0, "No Contacts");
                            }
                        }

                        if ((y>=1 && y<=7) && (x<=1 && lastX>=8)) {
                            ENSURE_MSG(myContacts==2, "Triangle touch test: 2 contacts");
                        }
                        if ((y>=4 && y<=5) && (x<=4 && lastX>=8)) {
                            ENSURE(myMaxContact.contactType == EDGE_CONTACT);
                        }
                        if ((y>=5 && y<=7) && (x<=1 && lastX >= (y-2))) {
                            ENSURE_MSG(myMinContact.contactType == EDGE_CONTACT, "first hit hypotenuse");
                        }

                        if ((y==0) ||(y==9)) {
                            ENSURE_MSG(myContacts==0, "No Contacts");
                        } else if (y==1) {
                            if ((lastX < 3) || (x > 6) ) {
                                ENSURE_MSG(myContacts==0, "No Contacts");
                            } else {
                                if ((lastX==3) || (x==6) || (deltaX==0)){
                                    ENSURE_MSG(myContacts==1, "One Contact");
                                } else {
                                    ENSURE_MSG(myContacts==2, "Two contacts");
                                }
                                if (x <= 3 || x >= 6 ) {
                                    ENSURE_MSG(myMinContact.contactType == VERTEX_CONTACT, "min hit lower-right vertex");
                                } else {
                                    ENSURE_MSG(myMinContact.contactType == EDGE_CONTACT, "min hit lower edge");
                                }
                                if (lastX > 3 && lastX < 6) {
                                    ENSURE_MSG(myMaxContact.contactType == EDGE_CONTACT, "max hit lower-right vertex");
                                } else {
                                    ENSURE_MSG(myMaxContact.contactType == VERTEX_CONTACT, "max hit lower edge");
                                }
                                DPRINT(myContacts);
                            }
                        }
#ifdef GRID_Z_TEST
                        if (myContacts>=1 && myMinContact.t != 0) {
                            gridTest_Z(myMinContact.contactSphereCenter, myTriangle, mySphere.radius, myMinContact.contactPoint);
                        }
                        if (myContacts==2 && myMaxContact.t != 1) {
                            gridTest_Z(myMaxContact.contactSphereCenter, myTriangle, mySphere.radius, myMaxContact.contactPoint);
                        }
#endif
                    }
                }
            }
        }


        void run() {          
            Sphere<Number> mySphere(Point3<Number>(1,2,3), 0.5);
            Vector3<Number> myMotion(5, 0, 0);
            SweptSphereContact<Number> myMinContact;
            SweptSphereContact<Number> myMaxContact;
            
            {
                // triangle-plane is parallel and too far away
                Triangle<Number> myTriangle(Point3<Number>(0,0,0), Point3<Number>(5,0,0), Point3<Number>(1,0,0));
                Vector3<Number> myNormal = myTriangle.normal();
                Vector3<Vector3<Number> > myNormals(myNormal, myNormal, myNormal);
                            
                int myContacts = contacts(mySphere, myMotion, myTriangle, &myNormals, myMinContact, myMaxContact);
                ENSURE_MSG(myContacts == 0, "triangle-plane is parallel and too far away");
            }

            {
                // triangle-plane is parallel and embedded - triangle is too far away
                Triangle<Number> myTriangle(Point3<Number>(0,4,3), Point3<Number>(5,4,3), Point3<Number>(3,5,3));
                            
                int myContacts = contacts<Number>(mySphere, myMotion, myTriangle, 0, myMinContact, myMaxContact);
                ENSURE_MSG(myContacts == 0, "triangle-plane is parallel and embedded - triangle is too far away");
            }

            {
                // EMBEDDED-EMBEDDED
                // triangle-plane is parallel and embedded - triangle encompasses capsule
                Triangle<Number> myTriangle(Point3<Number>(0,0,3), Point3<Number>(0,4,3), Point3<Number>(20,2,3));
                Vector3<Number> myNormal = myTriangle.normal();
                Vector3<Vector3<Number> > myNormals(myNormal, myNormal, myNormal);
                            
                int myContacts = contacts<Number>(mySphere, myMotion, myTriangle, 0, myMinContact, myMaxContact);
                ENSURE_MSG(myContacts == 2, "triangle-plane is parallel and embedded - triangle encompasses capsule");
                ENSURE(myMinContact.contactType == EMBEDDED);
                ENSURE(myMinContact.t == 0);
                ENSURE(myMinContact.contactPoint == Point3<Number>(1, 2, 3));
                ENSURE(myMinContact.contactSphereCenter == Point3<Number>(1, 2, 3));
                ENSURE(myMinContact.contactSphereNormal == -myNormal); 
                ENSURE(myMinContact.contactNormal == myNormal);
                ENSURE(myMaxContact.contactType == EMBEDDED);
                ENSURE(myMaxContact.t == 1);
                ENSURE(myMaxContact.contactPoint == Point3<Number>(6, 2, 3));
                ENSURE(myMaxContact.contactSphereCenter == Point3<Number>(6, 2, 3));
                ENSURE(myMaxContact.contactSphereNormal == -myNormal); 
                ENSURE(myMaxContact.contactNormal == myNormal);
                DPRINT("=======");
                gridTest();
            }
        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        //addTest(new SweptSpherePointContact<float>("<float>));
        //addTest(new SweptSpherePointContact<double>("<double>));
        //addTest(new SweptSphereEdgeContact<float>("<float>));
        //addTest(new SweptSphereEdgeContact<double>("<double>));
        addTest(new SweptSphereTriangleContact<double>("<double>"));
    }
};

int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

	std::cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

