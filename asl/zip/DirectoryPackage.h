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
//    $RCSfile: DirectoryPackage.h,v $
//
//   $Revision: 1.3 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef __asl__DirectoryPackage_h_included__
#define __asl__DirectoryPackage_h_included__

#include "IPackage.h"

namespace asl {

class DirectoryPackage : public IPackage {
    public:
        DirectoryPackage(const std::string & theDirectory, bool theIgnoreDotFiles = true);
        const std::string & getPath() const { return _myDirectory; };
        FileList getFileList(const std::string & theSubDir = "");
        std::string findFile(const std::string & theRelativePath);
        Ptr<ReadableBlock> getFile(const std::string & theRelativePath);

    private:
        std::string _myDirectory;
        bool _myIgnoreDotFiles;

        std::string getAbsolutePath(const std::string & theRelativePath) const;
};

}
#endif
