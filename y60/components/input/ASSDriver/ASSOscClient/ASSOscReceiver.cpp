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

#include "ASSOscReceiver.h"

namespace y60 {

ASSOscReceiver::ASSOscReceiver(std::string theHost,
                               asl::Unsigned16 theSourcePort,
                               asl::Unsigned16 theDestinationPort)
  : _myBuffer(new char[ourBufferSize]),
    _myHost(theHost),
    _mySourcePort(theSourcePort),
    _myDestinationPort(theDestinationPort),
    _myConnection(INADDR_ANY, _mySourcePort),
    _myStream(_myBuffer, ourBufferSize),
    _myDidOverflow(false),
    _myEmpty(true),
    _myConfigured(false),
    _myBundleCounter(0),
    _myBounded(false),
    _myRegion(0,0,0,0)
{
}

ASSOscReceiver::~ASSOscReceiver()
{
    delete [] _myBuffer;
}

void
ASSOscReceiver::buildConfigureEvent(int theWidth, int theHeight)
{
    if(!_myDidOverflow) {
        _myEmpty = false;

        try {
            _myStream << osc::BeginMessage("/configure")
                      << 0
                      << theWidth << theHeight
                      << osc::EndMessage;
        } catch (osc::OutOfBufferMemoryException) {
            _myDidOverflow = true;
        }
    }
}

void
ASSOscReceiver::buildTransportLayerEvent(const std::string& theType)
{
    if(!_myDidOverflow) {
        _myEmpty = false;

        std::string myAddress("/");

        myAddress += theType;

        try {
            _myStream << osc::BeginMessage(myAddress.c_str())
                      << osc::EndMessage;
        } catch (osc::OutOfBufferMemoryException) {
            _myDidOverflow = true;
        }
    }
}

void
ASSOscReceiver::buildCursorEvent(const std::string&   theType,
                                 int                  theId,
                                 const asl::Vector3f& thePosition,
                                 float                theIntensity)
{
    if(!_myDidOverflow) {
        if(_myBounded) {
            asl::Vector2f myPosition(thePosition[0], thePosition[1]);

            if(!_myRegion.contains(myPosition)) {
                return;
            }
        }

        _myEmpty = false;

        std::string myAddress("/");

        myAddress += theType;

        try {
            _myStream << osc::BeginMessage(myAddress.c_str())
                      << theId
                      << thePosition[0] << thePosition[1] << thePosition[2]
                      << theIntensity
                      << osc::EndMessage;
        } catch (osc::OutOfBufferMemoryException) {
            _myDidOverflow = true;
        }
    }
}

void
ASSOscReceiver::restrictToRegion(const asl::Box2f& theRegion)
{
    _myBounded = true;
    _myRegion = theRegion;
}

void
ASSOscReceiver::prepare()
{
    if(_myDidOverflow) {
        AC_WARNING << "Osc transmit buffer overflow.";
        _myDidOverflow = false;
        _myConfigured = false;
    }

    _myEmpty = true;
    _myStream.Clear();

    _myStream << osc::BeginBundle(_myBundleCounter);

    if(!_myConfigured) {
        buildConfigureEvent(0, 0); // XXX
        _myConfigured = true;
    }
}

void
ASSOscReceiver::send()
{
    if(!_myEmpty && !_myDidOverflow && _myConfigured) {
        _myStream << osc::EndBundle;

        try {
            _myConnection.sendTo(inet::getHostAddress(_myHost.c_str()), _myDestinationPort, _myStream.Data(), _myStream.Size());
        } catch (const inet::SocketException& ex) {
            AC_WARNING << "Could not send to " << _myHost << ":" << _myDestinationPort << ": " << ex;
            reset();
        }
    }
}

void
ASSOscReceiver::reset()
{
    _myConfigured = false;
}

} // end of namespace y60
