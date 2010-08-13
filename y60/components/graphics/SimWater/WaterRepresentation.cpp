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

#include "WaterRepresentation.h"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <assert.h>
#include <list>
#include <vector>

#if defined(_MSC_VER)
#    pragma warning(push,1)
#endif
#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#include <asl/base/Dashboard.h>
#include <asl/base/Logger.h>
#include <asl/math/numeric_functions.h>

#include <y60/glutil/GLUtils.h>

#include "WaterSimulation.h"

using namespace std;  // automatically added!
using namespace asl;  // manually added!

#ifndef SQR
#define SQR(a) ((a) * (a))
#endif

#ifndef ABS
#define ABS(a) (((a) < 0) ? (-(a)) : (a))
#endif

#define DB(x) //x

namespace y60 {

const string WaterRepresentation::TextureClassNames[] =
{ "#nomap", "floormaps", "surfacemaps", "cubemaps", "puzzlemaps" };

const float WATER_LEVEL = -200.f;
const float WATER_DEPTH = 10.f;

const float CLEAR_LEVEL = 0.;
const float CRACK_LEVEL = 1.;
const float RAISED_CRACK_LEVEL = 10.;
const float SURFACE_LEVEL = 2.;
const float CRACK_EDGE_LEVEL_HIGH = 5;
const float CRACK_EDGE_LEVEL_LOW = 3;

WaterRepresentation::WaterRepresentation() :
    _bufferAllocator(),
    _vertexBuffer(0),
    _waterSimulation(),
    _objectPosScaleX(1.0f),
    _objectPosScaleY(1.0f),
    _objectPosOffsetX(0.0f),
    _objectPosOffsetY(0.0f),
    _drawCaustics(true),
    _drawReflections(true),
    _drawRefractions(false), //  done by caustics
    _drawWireFrame( false ),
    _dataOffsetX(0),
    _dataOffsetY(0),
    _dataWidth(0),
    _dataHeight(0),
    _displayWidth(100),
    _displayHeight(100),
    _displayOffsetX(0),
    _displayOffsetY(0),
    _reflectionAlphaBias(0.05f),
    _reflectionAlphaScale(1.f -  0.05f),
    _reflectionNormalScale(0.2f),
    _refractionScale(1.f),
    _causticBias(.75f),
    _causticScale(0.f),
    _causticSqrScale(1.f),
    _causticNegativeScale(0.25f),
    _normalMax(.5f),
    _stripIndices(0),
    _renderSurfaceEnabled(false),
    _crackWidth(0.5f),
    _innerIceThickness(2.0f),
    _outerIceThickness(4.0f),
    _renderCrackField(false),
    _surfaceOpacity(0.95f)
{
    for (int i=0; i<6; i++) {
        _cubeMapSideID[i] = 0;
    }
    resetParameters();
}


WaterRepresentation::~WaterRepresentation() {
    if (_vertexBuffer) {
        if (_bufferAllocator) {
            _bufferAllocator->freeSingleBuffer();
            _vertexBuffer = 0;
        } else {
            //free(_bufferAllocator);
            _bufferAllocator = BufferAllocatorPtr();
            _vertexBuffer= 0;
        }
    }
    if (_bufferAllocator) {
        //delete _bufferAllocator;
        _bufferAllocator = BufferAllocatorPtr();
    }
    if (_stripIndices) {
        delete[] _stripIndices;
        _stripIndices = 0;
    }
}

void
WaterRepresentation::resetParameters() {
    _reflectionAlphaBias = 0.05f;
    _normalMax = .5f;
    _reflectionAlphaScale = 1.f -  0.05f;
    _refractionScale = 1.f;
    _reflectionNormalScale = .2f;
    _causticBias = .75f;
    _causticScale = 0.f;
    _causticSqrScale = 1.f;
    _causticNegativeScale = 0.25f;
}


// init internal data to fit water array size
void
WaterRepresentation::init(WaterSimulationPtr waterSim, int width, int height,
                          int dataOffsetX, int dataOffsetY,
                          int sceneDisplayWidth, int sceneDisplayHeight,
                          int displayWidth, int displayHeight,
                          int displayOffsetX, int displayOffsetY,
                          BufferAllocatorPtr bufferAllocator)
{
    cerr << "WaterRepresentation::init " << endl
         << "dataWidth=" << width << ", dataHeight= " << height << endl
         << ", dataOffsetX= " << dataOffsetX << ", dataOffsetY= " << dataOffsetY << endl
         << ", sceneDisplayWidth= " << sceneDisplayWidth
         << ", sceneDisplayHeight= " << sceneDisplayHeight << endl
         << ", displayWidth= " << displayWidth << ", displayHeight= " << displayHeight << endl
         << ", displayOffsetX= " << displayOffsetX << ", displayOffsetY= " << displayOffsetY
         << endl;

    assert(waterSim);
    _waterSimulation = waterSim;

    _dataOffsetX = dataOffsetX;
    _dataOffsetY = dataOffsetY;
    _dataWidth = width;
    _dataHeight = height;

    _displayWidth = displayWidth;
    _displayHeight = displayHeight;
    _displayOffsetX = displayOffsetX;
    _displayOffsetY = displayOffsetY;

    int numLinesPerBuffer = computeNumLinesPerBuffer();
    int stripLength = computeStripLength();
    int floatsToAllocate = (_dataHeight+1)*(_dataWidth+1) * VERTEX_DATA_STRIDE;
    int bytesToAllocate = floatsToAllocate * sizeof(GLfloat);

    cerr << "   stripLength=" << stripLength << ", bytesToAllocate= " << bytesToAllocate << endl;

    _stripIndices = new GLuint[numLinesPerBuffer * stripLength];

    if (bufferAllocator) {
        _vertexBuffer = (GLfloat*) bufferAllocator->allocateSingleBuffer(
                                                bytesToAllocate);
    } else {
        _vertexBuffer = (GLfloat*) malloc(bytesToAllocate);
    }

    assert(_vertexBuffer);

    assert((_dataHeight % NUM_VAR_BUFFERS) == 0);


    for(int i=0; i < NUM_VAR_BUFFERS; i++) {
        _varBuffer[i]._pointer = _vertexBuffer + (i * _dataWidth *
                                 computeNumLinesPerBuffer() * VERTEX_DATA_STRIDE);
    }
    for (int i=0; i<floatsToAllocate; i++) {
        _vertexBuffer[i] = 0;
    }


    for (int j=0; j < numLinesPerBuffer; j++) {

        int currStripIndex = stripLength * j;

        _stripIndices[currStripIndex] = j * _dataWidth;
        _stripIndices[currStripIndex+1] = (j+1) * _dataWidth;

        for (int i=2; i< stripLength; i++) {
            _stripIndices[currStripIndex+i] = _stripIndices[currStripIndex+i-2] + 1;
        }
    }

    for (int currentBuffer=0; currentBuffer < NUM_VAR_BUFFERS; currentBuffer++) {

        GLfloat * v = _varBuffer[currentBuffer]._pointer;
        for (int j = 0; j <= numLinesPerBuffer; j++) {

            GLfloat * vertexLinePtr = v + j * _dataWidth * VERTEX_DATA_STRIDE;

            for (int i=0; i<= _dataWidth; i++) {
                vertexLinePtr[0] = i - ((float)(_dataWidth)/ 2.f);  // xpos
                vertexLinePtr[1] = currentBuffer * numLinesPerBuffer + j -
                                    ((float)(_dataHeight) / 2.f);  // ypos

                vertexLinePtr[0] += (float) _displayOffsetX;
                vertexLinePtr[1] += (float) _displayOffsetY;

                vertexLinePtr[2] = - 200;
                // z of normal
                vertexLinePtr[5] = 1;

                vertexLinePtr += VERTEX_DATA_STRIDE;
            }
        }
    }

    _objectPosScaleX = (double(sceneDisplayWidth) / 10000.);
    _objectPosScaleY = (double(sceneDisplayHeight) / 10000.);

    _objectPosOffsetX = double(_displayOffsetX) - (double(_dataWidth) / 2.) -
                        double(_dataOffsetX);
    _objectPosOffsetY = double(_displayOffsetY) - (double(_dataHeight) / 2.) -
                        double(_dataOffsetY);

    cerr << "WaterRepresentation::init _objectPosScaleX= " << _objectPosScaleX
         << ", _objectPosScaleY= "  << _objectPosScaleY
         << ", _objectPosOffsetX= " << _objectPosOffsetX
         << ", _objectPosOffsetY= " << _objectPosOffsetY << endl;

    initRender();
    AC_DEBUG << "WaterRepresentation::init() finished";
}

void
WaterRepresentation::setDataOffset( const Vector2i & theOffset ) {
    _dataOffsetX = theOffset[0];
    _dataOffsetY = theOffset[1];
}

void
WaterRepresentation::setDisplayOffset( const Vector2i & theOffset ) {
    _displayOffsetX = theOffset[0];
    _displayOffsetY = theOffset[1];
}

void
WaterRepresentation::dataCoordToScreenCoord(const Vector2f & theDataCoord, Vector2f & screenCoord) {
    screenCoord[0] = theDataCoord[0] - float(_dataWidth)/2.0f + _displayOffsetX - _dataOffsetX;
    screenCoord[1] = theDataCoord[1] - float(_dataHeight)/2.0f + _displayOffsetY - _dataOffsetY;
}

void
WaterRepresentation::copyWaterDataToVertexBuffer() {
    for (int currentBuffer=0; currentBuffer < NUM_VAR_BUFFERS; currentBuffer++) {
        copyWaterDataToVertexBuffer(currentBuffer);
    }
}

void
WaterRepresentation::copyWaterDataToVertexBuffer(int currentBuffer) {

    assert(_waterSimulation);

    int     numLinesPerBuffer = computeNumLinesPerBuffer();
    int     startLine = currentBuffer * numLinesPerBuffer;
    int     endLine = startLine + numLinesPerBuffer;

    GLfloat * v = _varBuffer[currentBuffer]._pointer;

    Texture & myFloorMap = currentTexture(floormaps);
    const float texFactorX = (_displayWidth / float(myFloorMap.myWidth)) / (float)_dataWidth;
    const float texFactorY = (_displayHeight / float(myFloorMap.myHeight)) / (float)_dataHeight;

    int currentLine = 0;
#ifdef COMPUTE_WATER_DEPTH
    float minWaterDepth = FLT_MAX;
    float maxWaterDepth = FLT_MIN;
#endif
    for(int j = startLine ; j <= endLine; j++ ) {

        register GLfloat * vertexLinePtr = v + currentLine * _dataWidth * VERTEX_DATA_STRIDE;

        GLfloat * waterDataLine = _waterSimulation->getWaterDataLine(_dataOffsetY + j) +
                                    _dataOffsetX;
        GLfloat * prevWaterDataLine = _waterSimulation->getWaterDataLine(_dataOffsetY + j - 1) +
                                    _dataOffsetX;
        GLfloat * nextWaterDataLine = _waterSimulation->getWaterDataLine(_dataOffsetY + j + 1) +
                                    _dataOffsetX;

        for( int i=0 ; i < _dataWidth ; i++ ) {

            // only z needs to be updated
            vertexLinePtr[2] = (5.f*(waterDataLine[_dataOffsetX + i])) + WATER_LEVEL;

#ifdef COMPUTE_WATER_DEPTH
            if (waterDataLine[i] < minWaterDepth) {
                minWaterDepth = waterDataLine[i];
            }
            if (waterDataLine[i] > maxWaterDepth) {
                maxWaterDepth = waterDataLine[i];
            }
#endif
            //  compute normal
            float normalX;
            float normalY;
			normalX = waterDataLine[i-1] - waterDataLine[i + 1];
			normalY = prevWaterDataLine[i] - nextWaterDataLine[i];

            normalX = clamp(normalX, -_normalMax, _normalMax);
            normalY = clamp(normalY, -_normalMax, _normalMax);

            // normale eintragen, opengl normalisiert selbst
            vertexLinePtr[3] = normalX * _reflectionNormalScale;
            vertexLinePtr[4] = normalY * _reflectionNormalScale;
            //vertexLinePtr[5] = 1;

            // compute texture coordinates
            vertexLinePtr[6] = (((float)i) -
                            (normalX * _refractionScale * waterDataLine[i])) * texFactorX;
            vertexLinePtr[7] = (((float)
                            (_displayHeight-j))+
                            (normalY * _refractionScale * waterDataLine[i])) * texFactorY;

            float heightValue = maximum(-1.f, minimum(1.f, waterDataLine[i]));
            float colorValue = _causticBias + ((heightValue > 0) ?
                                        (_causticScale * heightValue +
                                        _causticSqrScale * SQR(heightValue)) :
                                        (_causticNegativeScale * (heightValue)));

            vertexLinePtr[8] = colorValue;
            vertexLinePtr[9] = colorValue;
            vertexLinePtr[10] = colorValue;

            float alphaValue = _reflectionAlphaBias +
                               _reflectionAlphaScale * (normalX + normalY);
            vertexLinePtr[11] = alphaValue;

            vertexLinePtr += VERTEX_DATA_STRIDE;

        }
        currentLine++;
    }
#ifdef COMPUTE_WATER_DEPTH
    cerr << "minWaterDepth= " << minWaterDepth << endl;
    cerr << "maxWaterDepth= " << maxWaterDepth << endl;
#endif
}

bool
WaterRepresentation::initExtensions(const char * extension) {
  if ( !hasCap(extension)) {
    AC_ERROR << "requires the " << extension << " OpenGL extension to work.";
    return false;
  }
  return true;
}


//This function loads a cube map and stores it under an arbitrary class and object id.
bool
WaterRepresentation::loadCubeMapTexture(TextureClass theClassID,
                                        short theObjectID,
                                        const string textureFileNames[])
{

   cerr << "#INFO : loading cube texture map class " << TextureClassName(theClassID)
         << " into map object= " << theObjectID << endl;

    Texture & myTexture = _myTextures[theClassID][theObjectID];

    if (!myTexture.myID) {
        glGenTextures(1, &myTexture.myID);
    }

	//Bind the texture.
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, myTexture.myID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //N.B.!! LOOK HERE! Auto mipmap extension used.
	//glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB,
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB,
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB,
                    GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	PLAnyPicDecoder myDecoder;

	//Loop to load all images.

    int & imageWidth = myTexture.myWidth;
    int & imageHeight = myTexture.myHeight;

    imageWidth=0;
    imageHeight = 0;

	for(unsigned int i=0; i<6; i++) {
        PLAnyBmp myBmp;

        cerr << "Loading bitmap for cube map '"
            << textureFileNames[i].c_str() << "'..." << flush;
        try {
            myDecoder.MakeBmpFromFile (textureFileNames[i].c_str(), &myBmp);
        } catch (...) {
            cerr << "FAILED" << endl;
            return false;
        }
        cerr << "OK" << endl;
        DB(cerr << "image alpha : " << myBmp.HasAlpha() << endl);
        DB(cerr << "image width : " << myBmp.GetWidth() << endl);
        DB(cerr << "image height : " << myBmp.GetHeight() << endl);
		glTexImage2D(_cubeMapSideID[i], 0,
                     GL_RGBA8, myBmp.GetWidth(),
                     myBmp.GetHeight(),
					 0,  (myBmp.HasAlpha()) ? GL_BGRA : GL_BGR, GL_UNSIGNED_BYTE, myBmp.GetPixels());
        if (imageWidth) {
            if ((imageWidth != myBmp.GetWidth()) || (imageHeight != myBmp.GetHeight())) {
                cerr << "#ERROR : Cubemap images MUST all be of same size!";
                exit(-1);
            }
        }
        imageWidth = myBmp.GetWidth();
        imageHeight = myBmp.GetHeight();
	}
//	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    return true;
}

//This function loads a cube map and stores it under an arbitrary class and object id.
bool
WaterRepresentation::loadTexture(TextureClass theClassID,
                                 short theObjectID,
                                 const char * theTextureFileName)
{

    AC_PRINT << "#INFO : loading texture map class " << TextureClassName(theClassID)
         << " into map object= " << theObjectID;

    Texture & myTexture = _myTextures[theClassID][theObjectID];

    if (!myTexture.myID) {
        glGenTextures(1, &myTexture.myID);
    }

	//Bind the texture.
	glBindTexture(GL_TEXTURE_2D, myTexture.myID);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

	PLAnyPicDecoder myDecoder;
    PLAnyBmp myBmp;

    cerr << "Loading bitmap for texture '"
         << theTextureFileName << "' .." << flush;
    try {
        myDecoder.MakeBmpFromFile (theTextureFileName, &myBmp);
    } catch (...) {
        cerr << "FAILED" << endl;
        return false;
    }
    cerr << "OK" << endl;

    if (!asl::powerOfTwo(myBmp.GetWidth()) || !asl::powerOfTwo(myBmp.GetHeight())) {
        cerr << "#WARNING: texture " << theTextureFileName << " is not 'powerOfTwo', but "
             << myBmp.GetWidth() << "x" << myBmp.GetHeight()
             << ". Resizing the texture internally..." << endl;
        myBmp.ApplyFilter(PLFilterResizeBilinear(256, 256));
    }

    //Store that section. ..
    /*
    glTexImage2D(GL_TEXTURE_2D, 0,
            (myBmp.HasAlpha()) ? GL_RGBA8 : GL_RGB8, myBmp.GetWidth(),
            myBmp.GetHeight(),
            0, (myBmp.HasAlpha()) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, myBmp.GetPixels());
    */
    glTexImage2D(GL_TEXTURE_2D, 0,
            (myBmp.HasAlpha()) ? GL_RGBA8 : GL_RGB8, myBmp.GetWidth(),
            myBmp.GetHeight(),
            0, (myBmp.HasAlpha()) ? GL_BGRA : GL_BGR, GL_UNSIGNED_BYTE, myBmp.GetPixels());

    myTexture.myWidth = myBmp.GetWidth();
    myTexture.myHeight = myBmp.GetHeight();

	return true;
}

bool
WaterRepresentation::activateTexture(TextureClass theClassID, short newObjectID) {
    if (_myTextures.find(theClassID) == _myTextures.end()) {
        AC_ERROR << "WaterRepresentation::activateTexture: no texture class "
             << TextureClassName(theClassID);
        return false;
    }
    if (_myTextures[theClassID].find(newObjectID) != _myTextures[theClassID].end()) {
        _currentID[theClassID] = newObjectID;
        AC_DEBUG << "#INFO : WaterRepresentation::activateTexture: activating map object id = " << newObjectID
             << " class " << TextureClassName(theClassID);
        return true;
    }
    AC_ERROR << "WaterRepresentation::activateTexture: map object " << newObjectID << " is not defined for"
             << " class " << TextureClassName(theClassID);
    return false;
}


bool
WaterRepresentation::activateTextureIndex(TextureClass theClassID, int newIndex) {

    if (_myTextures.find(theClassID) == _myTextures.end()) {
        cerr << "#ERROR : WaterRepresentation::activateTextureIndex: no texture class "
             << TextureClassName(theClassID) << endl;
        return false;
    }

    map<short,Texture>::iterator it = _myTextures[theClassID].begin();
    for (int i = 0; i < newIndex ; ++i) {
        if (it == _myTextures[theClassID].end()) {
            cerr << "#ERROR : map index " << newIndex << " for"
                 << " class " << TextureClassNames[theClassID]
                 << " is not defined, only " << i << " textures defined." << endl;
            return false;
        }
        ++it;
    }

    _currentID[theClassID] = it->first;

    cerr << "#INFO : WaterRepresentation::activateTextureIndex: new current id for"
             << " class " << TextureClassNames[theClassID] << " is " << it->first << endl;

    return true;
}

bool WaterRepresentation::getTextureIndex(TextureClass theClassID,
                                          short whichObjectID,
                                          int & theResultIndex) {

    if (_myTextures.find(theClassID) == _myTextures.end()) {
        cerr << "#ERROR : WaterRepresentation::getTextureIndex: no texture class "
             << TextureClassName(theClassID) << endl;
        return false;
    }

    int myIndex = 0;
    for (map<short,Texture>::iterator it = _myTextures[theClassID].begin();
         it != _myTextures[theClassID].end();
         ++it)
    {
        if (it->first == whichObjectID) {
            theResultIndex = myIndex;
            //cerr << "#INFO : returning found index " << theResultIndex << endl;
            return true;
        }
        ++myIndex;
    }
    cerr << "#INFO : object not found" << theResultIndex << endl;
    return false;
}


bool
WaterRepresentation::activateOtherTexture(TextureClass theClassID, int theIndexOffset) {

    if (_myTextures.find(theClassID) == _myTextures.end()) {
        cerr << "#ERROR : WaterRepresentation::activateOtherTexture: no texture class "
             << TextureClassName(theClassID) << endl;
        return false;
    }

    int myIndex;
    if (getTextureIndex(theClassID, _currentID[theClassID], myIndex)) {
        return activateTextureIndex(theClassID, myIndex + theIndexOffset);
    }
    return false;
}

bool
WaterRepresentation::initRender() {
    AC_DEBUG << "WaterRepresentation::initRender()";

    assert(_vertexBuffer);

    // check extensions
    if (!initExtensions("GL_NV_vertex_array_range")) {
        return false;
    }
    if (!initExtensions("GL_NV_fence")) {
        return false;
    }

    if (!initExtensions("GL_NV_texgen_reflection")) {
        return false;
    }
     if (!initExtensions("GL_ARB_texture_cube_map")) {
        return false;
    }
    if (!initExtensions("GL_SGIS_generate_mipmap")) {
        return false;
    }
    if (!initExtensions("GL_ARB_multitexture")) {
        return false;
    }
    if (!initExtensions("GL_ARB_texture_env_add")) {
        return false;
    }

    glShadeModel( GL_SMOOTH );

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glDisable( GL_CULL_FACE );
    glEnable( GL_NORMALIZE );

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);

    glVertexArrayRangeNV((_dataHeight+1)*(_dataWidth+1)*VERTEX_DATA_STRIDE*sizeof(GLfloat),
                         _vertexBuffer);

#ifdef USE_VAR_FENCE // XXX never defined
    for(int i=0; i < NUM_VAR_BUFFERS; i++) {
        glGenFencesNV(1, &_varBuffer[i]._fence);
    }
#endif

    _cubeMapSideID[0] = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
    _cubeMapSideID[1] = GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
    _cubeMapSideID[2] = GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
    _cubeMapSideID[3] = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
    _cubeMapSideID[4] = GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
    _cubeMapSideID[5] = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;

    AC_DEBUG << "WaterRepresentation::initRender() finished";

    return true;
}



void
WaterRepresentation::preRender() {

    copyWaterDataToVertexBuffer();
}

void
WaterRepresentation::renderSurface() {

    if (hasTexture(surfacemaps, _currentID[surfacemaps])) {
        // set up texture
        Texture & myTexture = currentTexture(surfacemaps);

        glActiveTexture(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // GL_DECAL);
        assert( myTexture.myID);
        glBindTexture(GL_TEXTURE_2D, myTexture.myID);
    } else {
        cerr << "#WARNING: WaterRepresentation::renderSurface(): can't find fileObject class "
            << TextureClassName(surfacemaps)
            << " id " <<  _currentID[surfacemaps] << endl;
        glActiveTexture(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float   z = SURFACE_LEVEL;

    Vector2f    v0, v1;
    dataCoordToScreenCoord(Vector2f(float(_dataOffsetX), float(_dataOffsetY)), v0);
    dataCoordToScreenCoord(Vector2f(float(_dataOffsetX + _dataWidth), float(_dataOffsetY+_dataHeight)), v1);

    glColor4f(1, 1, 1, _surfaceOpacity);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.f, 0.f);
    glVertex3f(v0[0], v0[1], z);
    glTexCoord2f(1.f, 0.f);
    glVertex3f(v1[0], v0[1], z);
    glTexCoord2f(1.f, 1.f);
    glVertex3f(v1[0], v1[1], z);
    glTexCoord2f(0.f, 1.f);
    glVertex3f(v0[0], v1[1], z);
    glEnd();

    glAlphaFunc(GL_ALWAYS, 0.0f);
    glDisable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void
WaterRepresentation::render() {

    MAKE_SCOPE_TIMER(WaterRepresentation_render);

    // XXX dunno yet
    //setDefaultGLState();

    /*
    if (_drawWireFrame) {
        glPolygonMode( GL_FRONT, GL_LINE );
        glPolygonMode( GL_BACK, GL_LINE );
    } else {
        glPolygonMode( GL_FRONT, GL_FILL );//solid
        glPolygonMode( GL_BACK, GL_FILL );
    }
    */



    glActiveTexture(GL_TEXTURE0_ARB);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);

    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    glDisable( GL_DEPTH_TEST );

    int stripLength = computeStripLength();
    int numLinesPerBuffer = computeNumLinesPerBuffer();

    if ((_drawCaustics) || (_drawRefractions)) {

        glActiveTexture(GL_TEXTURE0_ARB);
        glClientActiveTexture(GL_TEXTURE0_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnable(GL_TEXTURE_2D);

        if (_drawRefractions) {
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        } else{
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }

        glBindTexture(GL_TEXTURE_2D, currentTexture(floormaps).myID);

        glEnableClientState(GL_COLOR_ARRAY);

        glTranslatef(0, 0, WATER_DEPTH);

        //  although currently not used, we keep the code to be able to reenable it again
        for (int currentBuffer = 0; currentBuffer < NUM_VAR_BUFFERS; currentBuffer ++) {

            GLfloat *v = _varBuffer[currentBuffer]._pointer;

            glVertexPointer(3, GL_FLOAT, VERTEX_DATA_STRIDE * sizeof(GLfloat), v);
            glNormalPointer(GL_FLOAT, VERTEX_DATA_STRIDE * sizeof(GLfloat), (v + 3) );
            glTexCoordPointer(2, GL_FLOAT, VERTEX_DATA_STRIDE * sizeof(GLfloat), (v + 6));
            glColorPointer(3, GL_FLOAT, VERTEX_DATA_STRIDE * sizeof(GLfloat), (v + 8) );

            for (int currentStrip = 0; currentStrip < numLinesPerBuffer; currentStrip++) {
                GLuint * strip = &_stripIndices[currentStrip * stripLength];

                glDrawElements(GL_QUAD_STRIP, stripLength,
                        GL_UNSIGNED_INT, strip);

            }
        }

        glTranslatef(0, 0, -WATER_DEPTH);

        glDisableClientState(GL_COLOR_ARRAY);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);

        glEnable( GL_BLEND );
        glEnable( GL_DEPTH_TEST );
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    }

    if (_drawReflections) {

        glActiveTexture(GL_TEXTURE0_ARB);
        glClientActiveTexture(GL_TEXTURE1_ARB);
        //Enable cube mapping.
        glEnable(GL_TEXTURE_CUBE_MAP_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);

        //Enable cube mapping
    	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);

        //bind the texture.
        Texture & currentEnvironmentMap = currentTexture(cubemaps);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, currentEnvironmentMap.myID);

        glEnableClientState(GL_COLOR_ARRAY);

        Texture & currentSurface = currentTexture(surfacemaps);

        for (int currentBuffer = 0; currentBuffer < NUM_VAR_BUFFERS; currentBuffer ++) {

            GLfloat *v = _varBuffer[currentBuffer]._pointer;

            glVertexPointer(3, GL_FLOAT, VERTEX_DATA_STRIDE * sizeof(GLfloat), v);
            if (currentSurface.myID) {
                glTexCoordPointer(2, GL_FLOAT, VERTEX_DATA_STRIDE * sizeof(GLfloat), (v + 6));
            }

            glNormalPointer(GL_FLOAT, VERTEX_DATA_STRIDE * sizeof(GLfloat), (v + 3) );
            glColorPointer(4, GL_FLOAT, VERTEX_DATA_STRIDE * sizeof(GLfloat), (v + 8) );

            for (int currentStrip = 0; currentStrip < numLinesPerBuffer;
                 currentStrip++)
            {
                GLuint * strip = &_stripIndices[currentStrip * stripLength];

                glDrawElements(GL_QUAD_STRIP, stripLength,
                               GL_UNSIGNED_INT, strip);
            }
        }
    }

    if (_drawCaustics) {
        glEnable( GL_DEPTH_TEST );
        glDisable( GL_BLEND );
    }

    if (_drawReflections) {
        //Disable cube mapping
        glActiveTexture(GL_TEXTURE0_ARB);
        glClientActiveTexture(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
    }

    if (_renderSurfaceEnabled) {
        renderSurface();
    }
}


void
WaterRepresentation::setDefaultGLState() {

    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPolygonMode( GL_FRONT, GL_FILL );//solid
    glPolygonMode( GL_BACK, GL_FILL );

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);

    glActiveTexture(GL_TEXTURE0_ARB);
    glClientActiveTexture(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);

    glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // texture unit 2

    glActiveTexture(GL_TEXTURE1_ARB);
    glClientActiveTexture(GL_TEXTURE1_ARB);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);

    glActiveTexture(GL_TEXTURE0_ARB);
}

}; // namespace y60
