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

//own header
#include "FFMpegLockManager.h"

extern "C" {
#    include <libavformat/avformat.h>
}

#include <asl/base/Logger.h>
#include <boost/thread.hpp>


namespace y60 {

    int
    lockmanager_callback(void **mutex, enum AVLockOp op) {
        if (NULL == mutex) return -1;
        switch(op) {
            case AV_LOCK_CREATE:
                {
                *mutex = NULL;
                boost::mutex * m = new boost::mutex();
                *mutex = static_cast<void*>(m);
                break;
                }
            case AV_LOCK_OBTAIN:
                {
                boost::mutex * m =  static_cast<boost::mutex*>(*mutex);
                m->lock();
                break;
                }
            case AV_LOCK_RELEASE:
                {
                boost::mutex * m = static_cast<boost::mutex*>(*mutex);
                m->unlock();
                break;
                }
            case AV_LOCK_DESTROY:
                {
                boost::mutex * m = static_cast<boost::mutex*>(*mutex);
                delete m;
                break;
                }
            default:
                break;
            }
        return 0;
    }

    FFMpegLockManager::FFMpegLockManager() {
        av_lockmgr_register(&lockmanager_callback);
    }
    FFMpegLockManager::~FFMpegLockManager() {
        av_lockmgr_register(NULL);
    }
}
