use("SceneTester.js");

var ourShow = new SceneTester(arguments);

try {
    ourShow.setup();
    var myMaterial = window.scene.materials.find("material[@name = 'lambert2']");
    var myTextureUnit = myMaterial.find(".//textureunit");
    var myTexture = myTextureUnit.getElementById(myTextureUnit.texture);
    myTexture.mag_filter="nearest";
    myTexture.min_filter="nearest";
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
