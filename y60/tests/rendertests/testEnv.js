use("SceneTester.js");

var ourShow = new SceneTester(arguments);

try {
    ourShow.setup();

    ourShow.saveLoadFlag = false;

    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
