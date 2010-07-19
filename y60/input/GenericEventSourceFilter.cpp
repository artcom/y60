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

#include "GenericEventSourceFilter.h"

using namespace std;
namespace y60 {
    GenericEventSourceFilter::GenericEventSourceFilter() {}
    GenericEventSourceFilter::~GenericEventSourceFilter() {
    }

    void GenericEventSourceFilter::addCursorFilter(string theEventType, string theIdAttributeName) {
        _myCursorFilter.push_back( CursorFilter(theEventType, theIdAttributeName));

    }
    // apply filter in place
    void GenericEventSourceFilter::applyFilter(EventPtrList & theEventList) {
        for (unsigned i = 0; i < _myCursorFilter.size(); i++) {
            //AC_INFO << "filter cursor events with type: " << _myCursorFilter[i]._myEventType << " and cursor id attribute: " << _myCursorFilter[i]._myCursorAttributeName;
            applyCursorFilter(_myCursorFilter[i]._myEventType, _myCursorFilter[i]._myCursorAttributeName, theEventList);
        }
    }
    void GenericEventSourceFilter::applyCursorFilter(std::string theEventType, std::string theIdAttributeName, EventPtrList & theEventList) {
	    std::map<int, std::vector<GenericEventPtr > > myEvents2Shrink;
	    EventPtrList::iterator myIt = theEventList.begin();
	    unsigned int counter= 0;
        for (; myIt !=theEventList.end(); ) {
		    counter++;
		    GenericEventPtr myGenericEvent(dynamic_cast_Ptr<GenericEvent>(*myIt));
		    dom::NodePtr myNode = myGenericEvent->getNode();
		    int myCursorId = asl::as<int>(myNode->getAttributeString(theIdAttributeName));
		    std::string myEventType = myNode->getAttributeString("type");
		    if (myEventType == theEventType) {
			    if (myEvents2Shrink.find(myCursorId) == myEvents2Shrink.end()) {
				    myEvents2Shrink[myCursorId] = std::vector<GenericEventPtr>();
			    }
			    myEvents2Shrink[myCursorId].push_back(myGenericEvent);
                myIt = theEventList.erase(myIt);
            } else {
                ++myIt;
            }
        }

        std::map<int, std::vector<GenericEventPtr > >::iterator myEndIt2   = myEvents2Shrink.end();
	    std::map<int, std::vector<GenericEventPtr > >::iterator myIt2 = myEvents2Shrink.begin();
	    for(; myIt2 !=  myEndIt2; ++myIt2){
		    theEventList.push_back((myIt2->second)[(myIt2->second).size()-1]);
	    }
    }

    void GenericEventSourceFilter::analyzeEvents(EventPtrList theEventList, std::string theIdAttributeName) {
        AC_INFO << "############## analyzeEvents cursor events:";
	    std::map<int, std::map<std::string, int> > myCursorEventCounter;
	    {
		    EventPtrList::iterator myEndIt   = theEventList.end();
		    EventPtrList::iterator myIt = theEventList.begin();
		    for(; myIt !=  myEndIt; ++myIt){
			    GenericEventPtr myGenericEvent(dynamic_cast_Ptr<GenericEvent>(*myIt));
			    dom::NodePtr myNode = myGenericEvent->getNode();
			    int myCursorId = asl::as<int>(myNode->getAttributeString(theIdAttributeName));
			    std::string myCursorType = myNode->getAttributeString("type");
			    if (myCursorEventCounter.find(myCursorId) == myCursorEventCounter.end()) {
				    myCursorEventCounter[myCursorId][myCursorType] = 0;
			    } else {
				    if (myCursorEventCounter[myCursorId].find(myCursorType) == myCursorEventCounter[myCursorId].end()) {
					    myCursorEventCounter[myCursorId][myCursorType] = 0;
				    }
			    }
			    myCursorEventCounter[myCursorId][myCursorType]++;
		    }
	    }
	    {
		    std::map<int, std::map<std::string, int> >::iterator myEndIt   = myCursorEventCounter.end();
		    std::map<int, std::map<std::string, int> >::iterator myIt = myCursorEventCounter.begin();
		    for(; myIt !=  myEndIt; ++myIt){
			    std::map<std::string, int>::iterator myEndItType   = myIt->second.end();
			    std::map<std::string, int>::iterator myItType      = myIt->second.begin();
			    for(; myItType !=  myEndItType; ++myItType){
				    AC_INFO << "Cursor id: " << myIt->first << " with type: " << myItType->first << " has # " << myItType->second << " events";
			    }
		    }
		    AC_INFO << "-------";
	    }
    }

}
