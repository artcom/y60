use("SceneTester.js");

var ourShow = new SceneTester(arguments);

try {
    ourShow.setup();
    window.camera.frustum.type = ProjectionType.orthonormal;
    window.camera.frustum.width = 2.0;
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
