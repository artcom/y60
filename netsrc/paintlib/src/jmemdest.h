extern "C" {
#ifndef JCONFIG_INCLUDED
#include "jconfig.h"
#endif
#if JPEG_LIB_VERSION < 80

GLOBAL(void)
jpeg_mem_dest (j_compress_ptr cinfo,
              JOCTET * pData,
              int FileSize,
              PLDataSink * pDataSrc
);

#endif
}
