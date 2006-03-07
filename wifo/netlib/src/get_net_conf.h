#ifndef _ENVDISCOVERY_
#define _ENVDISCOVERY_

/*typedef enum 
{
   StunTypeUnknown=0,
   StunTypeOpen,
   StunTypeConeNat,
   StunTypeRestrictedNat,
   StunTypePortRestrictedNat,
   StunTypeSymNat,
   StunTypeSymFirewall,
   StunTypeBlocked,
   StunTypeFailure
} NatType;*/

typedef		struct net_info_s
{
	/** enum */
	int		NatType;

	/**
	 * Check if UDP SIP port (5060) is opened or not
	 * boolean
	 */
	int		SIP_Port;

	/** boolean UNUSED */
	int		Audio_Port;

	/**
	 * Check if TCP HTTP port (80) is opened or not
	 * boolean
	 */
	int		Http_Port;

	/**
	 * Check if TCP HTTPS port (443) is opened or not
	 * boolean
	 */
	int		Https_Port;

	/** Local HTTP Proxy address */
	char		*Proxy_IP;

	/** Local HTTP Proxy Port */
	int		Proxy_Port;
}			net_info_t;

#ifdef __cplusplus
extern "C" {
#endif

void	GetNetConf(net_info_t *ptr, const char *StunServer);

#ifdef __cplusplus
}
#endif

#endif
