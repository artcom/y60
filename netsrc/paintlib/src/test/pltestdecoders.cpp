/*
/--------------------------------------------------------------------
|
|      $Id: pltestdecoders.cpp,v 1.30 2004/10/02 22:23:13 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "config.h"
#include "pltestdecoders.h"
#include "plpaintlibdefs.h"
#include "planybmp.h"

#include "planydec.h"
#include "testpsddecoder.h"
#include "pltestexif.h"
#include "pltester.h"

#include <string>
#include <iostream>

using namespace std;

#ifndef WIN32
#include <sys/stat.h>

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/types.h>
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#include <fcntl.h>
#include <unistd.h>

#endif

//#define CREATE_TEST_DATA
//#define INCLUDE_SUBDIRS

PLTestDecoders::PLTestDecoders ()
{
}

PLTestDecoders::~PLTestDecoders ()
{
}

bool testDirectoryExists() {
#ifdef WIN32
  WIN32_FIND_DATA FindData;
  string s = PLTester::m_sTestFileDir+"*.*";
  HANDLE hSearch = FindFirstFile (s.c_str(), &FindData);
  return (hSearch != INVALID_HANDLE_VALUE);
#else
  struct dirent **namelist;
  int SourceFileCount = scandir(PLTester::m_sTestFileDir.c_str(), &namelist, 0, alphasort);
  return (SourceFileCount > 0); // Empty=0, error=-1
#endif
}

void PLTestDecoders::RunTests ()
{
  cerr << "Running Decoder tests...\n";

#ifdef CREATE_TEST_DATA
  m_InfoFile = fopen (PLTester::m_sTestFileList.c_str(), "w");
  if (m_InfoFile == 0)
  {
    cerr << "  Could not create test data file " << PLTester::m_sTestFileList << "." << endl;
    cerr << "             ---->> Skipping." << endl;
  }
  else
  {
    cerr << "  Writing test data file to " << PLTester::m_sTestFileList << "." << endl;
    getDirInfo ("");
    fclose(m_InfoFile);
  }
#else
  //TODO: Get length of file and allocate enough memory.
  char * pszInfoFile = new char[65535];
  m_InfoFile = fopen (PLTester::m_sTestFileList.c_str(), "r");
  if (m_InfoFile == 0)
  {
    cerr << "Could not open test image information file " << PLTester::m_sTestFileList << endl;
    cerr << "No decoder tests performed.\n";
    SetFailed();
  }
  else if (!testDirectoryExists())
  {
    cerr << "Could not open test image directory " << PLTester::m_sTestFileDir << endl;
    cerr << "No decoder tests performed.\n";
    SetFailed();
  }
  else
  {
    int BytesRead = fread (pszInfoFile, 1, 65535, m_InfoFile);
    fclose (m_InfoFile);
    pszInfoFile[BytesRead] = 0;
    string sInfoFile(pszInfoFile);
    bool bDone = false;
    int EOLPos = 0;
    while (!bDone)
    {
      int LastEOLPos = EOLPos;
      EOLPos = sInfoFile.find ('\n', EOLPos+1);
      if (EOLPos != string::npos)
      {
        string sCurLine = sInfoFile.substr(LastEOLPos, EOLPos-LastEOLPos);
        EOLPos+=1;
        testImage (sCurLine);
      }
      else
      {
        bDone = true;
      }
    }
    cerr << "Testing PSD decoder...\n";
    PLTestPSDDecoder PSDTester;
    PSDTester.RunTests();

    PLTestExif TestExif;
    TestExif.RunTests();
    AggregateStatistics(TestExif);
  }
  testFileNotFound();
#endif
}


#ifndef WIN32
// this function is used to select all files that do not start with a dot
int selectFile(const struct dirent * theEntry)
{
  if (theEntry->d_name[0] == '.')
  {
    return 0;
  }
  return 1;
}
#endif

void PLTestDecoders::getDirInfo (const char * pszDirName)
{
  cerr << "Searching directory " << PLTester::m_sTestFileDir.c_str() << pszDirName << "." << endl;

#ifdef WIN32
  WIN32_FIND_DATA FindData;

  char sz[256];
  sprintf (sz, "%s%s*.*", PLTester::m_sTestFileDir.c_str(), pszDirName);
  HANDLE hSearch = FindFirstFile (sz, &FindData);
  bool bOK = (hSearch != INVALID_HANDLE_VALUE);
  while (bOK)
  {
    if (FindData.cFileName[0] != '.')
      if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      { // Recurse directories
#ifdef INCLUDE_SUBDIRS
        sprintf (sz, "%s%s\\", pszDirName, FindData.cFileName);
        getDirInfo (sz);
#endif
      }
      else
      {
        addToInfoFile (pszDirName, FindData.cFileName);
      }
    bOK = (FindNextFile (hSearch, &FindData) != 0);
  }
  FindClose (hSearch);
#else
  struct dirent **namelist;
  string s;
  s = PLTester::m_sTestFileDir+pszDirName;
  int SourceFileCount = scandir(s.c_str(), &namelist, selectFile, alphasort);
  for (int i=0; i < SourceFileCount; ++i)
  {
    addToInfoFile (pszDirName, namelist[i]->d_name);
  }
#endif
}

void PLTestDecoders::addToInfoFile (const char * pszDirName, const char * pszFName)
{
  PLAnyPicDecoder Decoder;
  try
  {
    char szFullName[1024];
    sprintf(szFullName, "%s%s%s", PLTester::m_sTestFileDir.c_str(), pszDirName, pszFName);
    Decoder.OpenFile (szFullName);
    PLAnyBmp Bmp;
    Decoder.MakeBmp(&Bmp);
    char sz[256];
    Decoder.AsString (sz, 256);
    fprintf (m_InfoFile, "%s%s: %s\n", pszDirName, pszFName, sz);
    Decoder.Close();
  }
  catch (PLTextException e)
  {
    if (e.GetCode() != PL_ERRACCESS_DENIED &&
        e.GetCode() != PL_ERRUNKNOWN_FILE_TYPE)
    {
      fprintf (m_InfoFile, "%s%s: { Error, code: %i}\n", pszDirName, pszFName, e.GetCode());
      Decoder.Close();
    }
  }
}

void PLTestDecoders::testImage (string sLine)
{
#ifndef WIN32
  // Remove \r if it's there
  int CRPos = sLine.find("\r");
  if (CRPos != string::npos)
  {
    sLine = sLine.substr(0, CRPos);
  }
#endif

  int ColonPos = sLine.find(':');
  string sFName = sLine.substr(0, ColonPos);
  cerr << "    " << sFName.c_str() << ": ";
  string sExpectedInfo = sLine.substr(ColonPos+2);

  PLAnyPicDecoder Decoder;
  char szRealInfo[256];
  try
  {
    // Standard MakeBmpFromFile
    cerr << "File, ";
    PLAnyBmp Bmp;
    Decoder.MakeBmpFromFile ((PLTester::m_sTestFileDir+sFName).c_str(), &Bmp);
    Bmp.AsString (szRealInfo, 256);
    Test (sExpectedInfo == szRealInfo);

#ifdef WIN32
    {
      cerr << "unicode file, ";
      wchar_t UCName[1024];
      mbstowcs(UCName, (PLTester::m_sTestFileDir+sFName).c_str(), 1024);
      Decoder.MakeBmpFromFileW (UCName, &Bmp);
      Bmp.AsString (szRealInfo, 256);
      Test (sExpectedInfo == szRealInfo);
    }
#endif

    // URL test.
    cerr << "url\n";
    string sURL = makeURLFromFName (PLTester::m_sTestFileDir+sFName);
    Decoder.MakeBmpFromURL(sURL.c_str(), &Bmp);
    Bmp.AsString (szRealInfo, 256);
    Test (sExpectedInfo == szRealInfo);

    // Multi-Phase Load
    Decoder.OpenFile ((PLTester::m_sTestFileDir+sFName).c_str());
    Decoder.AsString (szRealInfo, 256);
    Test (sExpectedInfo == szRealInfo);
    Decoder.MakeBmp(&Bmp);
    Decoder.Close();

  }
  catch (PLTextException e)
  {
    if (e.GetCode() == PL_ERRFORMAT_NOT_COMPILED)
    {
      cerr << "      --> Test skipped.\n";
    }
    else
    {
      sprintf (szRealInfo, "{ Error, code: %i}", e.GetCode());
      cerr << endl << "      --> " << (const char *)e << endl;
      Test (sExpectedInfo == szRealInfo);
      Decoder.Close();
    }
  }
}

void PLTestDecoders::testFileNotFound()
{
  cerr << "Testing file not found exception.\n";
  bool bCorrectException = false;
  try 
  {
    PLAnyPicDecoder Decoder;
    PLAnyBmp Bmp;
    Decoder.MakeBmpFromFile ("thisfiledoesntexist.bmp", &Bmp);
  } 
  catch (PLTextException e)
  {
    if (e.GetCode() == PL_ERRFILE_NOT_FOUND)
    {
      bCorrectException = true;
    }
  }
  Test(bCorrectException);
}

string PLTestDecoders::makeURLFromFName(const string& sFName)
{
  char szFullFName[512];
#ifdef WIN32
  _fullpath(szFullFName, sFName.c_str(), 512);
  while (char * pszSlash = strchr(szFullFName, '\\'))
  {
    *pszSlash ='/';
  }
#else
  getcwd(szFullFName, 512);
  strcat(szFullFName, "/");
  strcat (szFullFName, sFName.c_str());
#endif
  return string("file://")+szFullFName;
}

/*
/--------------------------------------------------------------------
|
|      $Log: pltestdecoders.cpp,v $
|      Revision 1.30  2004/10/02 22:23:13  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.29  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.28  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.27  2004/09/09 16:52:50  artcom
|      refactored PixelFormat
|
|      Revision 1.26  2004/08/20 14:39:41  uzadow
|      Added rle format to sgi decoder. Doesn't work yet, though.
|
|      Revision 1.25  2004/04/15 20:22:22  uzadow
|      - All test output goes to cerr now.
|      - Fixed behaviour when test images were not available.
|
|      Revision 1.24  2004/04/15 19:09:38  uzadow
|      - Moved TestBmpList to test source, where it is versioned.
|      - All test output goes to cerr now.
|
|      Revision 1.23  2004/03/04 18:05:58  uzadow
|      Fixed linux file not found exception bug.
|
|      Revision 1.22  2003/08/03 12:03:22  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.21  2003/06/03 14:12:12  artcom
|      Only one test failure if test images aren't available.
|
|      Revision 1.20  2003/04/18 20:33:00  uzadow
|      Linux exif changes
|
|      Revision 1.19  2003/04/13 21:51:43  uzadow
|      Added exif loading - windows ver.
|
|      Revision 1.18  2003/02/27 23:01:37  uzadow
|      Linux version of PLFilterRotate, nicer test output
|
|      Revision 1.17  2003/02/24 22:10:35  uzadow
|      Linux version of MakeBmpFromURL() tests
|
|      Revision 1.16  2003/02/24 20:59:40  uzadow
|      Added windows url support, url support tests.
|
|      Revision 1.15  2003/02/24 16:19:50  uzadow
|      Added url source (linux only), improved configure support for libungif detection.
|
|      Revision 1.14  2003/02/15 21:27:00  uzadow
|      Added win32 version of url data source.
|
|      Revision 1.13  2002/11/18 14:45:37  uzadow
|      Added PNG compression support, fixed docs.
|
|      Revision 1.12  2002/11/06 22:47:00  uzadow
|      *** empty log message ***
|
|      Revision 1.11  2002/11/04 22:40:13  uzadow
|      Updated for gcc 3.1
|
|      Revision 1.10  2002/11/04 21:12:36  uzadow
|      *** empty log message ***
|
|      Revision 1.9  2002/11/02 23:03:29  uzadow
|      Fixed BmpInfo handling for transparent gifs
|
|      Revision 1.8  2002/11/02 22:38:28  uzadow
|      Fixed \\r handling in TestBmpList.txt, removed libtool from cvs
|
|      Revision 1.7  2002/10/20 22:11:53  uzadow
|      Partial doc update.
|      Fixed tests when images were not available.
|
|      Revision 1.6  2002/08/09 22:43:46  uzadow
|      no message
|
|      Revision 1.5  2002/08/07 18:50:39  uzadow
|      More cygwin compatibility changes.
|
|      Revision 1.4  2002/08/06 20:13:37  uzadow
|      Cross-platform stuff.
|
|      Revision 1.3  2002/08/05 20:43:12  uzadow
|      cygwin compatibility changes
|
|      Revision 1.2  2002/08/04 21:20:42  uzadow
|      no message
|
|      Revision 1.1  2002/08/04 20:12:34  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from
|      images without loading the whole image and proper greyscale support.
|      Major improvements in tests.
|
|
|
\--------------------------------------------------------------------
*/
