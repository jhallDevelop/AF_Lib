#if defined(_WIN32) || defined(_WIN64)
// This part is for Windows platforms (MSVC compiler)
#ifdef AF_LIB_EXPORTS // This macro should be defined by the library project itself
#define AF_LIB_API __declspec(dllexport)
#else
#define AF_LIB_API __declspec(dllimport)
#endif
#else
// This part is for non-Windows platforms (GCC, Clang on macOS, Linux)
#define AF_LIB_API __attribute__((visibility("default")))
#endif
