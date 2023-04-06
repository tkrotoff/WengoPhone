#ifndef __UTIL_EXPORTS__
#define __UTIL_EXPORTS__

#ifdef WIN32
#if defined(BUILD_UTILITIES_DLL)
#define PHAPI_UTIL_EXPORTS __declspec(dllexport) 
#elif defined(USE_UTILITIES_DLL)
#define PHAPI_UTIL_EXPORTS __declspec(dllimport) 
#endif
#endif

#ifndef PHAPI_UTIL_EXPORTS
#define PHAPI_UTIL_EXPORTS 
#endif

#endif /* __UTIL_EXPORTS__ */

