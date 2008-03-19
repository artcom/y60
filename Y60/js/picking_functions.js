//=============================================================================
// Copyright (C) 2003-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function getViewportAt(theScreenPixelX, theScreenPixelY, theCanvas) {
    if (!theCanvas) {
        var errormessage = "no canvas";
        throw new Exception(errormessage);
    }
    var myViewportCount = theCanvas.childNodesLength("viewport");
    for (var i= myViewportCount-1; i >= 0; --i) {
        var myViewport = theCanvas.childNode("viewport",i);
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

    /*******************************************************/
    /**    Public Methods                                 **/
    /*******************************************************/
    obj.getViewportAt = function(theScreenPixelX, theScreenPixelY, theCanvas) {
        if (!theCanvas) {
            theCanvas = _myRenderWindow.canvas;
        }
        return getViewportAt(theScreenPixelX, theScreenPixelY, theCanvas);
    }


    /** Find intersection closest to the current camera.
     * An optional world space clipping plane can be specified
     */
    obj.pickPosition = function(theScreenPixelX, theScreenPixelY, theClippingPlane) {
        var myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            return null;
        }
        var myCamera = window.scene.world.getElementById(myViewport.camera);
        var myClosestPosition = null;
        var myInfo = obj.pickIntersection(theScreenPixelX, theScreenPixelY);
        if (myInfo) {
            var myIntersections = myInfo.info.intersections;
            if (myIntersections.length > 0) {
                var myCameraPos = new Point3f(myCamera.globalmatrix.getTranslation());
                var myClosestDistance = null; 
                for (var i = 0; i < myIntersections.length; ++i) {
                    var myDistance = distance(myIntersections[i].position,myCameraPos);

                    if ( theClippingPlane ) {
                        var myDistanceToPlane = signedDistance(myIntersections[i].position, 
                                                               theClippingPlane);
                        if (myDistanceToPlane > 0) {
                            if ( myClosestDistance == undefined 
                                 || myDistance < myClosestDistance) 
                            {
                                myClosestDistance = myDistance;
                                myClosestPosition = myIntersections[i].position;
                            }
                        }
                    } else {
                        if ( myClosestDistance == undefined || myDistance < myClosestDistance) {
                            myClosestDistance = myDistance;
                            myClosestPosition = myIntersections[i].position;
                        }
                    }
                }
            }
        }
        return myClosestPosition;
    }


    obj.pickBody = function(theScreenPixelX, theScreenPixelY) {
        var myIntersection = obj.pickIntersection(theScreenPixelX, theScreenPixelY);
        if (myIntersection) {
            return myIntersection.info.body;
        } else {
            return null;
        }
    }
    
    
    
    obj.pickBodyByWorldPos = function(theWorldPos, theCamera) {
        if (theCamera == undefined) {
            theCamera = _RenderWindow.camera;
        }
        var myCameraPos = new Point3f(theCamera.globalmatrix.getTranslation());
        var myRay = new Ray(myCameraPos, difference( theWorldPos, myCameraPos) );
        var myFarPos = intersection( myRay, theCamera.frustum.far_plane );
        var myLineSegment = new LineSegment(myCameraPos, myFarPos);
        var myWorld = getDescendantByTagName(_myRenderWindow.scene.dom, "world", true);
        
        var myIntersection = nearestIntersection(myWorld, myLineSegment);
        if (myIntersection) {
            return myIntersection.info.body;
        } else {
            return null;
        }
    }



    obj.pickPointOnPlane = function( theScreenX, theScreenY, thePlane) {
        var myRay = getCameraToScreenRay( theScreenX, theScreenY);
        return intersection( myRay, thePlane );
    }



    obj.pickBodyBySweepingSphereFromBodies = function(theScreenPixelX, theScreenPixelY, 
                                                      theSphereRadius, theRootNode, theViewport)
    {
        var myViewport = null;
        if (theViewport != undefined) {
            myViewport = theViewport;
        } else {
            myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);            
        }
        if (!myViewport) {
            // we picked right into the Nothing
            print("picked nothing");
            return null;
        }
        var myCamera = theRenderWindow.scene.world.getElementById(myViewport.camera);
        var myClosestBody = null;
        var myCollisions = obj.pickCollisionsBySweepingSphereFromBodies(theScreenPixelX, 
                                                                        theScreenPixelY, 
                                                                        theSphereRadius, 
                                                                        theRootNode,
                                                                        myViewport);
        if (myCollisions) {
            if (myCollisions.length > 0) {
                var myCameraPos =  new Point3f(myCamera.globalmatrix.getTranslation());
                var body = myCollisions[0].body;
                var mySphereCenter = myCollisions[0].collisions[0].min.sphereCenter;
                var myClosestDistance = distance(myCameraPos, mySphereCenter);;
                for (var jj = 0; jj <  myCollisions[0].collisions.length; jj++) {
                    mySphereCenter = myCollisions[0].collisions[jj].min.sphereCenter;
                    var myDistance = distance(myCameraPos, mySphereCenter);
                    if (myDistance < myClosestDistance) {
                        myClosestDistance = myDistance;
                    }
                }
                var myClosestBody = body;
                for(var i=1;i<myCollisions.length;++i) {
                    body = myCollisions[i].body;
                    for (var jj = 0; jj <  myCollisions[i].collisions.length; jj++) {
                        var mySphereCenter =  myCollisions[i].collisions[jj].min.sphereCenter;
                        var myDistance =  distance(myCameraPos, mySphereCenter);
                        if (myDistance < myClosestDistance) {
                            myClosestDistance = myDistance;
                            myClosestBody = body;
                        }
                    }
                }
            }
        }
        return myClosestBody;
    }
    

    obj.transformWorldToClip = function(theWorldPos, theCamera) {
        if (theCamera == undefined) {
            theCamera = _myRenderwindow.camera;
        }
        var myProjectionMatrix = new Matrix4f(theCamera.globalmatrix);
        myProjectionMatrix.invert();
        myProjectionMatrix.postMultiply(theCamera.frustum.projectionmatrix);
        return product(theWorldPos, myProjectionMatrix);
    }

    
    
    obj.
    transformClipToWorld = function(theClipPos,theScreenPixelX,theScreenPixelY) {
        var myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            Logger.error("No viewport for screen coordinates " + theScreenPixelX 
                         + ","+theScreenPixelY + " at " + __FILE__ + ":" + __LINE__);
            throw new Exception("No viewport for screen coordinates " 
                                + theScreenPixelX + ","+theScreenPixelY);
        }
        return transformClipToWorld(theClipPos,myViewport.getElementById(myViewport.camera));
    }

    /*******************************************************/
    /**    Private Methods                                **/
    /*******************************************************/

    function getScreenPos(theScreenPixelX, theScreenPixelY) {
	    var myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            throw new Exception("No viewport at screen coordinates");
        }
        return transformScreenToWorld(theScreenPixelX, theScreenPixelY, myViewport);
    }

    
    
    function getCameraPos() {
        return new Point3f(_myRenderWindow.camera.globalmatrix.getTranslation());
    }

    
    
    function getCameraToScreenRay(theScreenPixelX, theScreenPixelY, theCamera) {
        if (theCamera == undefined) {
            theCamera = _myRenderWindow.camera;
        }
        var myScreenPos = getScreenPos(theScreenPixelX, theScreenPixelY);
        var myCameraPos = new Point3f(theCamera.globalmatrix.getTranslation());
        return new Ray(myCameraPos, myScreenPos);
    }

    
    
    function getRayThroughScreen(theScreenPixelX, theScreenPixelY) {
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
    }

    
    
    obj.getLineSegmentThroughScreen = function(theScreenPixelX, theScreenPixelY) {
        var myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        if (!myViewport) {
            return null;
        }
        var myNearPlanePos = transformScreenToWorld(theScreenPixelX, theScreenPixelY, 
                                                    myViewport, -1);
        var myFarPlanePos = transformScreenToWorld(theScreenPixelX, theScreenPixelY, 
                                                   myViewport, 1);
        var myViewLineSegment = new LineSegment(myNearPlanePos, myFarPlanePos);
        return myViewLineSegment;
    }

    
    
    obj.pickIntersection = function(theScreenPixelX, theScreenPixelY) {
        var myLineSegment = obj.getLineSegmentThroughScreen(theScreenPixelX, theScreenPixelY);
        //var myWorld = getDescendantByTagName(_myRenderWindow.scene.dom, "world", true);
        var myWorld = _myRenderWindow.scene.dom.find("worlds/world");
        return nearestIntersection(myWorld, myLineSegment);
    }

    
    
    obj.pickCollisionsBySweepingSphereFromBodies = function(theScreenPixelX, theScreenPixelY, 
                                                            theSphereRadius, theRootNode,
                                                            theViewport)
    {
        var myViewport = null;
        if (theViewport != undefined) {
            myViewport = theViewport;
        } else {
            myViewport = obj.getViewportAt(theScreenPixelX, theScreenPixelY);
        }
        
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
        var mySphere = new Sphere(myNearPlanePos, theSphereRadius);
        var myMotion = new Vector3f(difference(myFarPlanePos, myNearPlanePos));
        var myCollisions = Scene.collideWithBodies(theRootNode, mySphere, myMotion);
        return myCollisions;
    }

}
