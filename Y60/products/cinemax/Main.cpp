/////////////////////////////////////////////////////////////
// CINEMA SDK : MAIN MODULE								   //
/////////////////////////////////////////////////////////////
// VERSION    : CINEMA 4D								   //
/////////////////////////////////////////////////////////////
// (c) 1989-2002 MAXON Computer GmbH, all rights reserved  //
/////////////////////////////////////////////////////////////

// Starts the plugin registration
#include "c4d_include.h" 


// forward declarations 
Bool RegisterAcXmlExporter(void);

Bool _cdecl PluginStart(void) {
    // The one and only ART+COM XML-Export Plugin
	if (!RegisterAcXmlExporter()) return FALSE;	
	
	return TRUE;
}

void _cdecl PluginEnd(void) {
}

Bool _cdecl PluginMessage(LONG id, void *data) {
	//use the following lines to set a plugin priority
    
	switch (id)	{
		case C4DPL_INIT_SYS:
			if (!resource.Init()) return FALSE; // don't start plugin without resource
			return TRUE;

		case C4DMSG_PRIORITY: 
			return TRUE;
	}
    
	return FALSE;
}
