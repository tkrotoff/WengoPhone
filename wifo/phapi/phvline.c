#include <assert.h>
#include <eXosip/eXosip.h>

#include <phvline.h>
#include <phapi-old.h>


#if defined(WIN32) || defined(_WIN32_WCE)
#define snprintf _snprintf
#define strncasecmp strnicmp
#define strcasecmp stricmp
#define usleep(usecs) Sleep((usecs)/1000)
#endif


phVLine ph_vlines[PH_MAX_VLINES];

static osip_from_t *ph_parse_from(const char *userid);


phVLine* ph_vlid2vline(int vlid) 
{
	return ph_vlines + (vlid) - 1;
}

int ph_vline2vlid(phVLine* vl)
{
	return ((vl) - ph_vlines + 1);
}


phVLine *
ph_valid_vlid(int vlid)
{
	phVLine *vl = 0;

	if (vlid > 0 && vlid <= PH_MAX_VLINES)
	{
		vl = ph_vlid2vline(vlid);
		if (!vl->used || vl->LineState == OWPL_LINE_STATE_DELETING)
		{
			vl = 0;
		}
	}
	return vl;
}

// Allocate memory for a new phVLine structure
phVLine * vline_alloc()
{
  int i;

  for(i = 0; i < PH_MAX_VLINES; i++)
    {
      phVLine *vl = ph_vlines + i;

      if (!vl->used)
	{
	  memset(vl, 0, sizeof(*vl));
	  vl->used = 1;
	  vl->port = 0;
	  return vl;
	}

    }
  return 0;
}

// Free resource used by the specified phVLine pointer
void vline_free(phVLine *vl)
{
  if (vl->used)
    {
      osip_free(vl->username);
      if (vl->server)
	osip_free(vl->server);
      if (vl->proxy)
	osip_free(vl->proxy);
      if (vl->displayname)
	osip_free(vl->displayname);
      if (vl->followme)
	osip_free(vl->followme);
      if (vl->contact)
	osip_free(vl->contact);
      vl->used = 0;
    }
}

phVLine *
ph_find_vline_by_rid(int rid)
{
  int i;

  for(i = 0; i < PH_MAX_VLINES; i++)
    {
      phVLine *vl = ph_vlines + i;

      if (vl->used && rid == vl->rid)
	return vl;
    }
  return 0;
}

phVLine *
ph_find_matching_vline(const char *userid, int ignore)
{
  osip_from_t *from;
  phVLine *vl;
  char *host;
  char hostport[256];
  char *uname;

  hostport[0] = 0;
  from = ph_parse_from(userid);

  if (!from)
    return 0;

  if (from->url)
    uname = from->url->username;
  else
    uname = hostport;


  if (!(ignore & PHM_IGNORE_PORT) && from->url && from->url->port)
    {
      snprintf(hostport, sizeof(hostport), "%s:%s", from->url->host, from->url->port);
      host = hostport;
    }
  else if (ignore & PHM_IGNORE_HOST)
    host = 0;
  else if (from->url)
    {
      host = from->url->host;
    }
  else
    host = hostport;

  vl = ph_find_matching_vline2(uname, host, ignore);

  osip_from_free(from);

  return vl;

}

phVLine *
ph_find_matching_vline2(const char *username, const char* host, int ignore)
{
	char srvbuf[256];
	int port;
	
	ph_split_host_port(srvbuf, sizeof(srvbuf), host, &port);

	//return ph_find_matching_vline3(username, host, 0, ignore);
	return ph_find_matching_vline3(username, srvbuf, port, ignore);
}

phVLine *
ph_find_matching_vline3(const char *username, const char* host, int port, int ignore)
{
	int i;
	int   hostlen;
	int   unamelen;
	phVLine *vl, *defaultvl = 0;

	if (!username) {
		username = "";
	}
  
	hostlen = host ? strlen(host) : 0;
	unamelen =  strlen(username);

	if (port == 0) {
		port = 5060;
	}

	for( i = 0; i < PH_MAX_VLINES; i++)
    {
		int len;
		vl = ph_vlines + i;

		if (!vl->used) {
			continue;
		}
      
		if (!vl->server)
		{
			defaultvl = vl;
			continue;
		}
      
		len = strlen(vl->server);
      
		if ((ignore & PHM_IGNORE_HOST) || ((len == hostlen) && !strcasecmp(host, vl->server)))
		{
			len = strlen(vl->username);

			if ((len == unamelen) && !strcasecmp(username, vl->username))
			{
				if (ignore & PHM_IGNORE_PORT)
				{
					defaultvl = vl;
					break;
				}
				if (port == vl->port)
				{
					defaultvl = vl;
					break;
				} 
			}
		}
	}
	return defaultvl;
}

static time_t last_vline_refresh;
static time_t last_nat_refresh;

static void
ph_nat_refresh(phVLine *vl)
{
	char buf[128];
	char to[128];
	assert(vl);

	ph_vline_get_from(buf, sizeof(buf), vl);
	snprintf(to, sizeof(to), "sip:ping@%s", vl->server);
	phSendOptions(buf, to);
}

void ph_refresh_vlines()
{
	time_t now = time(NULL);

	if (now - last_vline_refresh > 5)
	{
		int i;
		phVLine *vl;

		for (i = 0; i < PH_MAX_VLINES; i++)
		{
			vl = ph_vlines + i;  
			if (!vl->used)
				continue;

			if (vl->LineState != OWPL_LINE_STATE_NOT_ACTIVE)
			{
				if (vl->server && vl->server[0] && vl->regTimeout > 0)
				{
					if ((now - vl->lastRegTime) > (vl->regTimeout - 5))
						phvlRegister(ph_vline2vlid(vl));
				}
			}
		}
		last_vline_refresh = time(0);
	}

	//if (!phcfg.use_tunnel && phcfg.nat_refresh_time > 0)
	if (phcfg.nat_refresh_time > 0)
	{
		int i;
		phVLine *vl;

		if (now - last_nat_refresh > phcfg.nat_refresh_time)
		{
			for( i = 0; i < PH_MAX_VLINES; i++)
			{
				vl = ph_vlines + i;  
				if (!vl->used)
					continue;

				if (vl->server && vl->server[0] && vl->regTimeout > 0)
				{
					ph_nat_refresh(vl);
				}
			}
		
			last_nat_refresh = time(0);
		}
	}
}

int
phvlRegister(int vlid)
{
  phVLine *vl = ph_vlid2vline(vlid);
  int ret = -1;
  char utmp[256];
  char stmp[256];
  char *server;


	assert(vl);
	assert(vl->username);
	assert(vl->server);	

	snprintf(utmp, sizeof(utmp), "sip:%s@%s", vl->username, vl->server);

	server = stmp;
	if (vl->port && vl->port != 5060)
	{
		/*     snprintf(stmp, sizeof(stmp), "sip:%s@%s:%d", vl->username, vl->server, vl->port); */
		snprintf(stmp, sizeof(stmp), "sip:%s:%d", vl->server, vl->port);
	}
	else
	{
		/*     snprintf(stmp, sizeof(stmp), "sip:%s@%s:%d", vl->username, vl->server, vl->port); */
		snprintf(stmp, sizeof(stmp), "sip:%s", vl->server); 
	}

	eXosip_lock();

	vl->rid = eXosip_register_init(utmp, server, vl->contact, vl->proxy);

	if (vl->rid >= 0)
	{
		if (vl->regTimeout > 0) {
			_owplLineSetState(vlid, OWPL_LINE_STATE_REGISTERING);
		}
		else {
			_owplLineSetState(vlid, OWPL_LINE_STATE_UNREGISTERING);
		}

		ret = eXosip_register(vl->rid, vl->regTimeout);

		if (ret == 0)
		{
			ret = vl->rid;
			vl->lastRegTime = time(0);
		}
	}

	eXosip_unlock();

	return ret;
}

int
phvlUnregister(int vlid)
{
	phVLine *vl = ph_vlid2vline(vlid);

	if (vl) {
		vl->regTimeout = 0;
		return phvlRegister(vlid);
	}
	return -1;
}

void ph_vlines_init()
{
	memset(ph_vlines, 0, sizeof(ph_vlines));
}

static osip_from_t *ph_parse_from(const char *userid)
{
	osip_from_t *from;

	osip_from_init(&from);
	if (!from) {
		return 0;
	}

	osip_from_parse(from, userid); 
	if (from->url && from->url->port && !strcmp(from->url->port, "5060"))
    {
		osip_free(from->url->port);
		from->url->port = NULL;
    }

	return from;
}

void
ph_vline_get_from(char *buf, int bufSize, phVLine *vl)
{
  char *un, *s;

  assert(buf);
  assert(vl);

  un = (vl->username && vl->username[0]) ? vl->username : "unknown";
  s =  (vl->server && vl->server[0]) ? vl->server : "localhost";

  if (vl->displayname && vl->displayname[0])
  {
    if (!strchr(vl->displayname, ' '))
    {
      snprintf(buf, bufSize, "%s <sip:%s@%s>", vl->displayname, un, s);
    }
    else
    {
      snprintf(buf, bufSize, "\"%s\" <sip:%s@%s>", vl->displayname, un, s);
    }
  }
  else
  {
    snprintf(buf, bufSize, "<sip:%s@%s>", un, s);
  }
}

void _owplLineSetState(OWPL_LINE hLine, VLineStateEnum LineState)
{
	phVLine *vl;
	vl = ph_valid_vlid(hLine);
	/* TODO: Add line callback management code here*/
	vl->LineState = LineState;
}