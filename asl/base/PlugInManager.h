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
//
//    $RCSfile: PlugInManager.h,v $
//
//     $Author: david $
//
//   $Revision: 1.4 $
//
// Description:
//
//=============================================================================

#ifndef _PLUGIN_MANAGER_INCLUDED_
#define _PLUGIN_MANAGER_INCLUDED_

#include <iostream>
#include <string>
#include <map>

#include "Ptr.h"
#include "Exception.h"
#include "PlugInBase.h"
#include "Singleton.h"

namespace asl {


/*! \addtogroup aslbase */
/* @{ */

DEFINE_EXCEPTION(PlugInException, Exception)

class PlugInManager :
    public Singleton<PlugInManager>
{
public:
    ~PlugInManager();
    void setSearchPath(const std::string & thePath);
    std::string getSearchPath() const;
    Ptr<PlugInBase> getPlugIn(const std::string & theName);
    bool isLoaded(const std::string & theName) const;
    static std::string getPlugInFileName(const std::string & thePlugInName, const char * theVariant = 0);
    static void* getFunction(DLHandle theHandle, const std::string & theName);

protected:
    PlugInManager();
    friend class SingletonManager;

    Ptr<PlugInBase> loadPlugIn(const std::string & theName);
    std::string lastError() const;

    typedef std::map< std::string, WeakPtr<PlugInBase> > PlugInCache;
    PlugInCache _myCache;
    std::string _mySearchPath;
private:
    PlugInManager(const PlugInManager&) {}
    PlugInManager & operator=(const PlugInManager &) {return *this;}
};

/* @} */


}

#endif

