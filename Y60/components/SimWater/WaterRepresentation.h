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

#ifndef _WaterRepresentation_h_
#define _WaterRepresentation_h_

#include "WaterSimulation.h"

#include <asl/Ptr.h>
#include <asl/Vector234.h>
#include <asl/string_functions.h>

#include <GL/gl.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <assert.h>
#include <string>
#include <map>


namespace y60 {

class BufferAllocator {
public:
    BufferAllocator() {}
    virtual ~BufferAllocator(){}
    
    virtual void *  allocateSingleBuffer(unsigned int numBytes) = 0;
    virtual bool    freeSingleBuffer() = 0;
};

typedef asl::Ptr<BufferAllocator> BufferAllocatorPtr;

const int NUM_VAR_BUFFERS = 10;
// 3 vertex position
// 3 vertex normal
// 2 texture coords
// 4 vertex color+alpha :
const int VERTEX_DATA_STRIDE = 12;


//class WaterSimulation;


class WaterRepresentation {
    
public:
    enum TextureClass { first = 1, floormaps = 1, surfacemaps = 2, cubemaps = 3, 
                        puzzlemaps = 4 , last = 4
                      };
    // do not forget to add name when adding new values to TextureClass enum
    static const std::string TextureClassNames[];

    static std::string TextureClassName(TextureClass c) {
        if (c < first && c > last) {
            return asl::as_string(short(c));
        }
        return TextureClassNames[c];
    }

    WaterRepresentation();
    
    ~WaterRepresentation();

//  make sure that (height % NUM_BUFFERS == 0) !!!

    // init internal data to fit water array size
    void    init(WaterSimulationPtr waterSim, int width, int height, 
                 int dataOffsetX, int dataOffsetY,
                 int sceneDisplayWidth, int sceneDisplayHeight,
                 int displayWidth, int displayHeight,
                 int displayOffsetX, int displayOffsetY,
                 BufferAllocatorPtr bufferAllocator = BufferAllocatorPtr(0) );
    
    void setDataOffset(const asl::Vector2i & theOffset);
    void setDisplayOffset(const asl::Vector2i & theOffset);

    void    reset();
    void    resetParameters();

    // update water data
    void    update();
    
    // render in opengl
    void    preRender();    // setup data and vertex buffer
    void    render();

    bool loadTexture(TextureClass theClassID, short theObjectID, const char * theTextureFileName);
    bool loadCubeMapTexture(TextureClass theClassID, short theObjectID, const std::string textureFileNames[]);

    bool activateTexture(TextureClass theClassID, short newObjectID);
    bool activateTextureIndex(TextureClass theClassID, int newIndex);
    bool getTextureIndex(TextureClass theClassID, short whichObjectID, int & theResultIndex); 
    bool activateOtherTexture(TextureClass theClassID, int theIndexOffset);

    bool hasTexture(TextureClass theClassID, short whichObjectID) {
        int myIndex;
        return getTextureIndex(theClassID,whichObjectID,myIndex);
    }

    float   getReflectionAlphaBias() const {
        return _reflectionAlphaBias;
    }
    void    setReflectionAlphaBias(float value) {
        _reflectionAlphaBias = value;
    }
    float   getReflectionAlphaScale() const {
        return _reflectionAlphaScale;
    }
    void    setReflectionAlphaScale(float value) {
        _reflectionAlphaScale = value;
    }
    float   getReflectionNormalScale() const {
        return _reflectionNormalScale;
    }
    void    setReflectionNormalScale(float value) {
        _reflectionNormalScale= value;
    }
    
    bool    getDrawWireFrame() const {
        return _drawWireFrame;
    }
    void    setDrawWireFrame(bool value) {
        _drawWireFrame = value;
    }
   
    bool    getDrawCaustics() const {
        return _drawCaustics;
    }
    void    setDrawCaustics(bool value) {
        _drawCaustics = value;
    }
    
    bool    getDrawReflections() const {
        return _drawReflections;
    }
    void    setDrawReflections(bool value) {
        _drawReflections= value;
    }
    
    bool    getDrawRefractions() const {
        return _drawRefractions;
    }
    void    setDrawRefractions(bool value) {
        _drawRefractions= value;
    }
    
    float   getRefractionScale() const {
        return _refractionScale;
    }
    void    setRefractionScale(float value) {
        _refractionScale= value;
    }
    
    float   getCausticBias() const {
        return _causticBias;
    }
    void    setCausticBias(float value) {
        _causticBias = value;
    }
    float   getCausticScale() const {
        return _causticScale;
    }
    void    setCausticScale(float value) {
        _causticScale = value;
    }
    float   getCausticSqrScale() const {
        return _causticSqrScale;
    }
    void    setCausticSqrScale(float value) {
        _causticSqrScale = value;
    }
    float   getCausticNegativeScale() const {
        return _causticNegativeScale;
    }
    void    setCausticNegativeScale(float value) {
        _causticNegativeScale = value;
    }
    // TODO: following sizes are not really used any more
    enum Sizes {
        MAX_GROUND_TEXTURES = 64,
        MAX_SURFACE_TEXTURES = 64,
        MAX_SPRITE_TEXTURES = 64,
        MAX_CUBE_TEXTURE_SETS = 64
    };

    struct Texture {
        GLuint              myID;
        int                 myWidth;
        int                 myHeight;
    };

    typedef short   ObjectID;

    Texture & currentTexture(TextureClass theClassID) {
        return _myTextures[theClassID][_currentID[theClassID]];
    }

    int getCurrentTextureIndex( TextureClass theClassID ) const {
        std::map<TextureClass, ObjectID>::const_iterator myIt = _currentID.find( theClassID );
        return myIt->second;
    }

    // remove any "special" GL state settings
    virtual void    setDefaultGLState();
    
private:
    BufferAllocatorPtr  _bufferAllocator;
    GLfloat *           _vertexBuffer;
    WaterSimulationPtr  _waterSimulation;

    std::map<TextureClass,std::map<ObjectID,Texture> > _myTextures;
    std::map<TextureClass,ObjectID> _currentID;

    //SceneSyncMaster::SceneObject    _mySceneObject[SceneSyncMaster::NUM_SCENE_OBJECTS];
    double              _objectPosScaleX;
    double              _objectPosScaleY;
    double              _objectPosOffsetX;
    double              _objectPosOffsetY;

    bool                _drawCaustics;
    bool                _drawReflections;
    bool                _drawRefractions;
    bool                _drawWireFrame;
    int                 _dataOffsetX;
    int                 _dataOffsetY;
    int                 _dataWidth;
    int                 _dataHeight;
    int                 _displayWidth;
    int                 _displayHeight;
    int                 _displayOffsetX;
    int                 _displayOffsetY;
    float               _reflectionAlphaBias;
    float               _reflectionAlphaScale;
    float               _reflectionNormalScale;
    float               _refractionScale;
    float               _causticBias;
    float               _causticScale;
    float               _causticSqrScale;
    float               _causticNegativeScale;
    float               _normalMax;

    GLuint              _cubeMapSideID[6];

    struct VAR_Buffer {
        GLfloat *   _pointer;
        GLuint      _fence;
    };
    VAR_Buffer          _varBuffer[NUM_VAR_BUFFERS];
    GLuint *            _stripIndices;
    bool                _renderSurfaceEnabled; 
    float               _crackWidth;
    float               _innerIceThickness;
    float               _outerIceThickness;
    bool                _renderCrackField;
    float               _surfaceOpacity;
    
    bool            initRender();
    bool            initExtensions(char* extension);

    inline int      computeNumLinesPerBuffer() const {
        return _dataHeight / NUM_VAR_BUFFERS;
    }

    inline int      computeStripLength() const {
        return 2 + 2 * (_dataWidth - 1);
    }
    
    void    copyWaterDataToVertexBuffer();
    void    copyWaterDataToVertexBuffer(int currentBuffer);

    void    dataCoordToScreenCoord(const asl::Vector2f & theDataCoord, asl::Vector2f & screenCoord);

    void    renderSurface();

};

typedef asl::Ptr<WaterRepresentation> WaterRepresentationPtr;

}; // namespace y60


#endif




