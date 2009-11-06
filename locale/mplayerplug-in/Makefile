#!make
################################################################################
# Copyright (c) 1996 Netscape Communications. All rights reserved.
################################################################################
#
# Template plugin makefile
# This makefile contains some of our defines for the compiler:
#
# XP_UNIX	This needs to get defined for npapi.h on unix platforms.
# PLUGIN_TRACE	Enable this define to get debug prints whenever the plugin
#		api gets control.

CC= gcc
CXX= g++
CFLAGS= -Wall -DXP_UNIX -DMOZ_X11 -fshort-wchar -I/usr/include/xulrunner-1.9.1.3/stable -I/usr/include/nspr   -I/usr/include/xulrunner-1.9.1.3 -g -O2      -g -O2  -Iinclude -fPIC     -DX_ENABLED 
LDFLAGS=    -lX11    -lX11  -lXt -lX11 -lSM -lICE   -lXext -lX11 -lXpm -lX11     
LIBOBJS= 
OBJ= plugin.o nsScriptablePeer.o npp_gate.o np_entry.o npn_gate.o \
	plugin-support.o plugin-setup.o plugin-list.o plugin-ui.o \
	plugin-threads.o strlcpy.o strlcat.o $(LIBOBJS)
OBJ_WMP= plugin.o nsScriptablePeer.o npp_gate.o np_entry.o npn_gate.o \
	plugin-support.o plugin-setup-wmp.o plugin-list.o plugin-ui.o \
	plugin-threads.o strlcpy.o strlcat.o $(LIBOBJS)
OBJ_QT= plugin.o nsScriptablePeer.o npp_gate.o np_entry.o npn_gate.o \
	plugin-support.o plugin-setup-qt.o plugin-list.o plugin-ui.o \
	plugin-threads.o strlcpy.o strlcat.o $(LIBOBJS)
OBJ_RM= plugin.o nsScriptablePeer.o npp_gate.o np_entry.o npn_gate.o \
	plugin-support.o plugin-setup-rm.o plugin-list.o plugin-ui.o \
	plugin-threads.o strlcpy.o strlcat.o $(LIBOBJS)
OBJ_GMP= plugin.o nsScriptablePeer.o npp_gate.o np_entry.o npn_gate.o \
	plugin-support.o plugin-setup-gmp.o plugin-list.o plugin-ui.o \
	plugin-threads.o strlcpy.o strlcat.o $(LIBOBJS)
OBJ_DVX= plugin.o nsScriptablePeer.o npp_gate.o np_entry.o npn_gate.o \
	plugin-support.o plugin-setup-dvx.o plugin-list.o plugin-ui.o \
	plugin-threads.o strlcpy.o strlcat.o $(LIBOBJS)
INSTALL = /usr/bin/install -c
SHAREDTARGET=mplayerplug-in
NP_HEADERS= include/npplat.h include/pluginbase.h
SUBDIR= languages
INSTALL_SUBDIR= install-languages
prefix=/usr/local
exec_prefix=${prefix}
sysconfdir=${prefix}/etc
libdir=${exec_prefix}/lib
datadir=${prefix}/share
LOCALEDIR=$(datadir)/locale
MOZILLA_HOME=
GECKO_XPIDL=/usr/bin/xpidl
GECKO_IDLDIR=/usr/share/idl/xulrunner-1.9.1.3

PIXMAPS=pixmaps/logo.xpm pixmaps/progress_left.xpm pixmaps/progress_right.xpm \
	pixmaps/progress_fill.xpm pixmaps/progress_middle.xpm

HEADERS=config.h $(NP_HEADERS)

SCRIPTABLEHEADERS=Source/nsScriptablePeer.h Source/plugin.h config.h \
	Source/plugin-setup.h Source/nsIScriptableMplayerPlugin.h $(HEADERS)

default all: config.h     mplayerplug-in mplayerplug-in-wmp mplayerplug-in-qt mplayerplug-in-rm mplayerplug-in-dvx languages

$(SHAREDTARGET): $(OBJ) Source/nsIScriptableMplayerPlugin.xpt
	$(CXX) -shared -o $(SHAREDTARGET).so $(OBJ) $(CFLAGS) $(LDFLAGS)
	cp Source/nsIScriptableMplayerPlugin.xpt mplayerplug-in.xpt

$(SHAREDTARGET)-wmp: $(OBJ_WMP) Source/nsIScriptableMplayerPlugin.xpt
	$(CXX) -shared -o $(SHAREDTARGET)-wmp.so $(OBJ_WMP) $(CFLAGS) $(LDFLAGS)
	cp Source/nsIScriptableMplayerPlugin.xpt mplayerplug-in-wmp.xpt

$(SHAREDTARGET)-qt: $(OBJ_QT) Source/nsIScriptableMplayerPlugin.xpt
	$(CXX) -shared -o $(SHAREDTARGET)-qt.so $(OBJ_QT) $(CFLAGS) $(LDFLAGS)
	cp Source/nsIScriptableMplayerPlugin.xpt mplayerplug-in-qt.xpt

$(SHAREDTARGET)-rm: $(OBJ_RM) Source/nsIScriptableMplayerPlugin.xpt
	$(CXX) -shared -o $(SHAREDTARGET)-rm.so $(OBJ_RM) $(CFLAGS) $(LDFLAGS)
	cp Source/nsIScriptableMplayerPlugin.xpt mplayerplug-in-rm.xpt

$(SHAREDTARGET)-gmp: $(OBJ_GMP) Source/nsIScriptableMplayerPlugin.xpt
	$(CXX) -shared -o $(SHAREDTARGET)-gmp.so $(OBJ_GMP) $(CFLAGS) $(LDFLAGS)
	cp Source/nsIScriptableMplayerPlugin.xpt mplayerplug-in-gmp.xpt

$(SHAREDTARGET)-dvx: $(OBJ_DVX) Source/nsIScriptableMplayerPlugin.xpt
	$(CXX) -shared -o $(SHAREDTARGET)-dvx.so $(OBJ_DVX) $(CFLAGS) $(LDFLAGS)
	cp Source/nsIScriptableMplayerPlugin.xpt mplayerplug-in-dvx.xpt

np_entry.o: plugingate/np_entry.cpp
	$(CXX) -c -o $@ $(CFLAGS) plugingate/np_entry.cpp

npp_gate.o: plugingate/npp_gate.cpp
	$(CXX) -c -o $@ $(CFLAGS) plugingate/npp_gate.cpp

npn_gate.o: plugingate/npn_gate.cpp
	$(CXX) -c -o $@ $(CFLAGS) plugingate/npn_gate.cpp

Source/nsIScriptableMplayerPlugin.h: Source/nsIScriptableMplayerPlugin.idl
	$(GECKO_XPIDL) -w -m header -I $(GECKO_IDLDIR) -I Source -e $@ $<

Source/nsIScriptableMplayerPlugin.xpt: Source/nsIScriptableMplayerPlugin.idl
	$(GECKO_XPIDL) -w -m typelib -I $(GECKO_IDLDIR) -I Source -e $@ $<

plugin.o: Source/plugin.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) $(LIBS) Source/plugin.cpp

plugin-setup.o: Source/plugin-setup.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) $(LIBS) -DSTD Source/plugin-setup.cpp

plugin-setup-wmp.o: Source/plugin-setup.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) $(LIBS) -DWMP Source/plugin-setup.cpp

plugin-setup-qt.o: Source/plugin-setup.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) $(LIBS) -DQT Source/plugin-setup.cpp

plugin-setup-rm.o: Source/plugin-setup.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) $(LIBS) -DRM Source/plugin-setup.cpp

plugin-setup-gmp.o: Source/plugin-setup.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) $(LIBS) -DGMP Source/plugin-setup.cpp

plugin-setup-dvx.o: Source/plugin-setup.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) $(LIBS) -DDVX Source/plugin-setup.cpp

plugin-support.o: Source/plugin-support.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) $(LIBS) Source/plugin-support.cpp

plugin-list.o: Source/plugin-list.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) $(LIBS) Source/plugin-list.cpp

plugin-ui.o: Source/plugin-ui.cpp $(SCRIPTABLEHEADERS) $(PIXMAPS)
	$(CXX) -c -o $@ $(CFLAGS) Source/plugin-ui.cpp

plugin-threads.o: Source/plugin-threads.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) Source/plugin-threads.cpp

nsScriptablePeer.o: Source/nsScriptablePeer.cpp $(SCRIPTABLEHEADERS)
	$(CXX) -c -o $@ $(CFLAGS) Source/nsScriptablePeer.cpp

memmem.o: extras/memmem.c $(HEADERS)
	$(CC) -c -o $@ $(CFLAGS) extras/memmem.c

strlcat.o: extras/strlcat.c $(HEADERS)
	$(CC) -c -o $@ $(CFLAGS) extras/strlcat.c

strlcpy.o: extras/strlcpy.c $(HEADERS)
	$(CC) -c -o $@ $(CFLAGS) extras/strlcpy.c

clean:
	$(RM) $(OBJ) $(OBJ_WMP) $(OBJ_QT) $(OBJ_RM) $(OBJ_GMP) $(OBJ_DVX) $(SHAREDTARGET)*.so $(SHAREDTARGET)*.xpt Source/nsIScriptableMplayerPlugin.h Source/nsIScriptableMplayerPlugin.xpt
	cd po; $(MAKE) clean

distclean: clean
	$(RM) po/Makefile
	$(RM) config.log config.status config.h Makefile

install: all 
	./install.sh $(DESTDIR)

languages:
	cd po; $(MAKE)

uninstall:
	./uninstall.sh

dist:	config.h
	./dist.sh

config.h: mplayerplug-in.spec.in configure.in config.h.in Makefile.in install.sh.in uninstall.sh.in dist.sh.in 
	./configure --prefix=/usr/local
	touch config.h
