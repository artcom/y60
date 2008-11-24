
/**
 * Load an xml string into the spark world.
 */
spark.loadString = function(theString, theParent) {
    var myNode = new Node(theString);
    return spark.loadDocument(myNode, theParent);
};

/**
 * Load an xml file into the spark world.
 */
spark.loadFile = function(theFile, theParent) {
    var myNode = new Node();
    myNode.parseFile(theFile);
    return spark.loadDocument(myNode, theParent);
};

/**
 * Load a dom tree into the spark world.
 * 
 * Note that this will keep references to the DOM tree
 * in the constructed components. If you wan't referential
 * independence, clone your dom first.
 */
spark.loadDocument = function(theNode, theParent) {
    var myRoot = theNode.firstChild;
    var myComponent = spark.instantiateRecursively(myRoot, theParent);
    myComponent.realize();
    myComponent.postRealize();
    return myComponent;
};


/**
 * Internal: instantiation engine.
 */
spark.instantiateRecursively = function(theNode, theParent) {

    if (!theNode){
        Logger.error("no node given.");
        exit(1);
    }

    // shortcut for defining template classes
    if(theNode.nodeName == "Template") {
        var myFile = theNode.src;
        var myName = theNode.name;
        Logger.info("Loading template " + myName + " from file " + myFile);
        spark[myName] = spark.LoadedClass(myName, myFile);
        return null;
    }

    if(theNode.nodeName == "#comment") {
        return null;
    }

    var myName = theNode.nodeName;
    print(myName);

    if (! (myName in spark.componentClasses)){
        Logger.warning("Skipping node '" +  myName + 
                     "'. No matching component found.");
        return null;
    }

    // instantiate the component
    var myInstance = spark.instantiateComponent(theNode);
    
    // add to parent if we have one
    if(theParent) {
        theParent.addChild(myInstance);
    }
    
    // trigger child instantiation / recurse
    myInstance.instantiateChildren(theNode);
    
    // break recursion
    return myInstance;
};

/**
 * Internal: instantiate a component
 */
spark.instantiateComponent = function(theNode) {
    Logger.info("Instantiating " + theNode.nodeName + ("name" in theNode ? " named " + theNode.name : ""));
    
    var myClassName = theNode.nodeName;
    var myNode = theNode;
    var myConstructor = spark.componentClasses[myClassName];
    return new myConstructor(myNode);
};

