//============================================================================
//
// Copyright (C) 2002-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _WaterSimulation_h_
#define _WaterSimulation_h_

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <assert.h>
#include <list>
#include <vector>

#include <asl/PosixThread.h>
#include <asl/ThreadLock.h>
#include <asl/ThreadSemaphore.h>

namespace video {
    
class WaterSimulation {
public:

    WaterSimulation(int numColumns, int numRows, float defaultDampingCoefficent=0.9993f);

    ~WaterSimulation();

    void    init();
    void    reset();
    void    resetDamping(bool freeze=false);

    void    simulationStep(float gTimeStep);


    // good for smaller arrays less overhead : stripSize = 10;
    // good for larger arrays (width > 100 ???)
    // better use of L2 cache : stripSize = 5;
    void    simulationMultiStep(float gTimeStep, int numSteps, int stripSize=5);
    void    simulationMultiStepInThread(asl::ThreadSemaphore & threadLock, float gTimeStep, int 
                                numSteps, int stripSize=5);



    inline int getNumRows() const {
        return _numRows;
    }
    inline int getNumColumns() const {
        return _numColumns;
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
    void    sinoidSplash(int xpos, int ypos, int magnitude, int radius, int frequency = 1);

    asl::ThreadSemaphore &    getComputeLock() {
        return * _computeLock;
    }

    void    setDefaultDampingCoefficient(float theValue) {
        _defaultDampingCoefficient = theValue;
    }
    
private:
    int     _numRows;
    int     _numColumns;
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


    
    class ComputeThread: public PosixThread {
    public:    
            ComputeThread (WaterSimulation & simulation) : PosixThread(),
                          _simulation(simulation), 
                          _doQuit(false),
                          _threadLock(0)
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


}; // namespace video

#endif


