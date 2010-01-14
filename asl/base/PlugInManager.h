/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description:  plugin manager
//
// Last Review: pavel 8.12.2005
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : ok
//    formatting             : ok
//    documentation          : poor
//    test coverage          : poor
//    names                  : ok
//    style guide conformance: ok
//    technical soundness    : ok
//    dead code              : poor
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : fair
//
//    recommendations:
//       - write documentation
//       - add some tests
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _PLUGIN_MANAGER_INCLUDED_
#define _PLUGIN_MANAGER_INCLUDED_

#include "asl_base_settings.h"

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

class ASL_BASE_DECL PlugInManager :
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

