/*
 * Copyright (C) 2005 David Ferlier <david.ferlier@wengo.fr>
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "phapi.h"

#define PHCONFIG_DEFAULT_AUDIO_PORT "10600"
#define PHCONFIG_DEFAULT_VIDEO_PORT "10602"
#define PHCONFIG_DEFAULT_SIP_PORT   5060
#define PHCONFIG_DEFAULT_STUN_SERVER "80.118.132.74"

/*
 * @brief Initializes a new phapi configuration structure
 *
 */ 

static phConfig_t *cfg = NULL;

phConfig_t *ph_config_new() {
	phConfig_t *cfg;
	cfg = (phConfig_t *)malloc(sizeof(phConfig_t));
	memset(cfg, 0, sizeof(phConfig_t));
	return cfg;
}

/*
 * @brief Fills a phapi configuration structure with default values
 * @param cfg The configuration to modify
 *
 */

int ph_config_initialize_default(phConfig_t *cfg) {
	if (cfg == NULL)
		return -1;

	cfg->local_audio_rtp_port = (char *)malloc(
		strlen(PHCONFIG_DEFAULT_AUDIO_PORT));
	strcpy(cfg->local_audio_rtp_port, PHCONFIG_DEFAULT_AUDIO_PORT);

	cfg->local_video_rtp_port = (char *)malloc(
		strlen(PHCONFIG_DEFAULT_VIDEO_PORT));
	strcpy(cfg->local_video_rtp_port, PHCONFIG_DEFAULT_VIDEO_PORT);

	cfg->sipport = PHCONFIG_DEFAULT_SIP_PORT;
	strcpy(cfg->stunserver, PHCONFIG_DEFAULT_STUN_SERVER);

	printf("Ports configuration %s, %s, %d\n",
		cfg->local_audio_rtp_port,
		cfg->local_video_rtp_port,
		cfg->sipport);

	return 0;
}

/*
 * @brief Returns the phcfg "singleton". This is ugly, but remember there
 * was a global phcfg before.
 *
 */ 

MY_DLLEXPORT phConfig_t *phGetConfig() {
	if (cfg == NULL) {
		cfg = ph_config_new();
		ph_config_initialize_default(cfg);
	}
	return cfg;
}
