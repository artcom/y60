//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

// requires core

/*
  Extensions to built-in **Vector2i** to **Vector4f**
    - magnitude
    - normalized()
*/

//////////////////////////////////////////////////////////////////////
// VectorXX methods and properties
//////////////////////////////////////////////////////////////////////

[Vector2f, Vector2i, Vector2d,
 Vector3f, Vector3i, Vector3d,
 Vector4f, Vector4i, Vector4d].forEach( function(objectType) {
    makeExtendable(objectType);
    
    objectType.getter('magnitude', function() {
        return magnitude(this);
    });
    
    objectType.method('normalized', function() {
        return normalized(this);
    });
    /*
    objectType.method('dot', function(theVector) {
        return dot(this,theVector);
    });
    */
});