//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef ASS_EVENT_SOURCE_INCLUDED
#define ASS_EVENT_SOURCE_INCLUDED

#include "ASSDriver.h"

namespace y60 {

class ASSEventSource : public ASSDriver,
                       public y60::IEventSource
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

    protected:
        void createEvent( asl::Unsigned64 theID, const std::string & theType,
                const asl::Vector2f & theRawPosition, const asl::Vector3f & thePosition3D);
        void createSyncEvent(asl::Unsigned64 theID );
    private:
        dom::NodePtr                 _myEventSchema;
        asl::Ptr<dom::ValueFactory>  _myValueFactory;
        y60::EventPtrList            _myEvents;
};

} // end of namespace y60

#endif // ASS_EVENT_SOURCE_INCLUDED
