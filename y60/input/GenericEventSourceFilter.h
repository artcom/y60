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

#ifndef _Y60_INPUT_GENERICEVENTSOURCEFILTER_INCLUDED_
#define _Y60_INPUT_GENERICEVENTSOURCEFILTER_INCLUDED_

#include "y60_input_settings.h"

#include "GenericEvent.h"
#include "IEventSource.h"
#include <asl/base/Ptr.h>
#include <deque>
#include <vector>

namespace y60 {
  
    struct CursorFilter {
        CursorFilter(const std::string & theEventType, const std::string & theIdAttributeName) : _myEventType(theEventType), _myCursorAttributeName(theIdAttributeName) {}
        std::string _myEventType;
        std::string _myCursorAttributeName;
    };
    struct PositionInfo {
        PositionInfo() {};
        PositionInfo(const asl::Vector3f & thePosition, const unsigned long long & theTimestamp):
            _myPosition(thePosition), 
            _myTimestamp(theTimestamp) {}
        asl::Vector3f _myPosition;
        unsigned long long _myTimestamp;
    };

    typedef std::map<int, std::deque<PositionInfo> > CursorPositionHistory;
    typedef std::deque<PositionInfo> CursorPositions;


    class Y60_INPUT_DECL GenericEventSourceFilter {
        public:
            static const unsigned int MAX_CURSOR_POSITIONS_IN_HISTORY;
            GenericEventSourceFilter();
            virtual ~GenericEventSourceFilter();
        protected:    
            void addCursorFilter(const std::string & theEventType, const std::string & theIdAttributeName);
            void applyFilter(EventPtrList & theEventList);
            void analyzeEvents(const EventPtrList & theEventList, const std::string & theIdAttributeName) const;
            bool allow2SendCursor(int theCursorId);
            void removeFromCursorList(int theCursorId);
            void setMaxCursorCount(int theMaxCursorCount) { _myMaxCursorCount = theMaxCursorCount; }

            asl::Vector3f getAveragePosition(const unsigned int theCursorId) const;
            inline asl::Vector3f getCurrentPosition(const unsigned int theCursorId) const {
                CursorPositionHistory::const_iterator myPositionsIt = _myCursorPositionHistory.find(theCursorId);
                if (myPositionsIt != _myCursorPositionHistory.end()) {
                    return myPositionsIt->second.back()._myPosition;
                } else {
                    return asl::Vector3f(0.0,0.0,0.0);
                }
            };

            void addPositionToHistory(const unsigned int theCursorId, const asl::Vector3f & thePosition, const unsigned long long & theTimestamp=asl::Time().millis());
            void addPositionToHistory(const unsigned int theCursorId, const asl::Vector2f & thePosition, const unsigned long long & theTimestamp=asl::Time().millis());
            asl::Vector3f calculateAveragePosition(const unsigned int theCursorId, const asl::Vector3f & thePosition, const unsigned long long & theTimestamp=asl::Time().millis());
            asl::Vector2f calculateAveragePosition(const unsigned int theCursorId, const asl::Vector2f & thePosition, const unsigned long long & theTimestamp=asl::Time().millis());
            void clearCursorHistoryOnRemove(const EventPtrList & theEventList);

            std::map<int, std::deque<PositionInfo> >   _myCursorPositionHistory;
            unsigned int _myMaxCursorPositionsInHistory;
            bool _myFilterMultipleMovePerCursorFlag;
        private:          
            void applyCursorFilter(const std::string & theEventType, const std::string & theIdAttributeName, EventPtrList & theEventList);

            std::vector<CursorFilter>                    _myCursorFilter;
            int                                          _myMaxCursorCount;
            std::vector<int>                             _myCursorsInUse;
            
    };

    
}

#endif //_Y60_INPUT_GENERICEVENTSOURCEFILTER_INCLUDED_
