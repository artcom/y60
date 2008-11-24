
/**
 * Component base class.
 * 
 * Components are XML-initializable objects organized
 * in a hierarchy. They can be graphical or not.
 */

spark.sceneNodeMap = {};
spark.ourComponentsByNameMap = {};
spark.onClickListeners = {};
spark.onMoveListeners = {};
spark.onReleaseListeners = {};

spark.Component = spark.AbstractClass("Component");

function unpenner() {
   var theEasingFunction = arguments[0];
   var theEasingArguments = Array.prototype.slice.call(arguments, 1);

   var myArguments = [theEasingFunction, 0.0, 1.0, 1.0];
   myArguments = myArguments.concat(theEasingArguments);

   return rcurry.apply(this, myArguments);
} 

spark.Component.Constructor = function(Protected) {
    var Public = this;

    var _myName = null;
    
    Public.name getter = function() {
        return _myName;
    };

    // XXX: really bad solution.
    //      this should be higher up and more informative.
    Public.toString = function (){
        return _myName;
    };
    
    var _myParent = null;

    Public.parent getter = function() {
        return _myParent;
    };

    Public.parent setter = function(theParent) {
        _myParent = theParent;
    };

    
    var _myNode = null;
    
    Public.node getter = function() {
        return _myNode;
    };


    Public.initialize = function(theNode) {
        _myNode   = theNode;
        _myName   = Protected.getString("name", "");
    };

    // XXX: realize and post-realize should be one thing,
    //      with the remaining coupled properties uncoupled.
    Public.realize = function() {
        spark.ourComponentsByNameMap[Public.name] = Public;
        Logger.info("Realizing component named " + Public.name);
    };
    Public.postRealize = function() {
        Logger.info("Post-Realizing component named " + Public.name);
    };

    
    Public.instantiateChildren = function() {
    };

    
    Protected.getString = function(theName, theDefault) {
        if(theName in _myNode) {
            return _myNode[theName];
        } else {
            if(theDefault == null) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };
    
    // XXX: this is a very very ugly hack
    Public.acquisition = function(theComponentTypeName, theItemId) {
        return (_myNode.nodeName == theComponentTypeName &&  Public.node.getElementById(theItemId)) ? _myNode:null;
    }

    // XXX: I18N should be implemented with a property type
    Protected.realizeI18N = function(theItem, theAttribute) {
        var myConcreteItem = theItem;
        var myId = "";
        if (theAttribute == undefined) {
            myId = Protected.getString("multilanguageId", "");         
        } else {
            myId = Protected.getString(theAttribute, "");         
        }        
        if (myId != "") {
            // find multilanguage item
            var myI18N = Public.acquisition("I18N", myId);
            if (myI18N) {
                myConcreteItem = myI18N.getActiveItem(myId);
                if (myConcreteItem) {
                    myI18N.registerListener(Public);
                }
            }
        }
        return myConcreteItem;
    }
    
    Protected.getBoolean = function(theName, theDefault) {
        if(theName in _myNode) {
            return (_myNode[theName] == "true");
        } else {
            if(theDefault == null) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };
    
    Protected.getVector3f = function(theName, theDefault) {
        var myArray = Protected.getArray(theName, theDefault);
        if (myArray.length == 3) {
            return new Vector3f(myArray[0], myArray[1], myArray[2]);
        } else {
            return theDefault;
        }
    };

    Protected.getNumber = function(theName, theDefault) {
        if(theName in _myNode) {
            return Number(_myNode[theName]);
        } else {
            if(theDefault == null) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };
    
    // XXX: this requires a member type. else it don't make no sense.
    Protected.getArray = function(theName, theDefault) {
        if(theName in _myNode) {
            return _myNode[theName].substring(1, _myNode[theName].length -1).replace(/ /g,"").split(",");
        } else {
            if(theDefault == null) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };

};


/**
 * Containers, obviously, contain other components.
 * 
 * This class is abstract because there can be different
 * kinds of containers, mostly distinguishing between
 * graphical and non-graphical ones.
 */

spark.Container = spark.AbstractClass("Container");

spark.Container.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Component);

    var _myChildArray = [];
    var _myChildMap   = {};

    Public.children getter = function() {
        return _myChildArray; // XXX: clone?
    };

    Public.addChild = function(theChild) {
        _myChildArray.push(theChild);
        Public.node.appendChild(theChild.node);
        if(theChild.name) {
            _myChildMap[theChild.name] = theChild;
        }
        theChild.parent = Public;
    };

    // XXX: why was this introduced?
    Public.getChildAt = function(theIndex) {
        return _myChildArray[theIndex];
    };

    Public.getChildByName = function(theName) {
        if (theName in _myChildMap) {
            return _myChildMap[theName];
        } else {
            return null;
        }
    };

    // XXX: very very dirty hack
    Public.acquisition = function(theComponentTypeName, theItemId) {
        if (Public.node.nodeName == theComponentTypeName &&  
            (theItemId==undefined || Public.node.getElementById(theItemId))) {
            return Public.node;
        } 
        for(var i = 0; i < _myChildArray.length; i++) {
           if (_myChildArray[i].node.nodeName == theComponentTypeName  &&  
               (theItemId==undefined || _myChildArray[i].node.getElementById(theItemId))) {
                return _myChildArray[i];
            }       
        }
        if (Public.parent) {
            return Public.parent.acquisition(theComponentTypeName, theItemId);
        } else {
            return null;
        }
    }

    // XXX: rework realization
    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
        for(var i = 0; i < _myChildArray.length; i++) {
            _myChildArray[i].realize();
        }
    };    
    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();
        for(var i = 0; i < _myChildArray.length; i++) {
            _myChildArray[i].postRealize();
        }
    };

    Public.instantiateChildren = function(theNode) {
        for(var i = 0; i < theNode.childNodesLength(); i++) {
            var myChildNode = theNode.childNode(i);
            spark.instantiateRecursively(myChildNode, Public);
        }
    };
};


