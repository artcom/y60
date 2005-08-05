// ----------------------------------------------------------------------------
//
// Copyright (C) 2001-2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
// ----------------------------------------------------------------------------
//
// Name: Corba.cpp
//
// Purpose: All CORBA Handling is done in this module
//
// $RCSfile: Corba.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2003/05/12 14:20:23 $
// $Author: uzadow $
//
// $Log $
//
// ----------------------------------------------------------------------------

#include <OB/CORBA.h>
#include <OB/Util.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>

#include "../Interface/DirectorLink.h"
#include "Corba.h"


using namespace std;  // automatically added!



// ----------------------------------------------------------------------------
//

// global vars

static CORBA_ORB_var	orb = NULL;
static CORBA_Object_var	obj = NULL;		// general CORBA object
static DirectorLink_var	remote = NULL;  // narrow DirectorLink object

long	debugLevel = 0;
char	iorFilename [1024] = "\0";

// ----------------------------------------------------------------------------
//

int initDirectorLinkCORBA (char* errorMessage)
{
	if (errorMessage != NULL)
		strcpy (errorMessage, "<DirectorLinkError reason=\"not connected\"/>");

	try
	{
		// init ORB
		if ( !orb ) {
		   	char *argv[3];
			int argc;

			argv[0] = "prog";
			argv[1] = NULL;
			argc = 1;

			orb = CORBA_ORB_init(argc, argv);
		}

		// find valid IOR filename
		if (strlen (iorFilename) == 0 ) {
			strcpy(iorFilename, getenv( "DIRECTORLINK_IOR" ));
			if (strlen (iorFilename) == 0 )
				strcpy (iorFilename, "DirectorLink.ior");
		}

		// check IOR file
		// Refer to environment variable 'DIRECTORLINK_IOR'
		ifstream in(iorFilename, ios::in|ios::nocreate);
		if (in.fail())
		{
			if (errorMessage != NULL)
				sprintf( errorMessage, 
					"<DirectorLinkError reason="
					"\"file '%s': %s.\""
					"/>", iorFilename, strerror(errno) );
			return (0); // failed
		}

		// read IOR file
		char filebuffer [1024];
		in >> filebuffer;


		// find OBJ
		obj = orb -> string_to_object(filebuffer);

		if ( CORBA_is_nil(obj) ) {
			// cannot generate object, so init failed and no calling action will be executed
			if (errorMessage != NULL)
				sprintf( errorMessage, 
							"<DirectorLinkError reason="
							"\"cannot find general object from IOR\""
							"/>");
			return (0); // failed 
		}

		assert(!CORBA_is_nil(obj));

		remote = DirectorLink::_narrow(obj);

		if ( CORBA_is_nil(remote) ) {
			// cannot generate narrow object, so init failed and no calling action will be executed
			if (errorMessage != NULL)
				sprintf( errorMessage, 
							"<DirectorLinkError reason="
							"\"cannot find narrow object\""
							">"
							"</error>");
			return (0); // failed
		}

		assert(!CORBA_is_nil(remote));
	}
#ifdef __GNUG__
    catch(CORBA_COMM_FAILURE& ex)
#else
    catch(CORBA_SystemException& ex)
#endif
    {
		if (errorMessage != NULL)
			sprintf( errorMessage,
							"<DirectorLinkError reason="
							"\"CORBA exception: %s\""
							"/>", 
							OBExceptionToString (ex));

		if (debugLevel >= 2)
			OBPrintException(ex);

		return (0); // failed
    }

    return (1); // succeeded
}


// ----------------------------------------------------------------------------
//

int callDirectorLinkCORBA ( const char *inMessage, char* outMessage )

{
	char* resultMessage;

	if (CORBA_is_nil(remote)) {
		if (outMessage != NULL)
			sprintf( outMessage,
						"<DirectorLinkError reason="
						"\"internal error: no remote object\""
						"/>");
		return (0);   // failed
	} else {
		try
		{
			resultMessage = remote -> callY50 ( inMessage );
		}

#ifdef __GNUG__
		catch(CORBA_COMM_FAILURE& ex)
#else
		catch(CORBA_SystemException& ex)
#endif
		{
			if (outMessage != NULL)
				sprintf( outMessage,
							"<DirectorLinkError reason="
							"\"CORBA exception: %s\""
							"/>", 
							OBExceptionToString (ex));
			if (debugLevel >= 2)
				OBPrintException(ex);

			return (0);  // failed
		} 
	}

	// dup string so CORBA can release it.
	strcpy (outMessage, resultMessage);
	CORBA_string_free(resultMessage);

	return (1); // succeeded
}


