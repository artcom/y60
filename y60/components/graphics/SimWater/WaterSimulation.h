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

#ifndef _WaterSimulation_h_
#define _WaterSimulation_h_

#include <asl/base/Ptr.h>
#include <asl/math/Vector234.h>
#include <asl/base/PosixThread.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/ThreadSemaphore.h>
#include <asl/base/Logger.h>

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <assert.h>
#include <list>
#include <vector>

#ifndef M_SQRT2
#   define M_SQRT2 sqrt(2.0f)
#endif

namespace y60 {

class WaterSimulation {
public:

    WaterSimulation( asl::Vector2i theSimulationSize, float defaultDampingCoefficent=0.9993f);

    ~WaterSimulation();

    void    init();
    void    reset();
    void    setDamping(float theDampingValue);
    float   getDamping() const;

    void    simulationStep(float gTimeStep);


    // good for smaller arrays less overhead : stripSize = 10;
    // good for larger arrays (width > 100 ???)
    // better use of L2 cache : stripSize = 5;
    void    simulationMultiStep(float gTimeStep, int numSteps, int stripSize=5);
    void    simulationMultiStepInThread(asl::ThreadSemaphore & threadLock, float gTimeStep, int
                                numSteps, int stripSize=5);



    inline int getNumRows() const {
        return _mySize[1];
    }
    inline int getNumColumns() const {
        return _mySize[0];
    }

    //  we allocated an array that is slightly larger (for computation of normals!)
    inline float   * getWaterArray() {
        return _waterArray[_currentBuffer] + _numAllocatedColumns + 1;
    }

    inline float * getWaterDataLine(int lineNum) {
        assert(lineNum >= 0);
        assert(lineNum < _numAllocatedRows);
        return (_waterArray[_currentBuffer] + (lineNum+1) * _numAllocatedColumns + 1);
    }

    float & getWaterValue(int x, int y) {
        assert(x >= 0);
        assert(x < _numAllocatedColumns);
        assert(y >= 0);
        assert(y < _numAllocatedRows);
        return *(_waterArray[_currentBuffer] + (y+1) * _numAllocatedColumns + (x+1));
    }

    float & getWaterValue(int buffer, int x, int y) {
        assert(x >= 0);
        assert(x < _numAllocatedColumns);
        assert(y >= 0);
        assert(y < _numAllocatedRows);
        return *(_waterArray[buffer] + (y+1) * _numAllocatedColumns + (x+1));
    }

    inline float   * getDampingArray() {
        return _dampingArray;
    }

    float & getDampingValue(int x, int y) {
        assert(x >= 0);
        assert(x < _numAllocatedColumns);
        assert(y >= 0);
        assert(y < _numAllocatedRows);
        return *(_dampingArray + (y+1) * _numAllocatedColumns + (x + 1));
    }

    void    parabolicSplash(int xpos, int ypos, int magnitude, int radius);
    void    sinoidSplash(int xpos, int ypos, float magnitude, int radius, int frequency = 1);

    asl::ThreadSemaphore &    getComputeLock() {
        return * _computeLock;
    }

    void    setDefaultDampingCoefficient(float theValue) {
        _defaultDampingCoefficient = theValue;
    }

private:
    asl::Vector2i _mySize;
    int     _numAllocatedRows;
    int     _numAllocatedColumns;
    int     _currentBuffer;
    float * _waterArray[2];
    float * _dampingArray;
    float   _defaultDampingCoefficient;
    bool    _drawCaustics;
    bool    _drawReflections;
    bool    _drawRefractions;

    inline float waveSpeed() const { // "c"
        return 2.f;
    }

    inline float waveCoefficient() const { // "h"
//        return 2*float(0.1*c*sqrt(2));
        return 2.f*float( 0.1f * waveSpeed() * M_SQRT2);
    }

    inline float getDampingValueFAST(int x, int y) const {
        return *(_dampingArray + y * _numAllocatedColumns + x + 1);
    }

    void        switchBuffers();



    class ComputeThread: public asl::PosixThread {
    public:
            ComputeThread (WaterSimulation & simulation) : PosixThread(),
                          _threadLock(0),
                          _timeStep(0.0),
                          _numSteps(0),
                          _stripSize(0),
                          _doQuit(false),
                          _simulation(simulation)
            { }

            virtual ~ComputeThread() {}

            void    setup(asl::ThreadSemaphore & threadLock, float dt,
                          int numSteps, int stripSize) {
                _threadLock = & threadLock;
                _timeStep = dt;
                _numSteps = numSteps;
                _stripSize = stripSize;
            }

            void    setQuitFlag() {
                _doQuit = true;
            }

    private:
        asl::ThreadSemaphore *  _threadLock;
        float           _timeStep;
        int             _numSteps;
        int             _stripSize;
        bool            _doQuit;

        WaterSimulation &   _simulation;
            virtual void run ();
    };

    ComputeThread *    _computeThread;
    asl::ThreadSemaphore *   _computeLock;
};

typedef asl::Ptr<WaterSimulation> WaterSimulationPtr;

}; // namespace y60

#endif


