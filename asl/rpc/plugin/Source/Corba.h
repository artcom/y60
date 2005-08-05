
// ----------------------------------------------------------------------------
// Name: Corba.h
//
// Purpose: All CORBA Handling is done in this module
//
// $RCSfile: Corba.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2003/05/12 14:20:23 $
// $Author: uzadow $
//
// $Log $
// ----------------------------------------------------------------------------

#ifndef _ac_DirectorLink_Corba_h_
#define _ac_DirectorLink_Corba_h_

int initDirectorLinkCORBA ( char* message_error);
int callDirectorLinkCORBA ( const char *message_in, char* message_out);

extern long debugLevel;		// 0: no debug, 1: essential debug, 2: full debug to console

#define MAX_FILENAME_SIZE 1024
extern char iorFilename[MAX_FILENAME_SIZE];

#endif
