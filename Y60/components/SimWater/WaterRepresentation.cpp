//=============================================================================
//
// Copyright (C) 2000-2002, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>

#include "WaterRepresentation.h"
#include "WaterSimulation.h"

#include <asl/Logger.h>
#include <asl/numeric_functions.h>

#include <GL/glut.h>
#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#include <paintlib/Filter/plfilterresizebilinear.h>


#include <stdio.h>
#include <values.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <assert.h>
#include <list>
#include <vector>

using namespace std;  // automatically added!
using namespace asl;  // manually added!

#ifndef SQR
#define SQR(a) ((a) * (a))
#endif

#ifndef ABS
#define ABS(a) (((a) < 0) ? (-(a)) : (a))
#endif


namespace video {


// FOR THE FAMOUS WHISKY THING:
#define RUNNING_WATER_TEST

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


// only this number of cracks will be rendered with smooth corners, the others
// will only be erased (sharp edges)
const int MAX_DRAWABLE_CRACKS = 2000;

struct Vec3 {
    Vec3(float theX=0, float theY=0, float theZ=0) :
        x(theX), y(theY), z(theZ)
    {
    }
    float x, y, z;
};


// special NVIDIA extension, ins special NVIDIA GL/gl.h:
extern "C" {
void glVertexArrayRangeNV (GLsizei size, const GLvoid *pointer);
}

inline float minimum(float a, float b) {
	return a < b ? a : b;
}
inline float maximum(float a, float b) {
	return a > b ? a : b;
}
inline float clamp(float v, float min, float max) {
    if (v > max) {
        v = max;
    } else
    if (v < min) {
        v = min;
    }
    return v;
}

WaterRepresentation::WaterRepresentation() :
    _vertexBuffer(0),
    _dataOffsetX(0),
    _dataOffsetY(0),
    _dataWidth(0),
    _dataHeight(0),
    _displayWidth(100),
    _displayHeight(100),
    _displayOffsetX(0),
    _displayOffsetY(0),
    _bufferAllocator(0),
    _waterSimulation(0),
    _stripIndices(0),
    _drawWireFrame(false),
    _drawCaustics(true),
    _drawReflections(true),
    _drawRefractions(false), //  done by caustics
    _reflectionAlphaBias(0.05f),
    _normalMax(.5f),
    _reflectionAlphaScale(1.f -  0.05f),
    _refractionScale(1.f),
    _reflectionNormalScale(.2),
    _causticBias(.75f),
    _causticScale(0.f),
    _causticSqrScale(1.f),
    _causticNegativeScale(0.25f),
    _objectPosScaleX(1.),
    _objectPosScaleY(1.),
    _objectPosOffsetX(0.),
    _objectPosOffsetY(0.),
    _renderSurfaceEnabled(false),
    _crackWidth(.5),
    _innerIceThickness(2.),
    _outerIceThickness(4.),
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
            free(_bufferAllocator);
            _vertexBuffer= 0;
        }
    }
    if (_bufferAllocator) {
        delete _bufferAllocator;
        _bufferAllocator = 0;
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
    _reflectionNormalScale = .2;
    _causticBias = .75f;
    _causticScale = 0.f;
    _causticSqrScale = 1.f;
    _causticNegativeScale = 0.25f;
}


// init internal data to fit water array size
void    
WaterRepresentation::init(WaterSimulation * waterSim, int width, int height,
                          int dataOffsetX, int dataOffsetY, 
                          int sceneDisplayWidth, int sceneDisplayHeight,
                          int displayWidth, int displayHeight,
                          int displayOffsetX, int displayOffsetY,
                          BufferAllocator * bufferAllocator) 
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
                                    ((double)(_dataHeight) / 2.f);  // ypos

                vertexLinePtr[0] += (float) _displayOffsetX;
                vertexLinePtr[1] += (float) _displayOffsetY;

                vertexLinePtr[2] = - 200;
                // z of normal
                vertexLinePtr[5] = 1;

                vertexLinePtr += VERTEX_DATA_STRIDE;
            }
        }
    }

    /*
    fill(_mySceneObject, _mySceneObject + SceneSyncMaster::NUM_SCENE_OBJECTS, 
         SceneSyncMaster::SceneObject());
    */
    
    //_objectPosScaleX = (double(_dataWidth) / 10000.);
    //_objectPosScaleY = (double(_dataHeight) / 10000.);
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
WaterRepresentation::dataCoordToScreenCoord(const Vector2f & theDataCoord, Vector2f & screenCoord) {
    //screenCoord.x = theDataCoord.x - float(_dataWidth)/2. + _displayOffsetX;
    //screenCoord.y = theDataCoord.y - float(_dataHeight)/2. + _displayOffsetY;
    screenCoord[0] = theDataCoord[0] - float(_dataWidth)/2. + _displayOffsetX - _dataOffsetX;
    screenCoord[1] = theDataCoord[1] - float(_dataHeight)/2. + _displayOffsetY - _dataOffsetY;
}


/*
void
WaterRepresentation::setSceneObjects(const SceneSyncMaster::SceneObject 
                                     theObjects[SceneSyncMaster::NUM_SCENE_OBJECTS])
{
    copy(theObjects, theObjects + SceneSyncMaster::NUM_SCENE_OBJECTS, _mySceneObject);
}

bool
WaterRepresentation::addCrack(float theIntensity, const Vector2f & theOrigin, 
        const Vector2f & theDirection, int theNumSegments, float theSegmentSize, 
        float theVariation)
{
    //cerr << "WaterRepresentation::addCrack origin= " << theOrigin.x << "," << theOrigin.y << endl;
    
    _crackSimulator.addCrackPair(theIntensity, theOrigin, theDirection, theNumSegments, 
            theSegmentSize, theVariation);
    //cerr << "    numCracks= " << _crackSimulator.getNumCracks() 
    //     << " (num segments=" << _crackSimulator.computeNumCrackSegments() << endl;
     return true;
 }

void
WaterRepresentation::clearCracks() {
    _crackSimulator.clear();
    _crackSimulator.getCrackList().clear();
}

void
WaterRepresentation::setCracks(CrackList & theCracks) {
    _crackSimulator.getCrackList().clear();
    CrackList::iterator it = theCracks.begin();
    while (it != theCracks.end()) {
        _crackSimulator.getCrackList().push_back(*it++);
    }
}
*/

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

//            float normalX = ((*z1)[j][i-3] * .5f + (*z1)[j][i-2]) - 
//                            ((*z1)[j][i-1]   + (*z1)[j][i] * .5f);
//            float normalY = ((*z1)[j-3][i] * .5f + (*z1)[j-2][i]) -
//                            ((*z1)[j-1][i]   + (*z1)[j][i] * .5f);
//            normalX = ((*z1)[j-2][i-3] + (*z1)[j-1][i-2]) - 
//                      ((*z1)[j][i-1]   + (*z1)[j][i]);
//            normalY = ((*z1)[j-3][i-2]   + (*z1)[j-2][i-1]) -
//                      ((*z1)[j-1][i]   + (*z1)[j][i]);
                            
//			float normalX = ((*z1)[j-1][i-1] - (*z1)[j-1][i-1]);
//			float normalY = ((*z1)[j-1][i] - (*z1)[j][i]);

            //const float normalMax = .5f;
  
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

//vertexLinePtr[6] = ((float)i) * texFactorX;
//vertexLinePtr[7] = ((float)(_displayHeight-j)) * texFactorY;

            float heightValue = maximum(-1.f, minimum(1.f, waterDataLine[i]));
            //float colorValue = .75f + .5f * ((heightValue > 0) ? 
            //                   (SQR(heightValue)) :
            //                   -(SQR(heightValue)));
            //float colorValue1 = .75f + 0.25f * heightValue;
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
WaterRepresentation::initExtensions(char* extension) {
  if (!glutExtensionSupported(extension)) {
    cout <<"requires the "<<extension<<" OpenGL extension to work.\n";
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

    //glEnable(GL_TEXTURE_CUBE_MAP_ARB);

    Texture & myTexture = _myTextures[theClassID][theObjectID];

    if (!myTexture.myID) {
        glGenTextures(1, &myTexture.myID);
    }

	//Bind the texture.
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, myTexture.myID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//N.B.!! LOOK HERE! Auto mipmap extension used.
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

	PLAnyPicDecoder myDecoder;

	//Loop to load all images.

    int & imageWidth = myTexture.myWidth;
    int & imageHeight = myTexture.myHeight;
    
    imageWidth=0;
    imageHeight = 0;

	for(unsigned int i=0; i<6; i++) {
        PLAnyBmp *  myBmp = new PLAnyBmp;
        
        cerr << "Loading bitmap for cube map '" 
            << textureFileNames[i].c_str() << "'..." << flush;
        try {
            myDecoder.MakeBmpFromFile (textureFileNames[i].c_str(), myBmp);
        } catch (...) {
            cerr << "FAILED" << endl;
            return false;
        }
        cerr << "OK" << endl;

		//Store that section.
		glTexImage2D(_cubeMapSideID[i], 0, 
                     GL_RGB8, myBmp->GetWidth(), 
                     myBmp->GetHeight(),
					 0, GL_RGBA, GL_UNSIGNED_BYTE, myBmp->GetPixels());

        if (imageWidth) {
            if ((imageWidth != myBmp->GetWidth()) || (imageHeight != myBmp->GetHeight())) {
                cerr << "#ERROR : Cubve map images MUST all be of same size!";
                exit(-1);
            }
        }
        imageWidth = myBmp->GetWidth();
        imageHeight = myBmp->GetHeight();

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

    cerr << "#INFO : loading texture map class " << TextureClassName(theClassID) 
         << " into map object= " << theObjectID << endl;

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
    PLAnyBmp *  myBmp = new PLAnyBmp;

    cerr << "Loading bitmap for texture '" 
         << theTextureFileName << "' .." << flush;
    try {
        myDecoder.MakeBmpFromFile (theTextureFileName, myBmp);
    } catch (...) {
        cerr << "FAILED" << endl;
        return false;
    }
    cerr << "OK" << endl;

    if (!asl::powerOfTwo(myBmp->GetWidth()) || !asl::powerOfTwo(myBmp->GetHeight())) {
        cerr << "#WARNING: texture " << theTextureFileName << " is not 'powerOfTwo', but "
             << myBmp->GetWidth() << "x" << myBmp->GetHeight()
             << ". Resizing the texture internally..." << endl;
        myBmp->ApplyFilter(PLFilterResizeBilinear(256, 256));
    }
    
    //Store that section. ..
    glTexImage2D(GL_TEXTURE_2D, 0, 
            (myBmp->HasAlpha()) ? GL_RGBA8 : GL_RGB8, myBmp->GetWidth(), 
            myBmp->GetHeight(),
            0, (myBmp->HasAlpha()) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, myBmp->GetPixels());

    myTexture.myWidth = myBmp->GetWidth();
    myTexture.myHeight = myBmp->GetHeight();

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

    /*
    cerr << "#INFO : WaterRepresentation::activateTextureIndex: activating map index= " << index 
             << " class " << (void*) TextureClassNames[theClassID] << endl;
             */

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
    //cerr << "#INFO :  WaterRepresentation::getTextureIndex: looking up map index for object = " << whichObjectID
    //         << " class " << TextureClassName(theClassID) << endl;

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

#ifdef USE_VAR_FENCE
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

#if 0
void 
WaterRepresentation::renderSceneObjects() {
    
    //float pixelToScreenX = float(_dataWidth) / float(_displayWidth);
    //float pixelToScreenY = float(_dataHeight) / float(_displayHeight);
    //float pixelToScreenX = float(_displayWidth) / float(_dataWidth);
    //float pixelToScreenY = float(_displayHeight) / float(_dataHeight);
    
    //cerr << "pixelToScreenX= " << pixelToScreenX << endl;
    //cerr << "pixelToScreenY= " << pixelToScreenY << endl;
    
    float pixelToScreenX = _objectPosScaleX;
    float pixelToScreenY = _objectPosScaleY;
    float pixelToScreenZ = 1.0;
    int numSceneObjectsRendered = 0;


    for (int i=0; i < SceneSyncMaster::NUM_SCENE_OBJECTS; i++) {

        if (_mySceneObject[i]._classID == SceneSyncMaster::SceneObject::NoObject) {
            continue;
        }

        float minV[3];
        float maxV[3];
        if (_mySceneObject[i]._classID != SceneSyncMaster::SceneObject::NoObject) {
            //cerr << "Scene object #" << i << " has xpos: " << _mySceneObject[i]._xPos 
            //     << " and ypos: " << _mySceneObject[i]._yPos
            //     << " and zpos: " << _mySceneObject[i]._zPos << endl;

            float   xPos = _mySceneObject[i]._xPos;
            float   yPos = 10000.f - _mySceneObject[i]._yPos;
            float   zPos = _mySceneObject[i]._zPos;
            
            minV[0] = (float(xPos-_mySceneObject[i]._xSize/2.)) * 
                        pixelToScreenX + _objectPosOffsetX;
            minV[1] = (float(yPos-_mySceneObject[i]._ySize/2.)) *
                        pixelToScreenY + _objectPosOffsetY;
            minV[2] = float(zPos-_mySceneObject[i]._zSize/2.) * 
                        pixelToScreenZ;
            maxV[0] = (float(xPos+_mySceneObject[i]._xSize/2.)) * 
                        pixelToScreenX + _objectPosOffsetX;
            maxV[1] = (float(yPos+_mySceneObject[i]._ySize/2.)) *
                        pixelToScreenY + _objectPosOffsetY;
            maxV[2] = float(zPos+_mySceneObject[i]._zSize/2.) * 
                        pixelToScreenZ;
        }
        
        minV[2] = 0;
        
        float zOffset = WATER_LEVEL;
        
        switch (_mySceneObject[i]._classID) {
            case SceneSyncMaster::SceneObject::Marker:

                numSceneObjectsRendered++;

                //glDepthFunc(GL_ALWAYS);
//                glEnable( GL_DEPTH_TEST );

                //glDisable( GL_BLEND );
                glColor4f(1,1,1,1.0);

                //cerr << "x1= " << (_mySceneObject[i]._xPos-_mySceneObject[i]._xSize/2) * 
                //        pixelToScreenX << endl;
                //cerr << "y1= " << (_mySceneObject[i]._yPos-_mySceneObject[i]._ySize/2) * 
                //        pixelToScreenY << endl;
                /*
                   glRectf(float(_mySceneObject[i]._xPos-_mySceneObject[i]._xSize/2.) * pixelToScreenX, 
                   float(_mySceneObject[i]._yPos-_mySceneObject[i]._ySize/2.) * pixelToScreenY,
                   float(_mySceneObject[i]._xPos+_mySceneObject[i]._xSize/2.) * pixelToScreenX, 
                   float(_mySceneObject[i]._yPos+_mySceneObject[i]._ySize/2.) * pixelToScreenY);
                 */
                if (_mySceneObject[i]._fileClassID) {
                    if (hasTexture(TextureClass(_mySceneObject[i]._fileClassID),_mySceneObject[i]._fileObjectID)) {
                        // set up texture
                        Texture & myTexture = _myTextures[TextureClass(_mySceneObject[i]._fileClassID)][_mySceneObject[i]._fileObjectID];
                        glActiveTextureARB(GL_TEXTURE0_ARB);
                        glEnable(GL_TEXTURE_2D);

                        if (_drawRefractions) {
                            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
                        } else{ 
                            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                        }
                        assert( myTexture.myID);
                        glBindTexture(GL_TEXTURE_2D, myTexture.myID);
                    } else {
                        cerr << "#WARNING: WaterRepresentation::renderSceneObjects(): can't find fileObject class "
                            << TextureClassName(TextureClass(_mySceneObject[i]._fileClassID))
                            << " id " << _mySceneObject[i]._fileObjectID 
                            << " referenced in " << i << "th object with class " << _mySceneObject[i]._classID
                            << " id " << _mySceneObject[i]._objectID << endl;

                    }
                } else {
                    glActiveTextureARB(GL_TEXTURE0_ARB);
                    glDisable(GL_TEXTURE_2D);
                }

                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glAlphaFunc(GL_GREATER, 0.1f);
                glEnable(GL_ALPHA_TEST);

                glBegin(GL_POLYGON);
                glTexCoord2f(0.f, 0.f);
                glVertex3f(minV[0],minV[1],minV[2] + zOffset);
                glTexCoord2f(1.f, 0.f);
                glVertex3f(maxV[0],minV[1],minV[2] + zOffset);
                glTexCoord2f(1.f, 1.f);
                glVertex3f(maxV[0],maxV[1],minV[2] + zOffset);
                glTexCoord2f(0.f, 1.f);
                glVertex3f(minV[0],maxV[1],minV[2] + zOffset);
                glTexCoord2f(0.f, 0.f);
                glVertex3f(minV[0],minV[1],minV[2] + zOffset);
                glEnd();

//                glDepthFunc( GL_LEQUAL );
                glEnable( GL_BLEND );
                glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
                glAlphaFunc(GL_ALWAYS, 0.0f);
                glDisable(GL_ALPHA_TEST);

                break;
            case SceneSyncMaster::SceneObject::NoObject:
                break;
            default:
                cerr << "#WARNING: unknown scene object type " << _mySceneObject[i]._objectID 
                    << " ignored." << endl;
                break;
        }
    }

    //AC_DB("Rendered " << numSceneObjectsRendered << " scene objects");
}
#endif

#if 0
void 
WaterRepresentation::renderSceneObjects() {
        
    float pixelToScreenX = _objectPosScaleX;
    float pixelToScreenY = _objectPosScaleY;
    float pixelToScreenZ = 1.0;
    int numSceneObjectsRendered = 0;


    for (int i=0; i < SceneSyncMaster::NUM_SCENE_OBJECTS; i++) {

        if (_mySceneObject[i]._classID == SceneSyncMaster::SceneObject::NoObject) {
            continue;
        }

        float minV[3];
        float maxV[3];
        Vec3  objCenter;

        
        if (_mySceneObject[i]._classID != SceneSyncMaster::SceneObject::NoObject) {
            //cerr << "Scene object #" << i << " has xpos: " << _mySceneObject[i]._xPos 
            //     << " and ypos: " << _mySceneObject[i]._yPos
            //     << " and zpos: " << _mySceneObject[i]._zPos << endl;

            objCenter.x = _mySceneObject[i]._xPos;
            objCenter.y = 10000.f - _mySceneObject[i]._yPos;
            objCenter.z = _mySceneObject[i]._zPos;

            //float   xPos = _mySceneObject[i]._xPos;
            //float   yPos = 10000.f - _mySceneObject[i]._yPos;
            //float   zPos = _mySceneObject[i]._zPos;
            
            minV[0] = float(-_mySceneObject[i]._xSize/2.);
            minV[1] = float(-_mySceneObject[i]._ySize/2.);
            minV[2] = float(-_mySceneObject[i]._zSize/2.);
            maxV[0] = float(_mySceneObject[i]._xSize/2.);
            maxV[1] = float(_mySceneObject[i]._ySize/2.);
            maxV[2] = float(_mySceneObject[i]._zSize/2.);
        }
        
        glPushMatrix();

        glTranslatef(_objectPosOffsetX, _objectPosOffsetY, 0);
        glTranslatef(objCenter.x * pixelToScreenX, objCenter.y * pixelToScreenY, 
                objCenter.z * pixelToScreenZ);
        glRotatef(float(_mySceneObject[i]._heading) / 10.f, 0, 0, 1.);
        glScalef(pixelToScreenX, pixelToScreenY, pixelToScreenZ);
       
        minV[2] = 0;
        float zOffset = WATER_LEVEL;          
        float alphaValue = 1.f;

#ifdef RUNNING_WATER_TEST
        alphaValue = 1.f - float(_mySceneObject[i]._pitch) / 10000.f;
#endif
      
        switch (_mySceneObject[i]._classID) {
            case SceneSyncMaster::SceneObject::Marker:

                numSceneObjectsRendered++;

                //glDepthFunc(GL_ALWAYS);
//                glEnable( GL_DEPTH_TEST );

                //glDisable( GL_BLEND );

                glColor4f(1, 1, 1, alphaValue);

                //cerr << "x1= " << (_mySceneObject[i]._xPos-_mySceneObject[i]._xSize/2) * 
                //        pixelToScreenX << endl;
                //cerr << "y1= " << (_mySceneObject[i]._yPos-_mySceneObject[i]._ySize/2) * 
                //        pixelToScreenY << endl;
                /*
                   glRectf(float(_mySceneObject[i]._xPos-_mySceneObject[i]._xSize/2.) * pixelToScreenX, 
                   float(_mySceneObject[i]._yPos-_mySceneObject[i]._ySize/2.) * pixelToScreenY,
                   float(_mySceneObject[i]._xPos+_mySceneObject[i]._xSize/2.) * pixelToScreenX, 
                   float(_mySceneObject[i]._yPos+_mySceneObject[i]._ySize/2.) * pixelToScreenY);
                 */
                if (_mySceneObject[i]._fileClassID) {
                    if (hasTexture(TextureClass(_mySceneObject[i]._fileClassID),_mySceneObject[i]._fileObjectID)) {
                        // set up texture
                        Texture & myTexture = _myTextures[TextureClass(_mySceneObject[i]._fileClassID)][_mySceneObject[i]._fileObjectID];
                        glActiveTextureARB(GL_TEXTURE0_ARB);
                        if (!_drawWireFrame) {
                            glEnable(GL_TEXTURE_2D);
                        }

                        //if (_drawRefractions) {
                        //    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
                        //} else{ 
                            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                        //}
                        assert( myTexture.myID);
                        glBindTexture(GL_TEXTURE_2D, myTexture.myID);
                    } else {
                        cerr << "#WARNING: WaterRepresentation::renderSceneObjects(): can't find fileObject class "
                            << TextureClassName(TextureClass(_mySceneObject[i]._fileClassID))
                            << " id " << _mySceneObject[i]._fileObjectID 
                            << " referenced in " << i << "th object with class " << _mySceneObject[i]._classID
                            << " id " << _mySceneObject[i]._objectID << endl;

                    }
                } else {
                    glActiveTextureARB(GL_TEXTURE0_ARB);
                    glDisable(GL_TEXTURE_2D);
                }

                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glAlphaFunc(GL_GREATER, 0.01f);
                glEnable(GL_ALPHA_TEST);

                glBegin(GL_POLYGON);
                glTexCoord2f(0.f, 0.f);
                //glVertex3f(minV[0],minV[1],minV[2] + zOffset);
                glVertex3f(maxV[0],maxV[1],minV[2] + zOffset);
                glTexCoord2f(1.f, 0.f);
                glVertex3f(maxV[0],minV[1],minV[2] + zOffset);
                glTexCoord2f(1.f, 1.f);
                //glVertex3f(maxV[0],maxV[1],minV[2] + zOffset);
                glVertex3f(minV[0],minV[1],minV[2] + zOffset);
                glTexCoord2f(0.f, 1.f);
                glVertex3f(minV[0],maxV[1],minV[2] + zOffset);
                glTexCoord2f(0.f, 0.f);
                //glVertex3f(minV[0],minV[1],minV[2] + zOffset);
                glVertex3f(maxV[0],maxV[1],minV[2] + zOffset);
                glEnd();

//                glDepthFunc( GL_LEQUAL );
                glEnable( GL_BLEND );
                glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
                glAlphaFunc(GL_ALWAYS, 0.0f);
                glDisable(GL_ALPHA_TEST);

                break;
            case SceneSyncMaster::SceneObject::NoObject:
                break;
            default:
                cerr << "#WARNING: unknown scene object type " << _mySceneObject[i]._objectID 
                    << " ignored." << endl;
                break;
        }
        
        glPopMatrix();
        
    }

    //AC_DB("Rendered " << numSceneObjectsRendered << " scene objects");
}




void
WaterRepresentation::eraseCrackSegment(CrackSegment & theCrackSegment, 
                                       CrackSegment & theNextCrackSegment, float pos)
{
    
    float xOrigin = theCrackSegment._myOrigin.x;
    float yOrigin = theCrackSegment._myOrigin.y;
    float xDestination = theCrackSegment._myDestination.x;
    float yDestination = theCrackSegment._myDestination.y;
    float startThickness = theCrackSegment._thickness;
    float endThickness = theNextCrackSegment._thickness;
    
    video::Vector2f v[4];
    bool startVertical = fabs(xDestination - xOrigin) > fabs(yDestination - yOrigin);
    bool endVertical = 
            (fabs(theNextCrackSegment._myDestination.x - theNextCrackSegment._myOrigin.x) > 
             fabs(theNextCrackSegment._myDestination.y - theNextCrackSegment._myOrigin.y));
    
    if (startVertical) {
        // thickness is "vertical"   

        v[0].x = xOrigin;
        v[0].y = yOrigin - _crackWidth * startThickness * _innerIceThickness;
        v[1].x = xOrigin;
        v[1].y = yOrigin + _crackWidth * startThickness * _innerIceThickness;
    } else {
        v[0].x = xOrigin - _crackWidth * startThickness * _innerIceThickness;
        v[0].y = yOrigin;
        v[1].x = xOrigin + _crackWidth * startThickness * _innerIceThickness;
        v[1].y = yOrigin;
    }

    if (endVertical) {
        v[2].x = xDestination;
        v[2].y = yDestination + _crackWidth * endThickness * _innerIceThickness;
        v[3].x = xDestination;
        v[3].y = yDestination - _crackWidth * endThickness * _innerIceThickness;
    } else {
        v[2].x = xDestination + _crackWidth * endThickness * _innerIceThickness;
        v[2].y = yDestination;
        v[3].x = xDestination - _crackWidth * endThickness * _innerIceThickness;
        v[3].y = yDestination;
    }
    
    Vector2f    screenCoord;
    
    glBegin(GL_POLYGON);
    for (int i=0; i< 4; i++) {
        dataCoordToScreenCoord(v[i], screenCoord);
        glVertex3f(screenCoord.x, screenCoord.y, RAISED_CRACK_LEVEL);
    }
    glEnd();

    if (_renderCrackField) {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        // visualize center of cracks
        glLineWidth(3.);
        glBegin(GL_LINES);
        dataCoordToScreenCoord(Vector2f(xOrigin, yOrigin), screenCoord);
        glColor4f(1,0,0,1);
        glVertex3f(screenCoord.x+0.5, screenCoord.y+0.5, RAISED_CRACK_LEVEL+2);
        dataCoordToScreenCoord(Vector2f(xDestination, yDestination), screenCoord);
        glColor4f(0,0,1,1);
        glVertex3f(screenCoord.x+0.5, screenCoord.y+0.5, RAISED_CRACK_LEVEL+2);
        glEnd();
        glLineWidth(1.);
        glColor4f(0,1,0,1);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    }
}

void
WaterRepresentation::eraseCrack(Crack & theCrack) {
    int segmentCount = theCrack.getNumSegments();
    for (int i=0; i < segmentCount-1; i++) {
        eraseCrackSegment(theCrack.getSegment(i), theCrack.getSegment(i+1),
                          float(i) / float(segmentCount));
    }
    if (segmentCount > 0) {
        Vector2f            endPos(theCrack.getSegment(segmentCount-1)._myDestination);
        CrackSegment    tempSegment(endPos, endPos, 0., 0, 0);
        
        eraseCrackSegment(theCrack.getSegment(segmentCount-1), 
                         tempSegment, 1);        
    }
}

void
WaterRepresentation::drawCrackSegment(CrackSegment & theCrackSegment, 
                                      CrackSegment & theNextCrackSegment, float pos) 
{

    float xOrigin = theCrackSegment._myOrigin.x;
    float yOrigin = theCrackSegment._myOrigin.y;
    float xDestination = theCrackSegment._myDestination.x;
    float yDestination = theCrackSegment._myDestination.y;
    float startThickness = theCrackSegment._thickness;
    float endThickness = theNextCrackSegment._thickness;
    
    static video::Vec3  v[8];
    static float        intensity[8];
    static float        alpha[8];
    static bool         initialized = false;

    if (!initialized) {
        initialized = true;
        float minIntensity = .35f;
        float maxIntensity = 1.f;

        intensity[0] = maxIntensity;
        intensity[1] = minIntensity;
        intensity[2] = minIntensity;
        intensity[3] = maxIntensity;
        intensity[4] = maxIntensity;
        intensity[5] = minIntensity;
        intensity[6] = minIntensity;
        intensity[7] = maxIntensity;

        float minAlpha = 0.f;
        float maxAlpha = .75f;
        alpha[0] = minAlpha; 
        alpha[1] = maxAlpha; 
        alpha[2] = maxAlpha; 
        alpha[3] = minAlpha; 
        alpha[4] = minAlpha; 
        alpha[5] = maxAlpha; 
        alpha[6] = maxAlpha; 
        alpha[7] = minAlpha; 
        
        v[0].z = CRACK_EDGE_LEVEL_LOW; 
        v[1].z = CRACK_EDGE_LEVEL_HIGH; 
        v[2].z = CRACK_EDGE_LEVEL_HIGH; 
        v[3].z = CRACK_EDGE_LEVEL_LOW; 
        v[4].z = CRACK_EDGE_LEVEL_LOW; 
        v[5].z = CRACK_EDGE_LEVEL_HIGH; 
        v[6].z = CRACK_EDGE_LEVEL_HIGH; 
        v[7].z = CRACK_EDGE_LEVEL_LOW; 

    }

    bool startVertical = fabs(xDestination - xOrigin) > fabs(yDestination - yOrigin);
    bool endVertical = 
            (fabs(theNextCrackSegment._myDestination.x - theNextCrackSegment._myOrigin.x) > 
             fabs(theNextCrackSegment._myDestination.y - theNextCrackSegment._myOrigin.y));
    
    float   innerStartWidth = _crackWidth * startThickness * _innerIceThickness;
    float   outerStartWidth = min(float(innerStartWidth) + 5.f, 
                                  _crackWidth * startThickness * _outerIceThickness);
    //if (outerStartWidth - innerStartWidth > 0.1) {
    //    outerStartWidth = min(float(innerStartWidth) + 5.f,
    //                          3.f * outerStartWidth + (outerStartWidth - innerStartWidth));
    //}
    
    if (startVertical) {
        // thickness is "vertical"   

        v[0].x = xOrigin;
        v[0].y = yOrigin - outerStartWidth;
        v[1].x = xOrigin;
        v[1].y = yOrigin - innerStartWidth;
        
        v[4].x = xOrigin;
        v[4].y = yOrigin + outerStartWidth;
        v[5].x = xOrigin;
        v[5].y = yOrigin + innerStartWidth;
    } else {
        v[0].x = xOrigin - outerStartWidth;
        v[0].y = yOrigin;
        v[1].x = xOrigin - innerStartWidth;
        v[1].y = yOrigin;

        v[4].x = xOrigin + outerStartWidth;
        v[4].y = yOrigin;
        v[5].x = xOrigin + innerStartWidth;
        v[5].y = yOrigin;
    }

    float   innerEndWidth = _crackWidth * endThickness * _innerIceThickness;
    float   outerEndWidth = min(float(innerEndWidth)+10.f,
                                _crackWidth * endThickness * _outerIceThickness);
    //if (outerEndWidth - innerEndWidth > 0.1) {
    //    outerEndWidth = min(float(innerEndWidth) + 5.f,
    //                          3.f * outerEndWidth + (outerEndWidth - innerEndWidth));
    //}
    if (endVertical) {
        v[2].x = xDestination;
        v[2].y = yDestination - innerEndWidth;
        v[3].x = xDestination;
        v[3].y = yDestination - outerEndWidth;
        
        v[6].x = xDestination;
        v[6].y = yDestination + innerEndWidth;
        v[7].x = xDestination;
        v[7].y = yDestination + outerEndWidth;
    } else {
        v[2].x = xDestination - innerEndWidth;
        v[2].y = yDestination;
        v[3].x = xDestination - outerEndWidth;
        v[3].y = yDestination;
        
        v[6].x = xDestination + innerEndWidth;
        v[6].y = yDestination;
        v[7].x = xDestination + outerEndWidth;
        v[7].y = yDestination;
    }
    
    Vector2f    screenCoord;
    
    glBegin(GL_POLYGON);
    for (int i=0; i< 4; i++) {
        glColor4f(intensity[i], intensity[i], intensity[i], alpha[i]);
        dataCoordToScreenCoord(Vector2f(v[i].x, v[i].y), screenCoord);
        glVertex3f(screenCoord.x, screenCoord.y, v[i].z);
    }
    glEnd();
    glBegin(GL_POLYGON);
    for (int i=4; i< 8; i++) {
        glColor4f(intensity[i], intensity[i], intensity[i], alpha[i]);
        dataCoordToScreenCoord(Vector2f(v[i].x, v[i].y), screenCoord);
        glVertex3f(screenCoord.x, screenCoord.y, v[i].z);
    }
    glEnd();

}



void
WaterRepresentation::drawCrack(Crack & theCrack) {
    int segmentCount = theCrack.getNumSegments();
    for (int i=0; i < segmentCount-1; i++) {
        drawCrackSegment(theCrack.getSegment(i), theCrack.getSegment(i+1),
                         float(i) / float(segmentCount));
    }
    if (segmentCount > 0) {
        Vector2f            endPos(theCrack.getSegment(segmentCount-1)._myDestination);
        CrackSegment    tempSegment(endPos, endPos, 0., 0, 0);
        
        drawCrackSegment(theCrack.getSegment(segmentCount-1), 
                         tempSegment, 1);        
    }
}


void 
WaterRepresentation::eraseCracks() {
    //cerr << "#cracks= " << _crackSimulator.getCrackList().size() << endl;
#if 0
/*
    float   x1 = _displayOffsetX - (_dataWidth / 2);
    float   y1 = _displayOffsetY - (_dataHeight / 2);
    float   x2 = _displayOffsetX + (_dataWidth / 2);
    float   y2 = _displayOffsetY + (_dataHeight / 2);
*/    
    float   z = CLEAR_LEVEL;
    
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND );
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);

    Vector2f    v0, v1;
    dataCoordToScreenCoord(Vector2f(_dataOffsetX, _dataOffsetY), v0);
    dataCoordToScreenCoord(Vector2f(_dataOffsetX+_dataWidth, _dataOffsetY+_dataHeight), v1);
            
    glColor4f(0, 1, 0, 0);
    glBegin(GL_POLYGON);
    glVertex3f(v0.x, v0.y, z);
    glVertex3f(v1.x, v0.y, z);
    glVertex3f(v1.x, v1.y, z);
    glVertex3f(v0.x, v1.y, z);
    glEnd();
/*    
    glColor4f(0, 1, 0, 0);
    glBegin(GL_POLYGON);
    glVertex3f(x1, y1, z);
    glVertex3f(x2, y1, z);
    glVertex3f(x2, y2, z);
    glVertex3f(x1, y2, z);
    glEnd();
*/    
#endif
    glDisable(GL_BLEND );
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    glAlphaFunc(GL_ALWAYS, 0.0f);
    glDisable(GL_ALPHA_TEST);
    
    CrackList::iterator it = _crackSimulator.getCrackList().begin();
    
    glDisable(GL_TEXTURE_2D);
    glColor4f(0., 1., 0., 1);  

    if (it != _crackSimulator.getCrackList().end()) {
        glColor4f(0., 1., 0., 0.1 + 2. * it->getSegment(0)._thickness);
        while (it != _crackSimulator.getCrackList().end()) {

            eraseCrack(*it);
            it++;
        }
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable( GL_BLEND );
}

void
WaterRepresentation::renderCracks() {
    
    CrackList::iterator it = _crackSimulator.getCrackList().begin();
    
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.1, .1, .1, .8);  

    int crackCount = 0;
    
    while (it != _crackSimulator.getCrackList().end()) {
        if (++crackCount > MAX_DRAWABLE_CRACKS) {
            break;
        }
        drawCrack(*it);
        it++;
    }

    // visualize crack field
    if (_renderCrackField) {
        glColor4f(0.1, 1, .1, .6);  

        for (int j=0; j<_crackSimulator.getHeight(); j++) {
            for (int i=0; i<_crackSimulator.getWidth(); i++) {
                if (_crackSimulator.getCrackField(i, j)._crackIndex>= 0) {
                    Vector2f    screenCoord1;
                    Vector2f    screenCoord2;

                    dataCoordToScreenCoord(Vector2f(i, j), screenCoord1);
                    dataCoordToScreenCoord(Vector2f(i+1, j+1), screenCoord2);
        
                    glBegin(GL_POLYGON);
                    glVertex3f(screenCoord1.x+0.5, screenCoord1.y+0.5, RAISED_CRACK_LEVEL+1);
                    glVertex3f(screenCoord2.x+0.5, screenCoord1.y+0.5, RAISED_CRACK_LEVEL+1);
                    glVertex3f(screenCoord2.x+0.5, screenCoord2.y+0.5, RAISED_CRACK_LEVEL+1);
                    glVertex3f(screenCoord1.x+0.5, screenCoord2.y+0.5, RAISED_CRACK_LEVEL+1);
                    glEnd();
                }
            }
        }
    }
}

#endif

void 
WaterRepresentation::renderSurface() {
    
    if (hasTexture(surfacemaps, _currentID[surfacemaps])) {
        // set up texture
        Texture & myTexture = currentTexture(surfacemaps);

        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // GL_DECAL);
        assert( myTexture.myID);
        glBindTexture(GL_TEXTURE_2D, myTexture.myID);
    } else {
        cerr << "#WARNING: WaterRepresentation::renderSurface(): can't find fileObject class "
            << TextureClassName(surfacemaps)
            << " id " <<  _currentID[surfacemaps] << endl;
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float   z = SURFACE_LEVEL;

    Vector2f    v0, v1;
    dataCoordToScreenCoord(Vector2f(_dataOffsetX, _dataOffsetY), v0);
    dataCoordToScreenCoord(Vector2f(_dataOffsetX+_dataWidth, _dataOffsetY+_dataHeight), v1);
            
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

    setDefaultGLState();

    if (_drawWireFrame) {
        glPolygonMode( GL_FRONT, GL_LINE );
        glPolygonMode( GL_BACK, GL_LINE );
    } else {
        glPolygonMode( GL_FRONT, GL_FILL );//solid
        glPolygonMode( GL_BACK, GL_FILL );
    }
    
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
    
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    glDisable( GL_DEPTH_TEST );
    
    int stripLength = computeStripLength();
    int numLinesPerBuffer = computeNumLinesPerBuffer();

    if ((_drawCaustics) || (_drawRefractions)) {

        glActiveTextureARB(GL_TEXTURE0_ARB);
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
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
    
    //renderSceneObjects();
    
    if (_drawReflections) {
        
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glClientActiveTextureARB(GL_TEXTURE1_ARB);
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

        //-------
        //if (_surfaceTextureID[_currentSurfaceIndex]) {  
/*
        if (currentTexture(surfacemaps).myID) {  
            glActiveTextureARB(GL_TEXTURE1_ARB);
            glClientActiveTextureARB(GL_TEXTURE1_ARB);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnable(GL_TEXTURE_2D);

//        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            glBindTexture(GL_TEXTURE_2D, currentTexture(surfacemaps).myID);
        }
*/        
        //-------
        
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
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
/*
        if (currentTexture(surfacemaps).myID) {  
            glActiveTextureARB(GL_TEXTURE1_ARB);
            glClientActiveTextureARB(GL_TEXTURE1_ARB);
            glDisable(GL_TEXTURE_2D);
        
            glActiveTextureARB(GL_TEXTURE0_ARB);
        }
*/        
    }

    if (_renderSurfaceEnabled) {
        //eraseCracks();
        renderSurface();
        //renderCracks();
        //renderSceneObjects();
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
    
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glClientActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    
    //glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);
    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // texture unit 2
    
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);

    glActiveTextureARB(GL_TEXTURE0_ARB);
}




}; // namespace video
