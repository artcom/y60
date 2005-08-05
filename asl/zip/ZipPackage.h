/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: ZipPackage.h,v $
//
//   $Revision: 1.4 $
//
// Description: unit test template file - change ZipFile to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef __asl__ZipPackage_h_included__
#define __asl__ZipPackage_h_included__

#include "IPackage.h"
#include "ZipFile.h"

namespace asl {

class ZipPackage : public IPackage {
    public:
        ZipPackage(const std::string & theZipFile);
        const std::string & getPath() const { return _myZipFilename; };
        FileList getFileList(const std::string & theSubDir);
        std::string findFile(const std::string & theRelativePath);
        Ptr<ReadableBlock> getFile(const std::string & theRelativePath);

    private:
        ZipFile _myZipFile;
        std::string _myZipFilename;
        FileList _myFileList;
};

}
#endif



