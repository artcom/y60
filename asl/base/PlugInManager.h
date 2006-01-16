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

