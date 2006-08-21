#ifndef Y60_SIM_WATER_INCLUDED
#define Y60_SIM_WATER_INCLUDED

#include "WaterSimulation.h"
#include "WaterRepresentation.h"

#include <asl/PlugInBase.h>
#include <y60/IRendererExtension.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/Documentation.h>

class SimWater :
    public asl::PlugInBase,
    public y60::IRendererExtension,
    public jslib::IScriptablePlugin
{
    public:
        SimWater(asl::DLHandle theDLHandle);
        virtual ~SimWater(); 

        JSFunctionSpec * Functions();

        void onUpdateSettings(dom::NodePtr theConfiguration);
        void onGetProperty(const std::string & thePropertyName,
                y60::PropertyValue & theReturnValue) const;

        void onSetProperty(const std::string & thePropertyName,
                const y60::PropertyValue & thePropertyValue);

        void onStartup(jslib::AbstractRenderWindow * theWindow);
        bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow);
        void handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent);
        void onFrame(jslib::AbstractRenderWindow * theWindow , double t);

        void onPreRender(jslib::AbstractRenderWindow * theRenderer);
        void onPostRender(jslib::AbstractRenderWindow * theRenderer);

        const char * ClassName();
        
        void splash(const asl::Vector2i & thePosition, int theMagnitude, int theRadius);
        int addFloormap(const std::string & theFilename);
        int addCubemap(const std::string theFilenames[]);
        void reset();

    private:
        SimWater();
        void loadTexturesFromConfig(const dom::Node & theConfig,
                                    y60::WaterRepresentation::TextureClass theClassID);
        void setWaterProjection();

        asl::Vector2i convertMouseCoordsToSimulation( const asl::Vector2i & theMousePos );

        y60::WaterSimulationPtr     _myWaterSimulation;
        y60::WaterRepresentationPtr _myWaterRepresentation;

        asl::Vector2i _mySimulationSize;
        asl::Vector2i _mySimulationOffset;
        asl::Vector2i _myDisplaySize;
        asl::Vector2i _myDisplayOffset;
        asl::Vector2i _myViewportSize;
        float _myWaterDamping;

        asl::Time _myStartTime;
        bool _myRunSimulationFlag;
        int _myIntegrationsPerFrame;
        float _myTimeStep;

        int _myFloormapCounter;    
        int _myCubemapCounter;    

};
#endif // Y60_SIM_WATER_INCLUDED

