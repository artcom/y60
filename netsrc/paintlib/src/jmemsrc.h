GLOBAL(void)
jpeg_mem_src (j_decompress_ptr cinfo,
              JOCTET * pData,
              int FileSize,
              void *pDataSrc,
              JMETHOD(void, notifyCppWorld, (j_common_ptr)));

/*
/--------------------------------------------------------------------
|
|      $Log: jmemsrc.h,v $
|      Revision 1.7  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.6  2000/10/30 21:45:04  uzadow
|      Added again after deleting the file by mistake.
|
|      Revision 1.4  2000/10/26 21:06:17  uzadow
|      Removed dependency on jpegint.h
|
|
\--------------------------------------------------------------------
*/
