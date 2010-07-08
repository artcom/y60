//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

spark.RoundedCornerMovie = spark.ComponentClass("RoundedCornerMovie");
spark.RoundedCornerMovie.Constructor = function(Protected) {
    
    var Base = {};
    var Public = this;
    
    this.Inherit(spark.RoundedCornerQuad);
    Base.realizeRoundedCornerQuad = Public.realize;
    Base.postRealizeRoundedCornerQuad = Public.postRealize;
    
    this.Inherit(spark.Movie);
    
    
    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
        Base.realizeRoundedCornerQuad();
    }
    
    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();
        Base.postRealizeRoundedCornerQuad();
    }
    
    
}
