
/*
 * Map of all component classes.
 */
spark.componentClasses = {};


/**
 * Define a component class.
 * 
 * Conceptually, this is a metaclass providing
 * component-class registration.
 * 
 * Should be called on the spark namespace like:
 *   spark.Fnord = spark.ComponentClass("Fnord");
 * 
 */
spark.ComponentClass = function(theName) {
    // generate the real constructor
    var myConstructor = Class.call(this, theName);

    // wrap the constructor with an initialization call
    var myInitializingConstructor = function(theNode) {
        myConstructor.call(this);
        if(theNode) {
            this.initialize(theNode);
        }
    };

    // register this class as a component class
    spark.componentClasses[theName] = myInitializingConstructor;

    // return the class so it can be put in the namespace
    return myInitializingConstructor;
};


/**
 * Define a templated component class.
 * 
 * Conceptually, this is a metaclass providing
 * component template instantiation.
 * 
 * Should be called on the spark namespace like:
 *   spark.Fnord = spark.LoadedClass("Fnord", "Fnord.spark");
 * 
 */
spark.LoadedClass = function(theClassName, theFile) {
    // load template from file
    var myDocument = new Node();
    myDocument.parseFile(theFile);

    // get the template base class constructor
    var myTemplateNode = myDocument.firstChild;
    var myBaseClass = myTemplateNode.nodeName;
    var myBaseConstructor = spark.componentClasses[myBaseClass];
    
    // build a constructor wrapper
    var myInitializingConstructor = function(theCallNode) {
        // clone template so we can change it
        var myWovenNode = myTemplateNode.cloneNode(true);

        // merge attributes from the template call
        var myAttributes = theCallNode.attributes;
        for (var i = 0; i < myAttributes.length; i++) {
            var myAttribute = myAttributes[i];
            myWovenNode[myAttribute.nodeName] = myAttribute.nodeValue;
        }

        // call the base class constructor
        myBaseConstructor.call(this, myWovenNode);

        // instantiate children
        this.instantiateChildren(myWovenNode);
    };

    // register our new class
    spark.componentClasses[theClassName] = myInitializingConstructor;

    // return the thing so it can be put in the namespace
    return myInitializingConstructor;
};

