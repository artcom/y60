
spark.setupCameraOrtho = function(theCamera, theWidth, theHeight) {
    theCamera.frustum = new Frustum();
    theCamera.frustum.width = theWidth;// * ourCameraZoomFactor;
    theCamera.frustum.height = theHeight;
    theCamera.frustum.type = ProjectionType.orthonormal;
    theCamera.position.x = theWidth/2;
    theCamera.position.y = theHeight/2;
    theCamera.position.z = 500;
    theCamera.orientation = Quaternionf.createFromEuler(new Vector3f(0,0,0));        
}
