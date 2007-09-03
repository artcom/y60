/*
 *=============================================================================
 * Copyright (C) 1993-2005, ART+COM AG Berlin
 *
 * These coded instructions, statements, and computer programs contain
 * unpublished proprietary information of ART+COM AG Berlin, and
 * are copy protected by law. They may not be disclosed to third parties
 * or copied or duplicated in any form, in whole or in part, without the
 * specific, prior written permission of ART+COM AG Berlin.
 *=============================================================================
 */

/*
 * This quick hack allows running ART+COM spidermonkey standalone.
 *
 * Compile with "cc -o main main.c -L o.ANT.LINUX.DBG -ljsspidermonkeyDBG -g"
 *           or "cc -o main main.c -L o.ANT.LINUX.REL -ljsspidermonkey"
 *
 * Run with "./main".
 *
 */

extern int mymain(int argc, char **argv);

int
main(int argc, char **argv)
{
  return mymain(argc, argv);
}

