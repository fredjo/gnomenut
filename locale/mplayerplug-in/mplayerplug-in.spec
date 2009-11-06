%define name mplayerplug-in
%define ver 3.55
%ifarch x86_64
%define mozilla_dir /usr/lib64/mozilla/
%else
%define mozilla_dir /usr/lib/mozilla/
%endif
%define prefix /usr
%define config_dir /etc
%{!?os_ver:%define os_ver fc%(cut -d' ' -f4 /etc/fedora-release )}

%define rel %{os_ver}
%{!?req:%define req %(if [ `cut -d' ' -f4 /etc/fedora-release | cut -d'.' -f1` -ge 5 ]; then echo "libXt-devel xorg-x11-proto-devel libXpm-devel"; else echo "xorg-x11-devel"; fi)}
%{!?mozreq:%define mozreq %(if [ ( `cut -d' ' -f4 /etc/fedora-release | cut -d'.' -f1` -le 5 ) -a ( `cut -d' ' -f4 /etc/fedora-release` -le 2)]; then echo "mozilla-devel > 1.5"; else echo "firefox-devel"; fi)}


Name: %{name}
Version: %{ver}
Release: 1.f%{fedora}
License: GPL
Group: Browser/Plugin
Summary: MPlayer Plugin for Netscape/Mozilla
URL: http://mplayerplug-in.sourceforge.net/
Packager: Kevin DeKorte <kdekorte@users.sourceforge.net>
Source0: %{name}-%{ver}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{ver}-root

BuildRequires:libXt-devel xorg-x11-proto-devel libXpm-devel firefox-devel gtk2-devel > 2.2.1 gettext
Obsoletes: mplayerplug-in < %{ver}
Provides: mplayerplug-in


%description
mplayerplug-in is a Mozilla browser plugin to allow playing embedded movies on web pages.

%prep
rm -rf %buildroot && %{__mkdir} -p %buildroot%{mozilla_dir} && %{__mkdir} -p %buildroot%{config_dir} && %{__mkdir} -p %buildroot%{mozilla_dir}plugins && %{__mkdir} -p %buildroot%{mozilla_dir}components
%setup -n %{name}-%{ver}

%build
%configure
%{__make} %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} install \
        DESTDIR="%{buildroot}"
%find_lang %{name}

%clean
rm -rf %buildroot

%files
%defattr (-,root,root)
%doc ChangeLog INSTALL README TODO
%{mozilla_dir}/plugins/mplayerplug-in.so
%{mozilla_dir}/plugins/mplayerplug-in-wmp.so
%{mozilla_dir}/plugins/mplayerplug-in-qt.so
%{mozilla_dir}/plugins/mplayerplug-in-rm.so
%{mozilla_dir}/plugins/mplayerplug-in-dvx.so
%{mozilla_dir}/plugins/mplayerplug-in.xpt
%{mozilla_dir}/plugins/mplayerplug-in-wmp.xpt
%{mozilla_dir}/plugins/mplayerplug-in-qt.xpt
%{mozilla_dir}/plugins/mplayerplug-in-rm.xpt
%{mozilla_dir}/plugins/mplayerplug-in-dvx.xpt
%config(noreplace) %{config_dir}/mplayerplug-in.conf
%config(noreplace) %{config_dir}/mplayerplug-in.types
%{_prefix}/share/locale/*/LC_MESSAGES/mplayerplug-in.mo

%changelog
* Mon Dec 18 2006 Kevin DeKorte
- v3.35

* Sun Aug 27 2006 Kevin DeKorte
- v3.31

* Sun Apr 9 2006 Kevin DeKorte
- v3.25

* Sun Feb 12 2006 Kevin DeKorte
- v3.21

* Mon Jan 30 2006 Kevin DeKorte
- v3.20
	See ChangeLog
* Mon Dec 08 2005 Kevin DeKorte
- v3.17
	See ChangeLog
* Mon Nov 28 2005 Kevin DeKorte
- v3.16
	See ChangeLog
* Fri Nov 04 2005 Kevin DeKorte
- v3.15
	See ChangeLog
* Tue Sep 13 2005 Kevin DeKorte
- v3.11
	Fix playlist problem
* Mon Sep 12 2005 Kevin DeKorte
- v3.10
	See ChangeLog
* Fri Aug 05 2005 Kevin DeKorte
- v3.05
	See ChangeLog
* Thu Jul 21 2005 Kevin DeKorte
- v3.01
	Fixed crash when "src" tag used and src is not found
	Allow SMIL support to be configurable for Real Media
	Added --enable-[wmp,qt,rm,gmp] flags to configure
- v3.00
	Warning message when using "--enable-x" option
	State setting in shutdown (remove unnecessary instance->)
- v2.99.1
	Install.sh fix
	make clean fix
	Crasher fix
	make filename embed tag attribute work properly
	Makefile fixups done by Greg Hosler
- v2.99.0
	Work with locking issues in play, ff and rew also rework order in play a bit.
	Korean translation by Joonil Kim
	Change an assertion into some if checks
	Handle case where plugin is shutdown, but thread is setup, but has not been signalled and so it may hang
	Redo ::Play function
	Add a missing lock to ::Play
	Add some more GTK_IS_WIDGET checks to the destroy code.
	Fix download percentage when dealing with large files.
	Fix issue with nomediacache=1 and autostart=1 and the buttons not displaying
	Removed unnecessary unlock from ::Play in the SetupPlayer section
	Plugin separation, made separate plugin for each video type
	Fixed a dumb bug in the ASF and ASX parseing code
	Cleaned up (un)installer code
	Fixed problem with spanish (es) missing on install
	More cleanups due to lib separation
	Added lock around deleteList in shutdown

* Tue Jun 14 2005 Kevin DeKorte
- v2.85
	French translation by AL 
	updated mimetype description MS ASF video for type video/x-ms-asf
	Build Fixes from Dag Wieers
	Minor change to getURLFilename
	Change to make NetFlix previews work
	Cap the cachebytes to twice the cachesize
	Fix up some issues with user-agent
	Italian translation by Roberto Inzerillo
	Brazilian Portugese translation by Danilo Bardusco
	Slovak translation by Jozef Riha
	Took & compare out of URLcmp so that http://www.dr.dk/netradio/?venstre=true should work.
	More fixes to make www.dr.dk to work better.
	Optimize the loop condition with 1 file to play
	Fix a bug in URLcmp with file:// urls
	Fix a bug in "loop" attribute make "loop=1" work.
	Merged and expanded Hiroa's loop patch.
	Russian translation by Nikolai Prokoschenko
	Danish translation by Anders Lind
	Progress Bar over video fix and crasher fix.
	Spanish translation by Azael Avalos
	getTime and getPercent patches from anonymous
	Added options cookies, nomouseinput and noconsolecontrols to config file to disable (set to 0) or enable (set to 1 [default])

* Sun Feb 13 2005 Kevin DeKorte
- v2.80
	Fix DPMS value on quit
	Add workaround to disable -xy, -x, -y mplayer flags when not on local display
		patch from Yasuhiro Matsumoto
	Add gettext language support (ja only provided, other languages still need to be done)
		patch from Yasuhiro Matsumoto	
	Patch to set UserAgent from Yasuhiro Matsumoto
	Polish translation by Marcin Bukat
	German translation by Rene Engelhard
	Dutch translation by Panagiotis Issaris
	Implement java script method SetFileName
	autostart/autoplay config option. Set to 0 to disable autostart of media.
	New Logo by Dusan Maliarik (and Gnome Crystal Icon Set)
	Fix to URLcmp to make http://ms.radio-canada.ca/CBFT/Telejournal200501182200_4.wmv work. 
	Implemented the following Javascript functions (QuickTime compatible)
		void SetIsLooping(in boolean loop);
		boolean GetIsLooping();
		void SetAutoPlay(in boolean autoPlay);
		boolean GetAutoPlay();
		void SetHREF(in string url);
		string GetHREF();
		void SetURL(in string url);
		string GetURL();
		string GetMIMEType();
	Fix up buttons and GUI layout in small windows
	Make STOP be down when autostart = 0
	Fix popup menu to work in letterbox bar areas
	Fix popup menu to work before media is started
	Fix menu callbacks to work if panel is not drawn
	Changed behavior of Stop/MediaComplete event to show mplayerplug-in logo when done.
	Autoconf changes to GTK1/2 Enabled flags.
	Fix crash with noembed=1 option set
	Fix compile issue with GTK1 code

* Wed Dec 15 2004 Kevin DeKorte
- v2.75
	Make sdp media urls work
	Make GTK1 code request XEmbed mode
	Only specify mplayer -cache option for streaming data
	Add mimetype application/x-ms-wmv to the list
	For realmedia, don't play when controls=statusbar
	Applied mms playlist detection patch from Yasuhiro Matsumoto (fixes TSN.ca among others)
	Fix for older mozilla include paths. Fix based on info from FreeBSD package patch.
	Applied sync A/V options patch from Yasuhiro Matsumoto
	Applied some Sun JDS patches from Hiroaki Nozaki - did not apply all since they used global variables
	Added in support for Nullsoft Streaming Video mimetype: application/x-nsv-vp3-mp3
	Fixed problem with http://www.vg.no/video media
	Added fallback to smb:// when getting a file:// url that is not found
	Make Netflix previews work after Netflix website update
	Apply NetBSD patches from Juan RP
	Add back in the signal mask patch and a slight pause
	Add some more pthread_cancel checks in
	Fixed a buffer overrun that was identified by Adam Feakin
	Fixes some minor nits with the fullscreen window in GTK2
	Applied more patches from Hiroaki Nozaki - more HTML property support
	Added config option enable_smil, default to 1 or enabled. Added so that SMIL support can be disabled independently
	Fix automake issues on FreeBSD
	Fix issue with http://www.camerata-trajectina.nl/, autoplay and buttons showing up
	Add support for keyboard commands, these only work when the mouse is over the buttons or background, not over media
		space/P/p - play or pause depending on current state
		S/s - stop
		</, - rewind 10 seconds
		>/. - fast forward 10 seconds
		F/f - fullscreen or not

* Sun Sep 26 2004 Kevin DeKorte
- v2.70
	GTK1 Widget fixes
	MMS URL saving
	GTK2 improvements
	Xinerama support
	
* Sat Jun 19 2004 Kevin DeKorte
- v2.66
	Fix Gentoo NPTL locking issue / crash

* Mon Jun 14 2004 Kevin DeKorte
- v2.65
	Fix some minor crash situations
	UI fixes in all modes
	Support gecko-sdk 1.7 and higher
	
* Thu Mar 18 2004 Kevin DeKorte
- v2.50
        See ChangeLog for complete listing
	Thread locking fixes
	More URL support
	GUI fixes
	
* Sun Feb 15 2004 Kevin DeKorte
- v2.11
	Added some missing defines for older compilers
	Fixed getBaseURL crasher on SUN
	Removed -cookies option in call to mplayer because it is not present on some versions of mplayer
  v2.10
	Fix crash and double play with American Idol site
	Configure fixes, added --with-gecko-sdk option if pkg-config does not work
	Fixed bug where first file in playlist was not being cleaned up
	Fixed casting error in memmem.c
	Playback reset if frame decode error
	New option cache-percent defaulted to 25 values are [0-100] used with
		cachesize, takes value of stream size and takes larger (huge Shrek plays on DSL now)
	Fix errors on building on a solaris 9 with gcc tools environment
	Fix compile issue on slackware
	
* Fri Feb 06 2004 Kevin DeKorte
- v2.0
	Massive code reorganization to get javascript commands to work
	Conversion of C code to C++ code
	Code retains all fixes of 1.3 candidate
	Code retains almost all functionality of 1.3 candidate
	functions/options that have been removed include:
		use of gmplayer, no longer an option, built-in gui planned
		logging to a file - logging to console easier
		download option - now automatic
		other options have now been made default
	mozilla-devel package now required to compile code.
	gcc -Wall is used now, so all those errors/warnings should be fixed
	strncpy/strncat replaced with strlcpy/strlcat, files included in archive
	Fixed browser hang when player crashes or is killed

	***Code Split - 1.3 candidate***
	Fix possible buffer overflow in buildPlaylist
	Correct mplayer shutdown process
	More UI enhancements from Erich
	Even more UI enhancements
	Fix XML parse error on smil files that don't have seq tags. Which DTD does SMIL use?
	Fix videotag not found omission
	Fix UI crasher FreeUI was called with uninitialized pointers
	Parse mplayer output to determine the actual size and play size of the media
	Support RealMedia with embedded rtsp streams, allows NASA TV to work
		needs enable-real=1 in mplayerplug-in.conf
		mplayer with RealMedia codecs and support
	Fix possible crash in NPP_Destroy
	Change buildPlaylist to be called from NPP_DestroyStream rather than after a magic value in NPP_Write
		This way we are sure we have the entire playlist downloaded.
	Found possible crash in smil format decode
	Added stat.c to make Solaris compile work
	Fixed bug in getURLBase
	Fixed handling of This->baseurl
	Fixed crash in ASX format handling, should fix http://www.rbcmp3.com
	Account for CR and LF instead of just LF in RM playlists
	Solaris compile issue - change stat.c references to lstat.c
	Fixed compile issue in lstat.c
	Applied Alexandre Pereira da Silva's player manager work. Should help those with "ps" issues.

* Wed Jan 20 2004 Kevin DeKorte
- v1.2
	QuickTime streaming, speed work
	Fix crasher on url http://www.video-c.co.uk/frontend/asp/microshow.asp?vidref=benn001&FileType=ADSLprog
		This was caused by an incorrect assumption about Quicktime MOV Reference files
		rmda elements can contain rmdr and rdrf in any order, files I had only showed one order.
	When keep-download=1 write mms url's into $dload-dir/playlist for reference
	All unwanted streams are now cancelled and not downloaded with QT autospeed.
	Changed qt-speed config option from a 0-12 number to [low|medium|high], this should be final now
	Fixed crasher in smil format support when bitrate not specified.
	Fixed bug with MSN Music site where http urls are self referencing and act like MMS Streams
	Applied patch from Erich that reduces the amount of flicker when updating the status screen
	Changed Full Redraw threshhold from 50 to 80, some sites were a little crammed.
	Merged patch from Erich to display progress bar during download
	Fixed bug where video was streamed rather then downloaded, when it should be downloaded
	Proper smil format support, with nested video tags with seq tags
	Found an extra ; in the speed adjustment code that was causing med and high speeds to be off

* Tue Jan 15 2004 Kevin DeKorte
- v1.1
- Security Fixes
- Minor bug fixes with saving streams
- Build system fixes
- Title Screen enhancement
- Updated status window
- Playlist support for http streams that act like mms streams
- See ChangeLog for full list of fixes

* Tue Dec 12 2003 Kevin DeKorte
- v1.0
- Added -zoom patch from Giuseppe Ghibò <ghibo@mandrakesoft.com>
- Patch to only kill the mplayer associated with the window being destroyed.
- This allows multiple streams to play in different windows
- Cosmetic "about:plugins" patch from Benjamin Larsson <banan@student.luth.se>
- More about:plugin work to fix an issue with AtomFilms, Quicktime was not coming up
- Keep trailing ; in mimetype list
- smil format support - Clone wars

* Mon Oct 20 2003 Kevin DeKorte
- v1.0pre3
- More fixes

* Thu Oct 16 2003 Kevin DeKorte
- v1.0pre2
- compatibility with mplayer 1.0pre2

* Fri Oct 03 2003 Kevin DeKorte
- v0.95
- True streaming support
- Improved playlist capability
- Improved status window
- Mplayer 0.92 is a prerequisite

* Wed Sep 10 2003 Kevin DeKorte
- Fix errors in 0.90
- Downloading feature disabled for now.

* Sun Sep 07 2003 Kevin DeKorte
- See ChangeLog for code changes

* Mon Mar 03 2003 Michael A. Peters <mpeters@mac.com>
- created spec file
