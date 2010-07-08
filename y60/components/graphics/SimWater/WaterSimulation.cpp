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

#if defined(_MSC_VER)
#   pragma warning(push,1)
#   ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN 1
#   endif
#   define NOMINMAX
#   include <windows.h>
#   undef max
#   pragma warning(pop)
#endif


#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <assert.h>

#if defined(LINUX) || defined(OSX)
    #include <unistd.h>
#endif

#include <y60/glutil/GLUtils.h>

#if defined(LINUX)
#include <GL/glxew.h>
#endif


#include "WaterSimulation.h"
#include <asl/base/ThreadSemaphore.h>

using namespace std;  // automatically added!

#ifndef SQR
#define SQR(a) ((a) * (a))
#endif

#ifndef ABS
#define ABS(a) (((a) < 0) ? (-(a)) : (a))
#endif

namespace y60 {

//#define DAMPING_TEST

WaterSimulation::WaterSimulation(asl::Vector2i theSimulationSize, float defaultDampingCoefficent) :
    _mySize( theSimulationSize ),
    _currentBuffer(0),
    _dampingArray(0),
    _defaultDampingCoefficient(defaultDampingCoefficent),
    _computeThread(0),
    _computeLock(0)
{
    //  we allocate an array that is slightly larger (for computation of normals!)
    _numAllocatedRows = _mySize[1] + 2;
    _numAllocatedColumns = _mySize[0] + 2;

    /*
    _waterArray[0] = (float*) malloc(_numAllocatedRows * _numAllocatedColumns * sizeof(float));
    _waterArray[1] = (float*) malloc(_numAllocatedRows * _numAllocatedColumns * sizeof(float));
    _dampingArray = (float*) malloc(_numAllocatedRows * _numAllocatedColumns * sizeof(float));
    */
    _waterArray[0] = new float[_numAllocatedRows * _numAllocatedColumns];
    _waterArray[1] = new float[_numAllocatedRows * _numAllocatedColumns];
    _dampingArray = new float[_numAllocatedRows * _numAllocatedColumns];

}

WaterSimulation::~WaterSimulation() {
    cerr << "================== DTOR ========================" << endl;

    if (_computeThread) {
        cerr << "deleting thread" << endl;
        _computeThread->join();
        delete _computeThread;
        _computeThread = 0;
    }

    if (_waterArray[0]) {
        cerr << "deleting water[0]" << endl;
        //free(_waterArray[0]);
        //_waterArray[0] = 0;
        delete [] _waterArray[0];
    }

    if (_waterArray[1]) {
        cerr << "deleting water[1]" << endl;
        //free(_waterArray[1]);
        //_waterArray[1] = 0;
        delete [] _waterArray[1];
    }

    if (_dampingArray) {
        cerr << "deleting damping array" << endl;
        //free(_dampingArray);
        //_dampingArray = 0;
        delete [] _dampingArray;
    }
    if (_computeLock) {
        cerr << "deleting compute lock" << endl;
        // XXX asl::ThreadSemFactory::instance()->freeLock(_computeLock);
        _computeLock = 0;
    }
    cerr << "=== DTOR DONE ===" << endl;
}

void
WaterSimulation::init() {
    reset();
    setDamping(_defaultDampingCoefficient);
}

void
WaterSimulation::reset() {

    for(int j = 0; j < _numAllocatedRows ; j++ ) {

        GLfloat * currBufferLine = _waterArray[_currentBuffer] + j * _numAllocatedColumns;
        GLfloat * prevBufferLine = _waterArray[1 - _currentBuffer] + j * _numAllocatedColumns;

        for( int i=0 ; i < _numAllocatedColumns ; i++ ) {
            prevBufferLine[i] = 0;
            currBufferLine[i] = 0;
        }
    }
}

void
WaterSimulation::setDamping(float theDampingValue) {

    //float   dampValue = freeze ? 0.f : _defaultDampingCoefficient;

    for(int j = 0; j < _numAllocatedRows ; j++ ) {

        GLfloat * dampBufferLine = _dampingArray + j * _numAllocatedColumns;

        for( int i= 0 ; i < _numAllocatedColumns ; i++ ) {


            dampBufferLine[i] = theDampingValue;
        }
    }
}

float
WaterSimulation::getDamping() const {
		return _dampingArray[0];
}

#if 0   // THE ORIGINAL


void
WaterSimulation::simulationStep(const float dt) {
    //precalculate coefficients
//    const float A = (c*dt/_h)*(_c*dt/_h);
    const float A = SQR(waveSpeed() * dt / waveCoefficient());

    //	const float AD = A/sqrt(2);
    const float B = 2.f - 4.f*A;// - 4*AD;

    float * currBuffer = _waterArray[_currentBuffer];
    float * prevBuffer = _waterArray[1 - _currentBuffer];

    //for(int j = 2; j < (_numAllocatedRows-2) ; j++ ) {
    for(int j = 1; j < _numAllocatedRows-1 ; j++ ) {

        GLfloat * currLine = prevBuffer + j * _numAllocatedColumns;
        GLfloat * thisLine = currBuffer + j * _numAllocatedColumns;
        GLfloat * prevLine = currLine - _numAllocatedColumns;
        GLfloat * nextLine = currLine + _numAllocatedColumns;

        for( int i=1 ; i<_numAllocatedColumns-1 ; i++ ) {

            thisLine[i] = (A*( prevLine[i] + nextLine[i] + currLine[i-1] + currLine[i+1] ) +
                + B * currLine[i]
                - thisLine[i]) * getDampingValueFAST(i, j);
        }
    }
    switchBuffers();
}
#endif

#if 1

#define register

void
WaterSimulation::simulationStep(const float dt) {
    //precalculate coefficients
//    const float A = (c*dt/_h)*(_c*dt/_h);
    register const float A = SQR(waveSpeed() * dt / waveCoefficient());
    //	const float AD = A/sqrt(2);
    register const float B = 2.f - 4.f*A;// - 4*AD;

    float * currBuffer = _waterArray[_currentBuffer];
    float * prevBuffer = _waterArray[1 - _currentBuffer];

    //for(int j = 2; j < (_numAllocatedRows-2) ; j++ ) {
    for(int j = 1; j < _numAllocatedRows-1 ; j++ ) {

        register GLfloat * thisLine = currBuffer + j * _numAllocatedColumns;
        register GLfloat * currLine = prevBuffer + j * _numAllocatedColumns;
        GLfloat * prevLine = currLine - _numAllocatedColumns;
        GLfloat * nextLine = currLine + _numAllocatedColumns;
        GLfloat * dampingLine = _dampingArray + j * _numAllocatedColumns;

        for( int i=1 ; i<_numAllocatedColumns-1 ; i++ ) {

            thisLine[i] = (A*( prevLine[i] + nextLine[i] + currLine[i-1] + currLine[i+1] ) +
                + B * currLine[i]
                - thisLine[i]) * dampingLine[i];
        }
    }
    switchBuffers();
}
#endif


#if 1

#define register


// stepped simulation approach

// good for smaller arrays
// less overhead
//const int numSteps = 5;
//const int stripSize = 10;

// good for larger arrays (width > 100 ???)
// better use of L2 cache
//const int numSteps = 5;
//const int stripSize = 5;

void
WaterSimulation::simulationMultiStep(float dt, int numSteps, int stripSize) {

//precalculate coefficients
//    const float A = (c*dt/_h)*(_c*dt/_h);
    register const float A = SQR(waveSpeed() * dt / waveCoefficient());

    //	const float AD = A/sqrt(2);
    register const float B = 2.f - 4.f*A;// - 4*AD;

    //assert(stripSize >= numSteps);
    //assert(_mySize[1] % stripSize == 2);

    int startLine = 0;
    int endLine = 0;

    int     currentBuffer = _currentBuffer;
    float * currBuffer = 0;
    float * prevBuffer = 0;


    int currentStrip = 0;
    int numStrips = (_mySize[1] / stripSize);

    // Phase 1: First strip
//cerr << "P1\n";
//cerr << "strip= " << 0 << endl;

    for (int step = 0; step < numSteps; step++) {
//cerr << "step= " << step << endl;

        currBuffer = _waterArray[currentBuffer];
        prevBuffer = _waterArray[1 - currentBuffer];

        startLine = 1;
        endLine = startLine + stripSize - 1;

        for(int j = startLine; j < endLine-step ; j++ ) {
//cerr << j << endl;
            register GLfloat * thisLine = currBuffer + j * _numAllocatedColumns;
            register GLfloat * currLine = prevBuffer + j * _numAllocatedColumns;
            GLfloat * prevLine = currLine - _numAllocatedColumns;
            GLfloat * nextLine = currLine + _numAllocatedColumns;
            GLfloat * dampingLine = _dampingArray + j * _numAllocatedColumns;

            for( int i=1 ; i<_numAllocatedColumns-1 ; i++ ) {

#ifdef DAMPING_TEST
                if (dampingLine[i] == 0. && thisLine[i] != .0f) {
                    dampingLine[i] = _defaultDampingCoefficient;
                    continue;
                }
#endif

                thisLine[i] = (A*( prevLine[i] + nextLine[i] + currLine[i-1] + currLine[i+1] ) +
                        + B * currLine[i]
                        - thisLine[i]) * dampingLine[i];


            }
        }
        currentBuffer = 1- currentBuffer;
    }

    // Phase 2: intermediate strips

//cerr << "P2\n";
    currentStrip = 1;

    for ( ; currentStrip < numStrips; currentStrip++) {
//cerr << "strip= " << currentStrip << endl;

        currentBuffer = _currentBuffer;

        for (int step = 0; step < numSteps; step++) {
//cerr << "step= " << step << endl;

            currBuffer = _waterArray[currentBuffer];
            prevBuffer = _waterArray[1 - currentBuffer];

            startLine = currentStrip * stripSize;
            endLine = startLine + stripSize;

            int j = startLine-step;

            register GLfloat * thisLine = currBuffer + j * _numAllocatedColumns;
            register GLfloat * currLine = prevBuffer + j * _numAllocatedColumns;
            GLfloat * prevLine = currLine - _numAllocatedColumns;
            GLfloat * nextLine = currLine + _numAllocatedColumns;
            GLfloat * dampingLine = _dampingArray + j * _numAllocatedColumns;

            for(; j < endLine-step ; j++ ) {
//cerr << j << endl;

                for( int i=1 ; i<_numAllocatedColumns-1 ; i++ ) {

#ifdef DAMPING_TEST
                    if (dampingLine[i] == 0. && thisLine[i] != .0f) {
                        dampingLine[i] = _defaultDampingCoefficient;
                        continue;
                    }
#endif
                    thisLine[i] = (A*( prevLine[i] + nextLine[i] + currLine[i-1] + currLine[i+1] ) +
                            + B * currLine[i]
                            - thisLine[i]) * dampingLine[i];
                }
                thisLine += _numAllocatedColumns;
                currLine += _numAllocatedColumns;
                prevLine += _numAllocatedColumns;
                nextLine += _numAllocatedColumns;
                dampingLine += _numAllocatedColumns;
            }
            currentBuffer = 1- currentBuffer;
        }
    }

    // Phase 3: last strip
    currentBuffer = _currentBuffer;

//cerr << "P3\n";
//cerr << "strip= " << currentStrip << endl;

    for (int step = 0; step < numSteps; step++) {
//cerr << "step= " << step << endl;

        currBuffer = _waterArray[currentBuffer];
        prevBuffer = _waterArray[1 - currentBuffer];

        startLine = currentStrip * stripSize;
        endLine = _numAllocatedRows-1;
//cerr << "startLine= " << startLine << endl;
//cerr << "endLine= " << endLine << endl;

        for(int j = startLine-step; j < endLine ; j++ ) {
//cerr << j << endl;

            register GLfloat * thisLine = currBuffer + j * _numAllocatedColumns;
            register GLfloat * currLine = prevBuffer + j * _numAllocatedColumns;
            GLfloat * prevLine = currLine - _numAllocatedColumns;
            GLfloat * nextLine = currLine + _numAllocatedColumns;
            GLfloat * dampingLine = _dampingArray + j * _numAllocatedColumns;

            for( int i=1 ; i<_numAllocatedColumns-1 ; i++ ) {
#ifdef DAMPING_TEST
                if (dampingLine[i] == 0. && thisLine[i] != .0f) {
                    dampingLine[i] = _defaultDampingCoefficient;
                    continue;
                }
#endif
                thisLine[i] = (A*( prevLine[i] + nextLine[i] + currLine[i-1] + currLine[i+1] ) +
                        + B * currLine[i]
                        - thisLine[i]) * dampingLine[i];
            }
        }
        currentBuffer = 1- currentBuffer;
    }
//exit(0);
    switchBuffers();
}
#endif


void
WaterSimulation::simulationMultiStepInThread(asl::ThreadSemaphore & threadLock, float dt,
                                             int numSteps, int stripSize)
{

    if (!_computeThread) {

        cerr << "Generating compute thread" << endl;

        //assert(asl::ThreadSemFactory::instance());
        assert(_computeLock == 0);
        _computeLock = new asl::ThreadSemaphore(); //asl::ThreadSemFactory::instance()->newLock();
        assert(_computeLock);

        cerr << "WaterSimulation::simulationMultiStepInThread: _computeLock.lock()" << endl;
        _computeLock->post();

        cerr << "WaterSimulation::simulationMultiStepInThread: threadLock.lock()" << endl;
        threadLock.post();

        _computeThread = new ComputeThread(*this);
        cerr << "forking compute thread" << endl;
        _computeThread->fork();

        sched_yield();
    }
    assert(_computeThread);

    _computeThread->setup(threadLock, dt, numSteps, stripSize);

    //cerr << "WaterSimulation::simulationMultiStepInThread: _computeLock.unlock()" << endl;
    _computeLock->wait();

    // now executing computeThread
}

void
WaterSimulation::switchBuffers() {
    _currentBuffer = 1 - _currentBuffer;
}

void
WaterSimulation::parabolicSplash(int xpos, int ypos, int magnitude, int radius) {

	for ( int x = xpos - radius; x < xpos + radius; ++x) {
		for (int y = ypos - radius; y < ypos + radius; ++y) {
			double dx = xpos - x;
			double dy = ypos - y;
			double distance = sqrt(dx * dx + dy * dy);
			if (distance <= radius) {
				float weight = 1.0f - float(distance/radius);
				weight *= weight;
				if (x >= 0 && x <= _mySize[0] && y >= 0 && y <= _mySize[1]) {
//					(*z)[y][x] = (*z1)[y][x] = -weight * magnitude;
					getWaterValue(0, x, y) = getWaterValue(1, x, y) =
                        -weight * magnitude;
				}
			}
		}
	}
}

void
WaterSimulation::sinoidSplash(int xpos, int ypos, float magnitude, int radius, int frequency) {

    /*
    cerr << "WaterSimulation::sinoidSplash() x: " << xpos << " y: " << ypos
        << " magnitude: " << magnitude << " radius: " << radius
        << " frequency: " << frequency << endl;
*/


    for ( int x = xpos - radius; x < xpos + radius; ++x) {
		for (int y = ypos - radius; y < ypos + radius; ++y) {
			double dx = xpos - x;
			double dy = ypos - y;
			double distance = sqrt(dx * dx + dy * dy);
			if (distance <= radius) {
				float weight = 1.0f - float(distance/radius);
				weight *= weight;
				if (x >= 0 && x < _mySize[0] && y >= 0 && y < _mySize[1] ) {
//#ifdef AIR_SPLASH
//					(*z)[y][x] = (*z1)[y][x] = (*z)[y][x] -sin(weight*frequency*3.14159/2) * magnitude;
					getWaterValue(0, x, y) = getWaterValue(1, x, y) =
                        getWaterValue(0, x, y) - float(sin(weight*frequency*3.14159/2)) * magnitude;
                    getDampingValue(x, y) = _defaultDampingCoefficient;
//#else
//					(*z)[y][x] = (*z1)[y][x] = -sin(weight*frequency*3.14159/2) * magnitude;
//#endif
				}
			}
		}
	}
}


void
WaterSimulation::ComputeThread::run() {

    for(;;) {

//        cerr << "WaterSimulation::ComputeThread::run() START" << endl;

//        cerr << "ComputeThread::run(): _computeLock.lock()" << endl;
        // waiting for main thread to let us loose
        _simulation.getComputeLock().post();

        assert(_threadLock);

        _simulation.simulationMultiStep(_timeStep, _numSteps, _stripSize);

        // signalling calling (main) thread that we are finished
        assert(_threadLock);
//        cerr << "ComputeThread::run(): threadLock.lock()" << endl;
        _threadLock->wait();

//        cerr << "WaterSimulation::ComputeThread::run() END" << endl;

//        cerr << "ComputeThread::run(): _computeLock.lock()" << endl;
        _simulation.getComputeLock().wait();

    }
}



}; // namespace y60

