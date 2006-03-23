//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: TypeTraits.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: TypeTraits.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: This file contains all the wellknown names for the
//               SOM
//
//=============================================================================

#ifndef _ac_y60_TypeTraits_h_
#define _ac_y60_TypeTraits_h_


#include "NodeNames.h"
#include "AcBool.h"
#include <asl/Vector234.h>
#include <asl/Quaternion.h>
#include <asl/string_functions.h>
#include <string>
#include "DataTypes.h"

namespace y60 {

    //======================================================================
    // some type name traits
    //    you can use getTypeName<asl::Vector4f>() to get the som node name
    //    for the corresponding type 'vector4f'
    //
    //======================================================================

    enum TypeId { BOOL,
                  INT,
                  VECTOR2I,
                  VECTOR3I,
                  VECTOR4I,
                  VECTOR_OF_VECTOR2I,
                  FLOAT,
                  VECTOR2F,
                  VECTOR3F,
                  VECTOR4F,
                  VECTOR_OF_VECTOR2F,
                  VECTOR_OF_VECTOR3F,
                  VECTOR_OF_VECTOR4F,
                  STRING,
                  SAMPLER1D,
                  SAMPLER2D,
                  SAMPLERCUBE,
                  SAMPLER3D,
                  VECTOR_OF_STRING,
                  VECTOR_OF_RANKED_FEATURE,
                  TARGETBUFFERS
    }; // TODO: move me up when file format compatibility isn't an issue

    static const char * TypeIdStrings[] =
    {
        "bool",
        "int",
        "vector2i",
        "vector3i",
        "vector4i",
        "vectorofvector2i",
        "float",
        "vector2f",
        "vector3f",
        "vector4f",
        "vectorofvector2f",
        "vectorofvector3f",
        "vectorofvector4f",
        "string",
        "sampler1d",
        "sampler2d",
        "samplerCUBE",
        "sampler3d",  // TODO: move me up when file format compatibility isn't an issue
        "vectorofstring",
        "vectorofrankedfeature",
        "targetbuffers",
        0
    };

    template <class T> struct NO_SUCH_SPECIALIZATION;

    template <class T>
    struct TypeIdTraits {
        NO_SUCH_SPECIALIZATION<T> Kaputt;
    };

    template <>
    struct TypeIdTraits<float> {
        static const TypeId type_id() {
            return FLOAT;
        }
    };
    template <>
    struct TypeIdTraits<std::string> {
        static const TypeId type_id() {
            return STRING;
        }
    };
    template <>
    struct TypeIdTraits<asl::Vector2f> {
        static const TypeId type_id() {
            return VECTOR2F;
        }
    };
    template <>
    struct TypeIdTraits<asl::Vector3f> {
        static const TypeId type_id() {
            return VECTOR3F;
        }
    };
    template <>
    struct TypeIdTraits<asl::Vector4f> {
        static const TypeId type_id() {
            return VECTOR4F;
        }
    };
    template <>
    struct TypeIdTraits<VectorOfString> {
        static const TypeId type_id() {
            return VECTOR_OF_STRING;
        }
    };
    template <>
    struct TypeIdTraits<VectorOfRankedFeature> {
        static const TypeId type_id() {
            return VECTOR_OF_RANKED_FEATURE;
        }
    };
    template <>
    struct TypeIdTraits<TargetBuffers> {
        static const TypeId type_id() {
            return TARGETBUFFERS;
        }
    };

    template <class T>
    const char * getTypeName() {
        return asl::getStringFromEnum(TypeIdTraits<T>::type_id(), TypeIdStrings);
    }

    //======================================================================
    // som type name traits
    //    you can use VectorTypeNameTrait<Vector2f>::name() to get the
    //    som node name for the corresponding vector type 'vectorofvector2f'
    //
    //    Q: any idea how we can merge these with the size traits found in
    //       scene/Primitive.h ? [DS]
    //======================================================================

    template <class T>
    struct VectorTypeNameTrait {
        VectorTypeNameTrait() {
            NO_SUCH_SPECIALIZATION<T> CompileTimeException;
        }
    };

    template <>
    struct VectorTypeNameTrait<y60::AcBool> {
        static const char * name() {
            return SOM_VECTOR_BOOL_NAME;
        }
    };

    template <>
    struct VectorTypeNameTrait<float> {
        static const char * name() {
            return SOM_VECTOR_FLOAT_NAME;
        }
    };

    template <>
    struct VectorTypeNameTrait<unsigned> {
        static const char * name() {
            return SOM_VECTOR_UNSIGNED_NAME;
        }
    };

    template <>
    struct VectorTypeNameTrait<std::string> {
        static const char * name() {
            return SOM_VECTOR_STRING_NAME;
        }
    };

    template <>
    struct VectorTypeNameTrait<asl::Vector2f> {
        static const char * name() {
            return SOM_VECTOR_VECTOR2F_NAME;
        }
    };

    template <>
    struct VectorTypeNameTrait<asl::Vector3f> {
        static const char * name() {
            return SOM_VECTOR_VECTOR3F_NAME;
        }
    };

    template <>
    struct VectorTypeNameTrait<asl::Vector4f> {
        static const char * name() {
            return SOM_VECTOR_VECTOR4F_NAME;
        }
    };
    template <>
    struct VectorTypeNameTrait<asl::Quaternionf> {
        static const char * name() {
            return SOM_VECTOR_QUATERNIONF_NAME;
        }
    };
}

#endif
