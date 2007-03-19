/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: ZipPackage.h,v $
//
//   $Revision: 1.4 $
//
// Description: unit test template file - change ZipReader to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef __asl__ZipPackage_h_included__
#define __asl__ZipPackage_h_included__

#include "IPackage.h"
#include "ZipReader.h"

namespace asl {

class ZipPackage : public IPackage {
    public:
        ZipPackage(const std::string & theZipReader);
        ~ZipPackage();
        const std::string & getPath() const { return _myZipFilename; };
        FileList getFileList(const std::string & theSubDir = "", bool theRecurseFlag = false);
        std::string findFile(const std::string & theRelativePath) const;
        Ptr<ReadableBlock> getFile(const std::string & theRelativePath);
        Ptr<ReadableStream> getStream(const std::string & theRelativePath);

    private:
        ZipReader _myZipReader;
        std::string _myZipFilename;
        FileList _myFileList;
};

}
#endif



