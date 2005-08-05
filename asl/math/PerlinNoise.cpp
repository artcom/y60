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
//    $RCSfile: PerlinNoise.cpp,v $
//
//     $Author: david $
//
//   $Revision: 1.4 $
//
// Description: simple 2d perlin noise with cosine interpolation
//              TODO: - 1d/3d variants
//
//=============================================================================
 

#include "PerlinNoise.h"
#include "numeric_functions.h"

#include <iostream>
#include <cmath>

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
PrimePool::createNoiseSeed(unsigned theOctaves, NoiseSeed & theSeed) {
    if (_myPool1.empty()) {
        createPrimeNumberList(10000, 20000, _myPool1);
        createPrimeNumberList(700000, 800000, _myPool2);
        createPrimeNumberList(1376310000, 1376320000, _myPool3);
    }
    
    theSeed.clear();
    for (unsigned i = 0; i < theOctaves; ++i) {
        Vector3i mySeed(getRandomElement(_myPool1),
                        getRandomElement(_myPool2),
                        getRandomElement(_myPool3));
        theSeed.push_back(mySeed);
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

PerlinNoise2D::PerlinNoise2D(const NoiseSeed & theSeed, float thePersistence) :
    _mySeeds(theSeed),
    _myPersistence(thePersistence)
{
}

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
PerlinNoise2D::setSeed(const NoiseSeed & theSeed) {
    _mySeeds = theSeed;
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
    //return (a * theGamma) + ( b * (1 - theGamma));
    float ft = float(theGamma * PI);
    float f = (1 - cos(ft)) * 0.5f;
    
    return  a * ( 1 - f) + b * f;    
}

} // endo of namespace asl
