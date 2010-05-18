#ifdef _WIN32
#   define ACMAKE_CMAKE_HEADER_ONLY_WORKAROUND_DECL __declspec( dllexport )
#else
#   define ACMAKE_CMAKE_HEADER_ONLY_WORKAROUND_DECL __attribute__((unused))
#endif
namespace {
    ACMAKE_CMAKE_HEADER_ONLY_WORKAROUND_DECL int cmake_header_only_workaround;
}
