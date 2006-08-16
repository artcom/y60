


#include "SimWater.h"
#include "AGPBufferAllocator.h"

#include <y60/JSVector.h>
#include <y60/AbstractRenderWindow.h>

const int SIMULATION_WIDTH = 480;
const int SIMULATION_HEIGHT = 240;
const std::string DATA_DIR("data");
const char * const ourCubeSides[6] = {"right","left","top","bottom","front","back"};

using namespace std;
using namespace asl;
using namespace jslib;
using namespace y60;

SimWater::SimWater(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    IRendererExtension("SimWater"),
    _mySimulationSize( SIMULATION_WIDTH + 2 , SIMULATION_HEIGHT + 2),
    _mySimulationOffset( 1, 1 ),
    _myDisplaySize(SIMULATION_WIDTH / 3, SIMULATION_HEIGHT / 2), // grouse like 3:2 display
    _myDisplayOffset(0, 0), // used for tiled rendering (grouse)
    _myWaterDamping(0.9993),
    _myRunSimulationFlag( true ),
    _myIntegrationsPerFrame( 3 ),
    _myTimeStep( 0.1 )
{}

SimWater::~SimWater() {
}

void 
SimWater::onUpdateSettings(dom::NodePtr theConfiguration) {
    AC_PRINT << "SimWater::onUpdateSettings()";
};

void 
SimWater::onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const
{
    AC_PRINT << "SimWater::onGetProperty()";
};

void 
SimWater::onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue)
{
    AC_PRINT << "SimWater::onSetProperty()";
};

void 
SimWater::onStartup(jslib::AbstractRenderWindow * theWindow)  {
    AC_PRINT << "SimWater::onStartup()";

    _myWaterSimulation = WaterSimulationPtr(
            new WaterSimulation( _mySimulationSize, _myWaterDamping ));
    _myWaterSimulation->init();
    _myWaterSimulation->resetDamping( false );

    _myWaterRepresentation = WaterRepresentationPtr( new WaterRepresentation() );
    dom::NodePtr myCanvas = theWindow->getCanvas();
    _myWaterRepresentation->init( _myWaterSimulation, _myDisplaySize[0], _myDisplaySize[1],
            _mySimulationOffset[0], _mySimulationOffset[1], 
            _mySimulationSize[0], _mySimulationSize[1],
            theWindow->getWidth(), theWindow->getHeight(),
            _myDisplayOffset[0], _myDisplayOffset[1],
            BufferAllocatorPtr( new AGPBufferAllocator ) );


    try {
        // XXX hardcoded filename
        dom::DocumentPtr myDoc( new dom::Document());
        myDoc->parseFile( "texturepool.xml" );
        dom::NodePtr myTextureNode = myDoc->childNode("texturepool");

        if (myTextureNode) {
            AC_PRINT << "Loading textures:";
            loadTexturesFromConfig( * myTextureNode, WaterRepresentation::floormaps );


            int numCubeTextures = (*myTextureNode)("cubemaps").childNodesLength("fileset");
            for (int i = 0; i < numCubeTextures; ++i) {
                string fileNames[6];

                const dom::Node & myFileSetNode = (*myTextureNode)("cubemaps")("fileset",i); 
                short objectID = asl::as<short>(myFileSetNode["objectid"].nodeValue());

                for (int s=0; s<6; s++) {
                    string fileName = myFileSetNode(ourCubeSides[s])["name"].nodeValue();
                    if (!fileName.size()) {
                        AC_DEBUG << "name for cubemap side " << ourCubeSides[s] << " not found" << endl;
                        assert(0);
                    }
                    fileNames[s] = DATA_DIR + "/" + fileName;
                }
                bool loadOK = _myWaterRepresentation->loadCubeMapTexture(
                        WaterRepresentation::cubemaps,objectID,fileNames);
                assert(loadOK);
            }

        } else {
            AC_ERROR << "No textures found." << endl;
        }

    
    } catch (const asl::Exception & ex) {
        AC_ERROR << "#ERROR: SimWater::onStartup():" << endl
                 << ex << endl
                 << "Aborting";
        exit(1);
    } catch (...) {
        AC_ERROR << "#ERROR: Unknown exception in SimWater::onStartup():" << endl
                 << "Aborting" << endl;
        exit(1);
    }

    _myWaterRepresentation->setDrawWireFrame( false );
	_myWaterRepresentation->activateTextureIndex(WaterRepresentation::floormaps,0);
	_myWaterRepresentation->activateTextureIndex(WaterRepresentation::cubemaps,0);

    _myStartTime.setNow();

};

void
SimWater::loadTexturesFromConfig(const dom::Node & theConfig, WaterRepresentation::TextureClass theClassID) {
    try {
        string myClassName = WaterRepresentation::TextureClassName(theClassID);
        int numFloorTextures = theConfig(myClassName).childNodesLength("file");
        for (int i = 0; i < numFloorTextures; ++i) {
            AC_PRINT << "texture: " << i;
            const dom::Node & myFileNode = theConfig(myClassName)("file",i); 
            string fileName = DATA_DIR + "/" + myFileNode["name"].nodeValue();
            short objectID = asl::as<short>(myFileNode["objectid"].nodeValue());
            bool loadOK = _myWaterRepresentation->loadTexture(theClassID,objectID,fileName.c_str());
            assert(loadOK);
        }
    } catch (const asl::Exception & ex) {
        AC_ERROR << "Water:: in loadTexturesFromConfig():" << endl;
        AC_ERROR << ex.what() << " AT " << ex.where() << endl;
        AC_ERROR << "Aborting" << endl;
        exit(1);
    } catch (...) {
        AC_ERROR << "#ERROR: Water:: in loadTexturesFromConfig():" << endl;
        AC_ERROR << "Aborting" << endl;
        exit(1);
    }
}


bool 
SimWater::onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    AC_PRINT << "SimWater::onSceneLoaded()";
    return true;
};

void 
SimWater::handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
    AC_DEBUG << "SimWater::handle()";
};

void 
SimWater::onFrame(jslib::AbstractRenderWindow * theWindow , double t) {
    AC_DEBUG << "SimWater::onFrame()";
    asl::Time myCurrentTime;
    myCurrentTime.setNow();
    double myRunTime = myCurrentTime - _myStartTime;

    if (_myRunSimulationFlag) {
        if (_myIntegrationsPerFrame == 1) {
            _myWaterSimulation->simulationStep(_myTimeStep);
        } else {
            _myWaterSimulation->simulationMultiStep(_myTimeStep, _myIntegrationsPerFrame, 5);
        }
        assert( _myWaterRepresentation);
        _myWaterRepresentation->preRender();
    }
            

};


void
SimWater::setWaterProjection() {
    
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

    float   left = (-((float)(_myDisplaySize[0])) / 2.f) + _myDisplayOffset[0];
    float   right = (((float)_myDisplaySize[0]) / 2.f-1.f) + _myDisplayOffset[0];
    float   top = (-((float)_myDisplaySize[1]) / 2.f) + _myDisplayOffset[1];
    float   bottom = (((float)_myDisplaySize[1]) / 2.f) + _myDisplayOffset[1];

	glOrtho( left, right, top, bottom, -100, 10000);
}

void 
SimWater::onPreRender(jslib::AbstractRenderWindow * theRenderer) {
    AC_DEBUG << "SimWater::onPreRender()";
};

void 
SimWater::onPostRender(jslib::AbstractRenderWindow * theRenderer) {
    AC_DEBUG << "SimWater::onPostRender()";


    {
        glDisable(GL_CULL_FACE); // XXX the whole water thingy seems to be backfacing
        glDisable(GL_LIGHTING);

        setWaterProjection();

        glMatrixMode( GL_MODELVIEW );

        glLoadIdentity();

        // camera transformation
        glTranslatef( 0.0, 0.0, 0.0);
        glScalef(1.0, 1.0, 1.0);
        glRotatef(0, 1.,0.,0.);
        glRotatef(0, 0.,1.,0.);
        glRotatef(0, 0.,0.,1.);

        glColor4f(1, 1, 1, 1);

        assert(_myWaterRepresentation);

        //AC_DEBUG << "gWaterRepresentation->render()" << endl;
        _myWaterRepresentation->render();

        //setEdgeProjection();

        /*
        glMatrixMode( GL_MODELVIEW );

        glLoadIdentity();

        glColor4f(1, 1, 1, 1);
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glPushAttrib(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glLineWidth(1.);
        */

    }

    //glPopClientAttrib();
    //glPopAttrib();

    CHECK_OGL_ERROR;
};

void 
SimWater::splash(const asl::Vector2i & thePosition, int theMagnitude, int theRadius) {
     AC_PRINT << "=== SPLASH ===";
     _myWaterSimulation->sinoidSplash(thePosition[0], thePosition[1], theMagnitude, theRadius);
}

const char *
SimWater::ClassName() {
    static const char * myClassName = "SimWater";
    return myClassName;
}


static JSBool
Splash(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;

    asl::Vector2i myPosition;
    convertFrom(cx, argv[0], myPosition);

    int myMagnitude;
    convertFrom(cx, argv[1], myMagnitude);

    int myRadius;
    convertFrom(cx, argv[2], myRadius);

    asl::Ptr<SimWater> myNative = getNativeAs<SimWater>(cx, obj);
    if (myNative) {
        myNative->splash(myPosition, myMagnitude, myRadius);
    } else {
        assert(myNative);
    }

    return JS_TRUE;
}


JSFunctionSpec *
SimWater::Functions() {
    static JSFunctionSpec myFunctions[] = {
        {"splash", Splash, 4},
        {0}
    };
    return myFunctions;
}

