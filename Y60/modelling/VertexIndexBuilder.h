//============================================================================
// Copyright (C) 2000-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//         $Id: VertexIndexBuilder.h,v 1.9 2005/01/12 17:22:01 danielk Exp $
//    $RCSfile: VertexIndexBuilder.h,v $
//     $Author: danielk $
//   $Revision: 1.9 $
//       $Date: 2005/01/12 17:22:01 $
//
//=============================================================================

#ifndef ac_y60_VertexIndexBuilder_included
#define ac_y60_VertexIndexBuilder_included

#include <y60/scene/ElementBuilder.h>
//#include <string>

// TODO:
// Add TriangleBuilder, TriangleStripBuilder, LineBuilder, etc.

namespace y60 {

    struct PointBuilder {
        std::string getType() const {
            return PrimitiveTypeStrings[POINTS];
        }

        void createIndices(ElementBuilder & theElementBuilder, unsigned theXVertexCount,
            unsigned theYVertexCount, unsigned theOffset = 0) const
        {
            if (theXVertexCount < 1 || theYVertexCount < 1) {
                throw asl::Exception("Need at least 1 vertex in X and Y direction", PLUS_FILE_LINE);
            }
            //const unsigned myXSubdivision = theXVertexCount - 1;
            const unsigned myYSubdivision = theYVertexCount - 1;
            for (unsigned y = 0; y < theYVertexCount; ++y) {
                for (unsigned x = 0; x < theXVertexCount; ++x) {
                    const unsigned myBaseIndex = theOffset + x * theYVertexCount + y;
                    const unsigned myTexBaseIndex = (myYSubdivision - y) * theXVertexCount + x;
                    theElementBuilder.appendIndex(POSITIONS, myBaseIndex);
                    theElementBuilder.appendIndex(NORMALS, myBaseIndex);
                    theElementBuilder.appendIndex(COLORS, myBaseIndex);                                        
                    theElementBuilder.appendIndex(getTextureRole(0), myTexBaseIndex);
                }
            }
        }
    };
    
    struct QuadBuilder {
        std::string getType() const {
            return PrimitiveTypeStrings[QUADS];
        }

        void createIndices(ElementBuilder & theElementBuilder, unsigned theXVertexCount,
                          unsigned theYVertexCount, unsigned theOffset = 0) const
        {
            if (theXVertexCount < 2 || theYVertexCount < 2) {
                throw asl::Exception("Need at least 2 vertices in X and Y direction", PLUS_FILE_LINE);
            }
            const unsigned myXSubdivision = theXVertexCount - 1;
            const unsigned myYSubdivision = theYVertexCount - 1;
            for (unsigned y = 0; y < myYSubdivision; ++y) {
                for (unsigned x = 0; x < myXSubdivision; ++x) {
                    const unsigned myBaseIndex = theOffset + y * theXVertexCount + x;
                    //const unsigned myBaseIndex = theOffset + y * myXSubdivision + x + y; // UH: this is the old version
                    const unsigned myTexBaseIndex = theOffset + (myYSubdivision - y) * theXVertexCount + x;
                    theElementBuilder.appendIndex(POSITIONS, myBaseIndex);
                    theElementBuilder.appendIndex(NORMALS, myBaseIndex);
                    theElementBuilder.appendIndex(COLORS, myBaseIndex);
                    theElementBuilder.appendIndex(getTextureRole(0), myTexBaseIndex);

                    theElementBuilder.appendIndex(POSITIONS, myBaseIndex + 1);
                    theElementBuilder.appendIndex(NORMALS, myBaseIndex + 1);
                    theElementBuilder.appendIndex(COLORS, myBaseIndex + 1);
                    theElementBuilder.appendIndex(getTextureRole(0), myTexBaseIndex + 1);

                    theElementBuilder.appendIndex(POSITIONS, myBaseIndex + theXVertexCount + 1);
                    theElementBuilder.appendIndex(NORMALS, myBaseIndex + theXVertexCount + 1);
                    theElementBuilder.appendIndex(COLORS, myBaseIndex + theXVertexCount + 1);
                    theElementBuilder.appendIndex(getTextureRole(0), myTexBaseIndex - theXVertexCount + 1);

                    theElementBuilder.appendIndex(POSITIONS, myBaseIndex + theXVertexCount);
                    theElementBuilder.appendIndex(NORMALS, myBaseIndex + theXVertexCount);
                    theElementBuilder.appendIndex(COLORS, myBaseIndex + theXVertexCount);
					theElementBuilder.appendIndex(getTextureRole(0), myTexBaseIndex - theXVertexCount);
                }
            }
        }
    };

    struct QuadStripBuilder {
        std::string getType() const {
            return PrimitiveTypeStrings[QUAD_STRIP];
        }

        void createIndices(ElementBuilder & theElementBuilder, unsigned theXVertexCount,
                           unsigned theYVertexCount, unsigned theOffset = 0) const
        {
            if (theXVertexCount < 2 || theYVertexCount < 2) {
                throw asl::Exception("Need at least 2 vertices in X and Y direction", PLUS_FILE_LINE);
            }
            const unsigned myYSubdivision = theYVertexCount - 1;
            for (unsigned y = 0; y < myYSubdivision; ++y) {
                for (unsigned x = 0; x < theXVertexCount; ++x) {
                    const unsigned myBaseIndex = theOffset + y * theXVertexCount + x;
                    const unsigned myTexBaseIndex = theOffset + (myYSubdivision - y) * theXVertexCount + x;
                    theElementBuilder.appendIndex(POSITIONS, myBaseIndex + theXVertexCount);
                    theElementBuilder.appendIndex(NORMALS, myBaseIndex + theXVertexCount);
                    theElementBuilder.appendIndex(COLORS, myBaseIndex + theXVertexCount);
                    theElementBuilder.appendIndex(getTextureRole(0), myTexBaseIndex - theXVertexCount);

                    theElementBuilder.appendIndex(POSITIONS, myBaseIndex);
                    theElementBuilder.appendIndex(NORMALS, myBaseIndex);
                    theElementBuilder.appendIndex(COLORS, myBaseIndex);
                    theElementBuilder.appendIndex(getTextureRole(0), myTexBaseIndex);
                }
            }
        }
    };
}

#endif
