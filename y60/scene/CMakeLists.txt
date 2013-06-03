
set(SOURCES
AnimationBase.cpp
AnimationBuilder.cpp
AnimationCharacter.cpp
AnimationClip.cpp
AnimationManager.cpp
Body.cpp
BodyBuilder.cpp
BuilderBase.cpp
Camera.cpp
CameraBuilder.cpp
Canvas.cpp
CanvasBuilder.cpp
CharacterBuilder.cpp
ClipBuilder.cpp
ElementBuilder.cpp
ExternalBuilder.cpp
Geometry.cpp
ImageBuilder.cpp
JointBuilder.cpp
Light.cpp
LightBuilder.cpp
LightSource.cpp
LightSourceBuilder.cpp
MaterialBase.cpp
MaterialBuilder.cpp
MovieBuilder.cpp
Picking.cpp
Primitive.cpp
Scene.cpp
SceneBuilder.cpp
SceneOptimizer.cpp
Shape.cpp
ShapeBuilder.cpp
Texture.cpp
TextureBuilder.cpp
TextureManager.cpp
TextureUnit.cpp
TransformBuilder.cpp
TransformBuilderBase.cpp
TransformHierarchyFacade.cpp
Viewport.cpp
World.cpp
WorldBuilder.cpp
WorldBuilderBase.cpp
Y60Decoder.cpp
)

set(HEADERS
Animation.h
AnimationBase.h
AnimationBuilder.h
AnimationCharacter.h
AnimationClip.h
AnimationManager.h
Body.h
BodyBuilder.h
BuilderBase.h
Camera.h
CameraBuilder.h
Canvas.h
CanvasBuilder.h
CharacterBuilder.h
ClipBuilder.h
ElementBuilder.h
ElementBuilder.h
Facades.h
Geometry.h
IFrameBuffer.h
IProgressNotifier.h
ISceneDecoder.h
IShader.h
ImageBuilder.h
IncludeFacade.h
Joint.h
JointBuilder.h
Light.h
LightBuilder.h
LightSource.h
LightSourceBuilder.h
LodFacade.h
MaterialBase.h
MaterialBuilder.h
MaterialParameter.h
MaterialTypes.h
MemoryResourceManager.h
MovieBuilder.h
Overlay.h
Picking.h
Primitive.h
PropertyListFacade.h
Record.h
ResourceManager.h
Scene.h
SceneBuilder.h
SceneOptimizer.h
Shape.h
ShapeBuilder.h
Texture.h
TextureBuilder.h
TextureManager.h
TextureTags.h
TextureUnit.h
Transform.h
TransformBuilder.h
TransformBuilderBase.h
TransformHierarchyFacade.h
VertexData.h
VertexDataImpl.h
VertexMemoryBase.h
Viewport.h
World.h
WorldBuilder.h
WorldBuilderBase.h
Y60Decoder.h
y60_scene_settings.h
)

set(TESTS
AnimationBuilder
Camera
LightSourceBuilder
Lights
Scene
ShapeBuilder
)

list(APPEND EXTERNS GLEW)

ac_add_library(
    y60scene y60/scene
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase asldom aslxpath aslmath aslzip y60base y60image y60video 
            paintlib
    EXTERNS ${EXTERNS}
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)
