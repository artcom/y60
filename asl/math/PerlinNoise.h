//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: PerlinNoise.h,v $
//
//     $Author: david $
//
//   $Revision: 1.6 $
//
// Description: simple 2d perlin noise with cosine interpolation
//              TODO: - 1d/3d variants
//                    - refactor to allow user specified seeds
//=============================================================================
 
#ifndef ASL_MATH_PERLIN_NOISE_INCLUDED
#define ASL_MATH_PERLIN_NOISE_INCLUDED

#include "Vector234.h"
#include "numeric_functions.h"

#include <vector>

namespace asl {

/*! @addtogroup aslmath */
/* @{ */

typedef std::vector<asl::Vector3i> NoiseSeed;

class PrimePool {
    public:
        static void createNoiseSeed(unsigned theOctaveCount, NoiseSeed & theSeed);

    private:
        PrimePool();
        static void createPrimeNumberList(int theStart, int theEnd, std::vector<int> & theList);
        static int getRandomElement(const std::vector<int> & theList);

        static std::vector<int> _myPool1;
        static std::vector<int> _myPool2;
        static std::vector<int> _myPool3;
};

class PerlinNoise2D {
    public:
        PerlinNoise2D(float thePersistence);
        PerlinNoise2D(const NoiseSeed & theSeed, float thePersistence);
        unsigned getOctaveCount() const;

        void setSeed(const NoiseSeed & theSeed);
        const NoiseSeed & getSeed() const;

        void setPersistence(const float & thePersistence);
        const float & getPersistence() const;

        float operator()(float x, float y) const;
        float operator()(const asl::Vector2f & thePosition) const;

    private:
        PerlinNoise2D();

        float interpolatedNoise(float x, float y, unsigned theOctave) const;
        float smoothedNoise(int theX, int theY, unsigned theOctave) const;
        float noise(int theX, int theY, unsigned theOctaveCount) const;

        float interpolate(float a, float b, float theGamma) const;

        NoiseSeed _mySeeds;
        float     _myPersistence;
};

/* @} */

} // end of namespace asl
#endif // ASL_MATH_PERLIN_NOISE_INCLUDED

