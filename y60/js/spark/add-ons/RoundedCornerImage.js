//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/*jslint */
/*globals spark*/

spark.RoundedCornerImage = spark.ComponentClass("RoundedCornerImage");
spark.RoundedCornerImage.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.Image);
    Public.Inherit(spark.RoundedCornerQuad);
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        Public.update();
    };

    Public.SetterOverride("image", function (theImage, theBaseSetter) {
        theBaseSetter(theImage);
        Public.update();
    });
};
