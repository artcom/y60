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

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

namespace y60 {

class Liimote : public WiiRemote {
    public:
        Liimote(const inquiry_info & theDeviceInfo, unsigned theId, const char * theName);
        virtual ~Liimote();

        virtual std::string getDeviceName() const;
        virtual void send(unsigned char theOutputReport[], unsigned theNumBytes);

        static std::vector<WiiRemotePtr> discover();

    protected:
        static void inputReportListener( asl::PosixThread & theThread );
        virtual void closeDevice();

        bdaddr_t _myBDAddress;
        uint8_t  _myBTClass[3];
        std::string _myName;

        int _myCtlSocket;
        int _myIntSocket;
};

typedef asl::Ptr<Liimote> LiimotePtr;

} // end of namespace

#endif // Y60_LIIMOTE_INCLUDED
