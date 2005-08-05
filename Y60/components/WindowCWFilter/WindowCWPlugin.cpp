//============================================================================
//
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//     $Author: martin $
//
//   $Revision: 1.3 $
//
// Description:
//
//=============================================================================

#include "PLFilterWindowCW.h"

#include <asl/PlugInBase.h>

#include <y60/PLFilterFactory.h>

#include <iostream>

#define DB(x) //x

using namespace std;
using namespace asl;
using namespace y60;

DEFINE_EXCEPTION(FilterArgumentCount, asl::Exception)

class WindowCWFilterFactory :
    public IPaintLibFilterFactory
{
    asl::Ptr<PLFilter> createFilter(const y60::VectorOfFloat & theParameters) {
        DB(cerr << "WindowCWFilterFactory::createFilter" << endl);
        if (theParameters.size() != 2) {
            throw FilterArgumentCount("WindowCWFilter expects two paramters", PLUS_FILE_LINE);
        }        
        return asl::Ptr<PLFilter>(new PLFilterWindowCW(theParameters[0], theParameters[1]));    
    }  
};

class WindowCWPlugin :
    public PlugInBase      
{
public:
    WindowCWPlugin(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle)
    {}    
protected:
	void _initializeSingletonManager(SingletonManager* theSingletonManager) {
	    PlugInBase::_initializeSingletonManager(theSingletonManager);
	    cerr << "adding windowCW filter" << endl;
	    PaintLibFilterFactory::get().addFilterFactory(
	        "windowCW", 
	        asl::Ptr<IPaintLibFilterFactory>(new WindowCWFilterFactory)
        );
    }

};

extern "C"
EXPORT PlugInBase* plfilterWindowCW_instantiatePlugIn(DLHandle myDLHandle) {
    return new WindowCWPlugin(myDLHandle);
}
