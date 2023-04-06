#!/usr/bin/perl -wp
# This script can be used to verify that headers guards matches the following template:
# #ifndef OW[A-Z_0-9]+_H
# #define OW[A-Z_0-9]+_H
# /* Some source code */
# endif /* OW[A-Z_0-9]+_H */
#
# It currently check *and* modify headers
# There is no command line option right now.
# 
# Use in conjunction with find is strongly encouraged:
# find trunk_ng/wengophone/ trunk_ng/libs/imwrapper/
# trunk_ng/libs/idle/ trunk_ng/libs/owbrowser/
# trunk_ng/libs/pixertool/ trunk_ng/libs/qtutil/
# trunk_ng/libs/samplerate/ trunk_ng/libs/sipwrapper/
# trunk_ng/libs/skinstyle/ trunk_ng/libs/softupdater/
# trunk_ng/libs/sound/ trunk_ng/libs/timer/ trunk_ng/libs/tinyxml/
# trunk_ng/libs/trayicon/ trunk_ng/libs/util/ trunk_ng/libs/webcam/
# trunk_ng/libs/wenbox/ trunk_ng/wifo/httptunnel/
# trunk_ng/wifo/miniua/ trunk_ng/wifo/netlib/ -iname \*.h | xargs
# ~/verify_headers.pl 2> /tmp/log

use strict;

if ($_ =~ /^(\#define|\#ifndef).*?([A-Z_0-9]+)_H$/) {
    print STDERR  "$_ is a header guard.\n";
    if ($2 =~ /^OW[^_].*/) {
	print STDERR "$_ already has an OW prefix, nothing to do...";
    } elsif ($2 =~ /^OW_.*/) {
	print STDERR "$_ already has an OW_ prefix, replacing it with OW...";
	s/^((\#define|\#ifndef).*?)OW_([A-Z_0-9]+)(_H)$/$2 OW$3$4/;
    } else {
	print STDERR "$_ does not have an OW prefix, fixing it...\n";
	s/^((\#define|\#ifndef).*?)([A-Z_0-9]+)(_H)$/$2 OW$3$4/;
    }
}
if ($_ =~ /^\#endif\s+\/\/\s*([A-Z_0-9]+)_H$/) {
    print STDERR "Found closing directive for header guard...\n";
    if ($1 =~ /^OW[^_].*/) {
	print STDERR "$_ already has an OW prefix, nothing to do...";
    } elsif ($1 =~ /^OW_.*/) {
	print STDERR "$_ already has an OW_ prefix, replacing it with OW...";
	s/^\#endif\s+\/\/\s*OW_([A-Z_0-9]+)_H$/\#endif \/\* OW$1_H \*\//;
    } else {
	print STDERR "Closing directive does not have OW prefix, fixing it...\n";
	s/^\#endif\s+\/\/\s*([A-Z_0-9]+)_H$/\#endif \/\* OW$1_H \*\//;
    }
}
		
if ($_ =~ /^\#endif\s+\/\*\s*([A-Z_0-9]+)_H\s*\*\/$/) {
    print STDERR "Found closing directive for header guard...\n";
    if ($1 =~ /^OW[^_].*/) {
	print STDERR "$_ already has an OW prefix, nothing to do...";
    } elsif ($1 =~ /^OW_.*/) {
	print STDERR "$_ already has an OW_ prefix, replacing it with OW...";
	s/^\#endif\s+\/\*\s*OW_([A-Z_0-9]+)_H\s*\*\//\#endif \/\* OW$1_H \*\//;
    } else {
	print STDERR "Closing directive does not have OW prefix, fixing it...\n";
	s/^\#endif\s+\/\*\s*([A-Z_0-9]+)_H\s*\*\//\#endif \/\* OW$1_H \*\//;
    }
}
