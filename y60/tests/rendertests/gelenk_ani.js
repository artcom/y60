use("SceneTester.js");

try {
    var ourShow = new SceneTester(arguments);
    ourShow.setup();
    ourShow.setTestDurationInFrames(30);
    window.fixedFrameTime = 0.04;
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
