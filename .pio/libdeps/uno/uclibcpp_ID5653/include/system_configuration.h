/**
 * Settings of uclibcpp for Arduino platform
 */

// Include version

/*
 * Target Features and Options
 */
#define __UCLIBCXX_HAS_FLOATS__ 1
#define __UCLIBCXX_HAS_LONG_DOUBLE__ 1
#undef __UCLIBCXX_HAS_TLS__
// #define __UCLIBCXX_WARNINGS__ ""
// #define __BUILD_EXTRA_LIBRARIES__ ""
// #define __HAVE_DOT_CONFIG__ 1

/*
 * String and I/O Stream Support
 * Streams implemented through arduino staff
 */
#undef __UCLIBCXX_HAS_WCHAR__
#define __UCLIBCXX_IOSTREAM_BUFSIZE__ 32 // Used only in fstream
#undef __UCLIBCXX_HAS_LFS__ // Does not seems to be used
#undef __UCLIBCXX_SUPPORT_CDIR__ // cin, cout, etc. will be implemented explicit
#undef __UCLIBCXX_SUPPORT_CIN__
#undef __UCLIBCXX_SUPPORT_COUT__
#undef __UCLIBCXX_SUPPORT_CERR__
#undef __UCLIBCXX_SUPPORT_CLOG__

/*
 * STL and Code Expansion
 */
#define __UCLIBCXX_STL_BUFFER_SIZE__ 32
#undef __UCLIBCXX_CODE_EXPANSION__ // Does not seems to be used
#undef __UCLIBCXX_EXPAND_CONSTRUCTORS_DESTRUCTORS__
#undef __UCLIBCXX_EXPAND_STRING_CHAR__
#undef __UCLIBCXX_EXPAND_VECTOR_BASIC__
#undef __UCLIBCXX_EXPAND_IOS_CHAR__
#undef __UCLIBCXX_EXPAND_STREAMBUF_CHAR__
#undef __UCLIBCXX_EXPAND_ISTREAM_CHAR__
#undef __UCLIBCXX_EXPAND_OSTREAM_CHAR__
#undef __UCLIBCXX_EXPAND_FSTREAM_CHAR__
#undef __UCLIBCXX_EXPAND_SSTREAM_CHAR__

/*
 * Library Installation Options
 */
// #define __UCLIBCXX_RUNTIME_PREFIX__ "/usr/uClibc++"
// #define __UCLIBCXX_RUNTIME_INCLUDE_SUBDIR__ "/include"
// #define __UCLIBCXX_RUNTIME_LIB_SUBDIR__ "/lib"
// #define __UCLIBCXX_RUNTIME_BIN_SUBDIR__ "/bin"
#undef __UCLIBCXX_EXCEPTION_SUPPORT__
// #undef __IMPORT_LIBSUP__
// #undef __IMPORT_LIBGCC_EH__
// #define __BUILD_STATIC_LIB__ 1
// #define __BUILD_ONLY_STATIC_LIB__ 1
#undef __DODEBUG__
