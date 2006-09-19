project(gaim)

# needed include directories to build gaim
# saves the variable in internal cache for later use
set(GAIM_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/src/protocols/oscar
  ${GLIB2_INCLUDE_DIRS}
  CACHE INTERNAL "gaim include directories"
)

# gaim lib and dependencies
# saves the variable in internal cache for later use
set(GAIM_LIBRARIES
  gaim
  resolv
  ${GLIB2_GMODULE_LIBRARY}
  ${GLIB2_GLIB_LIBRARY}
  ${GLIB2_GTHREAD_LIBRARY}
  CACHE INTERNAL "gaim libraries"
)

set(GAIM_DEFINITIONS
  -DHAVE_CONFIG_H
  -DHAVE_GLIB
  -DGAIM_STATIC_PRPL
  -DHAVE_SSL
)

include_directories(
  ${GAIM_INCLUDE_DIRS}
)

add_definitions(
  ${GAIM_DEFINITIONS}
)

set(gaim_SRCS
    src/src/account.c
    src/src/accountopt.c
    src/src/blist.c
    src/src/buddyicon.c
    src/src/cipher.c
    src/src/cmds.c
    src/src/connection.c
    src/src/conversation.c
    src/src/core.c
    src/src/debug.c
    src/src/desktopitem.c
    src/src/dnssrv.c
    src/src/eventloop.c
    src/src/ft.c
    src/src/gaim_buffer.c
    src/src/getopt.c
    src/src/getopt1.c
    src/src/idle.c
    src/src/imgstore.c
    src/src/log.c
    src/src/media.c
    src/src/mime.c
    src/src/network.c
    src/src/notify.c
    src/src/ntlm.c
    src/src/plugin.c
    src/src/pluginpref.c
    src/src/pounce.c
    src/src/prefs.c
    src/src/privacy.c
    src/src/proxy.c
    src/src/prpl.c
    src/src/request.c
    src/src/roomlist.c
    src/src/savedstatuses.c
    src/src/server.c
    src/src/signals.c
    src/src/sound.c
    src/src/sslconn.c
    src/src/status.c
    src/src/stringref.c
    src/src/stun.c
    src/src/upnp.c
    src/src/util.c
    src/src/value.c
    src/src/whiteboard.c
    src/src/xmlnode.c
    src/plugins/ssl/ssl.c
  # src/plugins/tcl/tcl.c
  # src/plugins/tcl/tcl_cmds.c
  # src/plugins/tcl/tcl_signals.c
  # src/plugins/tcl/tcl_glib.c
  # src/src/protocols/irc/cmds.c
  # src/src/protocols/irc/dcc_send.c
  # src/src/protocols/irc/irc.c
  # src/src/protocols/irc/msgs.c
  # src/src/protocols/irc/parse.c
    src/src/protocols/jabber/auth.c
    src/src/protocols/jabber/buddy.c
    src/src/protocols/jabber/chat.c
    src/src/protocols/jabber/disco.c
    src/src/protocols/jabber/iq.c
    src/src/protocols/jabber/jabber.c
    src/src/protocols/jabber/jutil.c
    src/src/protocols/jabber/message.c
    src/src/protocols/jabber/oob.c
    src/src/protocols/jabber/parser.c
    src/src/protocols/jabber/presence.c
    src/src/protocols/jabber/roster.c
    src/src/protocols/jabber/si.c
    src/src/protocols/jabber/xdata.c
    src/src/protocols/msn/cmdproc.c
    src/src/protocols/msn/command.c
    src/src/protocols/msn/dialog.c
    src/src/protocols/msn/directconn.c
    src/src/protocols/msn/error.c
    src/src/protocols/msn/group.c
    src/src/protocols/msn/history.c
    src/src/protocols/msn/httpconn.c
    src/src/protocols/msn/msg.c
    src/src/protocols/msn/msn.c
    src/src/protocols/msn/nexus.c
    src/src/protocols/msn/notification.c
    src/src/protocols/msn/object.c
    src/src/protocols/msn/page.c
    src/src/protocols/msn/servconn.c
    src/src/protocols/msn/session.c
    src/src/protocols/msn/slp.c
    src/src/protocols/msn/slpcall.c
    src/src/protocols/msn/slplink.c
    src/src/protocols/msn/slpmsg.c
    src/src/protocols/msn/slpsession.c
    src/src/protocols/msn/state.c
    src/src/protocols/msn/switchboard.c
    src/src/protocols/msn/sync.c
    src/src/protocols/msn/table.c
    src/src/protocols/msn/transaction.c
    src/src/protocols/msn/user.c
    src/src/protocols/msn/userlist.c
    src/src/protocols/msn/utils.c
  # src/src/protocols/napster/napster.c
  # src/src/protocols/novell/nmconference.c
  # src/src/protocols/novell/nmconn.c
  # src/src/protocols/novell/nmcontact.c
  # src/src/protocols/novell/nmevent.c
  # src/src/protocols/novell/nmfield.c
  # src/src/protocols/novell/nmmessage.c
  # src/src/protocols/novell/nmrequest.c
  # src/src/protocols/novell/nmrtf.c
  # src/src/protocols/novell/nmuser.c
  # src/src/protocols/novell/nmuserrecord.c
  # src/src/protocols/novell/novell.c
    src/src/protocols/oscar/bstream.c
    src/src/protocols/oscar/conn.c
    src/src/protocols/oscar/family_admin.c
    src/src/protocols/oscar/family_advert.c
    src/src/protocols/oscar/family_alert.c
    src/src/protocols/oscar/family_auth.c
    src/src/protocols/oscar/family_bart.c
    src/src/protocols/oscar/family_bos.c
    src/src/protocols/oscar/family_buddy.c
    src/src/protocols/oscar/family_chat.c
    src/src/protocols/oscar/family_chatnav.c
    src/src/protocols/oscar/family_feedbag.c
    src/src/protocols/oscar/family_icbm.c
    src/src/protocols/oscar/family_icq.c
    src/src/protocols/oscar/family_invite.c
    src/src/protocols/oscar/family_locate.c
    src/src/protocols/oscar/family_odir.c
    src/src/protocols/oscar/family_oservice.c
    src/src/protocols/oscar/family_popup.c
    src/src/protocols/oscar/family_stats.c
    src/src/protocols/oscar/family_translate.c
    src/src/protocols/oscar/family_userlookup.c
    src/src/protocols/oscar/misc.c
    src/src/protocols/oscar/msgcookie.c
    src/src/protocols/oscar/oscar.c
    src/src/protocols/oscar/peer.c
    src/src/protocols/oscar/rxhandlers.c
    src/src/protocols/oscar/rxqueue.c
    src/src/protocols/oscar/session.c
    src/src/protocols/oscar/snac.c
    src/src/protocols/oscar/tlv.c
    src/src/protocols/oscar/txqueue.c
    src/src/protocols/oscar/util.c
    src/src/protocols/yahoo/util.c
    src/src/protocols/yahoo/yahoo.c
    src/src/protocols/yahoo/yahoo_auth.c
    src/src/protocols/yahoo/yahoo_crypt.c
    src/src/protocols/yahoo/yahoo_doodle.c
    src/src/protocols/yahoo/yahoo_filexfer.c
    src/src/protocols/yahoo/yahoo_friend.c
    src/src/protocols/yahoo/yahoo_packet.c
    src/src/protocols/yahoo/yahoo_picture.c
    src/src/protocols/yahoo/yahoo_profile.c
    src/src/protocols/yahoo/yahoochat.c
    src/src/protocols/yahoo/ycht.c
    src/src/prefix.c
    src/plugins/ssl/ssl-gnutls.c
)

add_library(gaim STATIC ${gaim_SRCS})

target_link_libraries(${GAIM_LIBRARIES})

