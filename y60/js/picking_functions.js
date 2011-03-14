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

/*jslint white: true, plusplus: false, nomen: false*/
/*globals Exception, window, Point3f, distance, difference, Ray, intersection,
          LineSegment, Logger, Sphere, Scene, Vector3f, __FILE__, __LINE__,
          transformScreenToWorld, signedDistance, Matrix4f, nearestIntersection,
          transformClipToWorld, product*/

// TODO why is this a global function??
function getViewportAt(theScreenPixelX, theScreenPixelY, theCanvas) {
    if (!theCanvas) {
        var errormessage = "no canvas";
        throw new Exception(errormessage);
    }
    var myViewportCount = theCanvas.childNodesLength("viewport");
    for (var i = myViewportCount - 1; i >= 0; --i) {
        var myViewport = theCanvas.childNode("viewport", i);
        if (myViewport.top <= theScreenPixelY &&
            myViewport.height + myViewport.top >= theScreenPixelY &&
            myViewport.left <= theScreenPixelX &&
            myViewport.left + myViewport.width >= theScreenPixelX)
        {
            return myViewport;
        }
    }
    return null;
}

function Picking(theRenderWindow) {
    this.Constructor(this, theRenderWindow);
}

Picking.prototype.Constructor = function (obj, theRenderWindow) {
    var _myRenderWindow = theRenderWindow;

    /////////////////////
    // Private Methods //
    /////////////////////

    function getScreenPos(theScreenPixelX, theScreenPixelY) {
        var myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            throw new Exception("No viewport at screen coordinates");
        }
        return transformScreenToWorld(theScreenPixelX, theScreenPixelY, myViewport);
    }

    /*function getCameraPos() {
        return new Point3f(_myRenderWindow.camera.globalmatrix.getTranslation());
    }*/

    function getCameraToScreenRay(theScreenPixelX, theScreenPixelY, theCamera) {
        theCamera = theCamera || _myRenderWindow.camera;
        var myScreenPos = getScreenPos(theScreenPixelX, theScreenPixelY);
        var myCameraPos = new Point3f(theCamera.globalmatrix.getTranslation());
        return new Ray(myCameraPos, myScreenPos);
    }

    /*function getRayThroughScreen(theScreenPixelX, theScreenPixelY) {
        var myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            return null;
        }
        var myNearPlanePos = transformScreenToWorld(theScreenPixelX, theScreenPixelY,
                                                    myViewport, -1);
        var myFarPlanePos = transformScreenToWorld(theScreenPixelX, theScreenPixelY,
                                                   myViewport, 1);
        var myViewRay = new Ray(myNearPlanePos, myFarPlanePos);
        return myViewRay;
    }*/

    ////////////////////
    // Public Methods //
    ////////////////////
    
    obj.getViewportAt = function (theScreenPixelX, theScreenPixelY, theCanvas) {
        theCanvas = theCanvas || _myRenderWindow.canvas;
        return getViewportAt(theScreenPixelX, theScreenPixelY, theCanvas);
    };

    /** Find intersection closest to the current camera.
     * An optional world space clipping plane can be specified
     */
    obj.pickPosition = function (theScreenPixelX, theScreenPixelY, theClippingPlane) {
        var myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            return null;
        }
        // XXX TODO: exchange window with _myRenderWindow
        var myCamera = window.scene.world.getElementById(myViewport.camera);
        var myClosestPosition = null;
        var myInfo = obj.pickIntersection(theScreenPixelX, theScreenPixelY);
        if (myInfo) {
            var myIntersections = myInfo.info.intersections;
            if (myIntersections.length > 0) {
                var myCameraPos = new Point3f(myCamera.globalmatrix.getTranslation());
                var myClosestDistance = null;
                for (var i = 0; i < myIntersections.length; ++i) {
                    var myDistance = distance(myIntersections[i].position, myCameraPos);

                    if (theClippingPlane) {
                        var myDistanceToPlane = signedDistance(myIntersections[i].position,
                                                               theClippingPlane);
                        if (myDistanceToPlane > 0) {
                            if (myClosestDistance === undefined || myDistance < myClosestDistance) {
                                myClosestDistance = myDistance;
                                myClosestPosition = myIntersections[i].position;
                            }
                        }
                    } else {
                        if (myClosestDistance === undefined || myDistance < myClosestDistance) {
                            myClosestDistance = myDistance;
                            myClosestPosition = myIntersections[i].position;
                        }
                    }
                }
            }
        }
        return myClosestPosition;
    };

    obj.pickBody = function (theScreenPixelX, theScreenPixelY, theWorld) {
        var myIntersection = obj.pickIntersection(theScreenPixelX, theScreenPixelY, theWorld);
        if (myIntersection) {
            return myIntersection.info.body;
        } else {
            return null;
        }
    };

    obj.pickBodyByWorldPos = function (theWorldPos, theCamera) {
        theCamera = theCamera || _myRenderWindow.camera;

        var myCameraPos   = new Point3f(theCamera.globalmatrix.getTranslation());
        var myRay         = new Ray(myCameraPos, difference(theWorldPos, myCameraPos));
        var myFarPos      = intersection(myRay, theCamera.frustum.far_plane);
        var myLineSegment = new LineSegment(myCameraPos, myFarPos);
        var myWorld       = _myRenderWindow.scene.dom.find("//world");

        var myIntersection = nearestIntersection(myWorld, myLineSegment);
        if (myIntersection) {
            return myIntersection.info.body;
        } else {
            return null;
        }
    };

    obj.pickPointOnPlane = function (theScreenX, theScreenY, thePlane) {
        var myRay = getCameraToScreenRay(theScreenX, theScreenY);
        return intersection(myRay, thePlane);
    };

    obj.pickBodyBySweepingSphereFromBodies = function (theScreenPixelX, theScreenPixelY,
                                                       theSphereRadius, theRootNode, theViewport) {
        var myDistance, mySphereCenter, jj;
        var myViewport = theViewport || obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            // we picked right into the Nothing
            Logger.trace("picked nothing");
            return null;
        }
        var myCamera = theRenderWindow.scene.world.getElementById(myViewport.camera);
        var myClosestBody = null;
        var myCollisions = obj.pickCollisionsBySweepingSphereFromBodies(theScreenPixelX,
                                                                        theScreenPixelY,
                                                                        theSphereRadius,
                                                                        theRootNode,
                                                                        myViewport);
        if (myCollisions && myCollisions.length > 0) {
            var myCameraPos =  new Point3f(myCamera.globalmatrix.getTranslation());
            var body = myCollisions[0].body;
            mySphereCenter = myCollisions[0].collisions[0].min.sphereCenter;
            var myClosestDistance = distance(myCameraPos, mySphereCenter);
            for (jj = 0; jj <  myCollisions[0].collisions.length; jj++) {
                mySphereCenter = myCollisions[0].collisions[jj].min.sphereCenter;
                myDistance = distance(myCameraPos, mySphereCenter);
                if (myDistance < myClosestDistance) {
                    myClosestDistance = myDistance;
                }
            }
            myClosestBody = body;
            for (var i = 1; i < myCollisions.length; ++i) {
                body = myCollisions[i].body;
                for (jj = 0; jj <  myCollisions[i].collisions.length; jj++) {
                    mySphereCenter =  myCollisions[i].collisions[jj].min.sphereCenter;
                    myDistance =  distance(myCameraPos, mySphereCenter);
                    if (myDistance < myClosestDistance) {
                        myClosestDistance = myDistance;
                        myClosestBody = body;
                    }
                }
            }
        }
        return myClosestBody;
    };

    obj.transformWorldToClip = function (theWorldPos, theCamera) {
        theCamera = theCamera || _myRenderWindow.camera;
        var myProjectionMatrix = new Matrix4f(theCamera.globalmatrix);
        myProjectionMatrix.invert();
        myProjectionMatrix.postMultiply(theCamera.frustum.projectionmatrix);
        return product(theWorldPos, myProjectionMatrix);
    };

    obj.transformClipToWorld = function (theClipPos, theScreenPixelX, theScreenPixelY) {
        var myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            Logger.error("No viewport for screen coordinates " + theScreenPixelX +
                         "," + theScreenPixelY + " at " + __FILE__ + ":" + __LINE__);
            throw new Exception("No viewport for screen coordinates " +
                                theScreenPixelX + "," + theScreenPixelY);
        }
        return transformClipToWorld(theClipPos, myViewport.getElementById(myViewport.camera));
    };

    obj.getLineSegmentThroughScreen = function (theScreenPixelX, theScreenPixelY) {
        var myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            return null;
        }
        var myNearPlanePos = transformScreenToWorld(theScreenPixelX,
                                                    theScreenPixelY,
                                                    myViewport, -1);
        var myFarPlanePos = transformScreenToWorld(theScreenPixelX,
                                                   theScreenPixelY,
                                                   myViewport, 1);
        var myViewLineSegment = new LineSegment(myNearPlanePos, myFarPlanePos);
        return myViewLineSegment;
    };

    obj.pickIntersection = function (theScreenPixelX, theScreenPixelY, theWorld) {
        var myLineSegment = obj.getLineSegmentThroughScreen(theScreenPixelX, theScreenPixelY);
        if (myLineSegment === null) {
            return null;
        }
        //XXX.. this should not look just for first world in scene.. what about multiple worlds????
        var myWorld = theWorld || _myRenderWindow.scene.dom.find("worlds/world");
        return nearestIntersection(myWorld, myLineSegment);
    };

    obj.pickCollisionsBySweepingSphereFromBodies = function (theScreenPixelX, theScreenPixelY,
                                                             theSphereRadius, theRootNode,
                                                             theViewport) {
        var myViewport = myViewport || obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            // we picked right into the Nothing
            return null;
        }
        // collide the sphere with all bodies below the root node
        var myNearPlanePos = transformScreenToWorld(theScreenPixelX,
                                                    theScreenPixelY, myViewport, -1);
        var myFarPlanePos = transformScreenToWorld(theScreenPixelX,
                                                   theScreenPixelY, myViewport, 1);
        Logger.trace("Near plane position (in WorldCoordinates): " + myNearPlanePos);
        Logger.trace("Far plane position (in WorldCoordinates): " + myFarPlanePos);
        var mySphere     = new Sphere(myNearPlanePos, theSphereRadius);
        var myMotion     = new Vector3f(difference(myFarPlanePos, myNearPlanePos));
        var myCollisions = Scene.collideWithBodies(theRootNode, mySphere, myMotion);
        return myCollisions;
    };

};
