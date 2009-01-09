/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "DataTypes.h"

#include "NodeValueNames.h"
#include "FrustumValue.h"

namespace y60 {

    void registerSomImageTypes(dom::ValueFactory & theFactory) {

        typedef dom::ValueWrapper<RasterOfRGB>::Type WrapperType;
        theFactory.registerPrototype("RasterOfRGB",
                dom::ValuePtr(new WrapperType(RasterOfRGB(0,0,0),0)));
        theFactory.registerPrototype("RasterOfRGBA",
                dom::ValuePtr(new dom::ComplexValue<RasterOfRGBA,dom::MakeResizeableRaster>(RasterOfRGBA(0,0,0),0)));
        theFactory.registerPrototype("RasterOfBGR",
                dom::ValuePtr(new dom::ComplexValue<RasterOfBGR,dom::MakeResizeableRaster>(RasterOfBGR(0,0,0),0)));
        theFactory.registerPrototype("RasterOfBGRA",
                dom::ValuePtr(new dom::ComplexValue<RasterOfBGRA,dom::MakeResizeableRaster>(RasterOfBGRA(0,0,0),0)));
        theFactory.registerPrototype("RasterOfABGR",
                dom::ValuePtr(new dom::ComplexValue<RasterOfABGR,dom::MakeResizeableRaster>(RasterOfABGR(0,0,0),0)));
        theFactory.registerPrototype("RasterOfGRAY",
                dom::ValuePtr(new dom::ComplexValue<RasterOfGRAY,dom::MakeResizeableRaster>(RasterOfGRAY(0,0,0),0)));
        theFactory.registerPrototype("RasterOfALPHA",
                dom::ValuePtr(new dom::ComplexValue<RasterOfGRAY,dom::MakeResizeableRaster>(RasterOfGRAY(0,0,0),0)));
        theFactory.registerPrototype("RasterOfDEPTH",
                dom::ValuePtr(new dom::ComplexValue<RasterOfGRAY,dom::MakeResizeableRaster>(RasterOfGRAY(0,0,0),0)));
        theFactory.registerPrototype("RasterOfGRAY16",
                dom::ValuePtr(new dom::ComplexValue<RasterOfGRAY16,dom::MakeResizeableRaster>(RasterOfGRAY16(0,0,0),0)));
        theFactory.registerPrototype("RasterOfGRAY32",
                dom::ValuePtr(new dom::ComplexValue<RasterOfGRAY32,dom::MakeResizeableRaster>(RasterOfGRAY32(0,0,0),0)));

        theFactory.registerPrototype("RasterOfGRAYS16",
                dom::ValuePtr(new dom::ComplexValue<RasterOfGRAYS16,dom::MakeResizeableRaster>(RasterOfGRAYS16(0,0,0),0)));

        theFactory.registerPrototype("RasterOfDXT1",
                dom::ValuePtr(new dom::ComplexValue<RasterOfDXT1,dom::MakeResizeableRaster>(RasterOfDXT1(0,0,0),0)));
        theFactory.registerPrototype("RasterOfDXT1a",
                dom::ValuePtr(new dom::ComplexValue<RasterOfDXT1a,dom::MakeResizeableRaster>(RasterOfDXT1a(0,0,0),0)));
        theFactory.registerPrototype("RasterOfDXT3",
                dom::ValuePtr(new dom::ComplexValue<RasterOfDXT3,dom::MakeResizeableRaster>(RasterOfDXT3(0,0,0),0)));
        theFactory.registerPrototype("RasterOfDXT5",
                dom::ValuePtr(new dom::ComplexValue<RasterOfDXT5,dom::MakeResizeableRaster>(RasterOfDXT5(0,0,0),0)));

        theFactory.registerPrototype("RasterOfRGBA_HALF",
                dom::ValuePtr(new dom::ComplexValue<RasterOfRGBA_HALF,dom::MakeResizeableRaster>(RasterOfRGBA_HALF(0,0,0),0)));
        theFactory.registerPrototype("RasterOfRGB_HALF",
                dom::ValuePtr(new dom::ComplexValue<RasterOfRGB_HALF,dom::MakeResizeableRaster>(RasterOfRGB_HALF(0,0,0),0)));
        theFactory.registerPrototype("RasterOfRGBA_FLOAT",
                dom::ValuePtr(new dom::ComplexValue<RasterOfRGBA_FLOAT,dom::MakeResizeableRaster>(RasterOfRGBA_FLOAT(0,0,0),0)));
        theFactory.registerPrototype("RasterOfRGB_FLOAT",
                dom::ValuePtr(new dom::ComplexValue<RasterOfRGB_FLOAT,dom::MakeResizeableRaster>(RasterOfRGB_FLOAT(0,0,0),0)));
    }

    void registerSomMathTypes(dom::ValueFactory & theFactory) {
        theFactory.registerPrototype("Vector2i", dom::ValuePtr(new dom::VectorValue<asl::Vector2i>(asl::Vector2i(0,0),0)));
        theFactory.registerPrototype("Vector2f", dom::ValuePtr(new dom::VectorValue<asl::Vector2f>(asl::Vector2f(0.0,0.0),0)));
        theFactory.registerPrototype("Vector2d", dom::ValuePtr(new dom::VectorValue<asl::Vector2d>(asl::Vector2d(0.0,0.0),0)));

        theFactory.registerPrototype("Vector3i", dom::ValuePtr(new dom::VectorValue<asl::Vector3i>(asl::Vector3i(0,0,0),0)));
        theFactory.registerPrototype("Vector3f", dom::ValuePtr(new dom::VectorValue<asl::Vector3f>(asl::Vector3f(0,0,0),0)));
        theFactory.registerPrototype("Vector3d", dom::ValuePtr(new dom::VectorValue<asl::Vector3d>(asl::Vector3d(0,0,0),0)));

        theFactory.registerPrototype("Vector4f", dom::ValuePtr(new dom::VectorValue<asl::Vector4f>(asl::Vector4f(0,0,0,0),0)));
        theFactory.registerPrototype("Vector4d", dom::ValuePtr(new dom::VectorValue<asl::Vector4d>(asl::Vector4d(0,0,0,0),0)));

        theFactory.registerPrototype("Point2i", dom::ValuePtr(new dom::VectorValue<asl::Point2i>(asl::Point2i(0,0),0)));
        theFactory.registerPrototype("Point2f", dom::ValuePtr(new dom::VectorValue<asl::Point2f>(asl::Point2f(0.0f,0.0f),0)));
        theFactory.registerPrototype("Point2d", dom::ValuePtr(new dom::VectorValue<asl::Point2d>(asl::Point2d(0.0,0.0),0)));

        theFactory.registerPrototype("Point3f", dom::ValuePtr(new dom::VectorValue<asl::Point3f>(asl::Point3f(0,0,0),0)));
        theFactory.registerPrototype("Point3d", dom::ValuePtr(new dom::VectorValue<asl::Point3d>(asl::Point3d(0,0,0),0)));

        theFactory.registerPrototype("Point4f", dom::ValuePtr(new dom::VectorValue<asl::Point4f>(asl::Point4f(0,0,0,0),0)));
        theFactory.registerPrototype("Point4d", dom::ValuePtr(new dom::VectorValue<asl::Point4d>(asl::Point4d(0,0,0,0),0)));

        theFactory.registerPrototype("Matrix4f", dom::ValuePtr(new dom::SimpleValue<asl::Matrix4f>(asl::Matrix4f(),0)));
        theFactory.registerPrototype("Matrix4d", dom::ValuePtr(new dom::SimpleValue<asl::Matrix4d>(asl::Matrix4d(),0)));

        theFactory.registerPrototype("Quaternionf", dom::ValuePtr(new dom::SimpleValue<asl::Quaternionf>(asl::Quaternionf(),0)));
        theFactory.registerPrototype("Frustum", dom::ValuePtr(new dom::FrustumValue(asl::Frustum(),0)));

        theFactory.registerPrototype("Planef", dom::ValuePtr(new dom::SimpleValue<asl::Planef>(asl::Planef(),0)));

        theFactory.registerPrototype("Box2i", dom::ValuePtr(new dom::SimpleValue<asl::Box2i>(asl::Box2i(),0)));
        theFactory.registerPrototype("Box2f", dom::ValuePtr(new dom::SimpleValue<asl::Box2f>(asl::Box2f(),0)));
        theFactory.registerPrototype("Box2d", dom::ValuePtr(new dom::SimpleValue<asl::Box2d>(asl::Box2d(),0)));

        theFactory.registerPrototype("Box3i", dom::ValuePtr(new dom::SimpleValue<asl::Box3i>(asl::Box3i(),0)));
        theFactory.registerPrototype("Box3f", dom::ValuePtr(new dom::SimpleValue<asl::Box3f>(asl::Box3f(),0)));
        theFactory.registerPrototype("Box3d", dom::ValuePtr(new dom::SimpleValue<asl::Box3d>(asl::Box3d(),0)));
    }

    void registerSomVectorTypes(dom::ValueFactory & theFactory) {
        theFactory.registerPrototype("VectorOfBool",
                dom::ValuePtr(new dom::VectorValue<VectorOfBool,dom::MakeResizeableVector>(VectorOfBool(),0)));

        theFactory.registerPrototype("VectorOfUnsignedInt",
                dom::ValuePtr(new dom::VectorValue<VectorOfUnsignedInt,dom::MakeResizeableVector>(VectorOfUnsignedInt(),0)));

        theFactory.registerPrototype("VectorOfSignedInt",
                dom::ValuePtr(new dom::VectorValue<VectorOfSignedInt,dom::MakeResizeableVector>(VectorOfSignedInt(),0)));

        theFactory.registerPrototype("VectorOfFloat",
                dom::ValuePtr(new dom::VectorValue<VectorOfFloat,dom::MakeResizeableVector>(VectorOfFloat(),0)));

        theFactory.registerPrototype("VectorOfVector2i",
                dom::ValuePtr(new dom::VectorValue<VectorOfVector2i,dom::MakeResizeableVector>(VectorOfVector2i(),0)));

        theFactory.registerPrototype("VectorOfVector2f",
                dom::ValuePtr(new dom::VectorValue<VectorOfVector2f,dom::MakeResizeableVector>(VectorOfVector2f(),0)));

        theFactory.registerPrototype("VectorOfVector3f",
                dom::ValuePtr(new dom::VectorValue<VectorOfVector3f,dom::MakeResizeableVector>(VectorOfVector3f(),0)));

        theFactory.registerPrototype("VectorOfVector4f",
                dom::ValuePtr(new dom::VectorValue<VectorOfVector4f,dom::MakeResizeableVector>(VectorOfVector4f(),0)));

        theFactory.registerPrototype("VectorOfQuaternionf",
                dom::ValuePtr(new dom::VectorValue<VectorOfQuaternionf,dom::MakeResizeableVector>(VectorOfQuaternionf(),0)));

        theFactory.registerPrototype("VectorOfString",
                dom::ValuePtr(new dom::ComplexValue<VectorOfString,dom::MakeResizeableVector,dom::StringValue>(VectorOfString(),0)));

        theFactory.registerPrototype("VectorOfVectorOfString",
                dom::ValuePtr(new dom::ComplexValue<VectorOfVectorOfString,dom::MakeResizeableVector>(VectorOfVectorOfString(),0)));

        theFactory.registerPrototype("VectorOfVectorOfFloat",
                dom::ValuePtr(new dom::ComplexValue<VectorOfVectorOfFloat,dom::MakeResizeableVector>(VectorOfVectorOfFloat(),0)));

        theFactory.registerPrototype("VectorOfRankedFeature",
                dom::ValuePtr(new dom::ComplexValue<VectorOfRankedFeature,dom::MakeResizeableVector>(VectorOfRankedFeature(),0)));
    }

    void registerEnumBitsetTypes(dom::ValueFactory & theFactory) {
        theFactory.registerPrototype("BlendEquation", dom::ValuePtr(new dom::SimpleValue<y60::BlendEquation>(EQUATION_ADD,0)));
        theFactory.registerPrototype("VectorOfBlendFunction",
                dom::ValuePtr(new dom::VectorValue<VectorOfBlendFunction,dom::MakeResizeableVector>(VectorOfBlendFunction(),0)));

        theFactory.registerPrototype("RenderStyle", dom::ValuePtr(new dom::SimpleValue<y60::RenderStyle>(FRONT,0)));
        theFactory.registerPrototype("RenderStyles", dom::ValuePtr(new dom::SimpleValue<y60::RenderStyles>(y60::RenderStyles(0),0)));
        theFactory.registerPrototype("TargetBuffers", dom::ValuePtr(new dom::SimpleValue<y60::TargetBuffers>(y60::TargetBuffers(0),0)));
        theFactory.registerPrototype("VertexBufferUsage", dom::ValuePtr(new dom::SimpleValue<y60::VertexBufferUsage>(y60::getDefaultVertexBufferUsage(),0)));

        theFactory.registerPrototype("TextureApplyMode", dom::ValuePtr(new dom::SimpleValue<y60::TextureApplyMode>(y60::TextureApplyMode(MODULATE),0)));
        theFactory.registerPrototype("TextureSampleFilter", dom::ValuePtr(new dom::SimpleValue<y60::TextureSampleFilter>(y60::TextureSampleFilter(LINEAR),0)));
        theFactory.registerPrototype("TextureWrapMode", dom::ValuePtr(new dom::SimpleValue<y60::TextureWrapMode>(y60::TextureWrapMode(REPEAT),0)));

        theFactory.registerPrototype("TextureType", dom::ValuePtr(new dom::SimpleValue<y60::TextureType>(y60::TextureType(TEXTURE_2D),0)));        
        theFactory.registerPrototype("TextureUsage", dom::ValuePtr(new dom::SimpleValue<y60::TextureUsage>(y60::TextureUsage(PAINT),0)));        
        theFactory.registerPrototype("ResizePolicy", dom::ValuePtr(new dom::SimpleValue<asl::ResizePolicy>(asl::ResizePolicy(asl::ADAPT_VERTICAL),0)));        

        theFactory.registerPrototype("ImageType", dom::ValuePtr(new dom::SimpleValue<y60::ImageType>(y60::ImageType(SINGLE),0)));        
        theFactory.registerPrototype("PrimitiveType", dom::ValuePtr(new dom::SimpleValue<y60::PrimitiveType>(y60::PrimitiveType(POINTS),0)));        
    }

    void registerSomTypes(dom::ValueFactory & theFactory) {
        registerSomMathTypes(theFactory);
        registerSomVectorTypes(theFactory);
        registerSomImageTypes(theFactory);
        registerEnumBitsetTypes(theFactory);        
    }
}
