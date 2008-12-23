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
//    $RCSfile: PerlinNoise.cpp,v $
//
//     $Author: david $
//
//   $Revision: 1.4 $
//
// Description: simple 2d _myPerlin noise with cosine interpolation
//              TODO: - 1d/3d variants
//
//=============================================================================
 

#include "PerlinNoise.h"
#include "numeric_functions.h"

#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;
using namespace asl;

namespace asl {

std::vector<int> PrimePool::_myPool1;
std::vector<int> PrimePool::_myPool2;
std::vector<int> PrimePool::_myPool3;

void
PrimePool::createPrimeNumberList(int theStart, int theEnd, std::vector<int> & theList) {
    for (int i = theStart; i < theEnd; ++i) {
        if (isPrime(i)) {
            theList.push_back(i);
        }
    }
}

void
PrimePool::createNoiseSeed(unsigned theOctaves, NoiseSeed & theSeeds) {
    if (_myPool1.empty()) {
        createPrimeNumberList(10000, 20000, _myPool1);
        createPrimeNumberList(700000, 800000, _myPool2);
        createPrimeNumberList(1376310000, 1376320000, _myPool3);
    }
    
    theSeeds.clear();
    for (unsigned i = 0; i < theOctaves; ++i) {
        Vector3i mySeed(getRandomElement(_myPool1),
                        getRandomElement(_myPool2),
                        getRandomElement(_myPool3));
        theSeeds.push_back(mySeed);
    }
}

int 
PrimePool::getRandomElement(const std::vector<int> & theList) {
    int myIndex = int(float( rand() ) / RAND_MAX * theList.size());
    return theList[myIndex];
}


PerlinNoise2D::PerlinNoise2D(float thePersistence) :
    _myPersistence(thePersistence)
{}

PerlinNoise2D::PerlinNoise2D(const NoiseSeed & theSeeds, float thePersistence) :
    _mySeeds(theSeeds),
    _myPersistence(thePersistence)
{}

float 
PerlinNoise2D::operator()(const asl::Vector2f & thePosition) const {
    return (*this)(thePosition[0], thePosition[1]);
}

float 
PerlinNoise2D::operator()(float x, float y) const {
    unsigned n = getOctaveCount() - 1;
    float myTotal = 0;
    for (unsigned i = 0; i < n; ++i) {      
        float myFrequency = pow(2.0f, float(i));
        float myAmplitude = pow(_myPersistence, float(i));        
        myTotal = myTotal + interpolatedNoise(x * myFrequency, y * myFrequency, i) * myAmplitude;        
    }
    
    return myTotal;    
}

float
PerlinNoise2D::noise(int theX, int theY, unsigned theOctave) const {
    const Vector3i & mySeed = _mySeeds[theOctave];
    int n = theX + theY * 57;
    n = (n << 13) ^ n;
    return float( 1.0 - ( ( n * ( n * n * mySeed[0] + mySeed[1]) + mySeed[2]) & 0x7fffffff) / 1073741824.0);
}

void 
PerlinNoise2D::setSeed(const NoiseSeed & theSeeds) {
    _mySeeds = theSeeds;
}

const NoiseSeed & 
PerlinNoise2D::getSeed() const {
    return _mySeeds;
}

void 
PerlinNoise2D::setPersistence(const float & thePersistence) {
    _myPersistence = thePersistence;
}

const float & 
PerlinNoise2D::getPersistence() const {
    return _myPersistence;
}

unsigned
PerlinNoise2D::getOctaveCount() const {
    return _mySeeds.size();
}

float
PerlinNoise2D::smoothedNoise(int x, int y, unsigned theOctave) const {
    float corners = (noise(x-1, y-1, theOctave) + noise(x+1, y-1, theOctave) + 
                    noise(x-1, y+1, theOctave) + noise(x+1, y+1, theOctave))  / 16.0f;
    float sides   = (noise(x-1, y, theOctave)  + noise(x+1, y, theOctave)  + 
                    noise(x, y-1, theOctave)  + noise(x, y+1, theOctave))  /  8.0f;
    float center  = (noise(x, y, theOctave)) / 4.0f;

    return corners + sides + center;
}

float
PerlinNoise2D::interpolatedNoise(float x, float y, unsigned theOctave) const {    
    int integer_X    = int(x);
    float fractional_X = x - integer_X;
    
    int integer_Y    = int(y);
    float fractional_Y = y - integer_Y;
    
    float v1 = smoothedNoise(integer_X,     integer_Y, theOctave);
    float v2 = smoothedNoise(integer_X + 1, integer_Y, theOctave);
    float v3 = smoothedNoise(integer_X,     integer_Y + 1, theOctave);
    float v4 = smoothedNoise(integer_X + 1, integer_Y + 1, theOctave);
    
    float i1 = interpolate(v1 , v2 , fractional_X);
    float i2 = interpolate(v3 , v4 , fractional_X);
    
    return interpolate(i1 , i2 , fractional_Y);
}

float
PerlinNoise2D::interpolate(float a, float b, float theGamma) const {
    float ft = float(theGamma * PI);
    float f = (1 - cos(ft)) * 0.5f;   
    return  a * ( 1 - f) + b * f;    
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// The following code comes from andi schlegel and seems to be faster
//
///////////////////////////////////////////////////////////////////////////////////////////////

PerlinNoise3D::PerlinNoise3D(unsigned theOctaveCount, float theAmplitudeFalloff) : 
    _myOctaveCount(theOctaveCount),
    _myAmplitudeFalloff(theAmplitudeFalloff)
{
    for (int i = 0; i < 360; i++) {
        _mySinLUT[i] = sinf(i * 0.01745329f);
        _myCosLUT[i] = cosf(i * 0.01745329f);
    }

    for (unsigned i = 0; i < 4096; ++i)
        _myPerlin[i] = float(rand()) / RAND_MAX;
    }
}

float
PerlinNoise3D::noise(float f, float f1, float f2) {
    if (f < 0.0f) {
        f = -f;
    }
    if (f1 < 0.0f) {
        f1 = -f1;
    }
    if (f2 < 0.0f) {
        f2 = -f2;
    }

    int j = int(f);
    int k = int(f1);
    int l = int(f2);
    
    float f3 = f - float(j);
    float f4 = f1 - float(k);
    float f5 = f2 - float(l);
    float f8 = 0.0f;
    float f9 = 0.5f;
    for (unsigned i1 = 0; i1 < _myOctaveCount; ++i1) {
        int j1 = j + (k << 4) + (l << 8);
        float f6 = noise_fsc(f3);
        float f7 = noise_fsc(f4);
        float f10 = _myPerlin[j1 & 0xfff];
        f10 += f6 * (_myPerlin[j1 + 1 & 0xfff] - f10);
        float f11 = _myPerlin[j1 + 16 & 0xfff];
        f11 += f6 * (_myPerlin[j1 + 16 + 1 & 0xfff] - f11);
        f10 += f7 * (f11 - f10);
        j1 += 256;
        f11 = _myPerlin[j1 & 0xfff];
        f11 += f6 * (_myPerlin[j1 + 1 & 0xfff] - f11);
        float f12 = _myPerlin[j1 + 16 & 0xfff];
        f12 += f6 * (_myPerlin[j1 + 16 + 1 & 0xfff] - f12);
        f11 += f7 * (f12 - f11);
        f10 += noise_fsc(f5) * (f11 - f10);
        f8 += f10 * f9;
        f9 *= _myAmplitudeFalloff;
        j <<= 1;
        f3 *= 2.0f;
        k <<= 1;
        f4 *= 2.0f;
        l <<= 1;
        f5 *= 2.0f;
        if (f3 >= 1.0f) {
            j++;
            f3--;
        }
        if (f4 >= 1.0f) {
            k++;
            f4--;
        }
        if (f5 >= 1.0f) {
            l++;
            f5--;
        }
    }

    return f8;
} 
