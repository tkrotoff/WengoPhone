/*
 * Gaim's oscar protocol plugin
 * This file is the legal property of its developers.
 * Please see the AUTHORS file distributed alongside this file.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Family 0x0017 - Authentication.
 *
 * Deals with the authorizer for SNAC-based login, and also old-style
 * non-SNAC login.
 *
 */

#include "oscar.h"

#include "cipher.h"

#include <ctype.h>

#define USE_XOR_FOR_ICQ

#ifdef USE_XOR_FOR_ICQ
/**
 * Encode a password using old XOR method
 *
 * This takes a const pointer to a (null terminated) string
 * containing the unencoded password.  It also gets passed
 * an already allocated buffer to store the encoded password.
 * This buffer should be the exact length of the password without
 * the null.  The encoded password buffer /is not %NULL terminated/.
 *
 * The encoding_table seems to be a fixed set of values.  We'll
 * hope it doesn't change over time!
 *
 * This is only used for the XOR method, not the better MD5 method.
 *
 * @param password Incoming password.
 * @param encoded Buffer to put encoded password.
 */
static int aim_encode_password(const char *password, guint8 *encoded)
{
	guint8 encoding_table[] = {
#if 0 /* old v1 table */
		0xf3, 0xb3, 0x6c, 0x99,
		0x95, 0x3f, 0xac, 0xb6,
		0xc5, 0xfa, 0x6b, 0x63,
		0x69, 0x6c, 0xc3, 0x9f
#else /* v2.1 table, also works for ICQ */
		0xf3, 0x26, 0x81, 0xc4,
		0x39, 0x86, 0xdb, 0x92,
		0x71, 0xa3, 0xb9, 0xe6,
		0x53, 0x7a, 0x95, 0x7c
#endif
	};
	unsigned int i;

	for (i = 0; i < strlen(password); i++)
		encoded[i] = (password[i] ^ encoding_table[i]);

	return 0;
}
#endif

#ifdef USE_OLD_MD5
static int aim_encode_password_md5(const char *password, const char *key, guint8 *digest)
{
	GaimCipher *cipher;
	GaimCipherContext *context;

	cipher = gaim_ciphers_find_cipher("md5");

	context = gaim_cipher_context_new(cipher, NULL);
	gaim_cipher_context_append(context, (const guchar *)key, strlen(key));
	gaim_cipher_context_append(context, (const guchar *)password, strlen(password));
	gaim_cipher_context_append(context, (const guchar *)AIM_MD5_STRING, strlen(AIM_MD5_STRING));
	gaim_cipher_context_digest(context, 16, digest, NULL);
	gaim_cipher_context_destroy(context);

	return 0;
}
#else
static int aim_encode_password_md5(const char *password, const char *key, guint8 *digest)
{
	GaimCipher *cipher;
	GaimCipherContext *context;
	guchar passdigest[16];

	cipher = gaim_ciphers_find_cipher("md5");

	context = gaim_cipher_context_new(cipher, NULL);
	gaim_cipher_context_append(context, (const guchar *)password, strlen(password));
	gaim_cipher_context_digest(context, 16, passdigest, NULL);
	gaim_cipher_context_destroy(context);

	context = gaim_cipher_context_new(cipher, NULL);
	gaim_cipher_context_append(context, (const guchar *)key, strlen(key));
	gaim_cipher_context_append(context, passdigest, 16);
	gaim_cipher_context_append(context, (const guchar *)AIM_MD5_STRING, strlen(AIM_MD5_STRING));
	gaim_cipher_context_digest(context, 16, digest, NULL);
	gaim_cipher_context_destroy(context);

	return 0;
}
#endif

/*
 * The FLAP version is sent by itself at the beginning of authorization
 * connections.  The FLAP version is also sent before the cookie when connecting
 * for other services (BOS, chatnav, chat, etc.).
 */
faim_export int aim_sendflapver(OscarSession *sess, OscarConnection *conn)
{
	FlapFrame *fr;

	if (!(fr = flap_frame_new(sess, conn, AIM_FRAMETYPE_FLAP, 0x01, 4)))
		return -ENOMEM;

	aimbs_put32(&fr->data, 0x00000001);

	aim_tx_enqueue(sess, fr);

	return 0;
}

/*
 * This just pushes the passed cookie onto the passed connection, without
 * the SNAC header or any of that.
 *
 * Very commonly used, as every connection except auth will require this to
 * be the first thing you send.
 *
 */
faim_export int aim_sendcookie(OscarSession *sess, OscarConnection *conn, const guint16 length, const guint8 *chipsahoy)
{
	FlapFrame *fr;
	aim_tlvlist_t *tl = NULL;

	if (!(fr = flap_frame_new(sess, conn, AIM_FRAMETYPE_FLAP, 0x01, 4+2+2+length)))
		return -ENOMEM;

	aimbs_put32(&fr->data, 0x00000001);
	aim_tlvlist_add_raw(&tl, 0x0006, length, chipsahoy);
	aim_tlvlist_write(&fr->data, &tl);
	aim_tlvlist_free(&tl);

	aim_tx_enqueue(sess, fr);

	return 0;
}

#ifdef USE_XOR_FOR_ICQ
/*
 * Part two of the ICQ hack.  Note the ignoring of the key.
 */
static int goddamnicq2(OscarSession *sess, OscarConnection *conn, const char *sn, const char *password, ClientInfo *ci)
{
	FlapFrame *fr;
	aim_tlvlist_t *tl = NULL;
	int passwdlen;
	guint8 *password_encoded;

	passwdlen = strlen(password);
	if (!(password_encoded = (guint8 *)malloc(passwdlen+1)))
		return -ENOMEM;
	if (passwdlen > MAXICQPASSLEN)
		passwdlen = MAXICQPASSLEN;

	if (!(fr = flap_frame_new(sess, conn, AIM_FRAMETYPE_FLAP, 0x01, 1152))) {
		free(password_encoded);
		return -ENOMEM;
	}

	aim_encode_password(password, password_encoded);

	aimbs_put32(&fr->data, 0x00000001); /* FLAP Version */
	aim_tlvlist_add_str(&tl, 0x0001, sn);
	aim_tlvlist_add_raw(&tl, 0x0002, passwdlen, password_encoded);

	if (ci->clientstring)
		aim_tlvlist_add_str(&tl, 0x0003, ci->clientstring);
	aim_tlvlist_add_16(&tl, 0x0016, (guint16)ci->clientid);
	aim_tlvlist_add_16(&tl, 0x0017, (guint16)ci->major);
	aim_tlvlist_add_16(&tl, 0x0018, (guint16)ci->minor);
	aim_tlvlist_add_16(&tl, 0x0019, (guint16)ci->point);
	aim_tlvlist_add_16(&tl, 0x001a, (guint16)ci->build);
	aim_tlvlist_add_32(&tl, 0x0014, (guint32)ci->distrib); /* distribution chan */
	aim_tlvlist_add_str(&tl, 0x000f, ci->lang);
	aim_tlvlist_add_str(&tl, 0x000e, ci->country);

	aim_tlvlist_write(&fr->data, &tl);

	free(password_encoded);
	aim_tlvlist_free(&tl);

	aim_tx_enqueue(sess, fr);

	return 0;
}
#endif

/*
 * Subtype 0x0002
 *
 * This is the initial login request packet.
 *
 * NOTE!! If you want/need to make use of the aim_sendmemblock() function,
 * then the client information you send here must exactly match the
 * executable that you're pulling the data from.
 *
 * Java AIM 1.1.19:
 *   clientstring = "AOL Instant Messenger (TM) version 1.1.19 for Java built 03/24/98, freeMem 215871 totalMem 1048567, i686, Linus, #2 SMP Sun Feb 11 03:41:17 UTC 2001 2.4.1-ac9, IBM Corporation, 1.1.8, 45.3, Tue Mar 27 12:09:17 PST 2001"
 *   clientid = 0x0001
 *   major  = 0x0001
 *   minor  = 0x0001
 *   point = (not sent)
 *   build  = 0x0013
 *   unknown= (not sent)
 *
 * AIM for Linux 1.1.112:
 *   clientstring = "AOL Instant Messenger (SM)"
 *   clientid = 0x1d09
 *   major  = 0x0001
 *   minor  = 0x0001
 *   point = 0x0001
 *   build  = 0x0070
 *   unknown= 0x0000008b
 *   serverstore = 0x01
 *
 */
faim_export int aim_send_login(OscarSession *sess, OscarConnection *conn, const char *sn, const char *password, ClientInfo *ci, const char *key)
{
	FlapFrame *fr;
	aim_tlvlist_t *tl = NULL;
	guint8 digest[16];
	aim_snacid_t snacid;

	if (!ci || !sn || !password)
		return -EINVAL;

#ifdef USE_XOR_FOR_ICQ
	/* If we're signing on an ICQ account then use the older, XOR login method */
	if (isdigit(sn[0]))
		return goddamnicq2(sess, conn, sn, password, ci);
#endif

	if (!(fr = flap_frame_new(sess, conn, AIM_FRAMETYPE_FLAP, 0x02, 1152)))
		return -ENOMEM;

	snacid = aim_cachesnac(sess, 0x0017, 0x0002, 0x0000, NULL, 0);
	aim_putsnac(&fr->data, 0x0017, 0x0002, 0x0000, snacid);

	aim_tlvlist_add_str(&tl, 0x0001, sn);

	/* Truncate ICQ passwords, if necessary */
	if (isdigit(sn[0]) && (strlen(password) > MAXICQPASSLEN))
	{
		char truncated[MAXICQPASSLEN + 1];
		strncpy(truncated, password, MAXICQPASSLEN);
		truncated[MAXICQPASSLEN] = 0;
		aim_encode_password_md5(truncated, key, digest);
	}
	else
	{
		aim_encode_password_md5(password, key, digest);
	}

	aim_tlvlist_add_raw(&tl, 0x0025, 16, digest);

#ifndef USE_OLD_MD5
	aim_tlvlist_add_noval(&tl, 0x004c);
#endif

	if (ci->clientstring)
		aim_tlvlist_add_str(&tl, 0x0003, ci->clientstring);
	aim_tlvlist_add_16(&tl, 0x0016, (guint16)ci->clientid);
	aim_tlvlist_add_16(&tl, 0x0017, (guint16)ci->major);
	aim_tlvlist_add_16(&tl, 0x0018, (guint16)ci->minor);
	aim_tlvlist_add_16(&tl, 0x0019, (guint16)ci->point);
	aim_tlvlist_add_16(&tl, 0x001a, (guint16)ci->build);
	aim_tlvlist_add_32(&tl, 0x0014, (guint32)ci->distrib);
	aim_tlvlist_add_str(&tl, 0x000f, ci->lang);
	aim_tlvlist_add_str(&tl, 0x000e, ci->country);

	/*
	 * If set, old-fashioned buddy lists will not work. You will need
	 * to use SSI.
	 */
	aim_tlvlist_add_8(&tl, 0x004a, 0x01);

	aim_tlvlist_write(&fr->data, &tl);

	aim_tlvlist_free(&tl);

	aim_tx_enqueue(sess, fr);

	return 0;
}

/*
 * This is sent back as a general response to the login command.
 * It can be either an error or a success, depending on the
 * presence of certain TLVs.  
 *
 * The client should check the value passed as errorcode. If
 * its nonzero, there was an error.
 */
static int parse(OscarSession *sess, aim_module_t *mod, FlapFrame *rx, aim_modsnac_t *snac, ByteStream *bs)
{
	aim_tlvlist_t *tlvlist;
	aim_rxcallback_t userfunc;
	struct aim_authresp_info *info;
	int ret = 0;

	info = (struct aim_authresp_info *)malloc(sizeof(struct aim_authresp_info));
	memset(info, 0, sizeof(struct aim_authresp_info));

	/*
	 * Read block of TLVs.  All further data is derived
	 * from what is parsed here.
	 */
	tlvlist = aim_tlvlist_read(bs);

	/*
	 * No matter what, we should have a screen name.
	 */
	memset(sess->sn, 0, sizeof(sess->sn));
	if (aim_tlv_gettlv(tlvlist, 0x0001, 1)) {
		info->sn = aim_tlv_getstr(tlvlist, 0x0001, 1);
		strncpy(sess->sn, info->sn, sizeof(sess->sn));
	}

	/*
	 * Check for an error code.  If so, we should also
	 * have an error url.
	 */
	if (aim_tlv_gettlv(tlvlist, 0x0008, 1))
		info->errorcode = aim_tlv_get16(tlvlist, 0x0008, 1);
	if (aim_tlv_gettlv(tlvlist, 0x0004, 1))
		info->errorurl = aim_tlv_getstr(tlvlist, 0x0004, 1);

	/*
	 * BOS server address.
	 */
	if (aim_tlv_gettlv(tlvlist, 0x0005, 1))
		info->bosip = aim_tlv_getstr(tlvlist, 0x0005, 1);

	/*
	 * Authorization cookie.
	 */
	if (aim_tlv_gettlv(tlvlist, 0x0006, 1)) {
		aim_tlv_t *tmptlv;

		tmptlv = aim_tlv_gettlv(tlvlist, 0x0006, 1);

		info->cookielen = tmptlv->length;
		info->cookie = tmptlv->value;
	}

	/*
	 * The email address attached to this account
	 *   Not available for ICQ or @mac.com logins.
	 *   If you receive this TLV, then you are allowed to use 
	 *   family 0x0018 to check the status of your email.
	 * XXX - Not really true!
	 */
	if (aim_tlv_gettlv(tlvlist, 0x0011, 1))
		info->email = aim_tlv_getstr(tlvlist, 0x0011, 1);

	/*
	 * The registration status.  (Not real sure what it means.)
	 *   Not available for ICQ or @mac.com logins.
	 *
	 *   1 = No disclosure
	 *   2 = Limited disclosure
	 *   3 = Full disclosure
	 *
	 * This has to do with whether your email address is available
	 * to other users or not.  AFAIK, this feature is no longer used.
	 *
	 * Means you can use the admin family? (0x0007)
	 *
	 */
	if (aim_tlv_gettlv(tlvlist, 0x0013, 1))
		info->regstatus = aim_tlv_get16(tlvlist, 0x0013, 1);

	if (aim_tlv_gettlv(tlvlist, 0x0040, 1))
		info->latestbeta.build = aim_tlv_get32(tlvlist, 0x0040, 1);
	if (aim_tlv_gettlv(tlvlist, 0x0041, 1))
		info->latestbeta.url = aim_tlv_getstr(tlvlist, 0x0041, 1);
	if (aim_tlv_gettlv(tlvlist, 0x0042, 1))
		info->latestbeta.info = aim_tlv_getstr(tlvlist, 0x0042, 1);
	if (aim_tlv_gettlv(tlvlist, 0x0043, 1))
		info->latestbeta.name = aim_tlv_getstr(tlvlist, 0x0043, 1);

#if 0
	if (aim_tlv_gettlv(tlvlist, 0x0048, 1)) {
		/* beta serial */
	}
#endif

	if (aim_tlv_gettlv(tlvlist, 0x0044, 1))
		info->latestrelease.build = aim_tlv_get32(tlvlist, 0x0044, 1);
	if (aim_tlv_gettlv(tlvlist, 0x0045, 1))
		info->latestrelease.url = aim_tlv_getstr(tlvlist, 0x0045, 1);
	if (aim_tlv_gettlv(tlvlist, 0x0046, 1))
		info->latestrelease.info = aim_tlv_getstr(tlvlist, 0x0046, 1);
	if (aim_tlv_gettlv(tlvlist, 0x0047, 1))
		info->latestrelease.name = aim_tlv_getstr(tlvlist, 0x0047, 1);

#if 0
	if (aim_tlv_gettlv(tlvlist, 0x0049, 1)) {
		/* lastest release serial */
	}
#endif

	/*
	 * URL to change password.
	 */
	if (aim_tlv_gettlv(tlvlist, 0x0054, 1))
		info->chpassurl = aim_tlv_getstr(tlvlist, 0x0054, 1);

#if 0
	/*
	 * Unknown.  Seen on an @mac.com screen name with value of 0x003f
	 */
	if (aim_tlv_gettlv(tlvlist, 0x0055, 1)) {
		/* Unhandled */
	}
#endif

	sess->authinfo = info;

	if ((userfunc = aim_callhandler(sess, rx->conn, snac ? snac->family : 0x0017, snac ? snac->subtype : 0x0003)))
		ret = userfunc(sess, rx, info);

	aim_tlvlist_free(&tlvlist);

	return ret;
}

#ifdef USE_XOR_FOR_ICQ
/*
 * Subtype 0x0007 (kind of) - Send a fake type 0x0007 SNAC to the client
 *
 * This is a bit confusing.
 *
 * Normal SNAC login goes like this:
 *   - connect
 *   - server sends flap version
 *   - client sends flap version
 *   - client sends screen name (17/6)
 *   - server sends hash key (17/7)
 *   - client sends auth request (17/2 -- aim_send_login)
 *   - server yells
 *
 * XOR login (for ICQ) goes like this:
 *   - connect
 *   - server sends flap version
 *   - client sends auth request which contains flap version (aim_send_login)
 *   - server yells
 *
 * For the client API, we make them implement the most complicated version,
 * and for the simpler version, we fake it and make it look like the more
 * complicated process.
 *
 * This is done by giving the client a faked key, just so we can convince
 * them to call aim_send_login right away, which will detect the session
 * flag that says this is XOR login and ignore the key, sending an ICQ
 * login request instead of the normal SNAC one.
 *
 * As soon as AOL makes ICQ log in the same way as AIM, this is /gone/.
 *
 * XXX This may cause problems if the client relies on callbacks only
 * being called from the context of aim_rxdispatch()...
 *
 */
static int goddamnicq(OscarSession *sess, OscarConnection *conn, const char *sn)
{
	FlapFrame fr;
	aim_rxcallback_t userfunc;

	fr.conn = conn;

	if ((userfunc = aim_callhandler(sess, conn, 0x0017, 0x0007)))
		userfunc(sess, &fr, "");

	return 0;
}
#endif

/*
 * Subtype 0x0006
 *
 * In AIM 3.5 protocol, the first stage of login is to request login from the 
 * Authorizer, passing it the screen name for verification.  If the name is 
 * invalid, a 0017/0003 is spit back, with the standard error contents.  If 
 * valid, a 0017/0007 comes back, which is the signal to send it the main 
 * login command (0017/0002). 
 *
 */
faim_export int aim_request_login(OscarSession *sess, OscarConnection *conn, const char *sn)
{
	FlapFrame *fr;
	aim_snacid_t snacid;
	aim_tlvlist_t *tl = NULL;

	if (!sess || !conn || !sn)
		return -EINVAL;

#ifdef USE_XOR_FOR_ICQ
	if (isdigit(sn[0]))
		return goddamnicq(sess, conn, sn);
#endif

	aim_sendflapver(sess, conn);

	if (!(fr = flap_frame_new(sess, conn, AIM_FRAMETYPE_FLAP, 0x02, 10+2+2+strlen(sn)+8 )))
		return -ENOMEM;

	snacid = aim_cachesnac(sess, 0x0017, 0x0006, 0x0000, NULL, 0);
	aim_putsnac(&fr->data, 0x0017, 0x0006, 0x0000, snacid);

	aim_tlvlist_add_str(&tl, 0x0001, sn);

	/* Tell the server we support SecurID logins. */
	aim_tlvlist_add_noval(&tl, 0x004b);

	/* Unknown.  Sent in recent WinAIM clients.*/
	aim_tlvlist_add_noval(&tl, 0x005a);

	aim_tlvlist_write(&fr->data, &tl);
	aim_tlvlist_free(&tl);

	aim_tx_enqueue(sess, fr);

	return 0;
}

/*
 * Subtype 0x0007
 *
 * Middle handler for 0017/0007 SNACs.  Contains the auth key prefixed
 * by only its length in a two byte word.
 *
 * Calls the client, which should then use the value to call aim_send_login.
 *
 */
static int keyparse(OscarSession *sess, aim_module_t *mod, FlapFrame *rx, aim_modsnac_t *snac, ByteStream *bs)
{
	int keylen, ret = 1;
	aim_rxcallback_t userfunc;
	char *keystr;

	keylen = aimbs_get16(bs);
	keystr = aimbs_getstr(bs, keylen);

	/* XXX - When GiantGrayPanda signed on AIM I got a thing asking me to register 
	 * for the netscape network.  This SNAC had a type 0x0058 TLV with length 10.  
	 * Data is 0x0007 0004 3e19 ae1e 0006 0004 0000 0005 */

	if ((userfunc = aim_callhandler(sess, rx->conn, snac->family, snac->subtype)))
		ret = userfunc(sess, rx, keystr);

	free(keystr);

	return ret;
}

/**
 * Subtype 0x000a
 *
 * Receive SecurID request.
 */
static int got_securid_request(OscarSession *sess, aim_module_t *mod, FlapFrame *rx, aim_modsnac_t *snac, ByteStream *bs)
{
	int ret = 0;
	aim_rxcallback_t userfunc;

	if ((userfunc = aim_callhandler(sess, rx->conn, snac->family, snac->subtype)))
		ret = userfunc(sess, rx);

	return ret;
}

/**
 * Subtype 0x000b
 *
 * Send SecurID response.
 */
faim_export int aim_auth_securid_send(OscarSession *sess, const char *securid)
{
	OscarConnection *conn;
	FlapFrame *fr;
	aim_snacid_t snacid;
	int len;

	if (!sess || !(conn = aim_getconn_type_all(sess, AIM_CONN_TYPE_AUTH)) || !securid)
		return -EINVAL;

	len = strlen(securid);

	if (!(fr = flap_frame_new(sess, conn, AIM_FRAMETYPE_FLAP, 0x02, 10+2+len)))
		return -ENOMEM;

	snacid = aim_cachesnac(sess, OSCAR_FAMILY_AUTH, OSCAR_SUBTYPE_AUTH_SECURID_RESPONSE, 0x0000, NULL, 0);
	aim_putsnac(&fr->data, OSCAR_FAMILY_AUTH, OSCAR_SUBTYPE_AUTH_SECURID_RESPONSE, 0x0000, 0);

	aimbs_put16(&fr->data, len);
	aimbs_putstr(&fr->data, securid);

	aim_tx_enqueue(sess, fr);

	return 0;
}

static void auth_shutdown(OscarSession *sess, aim_module_t *mod)
{
	if (sess->authinfo) {
		free(sess->authinfo->sn);
		free(sess->authinfo->bosip);
		free(sess->authinfo->errorurl);
		free(sess->authinfo->email);
		free(sess->authinfo->chpassurl);
		free(sess->authinfo->latestrelease.name);
		free(sess->authinfo->latestrelease.url);
		free(sess->authinfo->latestrelease.info);
		free(sess->authinfo->latestbeta.name);
		free(sess->authinfo->latestbeta.url);
		free(sess->authinfo->latestbeta.info);
		free(sess->authinfo);
	}
}

static int snachandler(OscarSession *sess, aim_module_t *mod, FlapFrame *rx, aim_modsnac_t *snac, ByteStream *bs)
{

	if (snac->subtype == 0x0003)
		return parse(sess, mod, rx, snac, bs);
	else if (snac->subtype == 0x0007)
		return keyparse(sess, mod, rx, snac, bs);
	else if (snac->subtype == 0x000a)
		return got_securid_request(sess, mod, rx, snac, bs);

	return 0;
}

faim_internal int auth_modfirst(OscarSession *sess, aim_module_t *mod)
{

	mod->family = 0x0017;
	mod->version = 0x0000;
	mod->flags = 0;
	strncpy(mod->name, "auth", sizeof(mod->name));
	mod->snachandler = snachandler;
	mod->shutdown = auth_shutdown;

	return 0;
}