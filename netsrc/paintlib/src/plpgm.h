/*
/--------------------------------------------------------------------
|
|      Jose Miguel Buenaposada Biencinto. Mar 2000.
|
\--------------------------------------------------------------------
*/

//! \file
//! Defines structures and constants present in PGM files.

#ifndef INCL_PLPGM
#define INCL_PLPGM

typedef struct _PgmHeader
{
    int ImageType;           // Image Type
    int ImageWidth;          // Image Width
    int ImageHeight;         // Image Height
	int MaxGrayValue;  

} PGMHEADER;

#define PGM_MAXLINESIZE 80  // Maximum number of characters per line

// Definitions for image types.
#define PGM_P2 0
#define PGM_P5 1

#endif
