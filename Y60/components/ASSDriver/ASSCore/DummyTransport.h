//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef Y60_ASS_DUMMY_TRANSPORT_INCLUDED
#define Y60_ASS_DUMMY_TRANSPORT_INCLUDED

#include "TransportLayer.h"

namespace y60 {

class DummyTransport : public TransportLayer {

    public: 

        DummyTransport( const dom::NodePtr & theSettings );
        ~DummyTransport();

        bool settingsChanged( dom::NodePtr theSettings );

    protected: 

        void establishConnection();
        void readData();
        void writeData( const char * theData, size_t theSize );
        void closeConnection();

    private:

        void buildStatusLine(); 
        void generateGridValues();
        void calculateChecksum();
        void writeStatusToken( const char theToken, unsigned theNumber );

        unsigned char _myFrameCount;
        unsigned _myGenCheckSum;
        unsigned _myGridWidth;
        unsigned _myGridHeight;
        unsigned _myGridSpacing;

        unsigned char _myDimFactor;

        std::vector<unsigned char> _myStatusLine;
        std::vector<unsigned char> _myGridValues;

};

} // end of namespace

#endif // Y60_ASS_DUMMY_TRANSPORT_INCLUDED
