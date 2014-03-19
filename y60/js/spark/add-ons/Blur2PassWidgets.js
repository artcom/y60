//=============================================================================
// Copyright (C) 2011, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

spark.Blur2PassPlug = spark.AbstractClass("Blur2PassPlug");
spark.Blur2PassPlug.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    var _ = {};
    _.material = null;

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        _.material = Protected.material;
        _.material.enabled = false;
        addMaterialRequirement(_.material, "effect", "[10[2passBlur]]");
        _.material.enabled = true;
        
    };
    Public.__defineSetter__("blurpass", function(theValue) { 
        _.material.properties.blurpass = theValue;
    });
    Public.__defineSetter__("blur", function(theValue) { 
        _.material.properties.blur = theValue;
    });
    Public.__defineGetter__("blur", function() { 
        return _.material.properties.blur;
    });
};

spark.Canvas2PassBlur = spark.ComponentClass("Canvas2PassBlur");
spark.Canvas2PassBlur.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    Public.Inherit(spark.Canvas);
    Public.Inherit(spark.Blur2PassPlug);
    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        print("relaze : " + Public.name + "parent : " + Public.parent.name)
        Public.SetterOverride("width", function (theWidth, theBaseSetter) {
            theBaseSetter(theWidth);
            print("another setter width " + theWidth)
            Protected.material.properties.width = 1.0/theWidth;
        });
        Public.SetterOverride("height", function (theHeight, theBaseSetter) {
            theBaseSetter(theHeight);
            Protected.material.properties.height = 1.0/theHeight;
        });
        Public.size = Public.size;
        Public.blurpass = Protected.getNumber("blurpass", 0)
    };

};

spark.Canvas2PassPlug = spark.AbstractClass("Canvas2PassPlug");
spark.Canvas2PassPlug.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    var Private = {};
    Private.canvas_0 = null;
    Private.canvas_1 = null;

    const ATTRIB_BLACKLIST = {"scaleX":"", "scaleY":"", "scaleZ":"","rotationX":"", "rotationY":"", "rotationZ":"","x":"", "y":"", "z":""};
    Base.initialize = Public.initialize;
    Public.initialize = function (theNode, theWidgetNodeStr) {

        var myCanvasedWidgetStr = "<Canvas2PassBlur name='" + theNode.name + "_canvas_renderpass_2' blurpass='1' blur='0'>";
        var myWidgetNodeStr = "<" + theWidgetNodeStr + " ";
        for (var i = 0; i < theNode.attributes.length; i++) {
            var myAtt = theNode.attributes[i];
            if (! (ATTRIB_BLACKLIST.hasOwnProperty(myAtt.nodeName))) {
                myWidgetNodeStr += myAtt.nodeName + "='" + myAtt.nodeValue + "' ";
            }
        }
        myWidgetNodeStr += "/>"
        var myWidget = new Node(myWidgetNodeStr).firstChild;
        //myWidget.x = 0; myWidget.y = 0;myWidget.z = 0;
        myCanvasedWidgetStr += myWidget;
        myCanvasedWidgetStr += "</Canvas2PassBlur>";
        theNode.appendChild(new Node(myCanvasedWidgetStr).firstChild);

         Base.initialize(theNode);

    }

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        Private.canvas_0 = Public;
        Private.canvas_1 = Public.children[0];
        Public.renderwidget = Private.canvas_1.children[0];

        //Public.width = 1920;//Private.renderwidget.width;
        //Public.height = 350;//Private.renderwidget.height;

        //Private.canvas_1.width = 800;//Private.canvas_0.width;
        //Private.canvas_1.height = 350;//Private.canvas_0.height;
        Public.blur = Protected.getNumber("blur" , 0)    

    };
    Public.SetterOverride("blur", applyBlur);
    function applyBlur(theBlur, theBaseSetter) {
        theBaseSetter(theBlur);
        Private.canvas_1.blur = theBlur;
    }
    Public.__defineGetter__("widget", function () {
        return Public.renderwidget;
    });

};

spark.Movie2PassBlur = spark.ComponentClass("Movie2PassBlur");
spark.Movie2PassBlur.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.Canvas2PassBlur);
    Public.Inherit(spark.Canvas2PassPlug);    

    Base.initialize = Public.initialize;
    Public.initialize = function (theNode) {
        Base.initialize(theNode, "Movie");
    };
};

spark.Image2PassBlur = spark.ComponentClass("Image2PassBlur");
spark.Image2PassBlur.Constructor = function (Protected) {
    var Base = {};
    var Public = this;


    Public.Inherit(spark.Canvas2PassBlur);
    Public.Inherit(spark.Canvas2PassPlug);    

    Base.initialize = Public.initialize;
    Public.initialize = function (theNode) {
        Base.initialize(theNode, "Image");
    };
};



