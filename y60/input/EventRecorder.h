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
*/

#ifndef _Y60_INPUT_EVENTRECORDER_
#define _Y60_INPUT_EVENTRECORDER_

#include "y60_input_settings.h"

#include "IEventSource.h"
#include "IEventSink.h"
#include "Event.h"

#include <asl/base/Time.h>

#include <vector>


namespace y60 {

    class Y60_INPUT_DECL EventRecorder : public IEventSource, public IEventSink {
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
