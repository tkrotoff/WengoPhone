#ifndef __PTHREAD_H__
#define __PTHREAD_H__

#include <phapi-util/util-exports.h>

#define pthread_t unsigned int
#define pthread_attr_t	void

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	PHAPI_UTIL_EXPORTS int pthread_create(pthread_t * thread, pthread_attr_t * attr, unsigned int (__stdcall *start_routine)(void *), void * arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PTHREAD_H__ */
