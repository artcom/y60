spark.MultiQuadText = spark.ComponentClass("MultiQuadText");

spark.MultiQuadText.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.Text);

    /////////////////////
    // Private Methods //
    /////////////////////    
    function fillShapeData() {
        var myGlyphInfo = Public.glyphPositions;

        var myPositions  = [];
        var myTexCoords  = [];
        
        var myCharCount = myGlyphInfo.length/2;
        for (var i = 0 ; i < myCharCount; i++) {
            var myGlyph_start = myGlyphInfo[(i*2)];
            var myGlyph_end = myGlyphInfo[(i*2)+1];

            // set new vertex positions
            var myX1 = myGlyph_start[0];
            var myY1 = myGlyph_start[1];
            var myX2 = myGlyph_end[0]
            var myY2 = myGlyph_end[1];

            myPositions.push(new Vector3f(myX1, myY2, 0));
            myPositions.push(new Vector3f(myX2, myY2, 0));
            myPositions.push(new Vector3f(myX2, myY1, 0));                      
            myPositions.push(new Vector3f(myX1, myY1, 0));

            // set new vertex uv coordinates
            myX1 =      myGlyph_start[0] / (Public.width);
            myY1 = 1.0-(myGlyph_start[1] / (Public.height));
            myX2 =      myGlyph_end[0] / (Public.width);
            myY2 = 1.0-(myGlyph_end[1] / (Public.height));

            myTexCoords.push(new Vector2f(myX1, myY2));
            myTexCoords.push(new Vector2f(myX2, myY2));
            myTexCoords.push(new Vector2f(myX2, myY1));
            myTexCoords.push(new Vector2f(myX1, myY1));
                                                
        }
        Modelling.EnsureShapesQuadCount(Protected.shape, myPositions, myTexCoords);
    }
    
    Base.render = Protected.render;
    Protected.render = function () {
        Base.render();
        if ( Protected.shape) {
            var myNeededQuadCount = Public.glyphPositions.length/2;
            if (myNeededQuadCount > 0) {
                fillShapeData();
            }
        }
    }

};


