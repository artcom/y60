extern "C" {

GLOBAL(void)
jpeg_mem_dest (j_compress_ptr cinfo,
              JOCTET * pData,
              int FileSize,
              PLDataSink * pDataSrc
);

}
