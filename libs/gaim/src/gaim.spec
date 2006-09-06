# Older RPM doesn't define these by default
%{!?perl_vendorlib: %define perl_vendorlib %(eval "`%{__perl} -V:installvendorlib`"; echo $installvendorlib)}
%{!?perl_vendorarch: %define perl_vendorarch %(eval "`%{__perl} -V:installvendorarch`"; echo $installvendorarch)}
%{!?perl_archlib: %define perl_archlib %(eval "`%{__perl} -V:installarchlib`"; echo $installarchlib)}

# When not doing betas comment this out
# NOTE: %defines in spec files are evaluated in comments so the correct
#       way to comment it out is to replace the % with #
%define beta 3.1

%if "0%{?beta}" != "0"
%define gaimver %(echo "2.0.0beta3.1"|sed -e 's/cvs//; s/beta.*//')
%else
%define gaimver 2.0.0beta3.1
%endif

Summary:    A Gtk+ based multiprotocol instant messaging client
Name:       gaim
Version:    %gaimver
Release:    0%{?beta:.beta%{beta}}
Epoch:      1
License:    GPL
Group:      Applications/Internet
URL:        http://gaim.sourceforge.net/
Packager:   %{packager}
Source:     %{name}-2.0.0beta3.1.tar.gz
BuildRoot:  %{_tmppath}/%{name}-%{version}-root

# Generic build requirements
BuildRequires: libtool, pkgconfig, libao-devel, audiofile-devel
%{?_with_howl:BuildRequires: howl-devel}
%{?_with_silc:BuildRequires: /usr/include/silc/silcclient.h}
%{?_with_meanwhile:BuildRequires: meanwhile-devel}
%{?_with_tcl:BuildRequires: tcl, tk, /usr/include/tcl.h}
%{?_with_mono:BuildRequires: mono-devel}
%{?_with_dbus:BuildRequires: dbus-devel >= 0.35}
%{!?_without_gtkspell:BuildRequires: gtkspell-devel}
%{?_with_sasl:BuildRequires: cyrus-sasl-devel >= 2}
# For some reason perl isn't always automatically detected as a requirement :(
Requires: perl

# Mandrake 10.1 and lower || Mandrake 10.2 (and higher?)
%if "%{_vendor}" == "MandrakeSoft" || "%{_vendor}" == "Mandrakesoft" || "%{_vendor}" == "Mandriva"
# Mandrake/Mandriva requirements
BuildRequires: libgtk+2.0_0-devel, libnss3-devel, perl-devel
Obsoletes:  libgaim-remote0
%else

# SuSE & Red Hat / Fedora requirements
BuildRequires: gtk2-devel

%if "%{_vendor}" == "suse"
BuildRequires: gnutls-devel
%else
BuildRequires: mozilla-nss-devel
%endif
%endif

%if 0%{?_with_howl:1}
%package bonjour
Summary:    Bonjour plugin for Gaim
Group:      Applications/Internet
Requires:   gaim = %{epoch}:%{version}
%endif

%if 0%{?_with_silc:1}
%package silc
Summary:    SILC (Secure Internet Live Conferencing) plugin for Gaim
Group:      Applications/Internet
Requires:   gaim = %{epoch}:%{version}
%endif

%if 0%{?_with_meanwhile:1}
%package meanwhile
Summary:    Lotus Sametime plugin for Gaim using the Meanwhile library
Group:      Applications/Internet
Requires:   gaim = %{epoch}:%{version}
%endif

%if 0%{?_with_tcl:1}
%package tcl
Summary:    Tcl scripting support for Gaim
Group:      Applications/Internet
Requires:   gaim = %{epoch}:%{version}
%endif

%if 0%{?_with_mono:1}
%package mono
Summary:    Mono .NET plugin support for Gaim
Group:      Applications/Internet
Requires:   gaim = %{epoch}:%{version}
%endif

%package devel
Summary:    Development headers, documentation, and libraries for Gaim.
Group:      Applications/Internet
Requires:   pkgconfig, gaim = %{epoch}:%{version}

%description
Gaim allows you to talk to anyone using a variety of messaging
protocols, including AIM, ICQ, IRC, Yahoo!, Novell Groupwise, MSN
Messenger, Jabber, Gadu-Gadu, Napster, Lotus Sametime and Zephyr.
These protocols are implemented using a modular, easy to use design.
To use a protocol, just add an account using the account editor.

Gaim supports many common features of other clients, as well as many
unique features, such as perl scripting, TCL scripting and C plugins.

Gaim is NOT affiliated with or endorsed by America Online, Inc.,
Microsoft Corporation, Yahoo! Inc., or ICQ Inc.

%if 0%{?_with_howl:1}
%description bonjour
Bonjour plugin for Gaim
%endif

%if 0%{?_with_silc:1}
%description silc
SILC (Secure Internet Live Conferencing) plugin for Gaim
%endif

%if 0%{?_with_meanwhile:1}
%description meanwhile
Lotus Sametime plugin for Gaim using the Meanwhile library
%endif

%if 0%{?_with_tcl:1}
%description tcl
Tcl plugin loader for Gaim.  This package will allow you to write or
use Gaim plugins written in the Tcl programming language.
%endif

%if 0%{?_with_mono:1}
%description mono
Mono plugin loader for Gaim.  This package will allow you to write or
use Gaim plugins written in the .NET programming language.
%endif

%description devel
The gaim-devel package contains the header files, developer
documentation, and libraries required for development of gaim scripts
and plugins.

%prep
%setup -q -n %{name}-2.0.0beta3.1

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{_prefix} \
                                    --bindir=%{_bindir} \
                                    --datadir=%{_datadir} \
                                    --includedir=%{_includedir} \
                                    --libdir=%{_libdir} \
                                    --mandir=%{_mandir} \
                                    --sysconfdir=%{_sysconfdir} \
                                    --with-ao=%{_libdir} \
                                    --with-perl-lib=%{_prefix} \
                                    %{?_with_silc:--with-silc-includes=%{_includedir}/silc} \
                                    %{?_with_silc:--with-silc-libs=%{_libdir}/silc} \
                                    %{?_with_mono:--enable-mono} \
                                    %{!?_with_tcl:--disable-tcl} \
                                    %{!?_with_dbus:--disable-dbus} \
                                    %{?_without_gtkspell:--disable-gtkspell} \
                                    %{?_with_sasl:--enable-cyrus-sasl}

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make DESTDIR=$RPM_BUILD_ROOT install

rm -f $RPM_BUILD_ROOT%{_libdir}/gaim/*.la
rm -f $RPM_BUILD_ROOT%{_libdir}/*.la
rm -f $RPM_BUILD_ROOT%{perl_archlib}/perllocal.pod
find $RPM_BUILD_ROOT -type f -name .packlist -exec rm -f {} ';'
find $RPM_BUILD_ROOT -type f -name '*.bs' -empty -exec rm -f {} ';'

%if 0%{!?_with_howl:1}
rm -f $RPM_BUILD_ROOT%{_libdir}/gaim/libbonjour.so
%endif

%if 0%{!?_with_silc:1}
rm -f $RPM_BUILD_ROOT%{_libdir}/gaim/libsilcgaim.so
%endif

%if 0%{!?_with_meanwhile:1}
rm -f $RPM_BUILD_ROOT%{_libdir}/gaim/libsametime.so
%endif

%if 0%{!?_with_tcl:1}
rm -f $RPM_BUILD_ROOT%{_libdir}/gaim/tcl.so
%endif

%if 0%{!?_with_mono:1}
rm -f $RPM_BUILD_ROOT%{_libdir}/gaim/mono.so
rm -f $RPM_BUILD_ROOT%{_libdir}/gaim/*.dll
%endif


find $RPM_BUILD_ROOT%{_libdir}/gaim -type f -print | \
        sed "s@^$RPM_BUILD_ROOT@@g" | \
        grep -v libbonjour.so | \
        grep -v libsilcgaim.so | \
        grep -v libsametime.so | \
        grep -v tcl.so | \
        grep -v mono.so | \
        grep -v ".dll$" | \
        grep -v ".tcl$" > %{name}-%{version}-coreplugins

%clean
rm -rf %{buildroot}

%files -f %{name}-%{version}-coreplugins
%defattr(-, root, root)

%doc doc/the_penguin.txt doc/CREDITS NEWS COPYING AUTHORS COPYRIGHT
%doc README ChangeLog
%doc %{_mandir}/man1/*
%doc %{_mandir}/man3*/*

%dir %{_libdir}/gaim
%attr(755, root, root) %{perl_vendorarch}/Gaim*
%attr(755, root, root) %{perl_vendorarch}/auto/Gaim

%{_bindir}/*
%{_datadir}/locale/*/*/*
%{_datadir}/pixmaps/*
%dir %{_datadir}/sounds/gaim
%{_datadir}/sounds/gaim/*
%{_datadir}/applications/*

%if 0%{?_with_dbus:1}
%{_libdir}/libgaim-client.so.*
%{_datadir}/dbus-1/services/gaim.service
%endif

%if 0%{?_with_howl:1}
%files bonjour
%defattr(-, root, root)

%{_libdir}/gaim/libbonjour.*
%endif

%if 0%{?_with_silc:1}
%files silc
%defattr(-, root, root)

%{_libdir}/gaim/libsilcgaim.*
%endif

%if 0%{?_with_meanwhile:1}
%files meanwhile
%defattr(-, root, root)

%{_libdir}/gaim/libsametime.*
%endif

%if 0%{?_with_tcl:1}
%files tcl
%defattr(-, root, root)

%{_libdir}/gaim/tcl.so
%endif

%if 0%{?_with_mono:1}
%files mono
%defattr(-, root, root)

%{_libdir}/gaim/mono.so
%{_libdir}/gaim/*.dll
%endif

%files devel
%defattr(-, root, root)

%doc plugins/HOWTO
%doc HACKING PROGRAMMING_NOTES

%dir %{_includedir}/gaim
%{_includedir}/gaim/*.h
%{_libdir}/pkgconfig/gaim.pc
%{_datadir}/aclocal/gaim.m4
%if 0%{?_with_dbus:1}
%{_libdir}/libgaim-client.so
%endif

%changelog
* Sat Dec 17 2005 <stu@nosnilmot.com>
- Add support for beta versions so the subsequent releases are seen as newer
  by RPM
- Split of sametime support to gaim-meanwhile
- Use make DESTDIR=... instead of overloading prefix etc. when installing
- Default build to include cyrus-sasl support in Jabber
- Add --with dbus to build with DBUS support

* Sun Dec 04 2005 <siege@preoccupied.net>
- Added obsoletes gaim-meanwhile

* Sun Oct 30 2005 <stu@nosnilmot.com>
- Add separate gaim-bonjour package if built with --with-howl
- Add separate gaim-mono package if built with --with-mono
- Exclude some unwanted perl files

* Sat Aug 20 2005 <stu@nosnilmot.com>
- Include libgaimperl.so
- Include gaim.m4 in gaim-devel

* Thu Apr 28 2005 <stu@nosnilmot.com>
- Use perl_vendorlib & perl_archlib for better 64bit compat (Jeff Mahoney)
- Clean up Requires, most should be auto-detected
- Restore gtkspell-devel build requirement (and add --without gtkspell option)
- Fix Tcl build requirements to work across more distros
- Fix SILC build requirements to work across more distros

* Mon Oct 11 2004 John Jolly <john.jolly@gmail.com>
- Added if "%{_vendor}" == "suse" to handle GnuTLS libraries for SuSE

* Sat Oct  2 2004 Stu Tomlinson <stu@nosnilmot.com>
- If --with tcl or silc are not specified, make sure the plugins don't
  exist to prevent RPM complaining about unpackaged files

* Tue Jun 29 2004 Ethan Blanton <eblanton@cs.ohiou.edu>
- Change Tcl to use --with tcl, the same as SILC, and build a gaim-tcl
  package if specified.

* Thu Jun 24 2004 Mark Doliner <thekingant@users.sourceforge.net>
- Add --with silc rebuild option for compiling a separate gaim-silc
  RPM containing the silc protocol plugin (Stu Tomlinson).

* Wed Jun 23 2004 Ethan Blanton <eblanton@cs.ohiou.edu>
- Moved gaim headers and a pkgconfig configuration file into the
  gaim-devel RPM (Stu Tomlinson).

* Thu Jan 15 2004 Ethan Blanton <eblanton@cs.ohiou.edu>
- Removed the manual strip command, as it seems to be unwarranted if
  the necessary programs are properly installed.  (For me, this was
  elfutils.)

* Sun Jul 20 2003 Bjoern Voigt <bjoern@cs.tu-berlin.de>
- Added pkgconfig build dependency.
- if "%{_vendor}" != "MandrakeSoft" now also works with rpm 3.x.
- Added Gaim-specific directories to list of Gaim's files.

* Wed Jul 16 2003 Ethan Blanton <eblanton@cs.ohiou.edu>
- Complete spec file rewrite to take advantage of "new" RPM features
  and make things prettier.
- Use system-supplied %%{_prefix}, %%{_datadir}, etc. rather than
  attempt to define our own.
