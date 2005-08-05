//============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//         $Id: Projector.h,v 1.5 2004/11/27 11:19:07 ulrich Exp $
//     $Author: ulrich $
//   $Revision: 1.5 $
//       $Date: 2004/11/27 11:19:07 $
//
// Projector controller class.
//
//=============================================================================

#ifndef _ac_watchdog_Projector_h_
#define _ac_watchdog_Projector_h_

#include <string>
#include <dom/typedefs.h>

namespace asl {
    class SerialDevice;
}

class Logger;

class Projector {
public:
    /// Factory.
    static Projector* getProjector(const std::string& theType, int thePort);

    explicit Projector(int thePortNumber);
    virtual ~Projector();

    const std::string & getDescription() const { return _myDescription; }

    /// Logger.
    void setLogger(Logger* theLogger) { _myLogger = theLogger; }
    Logger* getLogger() const { return _myLogger; }
    
    /// Configure projector.
    virtual void configure(const dom::NodePtr & theConfigNode);

    /// Turn projector on/off.
    void powerUp() { power(true); }
    void powerDown() { power(false); }

    virtual void power(bool thePowerFlag) = 0;

    /// Input sources.
    enum VideoSource {
        NONE = 0,
        RGB_1, RGB_2, VIDEO, SVIDEO, DVI, VIEWER
    };
    void selectInput(const std::string& theSource);

    virtual void selectInput(VideoSource theSource) = 0;

    /// Lamp mode.
    virtual void lamps(unsigned theLampsMask) {}

    /// Lamp power.
    virtual void lampPower(bool thePowerHighFlag) {}

    /// Shutter mode.
    virtual void shutter(bool theShutterOpenFlag) {}

    /// Handle command.
    void setCommandEnable(bool theEnableFlag) { _myCommandEnable = theEnableFlag; }
    bool getCommandEnable() const { return _myCommandEnable; }

    virtual bool command(const std::string & theCommand);

    /// Projector status update.
    virtual void update() {}

protected:
    std::string _myDescription;

    asl::SerialDevice* getDevice() {
        return _mySerialDevice;
    }

    VideoSource getEnumFromString(const std::string& theSource) const;

private:
    asl::SerialDevice * _mySerialDevice;
    Logger *            _myLogger;
    bool                _myCommandEnable;

    //Projector();
};

#endif
