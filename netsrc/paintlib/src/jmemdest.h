#if 0 // XXX disabled because of a nameclash with jpeg8 
extern "C" {

GLOBAL(void)
jpeg_mem_dest (j_compress_ptr cinfo,
              JOCTET * pData,
              int FileSize,
              PLDataSink * pDataSrc
);

}
#endif
