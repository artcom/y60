//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_INPUT_EVENTRECORDER_
#define _Y60_INPUT_EVENTRECORDER_

#include "IEventSource.h"
#include "IEventSink.h"
#include "Event.h"

#include <asl/Time.h>

#include <vector>


namespace y60 {

    class EventRecorder : public IEventSource, public IEventSink {
        public:
            /// Recorder modes.
            enum Mode {
                STOP = 0,
                PLAY,
                RECORD
            };

            EventRecorder();
            virtual ~EventRecorder();

            virtual void init();
            virtual EventPtrList poll();
            virtual void handle(EventPtr theEvent);

            /**
             * Set recorder mode.
             * The command is ignored if the current mode is the desired mode.
             * @param theMode New recorder mode (see above).
             * @param theDiscardFlag If true then any previously
             * loaded/recorded events are discarded before recording.
             */
            void setMode(Mode theMode, bool theDiscardFlag = false);
            Mode getMode() const {
                return _myMode;
            }

            /**
             * Load events from given file.
             * @param theFilename XML file to load from.
             * @param theDiscardFlag If true then discard currently
             * loaded/recorded events before loading.
             * @return false if an error occurred, else true.
             */
            bool load(const std::string & theFilename, bool theDiscardFlag = true);

            /**
             * Save recorded events to given file.
             * @param theFilename XML file to save to.
             */
            void save(const std::string & theFilename) const;

        private:

            Mode _myMode;
            asl::Time _myStartTime;

            typedef std::vector<EventPtr> EventQueue;
            EventQueue _myEvents;
            EventQueue::iterator _myEventIter;

            bool filterEvent(const EventPtr & theEvent) const;
    };
}

#endif
