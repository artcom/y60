/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
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
        PerlinNoise2D(const NoiseSeed & theSeeds, float thePersistence);
        unsigned getOctaveCount() const;

        void setSeed(const NoiseSeed & theSeeds);
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

class PerlinNoise3D {
    public:
        PerlinNoise3D(unsigned theOctaveCount, float theAmplitudeFalloff = 0.5);

        float noise(float f1) {
            return noise(f1, 0.0, 0.0);
        }

        float noise(float f1, float f2) {
            return noise(f1, f2, 0.0);
        }

        float noise(asl::Vector2f f) {
            return noise(f[0], f[1], 0.0);
        }

        float noise(asl::Vector3f f) {
            return noise(f[0], f[1], f[2]);
        }

        float noise(float f1, float f2, float f3);

        float getAmplitudeFalloff() const {
            return _myAmplitudeFalloff;
        }

        void setAmplitudeFalloff(float theFalloff) {
            if (_myAmplitudeFalloff > 0) {
                _myAmplitudeFalloff = theFalloff;
            }
        }

        unsigned getOctaveCount() const {
            return _myOctaveCount;
        }

        void setOctaveCount(unsigned theCount) {
            _myOctaveCount = theCount;
        }

    private:
        float noise_fsc(float f) const {
            return 0.5f * (1.0f - _myCosLUT[(int)(f * 180.0f) % 360]);
        }

        unsigned _myOctaveCount;
        float _myAmplitudeFalloff;
        float _mySinLUT[360];
        float _myCosLUT[360];
        float _myPerlin[4096];
};

/* @} */

} // end of namespace asl
#endif // ASL_MATH_PERLIN_NOISE_INCLUDED

