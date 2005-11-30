@BOTTOM@

// Byte order in a 32 bit word
#ifdef WORDS_BIGENDIAN
#define MSB_FIRST  // Motorola et al.
#else
#define LSB_FIRST // Intel et al.
#endif

#undef PL_SUPPORT_TGA
#undef PL_SUPPORT_BMP
#undef PL_SUPPORT_TIFF
#undef PL_SUPPORT_PICT
#ifdef _WIN32
  #define PL_SUPPORT_WEMF
#endif
#undef PL_SUPPORT_PCX
#undef PL_SUPPORT_PGM
#undef PL_SUPPORT_PPM
#undef PL_SUPPORT_PSD
#undef PL_SUPPORT_SGI
#undef PL_SUPPORT_IFF85

#undef PL_SUPPORT_JPEG
#undef PL_SUPPORT_PNG
#undef PL_SUPPORT_GIF
#undef PL_SUPPORT_CURL
#undef PL_SUPPORT_SDL
#undef PL_SUPPORT_DFB

