//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_y60_PLDataSourceStreamAdapter_h_
#define _ac_y60_PLDataSourceStreamAdapter_h_

#include <OpenEXR/IlmImf/ImfIO.h>

class PLDataSourceStreamAdapter : public Imf::IStream {
    public:
        PLDataSourceStreamAdapter(PLDataSource & theDataSrc) :
            Imf::IStream(theDataSrc.GetName()),
            _myData(theDataSrc.GetBufferPtr(0)),
            _myDataSize(theDataSrc.GetFileSize()),
            _myReadPosition(0)
        {}

        bool read (char theDataSink[], int theDataLength) {
            if (_myReadPosition + theDataLength <= _myDataSize) {
                memcpy((void*)theDataSink, (void*)(_myData + _myReadPosition), theDataLength);
                _myReadPosition += theDataLength;
                return true;
            }
            return false;
        }

        Imf::Int64 tellg() {
            return _myReadPosition;
        }

        void seekg(Imf::Int64 thePosition) {
            _myReadPosition = thePosition;
        }

        void clear() {
            // No error flags to clear
        }
    private:
        PLBYTE *   _myData;
        int        _myDataSize;
        Imf::Int64 _myReadPosition;
};

#endif