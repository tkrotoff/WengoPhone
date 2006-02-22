/*
 * miniua -  minimalistic sip user agent
 *
 * Copyright (C) 2002,2003   Aymeric Moizard <jack@atosc.org>
 * Copyright (C) 2004        Vadim Lebedev <vadim@mbdsys.com>
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

/**
 * @file miniua.c
 * @brief minimalistic SIP User Agent
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/wait.h>
#include <errno.h>
//#include <unistd.h>
//#include <dirent.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>
#include <signal.h>

#ifdef WIN32
#define snprintf _snprintf
#define strncasecmp strnicmp
#define sleep(x) Sleep(x*1000)
#endif



#include "phapi.h"
#include "phrpc.h"


static int dtmf_mode = 3;
static int verboseFlag = 0;
static int autoring = 0;
static int autoans = 0;
static int autoreject = 0;
static int oldinit = 0;
static int novl = 0;
static int xfervlid = 0;
static int noinit = 0;

static int defvlid;

/* extern eXosip_t eXosip; */


static char  proxy_server[256], tunnel_server[256],  proxy_user[256], proxy_passwd[256];
int proxy_port, tunnel_port = 80;

static char phapi_server[32] = "127.0.0.1";



#if defined(__DATE__) && defined(__TIME__)
static const char server_built[] = __DATE__ " " __TIME__;
#else
static const char server_built[] = "unknown";
#endif



/**
 * call progress callback routine
 * @param cid    call id
 * @param info   call state information
 */
static void  callProgress(int cid, const phCallStateInfo_t *info);

static void  transferProgress(int cid, const phTransferStateInfo_t *info);
static void  confProgress(int cfid, const phConfStateInfo_t *info);
static void  regProgress(int regid, int status);

phCallbacks_t mycbk = { callProgress, transferProgress, confProgress, regProgress };

static char default_sip_target[128] = "nowhere";

static char currentid[512];  /* current sip identity */

static int callbackStatus;


void
usage (int code)
{
  printf ("\n\
usage:\n\
\n\
   miniua args\n\
\n\
\t [-c default sip target]\n\
\t [-l <log file>]\n\
\t [-f <from url>]\n\
\t [-d <verbose level>]\n\
\t [-s <phapi server address>[:<port>]]\n\
\t [-cp <phapi callback port>]\n\
\t [-p SIP proxy address]\n\
\t [-fp force proxy\n\
\t [-r userid,pass,realm,server]\n\
\t [-n nattype  (auto, none, fcone, rcone, prcone, sym)]\n\
\t [-stun addr:port  stun server spec (nape:port or addr:port]\n\
\t [-codecs codec preference list (PCMU,PCMA,GSM)\n\
\t [-m dtmfmode (1 - inband, 2 - rtp, 3 - both)\n\
\t [-nomedia actived nomedia mode (use only SIP signalling - without RTP)\n\
\t [-sipport SIP port number]\n\
\t [-autoring activate automatic RINGING answer on icomming calls\n\
\t [-autoredir activate automatic handling of 3xx responses\n\
\t [-autoans activate automatic call accept on icomming calls\n\
\t [-verbose when activated miniua echo all commands]\n\
\t [-noinit  don't call phInit]\n\
\t [-novl force old init semantics + avoid phAddVline call\n\
\t [-vad force VAD\n\
\t [-cng force CNG\n\
\t [-hdx level\n\
\t [-noaec\n\
\t [-h]\n\
\t [-v]\n");

    exit (code);
}


static int cmdloop(const char* userid, const char *regserver, FILE *file, int doecho);

static void getreginfo(const char *buf, char *user, char *pass, char *realm, char *server)
{
  const char *tok = strstr(buf, ",");

  while(buf < tok)
    *user++ = *buf++;
  *user = 0;

  tok = strstr(++buf, ",");
  while(buf < tok)
    *pass++ = *buf++;
  *pass = 0;

  tok = strstr(++buf, ",");
  while(buf < tok)
    *realm++ = *buf++;
  *realm = 0;

  strcpy(server, buf+1);
}


static char *
fixsipid(char *str, int size)
{
  if (str[0] == 0)
    return str;

  if (0 == strstr(str, "sip:"))
    {
      char *tmp = strdup(str);
      snprintf(str, size, "sip:%s", tmp);
      free(tmp);
    }
  return str;
}


  

int main(int argc, const char *const *argv) 
{

  /* deal with options */
  char c;
  int i;
  int send_subscription = 0;
  int needreg=0;
  char userid[256], regserver[256], realm[256], pass[256];
  char proxy[256];


  userid[0] =  regserver[0] = realm[0] =  pass[0] =
    proxy[0] = 0;



  phcfg.autoredir = 1;


  for( i = 1; i < argc; i++)
    {
      const char *arg = argv[i];

      if (!strcmp(arg, "-m"))
	{
	  dtmf_mode = 3 & atoi(argv[++i]);
	}
      else if (!strcmp(arg, "-c"))
	{
	  strncpy(default_sip_target, argv[++i], sizeof(default_sip_target));
	}
      else if (!strcmp(arg, "-f"))
	{
	  if (i == argc - 1)
	    usage(0);

	  strncpy(phcfg.identity, argv[++i], sizeof(phcfg.identity));
	  strncpy(currentid, argv[i], sizeof(currentid));
	  
	}
      else if (!strcmp(arg, "-d"))
	{
	  if (i == argc - 1)
	    usage(0);
	     
	  phDebugLevel = atoi(argv[++i]);
	}
      else if (!strcmp(arg, "-s"))
	{
	  char *colon;
	  if (i == argc - 1)
	    usage(0);
	  
	  strncpy(phapi_server, argv[++i], sizeof(phapi_server));
	  if (colon = strstr(phapi_server,":"))
	    {
	      phServerPort = atoi(arg+1);
	      *colon = 0;
	    }
	}
      else if (!strcmp(arg, "-l"))
	{
	  if (i == argc - 1)
	    usage(0);

	  phLogFileName = (char*) argv[++i];
	}
      else if (!strcmp(arg, "-p"))
	{
	  if (i == argc - 1)
	    usage(0);


	  strncpy(phcfg.proxy, argv[++i], sizeof(phcfg.proxy));
	}
      else if (!strcmp(arg, "-sipport"))
	{
	  if (i == argc - 1)
	    usage(0);


	  strncpy(phcfg.sipport, argv[++i], sizeof(phcfg.sipport));
	}
      else if (!strcmp(arg, "-fp"))
	phcfg.force_proxy = 1;
      else if (!strcmp(arg, "-r"))
	{
	  getreginfo(argv[++i], userid, pass, realm, regserver);
	  snprintf(phcfg.identity, sizeof(phcfg.identity), "sip:%s@%s", userid,regserver);
	  strncpy(currentid, phcfg.identity, sizeof(currentid));
	  needreg = 1;
	}

      else if (!strcmp(arg, "-v") || !strcmp(arg, "-V"))
	{
#ifdef VERSION
            printf ("miniua: version:     %s\n", VERSION);
#endif
            printf ("build: %s\n", server_built);
	    fflush(stdout);
	}
      else if (!strcmp(arg, "-h") || !strcmp(arg, "-?") || !strcmp(arg, "?"))
	{
#ifdef VERSION
            printf ("miniua: version:     %s\n", VERSION);
#endif
            printf ("build: %s\n", server_built);
            usage (0);
	    fflush(stdout);
	}
      else if (!strcmp(arg, "-n"))
	{
	  if (i == argc - 1)
	    usage(0);

	  strncpy(phcfg.nattype, argv[++i], sizeof(phcfg.nattype));
	}
      else if (!strcmp(arg, "-codecs"))
	{
	  if (i == argc - 1)
	    usage(0);

	  strncpy(phcfg.audio_codecs, argv[++i], sizeof(phcfg.audio_codecs));
	}
      else if (!strcmp(arg, "-nomedia"))
	phcfg.nomedia = 1;
      else if (!strcmp(arg, "-autoredir"))
	phcfg.autoredir = 1;
      else if (!strcmp(arg, "-stun"))
	{
	  strncpy(phcfg.stunserver, argv[++i], sizeof(phcfg.stunserver));
	}
      else if (!strcmp(arg, "-verbose"))
	{
	  verboseFlag = 1;
	}
      else if (!strcmp(arg, "-autoring"))
	{
	  autoring = 1;
	}
      else if (!strcmp(arg, "-autoans"))
	{
	  autoans = 1;
	}
      else if (!strcmp(arg, "-noinit"))
	{
	  noinit = 1;
	}
      else if (!strcmp(arg, "-novl"))
	{
	  novl = 1;
	}
      else if (!strcmp(arg, "-vad"))
	{
#ifdef EMBED
	  phcfg.vad = VAD_VALID_MASK | (500 & VAD_THRESHOLD_MASK);
#else
	  phcfg.vad = VAD_VALID_MASK | (1000 & VAD_THRESHOLD_MASK);
#endif
	}
      else if (!strcmp(arg, "-cng"))
	{
	  phcfg.cng = 1;
	}
      else if (!strcmp(arg, "-noaec"))
	{
	  phcfg.noaec = 1;
	}
      else if (!strcmp(arg, "-hdx"))
	{
	  phcfg.hdxmode = 1;
	  phcfg.vad = atoi(argv[++i]);
	}
    }



  fixsipid(currentid, sizeof(currentid));

  phcfg.identity[0] = 0;
  strncpy(proxy, phcfg.proxy, sizeof(proxy));
  phcfg.proxy[0] = 0;


  if (phDebugLevel > 0)
    {
#ifdef VERSION
      printf ("miniua: %s\n", VERSION);
#endif
      printf ("Debug level:        %i\n", phDebugLevel);
      if (phLogFileName == NULL)
        printf ("Log name:           Standard output\n");
      else
        printf ("Log name:           %s\n", phLogFileName);

      fflush(stdout);
    }


#ifdef SIGTRAP
  signal(SIGTRAP, SIG_IGN);
#endif

  if (!noinit && phInit(&mycbk, phapi_server, 1))
    {
      fprintf (stderr, "miniua: could not initialize phoneapi\n");
      exit(0);
    }


  /*  
   * we've got -r parameters so we need to prepare for autentication and 
   * register ourselves with the registration server
   */
  if (!noinit && needreg)
    {
      phAddAuthInfo(userid, userid, pass, 0, realm);
      defvlid = phAddVline(userid, regserver, proxy, 3600);

    }
  else if (currentid[0] != 0)
    {
      char *p;
      char server[256];

      strcpy(userid, currentid+4);
      p = strchr(userid, '@');
      if (p)
	{
	  *p = 0;
	  strncpy(server, p+1, sizeof(server));
	}
      if (!noinit && !novl)
	phAddVline(userid, server, NULL, 0);
    }


      

  printf("Welcome To Miniua\n");
  fflush(stdout);
  

  cmdloop(userid, regserver, stdin, 0);
  phTerminate();
  exit(0);
  return(0);
}







static void 
callProgress(int cid, const phCallStateInfo_t *info) 
{
  int ncid;
  int r;

  callbackStatus = 0;
  switch (info->event)
    {
      case phDIALING:
	printf("DIALING line=%d cid=%d uri=%s\n", info->vlid, cid, info->u.remoteUri);
	callbackStatus = 0;
	break;

      case phRINGING:
	printf("RINGING cid=%d uri=%s\n", cid, info->u.remoteUri);
	callbackStatus = 0;	
	break;

      case phRINGandSTART:
	printf("RINGINGandSTART cid=%d uri=%s\n", cid, info->u.remoteUri);
	callbackStatus = 0;	
	break;

      case phRINGandSTOP:
	printf("RINGandSTOP cid=%d uri=%s\n", cid, info->u.remoteUri);
	callbackStatus = 0;	
	break;


      case phNOANSWER:
	printf("NOANSWER cid=%d uri=%s\n", cid, info->u.remoteUri);
	callbackStatus = phNOANSWER;
	break;

      case phCALLBUSY:	
	printf("BUSY cid=%d uri=%s\n", cid, info->u.remoteUri);
	callbackStatus = phCALLBUSY;
	break;

      case phCALLREDIRECTED:
	printf("REDIRECTED cid=%d newcid=%d to=%s\n", cid, info->newcid, info->u.remoteUri);	
	callbackStatus = 0;
	break;

      case phCALLOK:
	printf("CALLOK cid=%d uri=%s\n", cid, info->u.remoteUri);
	callbackStatus = 0;
	break;

      case phCALLHELD: 
	printf("CALLHELD cid=%d  status=%d\n", cid, info->u.errorCode);
	callbackStatus = info->u.errorCode;
	break;

      case phCALLRESUMED:
	printf("CALLRESUMED cid=%d  status=%d\n", cid, info->u.errorCode);
	break;

      case phHOLDOK:
	printf("HOLDOK cid=%d  status=%d\n", cid, info->u.errorCode);
	break;

      case phRESUMEOK:
	printf("RESUMEOK cid=%d  status=%d\n", cid, info->u.errorCode);
	callbackStatus = info->u.errorCode;
	break;

      case phINCALL:
	printf("INCALL line=%d cid=%d to=%s from=%s\n", info->vlid, cid, info->localUri, info->u.remoteUri);
	if (autoring)
	  {
	  callbackStatus = r = phRingingCall(cid);
	  printf("phRingigCall(%d)=%d\n", cid,r);
	  }
	if (autoans)
	  {
	  callbackStatus = r = phAcceptCall(cid);
	  printf("phAcceptCall(%d)=%d\n", cid,r);
	  }
	if (autoreject)
	  {
	  callbackStatus = r = phRejectCall(cid, 488);
	  printf("phRejectCall(%d)=%d\n", cid,r);
	  }

	break;

      case phCALLCLOSED:
	printf("CALLCLOSED cid=%d  status=%d\n", cid, info->u.errorCode);
	callbackStatus = info->u.errorCode;
	break;


      case phCALLCLOSEDandSTOPRING:
	printf("CALLCLOSEDandSTOPRING cid=%d  status=%d\n", cid, info->u.errorCode);
	callbackStatus = info->u.errorCode;
	break;

      case phCALLERROR:
	printf("CALLERROR cid=%d  status=%d\n", cid, info->u.errorCode);
	callbackStatus = info->u.errorCode;
	break;

      case phDTMF:
	printf("DTMF cid=%d  digit=%c\n", cid, info->u.dtmfDigit);
	callbackStatus = 0;
	break;

      case phXFERREQ:
	printf("XFERREQ line=%d cid=%d newcid=%d to=%s", info->vlid, cid, info->newcid, info->u.remoteUri);
	//	ncid = phPlaceCall2(phcfg.identity, info->u.remoteUri, 0, cid);
	//	printf("New Call %d to %s\n", ncid, info->u.remoteUri);
	  
	xfervlid = info->vlid;
	callbackStatus = 0;
	break;

      case phXFERPROGRESS:
	printf("XFERPROGRESS cid=%d status=%d\n", cid, info->u.errorCode);
	callbackStatus = info->u.errorCode;
	break;

      case phXFEROK:
	printf("XFEROK cid=%d status=%d\n", cid, info->u.errorCode);
	//	phCloseCall(cid);
	callbackStatus = info->u.errorCode;
	break;

      case phXFERFAIL:
	printf("XFERFAIL cid=%d status=%d\n", cid, info->u.errorCode);
	phCloseCall(cid);
	callbackStatus = info->u.errorCode;
	break;

      case phCALLREPLACED:
	printf("CALLREPLACED cid=%d newcid=%d uri=%s\n", cid, info->newcid, info->u.remoteUri);
	phCloseCall(cid);
	callbackStatus = 0;	
	break;

    }

  fflush(stdout);
}

/**
 * transfer progress callback routine
 * @param cid    transfer id
 * @param info   transfer state information
 */
static void  
transferProgress(int cid, const phTransferStateInfo_t *info) 
{ 
}


/**
 * conference progress callback routine
 * @param cfid   conference id
 * @param info   conference state information
 */
static void  
confProgress(int cfid, const phConfStateInfo_t *info) 
{ 
}



static void  
regProgress(int regid, int status)
{
  char *cmd = (status & PH_UNREG_MASK) ? "UNREG" : "REG";

  printf("%s rid=%d, status=%d\n", cmd, regid, (status & ~PH_UNREG_MASK));
  callbackStatus = status & ~PH_UNREG_MASK;

  fflush(stdout);
}


/**
 * construct a valid uri from given input
 * @param buf   output buffer
 * @param uri   input
 * @param size  size of the output buffer
 */
static int 
geturi(char *buf, char* uri, int size)
{
  while(buf && isspace(*buf)) 
    buf++;

  uri[0] = 0;

  if (!buf)
    return 0;

  if (strncasecmp(buf, "sip:", 4))
    {
    strncat(uri, "sip:", size);
    uri += 4; size -= 4;
    }

  if (strchr(buf, '@'))
    snprintf(uri, size, buf);
  else
    snprintf(uri, size, "%s@%s", buf, default_sip_target);

  return 0;

}

enum CMDS {  
  CMD_CALL=1, CMD_HANGUP, CMD_ACCEPT, CMD_REJECT, CMD_RING, CMD_HOLD, CMD_RESUME, CMD_BUSY,
  CMD_DTMF, CMD_BTXFER, CMD_ATXFER, CMD_XFERCNF, CMD_OPT, CMD_UNREG, CMD_VLADD, CMD_VLDEL, 
  CMD_DTMFMODE, CMD_AUTOANS, CMD_AUTOREJ, CMD_AUTORING, CMD_SENDSF,
  CMD_AUTHADD, CMD_AUTHADD_2, CMD_SETID, CMD_EXIT, CMD_QUIT, CMD_LCALL, CMD_FOLLOW, CMD_SENDF, CMD_SLEEP, CMD_COMMENT, 
  CMD_EBREAK, CMD_SETV, CMD_NATINFO, CMD_ECHO, CMD_CONF, CMD_NOCONF, CMD_CONTACT, CMD_TUNPROXY, 
  CMD_TUNSERVER, CMD_TUNCONF, CMD_SIPPX, CMD_PHINIT, CMD_SUBSCRIBE, CMD_HELP 
};

struct cmd
{
  const   char  *cmd;
  const   char  *help;
  enum    CMDS  code;
};

const struct cmd cmdtab[] =
{
  { "lc", "vlid target\t- place Call to target using vlid is virtual line id", CMD_LCALL },
  { "fl", "target\t - set Follow Me", CMD_FOLLOW },
  { "h", "callid\t- Hangup the call", CMD_HANGUP },
  { "a", "callid\t- Accept incoming call", CMD_ACCEPT },
  { "r", "callid\t- Reject incoming call", CMD_REJECT },
  { "n", "callid\t- send riNging event for a call", CMD_RING },
  { "o", "callid\t- hOld the call", CMD_HOLD },
  { "u", "callid\t- resUme the call", CMD_RESUME },
  { "b", "0/1\t- set Busy flag (when 1 all incoming calls are automatically rejected)", CMD_BUSY },
  { "m", "callid dtmfchar\t- send dtMf signal", CMD_DTMF },
  { "bx", "callid target\t- Blind Xfer call to target", CMD_BTXFER },
  { "ax", "callid callid2\t- Assisted Xfer call to callid2", CMD_ATXFER },
  { "xp", "callid target\t- Place call in response to XferReq", CMD_XFERCNF },
  { "op", "callid\t- send OPtions", CMD_OPT }, 
  { "ur", "vlid\t- unregister", CMD_UNREG },
  { "vla", "username server [p=proxy t=timeout]\t- Add Virtual Line", CMD_VLADD },
  { "vld", "vlid\t- delete virtual line", CMD_VLDEL },
  { "auta","username userid  pass realm\t- Add authenitcation info", CMD_AUTHADD }, 
  { "auta2","username userid  pass\t- Add authenitcation info with realm empty", CMD_AUTHADD_2 }, 
  { "id", "userid\t- set current identity (ex: user@192.168.10.13)", CMD_SETID }, 
  { "sleep", "seconds\t- sleep number of seconds", CMD_SLEEP }, 
  { "dtmfmode"," 1/2/3/0\t- dtmf mode mask (1 - INBAND, 2 - OUTBABAND 3 or 0 - BOTH)", CMD_DTMFMODE },
  { "sf", "cid filname\t-  send sound file", CMD_SENDSF }, 
  { "q", "- Quit",  CMD_QUIT },
  { "ebreak", "code\t- exit if previous command returned error(code is the program exit code)",  CMD_EBREAK },
  { "exit", "code\t- exit with the specified error code",  CMD_EXIT },
  { "autoring", "0/1\t- change autoring setting (when set automatically generate RINGING on incoming calls)",  CMD_AUTORING },
  { "autoans", "0/1\t- change autoanswer setting (when set automatically accepts incoming calls)",  CMD_AUTOANS },
  { "autorej", "0/1\t- change autrejectd setting (when set automatically rejects incoming calls)",  CMD_AUTOREJ },
  { "verbose", "0/1\t- set verbose flag... When set the commands are echoed to stdout)",  CMD_SETV },
  { "echo", "text\t- display the given text",  CMD_ECHO },
  { "natinfo", "show NAT information", CMD_NATINFO },
  { "b", "vlid 0/1 set busy flag for vlid. If vlid==0 set global bust flag", CMD_BUSY },
  { "fl", "vlid uri\t set followme address for given vlid to given uri. If vlid==0 do global setting. Mising URI means reset it", CMD_FOLLOW },
  { "k", "cid1 cid2", CMD_CONF },
  { "nk", "cid1 cid2", CMD_NOCONF },
  { "vlc", "vlid contact", CMD_CONTACT },
  { "tp",  "addres port\t - set http proxy server and port", CMD_TUNPROXY },
  { "ts",  "server port\t - set tunnel server and port", CMD_TUNSERVER },
  { "tc",  "\t - configure the tunnel", CMD_TUNCONF },
  { "sippx", "proxy:port\t - set SIP proxy address", CMD_SIPPX },
  { "phi", "\t - do phInit",  CMD_PHINIT },
  { "sub", "vlid uri winfoflag\t - send a subscribe request",  CMD_SUBSCRIBE },
  { "?", "- show this message",  CMD_HELP }
};

#define NCMDS (sizeof(cmdtab)/sizeof(cmdtab[0]))

static int 
parseCmd(char *buf,  char **args)
{
  char *space;
  int  i, len, blen;

  if (buf[0] == '#')
    return CMD_COMMENT;

  *args = 0;
  space = strchr(buf, ' ');
  if (space)
    {
    *space++ = 0;
    while(isspace(*space))
      space++;

    *args = space;
    }

  blen = strlen(buf);
  for(i = 0; i < NCMDS; i++)
    {
      len = strlen(cmdtab[i].cmd);
      if ((len == blen) &&  !strncasecmp(buf, cmdtab[i].cmd, len))
	return cmdtab[i].code;
    }

  return CMD_HELP;
  

}


static void showhelp()
{
  int i;

  for(i = 0; i < NCMDS; i++)
    printf("%s\t%s\n", cmdtab[i].cmd, cmdtab[i].help);

  fflush(stdout);
}


static int doinclude(const char *userid, const char *server, char *buf)
{
  FILE *file;
  int doexit;

  while(*buf && isspace(*buf))
    buf++;

  file = fopen(buf, "r");
  if (file == NULL)
    {
      perror(buf);
      return 0;
    }
  
  printf("Including: %s\n", buf);
  fflush(stdout);

  doexit = cmdloop(userid, server, file, 1);

  fclose(file);

  printf("Done: %s\n", buf);
  fflush(stdout);

  return doexit;
}

  
static void dobreak(int x)
{
  exit(x);
}

  

static int cmdloop(const char* userid, const char *regserver, FILE *file, int doecho)
{
  char buf[256];
  char uri[256];
  int cid;
  char *args;
  int vlid;
  int prevret;


  while(!feof(file))
    {
      char *p;
      char tmp[16];
      int ret = -1;
      int  dtmf, skipresult = 0;
      char *rd;

      fflush(stdout);
      rd = fgets(buf, sizeof(buf), file);

      if (!rd)
	return 0;
      

      if (doecho || verboseFlag)
	{
	  printf(buf);
	  fflush(stdout);
	}

      p = strstr(buf, "\n");
      if (p)
	*p = 0;


      if (buf[0] == '!')
	{
	int doexit = doinclude(userid, regserver, buf+1);
	
	if (!doexit)
	  {
	    skipresult = 1;
	    continue;
	  }

	return 1;
	}


      switch(parseCmd(buf, &args))
	{
	case CMD_BTXFER:
	  {
	    p = args;
	    cid = atoi(args);


	    while(*p && isdigit(*p))
	      p++;

	    while(*p && isspace(*p))
	      p++;

	    if (!geturi(p, uri, 256))
	      ret = phBlindTransferCall(cid, uri);
	    
	    break;
	  }


	case CMD_ATXFER:
	  {
	    int cid2;

	    p = args;
	    cid = atoi(args);


	    while(*p && isdigit(*p))
	      p++;

	    while(*p && isspace(*p))
	      p++;

	    cid2 = atoi(p);
	    ret = phTransferCall(cid, cid2);
	    
	    break;
	  }

#if 1
	case CMD_CONF:
	  {
	    int cid2;

	    p = args;
	    cid = atoi(args);


	    while(*p && isdigit(*p))
	      p++;

	    while(*p && isspace(*p))
	      p++;

	    cid2 = atoi(p);
	    ret = phConf(cid, cid2);
	    
	    break;
	  }

	case CMD_NOCONF:
	  {
	    int cid2;

	    p = args;
	    cid = atoi(args);


	    while(*p && isdigit(*p))
	      p++;

	    while(*p && isspace(*p))
	      p++;

	    cid2 = atoi(p);
	    ret = phStopConf(cid, cid2);
	    
	    break;
	  }
#endif

	case CMD_CONTACT:
	  {
	    vlid = atoi(args);
	    
	    p = args;

	    while(*p && isdigit(*p))
	      p++;

	    while(*p && isspace(*p))
	      p++;

	    ret = phSetContact(vlid, p);
	    
	    break;
	  }
	case CMD_XFERCNF:
	  {
	    p = args;
	    cid = atoi(args);


	    while(*p && isdigit(*p))
	      p++;

	    while(*p && isspace(*p))
	      p++;

	    if (!geturi(p, uri, 256))
	      {
		ret = cid = phLinePlaceCall2(xfervlid, currentid, uri, 0, cid);
		printf("Call %d to %s\n", cid, uri);   fflush(stdout);
		skipresult = 1;
	      }
	    
	    break;
	  }

	case CMD_HANGUP:
	  cid = atoi(args);
	  ret = phCloseCall(cid);
	  break;
	case CMD_ACCEPT:
	case 'A':
	  cid = atoi(args);
	  ret = phAcceptCall(cid);
	  break;
	case CMD_REJECT:
	  cid = atoi(args);
	  ret = phRejectCall(cid, 488);
	  break;
	case CMD_RING:
	  cid = atoi(args);
	  ret = phRingingCall(cid);
	  break;

	case CMD_HOLD:
	  cid = atoi(args);
	  ret = phHoldCall(cid);
	  break;

	case CMD_RESUME:
	  cid = atoi(args);
	  ret = phResumeCall(cid);
	  break;

        case CMD_QUIT:
	  return 1;

	case CMD_BUSY:
	  {
	    char *tok;

	    vlid = atoi(args);
	    tok = strstr(args, " ");
	    
	    if (!vlid)
	      ret = phSetBusy(atoi(tok));
	    else
	      {
	        tok = strstr(NULL, " ");
		ret = phLineSetBusy(vlid, atoi(tok));
	      }
	    break;
	  }

	case CMD_DTMF:
	  sscanf(args, "%d %s", &cid, tmp);
	  ret = phSendDtmf(cid, tmp[0], dtmf_mode);
	  break;

	case CMD_OPT:
	  ret = phSendOptions(currentid, "sip:nobody@nobody.com");
	  break;


	case CMD_SETID:
	  p = args;
	  while(*p && isspace(*p))
	    p++;
	  strncpy(currentid, p, sizeof(currentid));
	  fixsipid(currentid, sizeof(currentid));
	  skipresult = 1;
	  break;

	case CMD_AUTHADD:
	  {
	    char *uname, *uid, *pass, *realm;

	    uname = strtok(args, " ");
	    uid = strtok(NULL, " ");
	    pass = strtok(NULL, " ");
	    realm = strtok(NULL, " ");

	    ret = phAddAuthInfo(uname, uid, pass, 0, realm);
	    break;
	  }

	case CMD_AUTHADD_2:
	  {
	    char *uname, *uid, *pass;

	    uname = strtok(args, " ");
	    uid = strtok(NULL, " ");
	    pass = strtok(NULL, " ");

	    ret = phAddAuthInfo(uname, uid, pass, 0, "");
	    break;
	  }

	case CMD_LCALL:
	  {
	    char *tok;
	    
	    vlid = atoi(args);
	    
	    tok = strtok(args, " ");
	    tok = strtok(NULL, " ");

	    if (!geturi(tok, uri, 256))
	      {
		ret = cid = phLinePlaceCall(vlid, uri, 0, 0);
		printf("Call %d to %s\n", cid, uri);   fflush(stdout);
		skipresult = 1;
	      } 

	    break;
	  }


	case CMD_SUBSCRIBE:
	  {
	    char *tok;
	    int sid;

	    vlid = atoi(args);
	    
	    tok = strtok(args, " ");
	    tok = strtok(NULL, " ");

	    if (!geturi(tok, uri, 256))
	      {
		int winfo;
		tok = strtok(NULL, " ");
		winfo = tok ? atoi(tok) : 0;

		ret = sid = phLineSubscribe(vlid, uri, winfo);
		printf("Subscribe %d to %s\n", sid, uri);   fflush(stdout);
		skipresult = 1;
	      } 

	    break;
	  }




	case CMD_SENDSF:
	  {
	    char *tok;
	    
	    cid = atoi(args);
	    
	    tok = strtok(args, " ");
	    tok = strtok(NULL, " ");

	    ret = phSendSoundFile(cid, tok);

	    break;
	  }


	case CMD_FOLLOW:
	  {
	    char *tok;
	    vlid = atoi(args);


	    tok = strtok(args, " ");
	    tok = strtok(NULL, " ");

	    geturi(tok , uri, 256);

	    if (!vlid)
	      {
		if (*uri != '\0')
			printf("Global Call Forward to uri=%s\n", uri);
		else
			printf("Removing Global Call Forward\n");
		ret = phSetFollowMe(uri);
		break;
	      }

	    if (*uri != '\0')
		    printf("Local Call Forward on VL %d to uri=%s\n", vlid, uri);
	    else
		    printf("Removing Local Call Forward on VL %d\n", vlid);
	    ret = phLineSetFollowMe(vlid, uri);

	    break;
	  }


	case CMD_VLDEL:
	  vlid = atoi(args);
	  ret = phDelVline(vlid);
	  break;



	case CMD_VLADD:
	  {
	    int timeout = 0;
	    char *username, *server = 0, *proxy = 0;

	    p = args;

	    while(*p && isspace(*p))
	      p++;
	   
	    username = p;

	    /* find timeout parameter */
	    p = strstr(args , "t=");
	    if  (!p)
	      p = strstr(args , "T=");
	    if (p)
	      timeout = atoi(p+2);

	    /* find proxy parameter */
	    p = strstr(args, "p=");
	    if  (!p)
	      p = strstr(args , "P=");

	    if (p)
	      {
		proxy = p + 2;
		p += 2;
		while(*p && !isspace(*p))
		  p++;
		*p = 0;
	      }

	    /* insert NUL after the server parameter */
	    p = strchr(username, ' ');
	    if (p)
	      {
		*p++ = 0;
		while(*p && isspace(*p))
		  p++;
	       
		server = p;
		while(*p && !isspace(*p))
		  p++;
		*p = 0;
	      }

	    /* insert NUL after the username parameter */
	    p = username;
	    while(*p && !isspace(*p))
	      p++;
		
	    *p = 0;

	    ret = phAddVline(username, server, proxy, timeout);
	    break;
	  }

	case CMD_DTMFMODE:
	  {
	    static const char *modestr[] = { "INBAND", "OUTBAND", "IN+OUT BAND" }; 
	    dtmf_mode = atoi(args);
	    dtmf_mode &= 3;
	    if (!dtmf_mode)
	      dtmf_mode = 3;
	    printf("DTMF MODE is %s\n", modestr[dtmf_mode-1]);   fflush(stdout);
	    skipresult = 1;
	  }
	  break;

	case CMD_SLEEP:
	  {
	    int secs;

	    secs = atoi(args);
	    
	    if (!secs)
	      secs = 1;

	    sleep(secs);
	    skipresult = 1;
	    ret = prevret;

	    break;
	  }
	  

        case CMD_EBREAK:
	  if (prevret < 0 || callbackStatus != 0)
	    dobreak(atoi(args));
	  skipresult = 1;
	  ret = prevret;
	  break;

        case CMD_EXIT:
	  exit(atoi(args));
	  break;



        case CMD_SETV:
	  verboseFlag = atoi(args) & 1;
          printf(verboseFlag ? "Verbose ON\n" : "Verbose OFF\n");   fflush(stdout);
	  skipresult = 1;
	  break;


        case CMD_AUTORING:
	  autoring = atoi(args) & 1;
          printf(autoring ? "AUTORING ON\n" : "AUTORING OFF\n");   fflush(stdout);
	  skipresult = 1;
	  break;

        case CMD_AUTOANS:
	  autoans = atoi(args) & 1;
          printf(autoans ? "AUTOANSWER ON\n" : "AUTOANSWER OFF\n");   fflush(stdout);
	  skipresult = 1;
	  break;

        case CMD_AUTOREJ:
	  autoreject = atoi(args) & 1;
          printf(autoreject ? "AUTOREJECT ON\n" : "AUTOREJECT OFF\n");   fflush(stdout);
	  skipresult = 1;
	  break;

        case CMD_NATINFO:
	  {
	    char  nttp[16], fwip[64];

	    ret = phGetNatInfo(nttp, sizeof(nttp), fwip, sizeof(fwip));

	    printf("NAT TYPE=%s, FW=%s\n",  nttp, fwip);   fflush(stdout);
	    break;
	  }


        case CMD_COMMENT:
	  ret = prevret;
	  skipresult = 1;
	  break;

	case CMD_ECHO:
	  ret = prevret;
	  skipresult = 1;
	  printf("%s\n", args);   fflush(stdout);
 

	case CMD_TUNPROXY:
	  {
	    char *s, *p;

	    s = strtok(args, " ");
	    p = strtok(NULL, " ");

	    strcpy(proxy_server, s);
	    if (p)
	      proxy_port = atoi(p);

	    ret = prevret;
	    skipresult = 1;
	    break;
	  }

	case CMD_TUNSERVER:
 	  {
	    char *s, *p;

	    s = strtok(args, " ");
	    p = strtok(NULL, " ");

	    strcpy(tunnel_server, s);
	    if (p)
	      tunnel_port = atoi(p);

	    ret = prevret;
	    skipresult = 1;
	    break;
	  }

	case CMD_TUNCONF:
	  ret = phTunnelConfig(proxy_server, proxy_port, tunnel_server, tunnel_port, proxy_user, proxy_passwd, 0);
	  break;


	case CMD_SIPPX:
	  {
	    char *p = args;

	    while(*p && isspace(*p))
	      p++;
	   
	    strncpy(phcfg.proxy, p, sizeof(phcfg.proxy));
	    ret = prevret;
	    skipresult = 1;

	    break;
	  }




	case CMD_PHINIT:
	  ret = phInit(&mycbk, phapi_server, 1);
	  break;


        default:
	  skipresult = 1;
	  showhelp();
	  break;

	

	}

      if (!skipresult)
	{
	  printf("result = %d\n", ret);
	  fflush(stdout);
	}
      prevret = ret;
    }

  return 0;
}

	  
  
