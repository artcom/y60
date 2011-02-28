use("SceneTester.js");

var ourShow = new SceneTester(arguments);

try {
    ourShow.setup();
    window.canvas.backgroundcolor = [1,1,1,1];
    window.scene.world.fogmode = "exp2";
    window.scene.world.fogcolor = [1,1,1,1];
    window.scene.world.fogrange = [0,1];
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
