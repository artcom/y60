//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef Y60_LIIMOTE_INCLUDED
#define Y60_LIIMOTE_INCLUDED

#include "WiiRemote.h"

namespace y60 {

class Liimote : public WiiRemote {
    public:
        virtual std::string getDeviceName() const;
        virtual void sendOutputReport(unsigned char out_bytes[], unsigned theNumBytes);
        static std::vector<WiiRemotePtr> discover();
    protected:
        virtual void closeDevice();

};

} // end of namespace

#endif // Y60_LIIMOTE_INCLUDED
