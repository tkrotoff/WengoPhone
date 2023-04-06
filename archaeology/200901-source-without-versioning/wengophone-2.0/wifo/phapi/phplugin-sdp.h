// <ncouturier>

#ifndef __PHPLUGIN_SDP_H__
#define __PHPLUGIN_SDP_H__

#ifdef WIN32
#if defined(BUILD_PHAPI_DLL)
#define MY_DLLEXPORT __declspec(dllexport) 
#elif defined(PHAPI_DLL)
#define MY_DLLEXPORT __declspec(dllimport) 
#endif
#endif

#ifndef MY_DLLEXPORT
#define MY_DLLEXPORT 
#endif 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


MY_DLLEXPORT unsigned int sdp_register_plugin();


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __PHPLUGIN_SDP_H__ */

// </ncouturier>
