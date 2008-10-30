
spark.nameToIdMap = {};

spark.registerNode = function(theName, theNode) {
    if(theName in spark.nameToIdMap) {
        throw new Exception("Spark-registered node name used twice.");
    }
    spark.nameToIdMap[theName] = theNode.id;
};

spark.getNode = function(theName) {
    if(theName in spark.nameToIdMap) {
        return window.scene.dom.getElementById(spark.nameToIdMap[theName]);
    } else {
        return null;
    }
};

spark.cachedImages = {};

spark.getCachedImage = function(thePath) {
    var myName = "cached-image-" + thePath;
    var myImage = spark.getNode(myName);

    if(!myImage) {
        myImage = Modelling.createImage(window.scene, thePath);
        myImage.name = myName;

        spark.registerNode(myName, myImage);
    }

    return myImage;
};

spark.getCachedTexture = function(thePath) {
    var myName = "cached-texture-" + thePath;
    var myTexture = spark.getNode(myName);

    if(!myTexture) {
        var myImage = spark.getCachedImage(thePath);

        myTexture = Modelling.createTexture(window.scene, myImage);
        myTexture.name = myName;

        spark.registerNode(myName, myTexture);
    }

    return myTexture;
};
