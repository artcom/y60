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

#ifndef ASS_EVENT_SOURCE_INCLUDED
#define ASS_EVENT_SOURCE_INCLUDED

#include "y60_eventsource_settings.h"
#include <y60/components/input/ASSDriver/ASSCore/ASSDriver.h>
#include <y60/input/GenericEventSourceFilter.h>

namespace y60 {

class ASSEventSource : public asl::PlugInBase,
                       public ASSDriver,
                       public y60::IEventSource,
                       public GenericEventSourceFilter
{
    public:
        ASSEventSource(asl::DLHandle theHandle);
        virtual y60::EventPtrList poll();


        const char * ClassName() {
            static const char * myClassName = "ASSEventSource";
            return myClassName;
        }

        virtual void onGetProperty(const std::string & thePropertyName,
                           PropertyValue & theReturnValue) const;
        virtual void onSetProperty(const std::string & thePropertyName,
                           const PropertyValue & thePropertyValue);
        // TODO virtual void onUpdateSettings(dom::NodePtr theSettings);

        void createTransportLayerEvent(const std::string & theType );
    protected:
        void createEvent( int theID, const std::string & theType,
                const asl::Vector2f & theRawPosition, const asl::Vector3f & thePosition3D,
                const asl::Box2f & theROI, float intensity, const ASSEvent & theEvent);
    private:
        dom::NodePtr                 _myEventSchema;
        asl::Ptr<dom::ValueFactory>  _myValueFactory;
        y60::EventPtrList            _myEvents;
};

} // end of namespace y60

#endif // ASS_EVENT_SOURCE_INCLUDED
