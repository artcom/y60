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
//     $Author: janbo $
//
//   $Revision: 1.3 $
//
// Description:
//
//=============================================================================

#include "GlurFilter.h"

#include <asl/PlugInBase.h>
#include <y60/PLFilterFactory.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

class GlurFilterFactory :
    public IPaintLibFilterFactory
{
    asl::Ptr<PLFilter> createFilter(const VectorOfFloat & theParameters) {
        //cerr << "GlurFilterFactory::createFilter" << endl;        
        if (!_myCachedFilterInstance || _myLastArguments != theParameters) {
            _myLastArguments = theParameters;
            _myCachedFilterInstance = asl::Ptr<PLFilter>(new GlurFilter(theParameters));    
        }
        return _myCachedFilterInstance;
    }  
private:
    asl::Ptr<PLFilter> _myCachedFilterInstance;
    VectorOfFloat _myLastArguments;
};

class GlurPlugin :
    public PlugInBase      
{
public:
    GlurPlugin(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle)
    {}    
protected:
	void _initializeSingletonManager(SingletonManager* theSingletonManager) {
	    PlugInBase::_initializeSingletonManager(theSingletonManager);
	    //cerr << "ADDING GLUR FILTER" << endl;
	    PaintLibFilterFactory::get().addFilterFactory(
	        "glur", 
	        asl::Ptr<IPaintLibFilterFactory>(new GlurFilterFactory)
        );
    }

};

extern "C"
EXPORT PlugInBase* GlurFilter_instantiatePlugIn(DLHandle myDLHandle) {
    return new GlurPlugin(myDLHandle);
}
