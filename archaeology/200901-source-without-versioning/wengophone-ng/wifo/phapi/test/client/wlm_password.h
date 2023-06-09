#ifndef WLM_PASSWORD_H
#define WLM_PASSWORD_H

#ifndef WINDOWS
#include <unistd.h>
#endif /* WINDOWS */

char *
wlm_sip_password_get
(
	const char * user_id,
	const char * domain_name,
	const char * password,
	char * sip_password,
	size_t sip_password_size
) ;

#endif /* WLM_PASSWORD_H */
