#!/bin/bash

SHAREDTARGET=mplayerplug-in
INSTALL='/usr/bin/install -c'
prefix=/usr/local
exec_prefix=${prefix}
sysconfdir=${prefix}/etc
libdir=${exec_prefix}/lib
datadir=${prefix}/share
LOCALEDIR=${datadir}/locale
DESTDIR=$1
MOZHOME=/plugins

if [ "$DESTDIR" ]; then 
	echo "Installing to $DESTDIR"
	$INSTALL -b -D -m0644 mplayerplug-in.conf ${DESTDIR}/etc/mplayerplug-in.conf 
	$INSTALL -b -D -m0644 mplayerplug-in.types ${DESTDIR}/etc/mplayerplug-in.types 
	for mod in     mplayerplug-in mplayerplug-in-wmp mplayerplug-in-qt mplayerplug-in-rm mplayerplug-in-dvx
	do
		$INSTALL -D -m0755 ${mod}.so ${DESTDIR}${libdir}/mozilla/plugins/${mod}.so 
		$INSTALL -D -m0755 Source/nsIScriptableMplayerPlugin.xpt ${DESTDIR}${libdir}/mozilla/plugins/${mod}.xpt 
	done
	make install -C po LOCALEDIR="${LOCALEDIR}" INSTALL="$INSTALL" DESTDIR="$DESTDIR"

else 

	if [ `whoami` != 'root' ]; then
		
		echo "Installing for user `whoami`"
		for mod in     mplayerplug-in mplayerplug-in-wmp mplayerplug-in-qt mplayerplug-in-rm mplayerplug-in-dvx
		do
			$INSTALL -D -m0755 ${mod}.so ${HOME}/.mozilla/plugins/${mod}.so 
			$INSTALL -D -m0755 Source/nsIScriptableMplayerPlugin.xpt ${HOME}/.mozilla/plugins/${mod}.xpt 
		done
		$INSTALL -b -D -m0644 mplayerplug-in.conf ${HOME}/.mozilla/mplayerplug-in.conf 
		$INSTALL -b -D -m0644 mplayerplug-in.types ${HOME}/.mozilla/mplayerplug-in.types 

	else
		echo "Installing plugin system wide"
		# system install code
		if [ -z ${MOZHOME} ]; then
		    MOZ=`which mozilla`
		    MOZHOME=`grep MOZILLA_FIVE_HOME= ${MOZ} | awk  -F \" '{ print $2 }'`
		    if [ -z ${MOZHOME} ]; then MOZHOME=`grep moz_libdir= ${MOZ} | awk  -F \" '{ print $2 }'`; fi
		    if [ -z ${MOZHOME} ]; then MOZHOME=`grep moz_libdir= ${MOZ} | awk  -F = '{ print $2 }'`; fi
		    MOZPLUGINS=`grep MOZ_PLUGIN_PATH ${MOZ} | awk -F = '{ print $2 }' | awk -F : '{ if ( $1 != "") if ( index($1,"_PLUGIN_") == 0) print $1 }'`
		    if [ ${MOZHOME} ]; then DEST=${MOZHOME}/plugins; fi
		    if [ ${MOZPLUGINS} ]; then DEST=${MOZPLUGINS}; fi
		else
		    DEST=${MOZHOME}
		fi
		if [ -z ${DEST} ]; then echo "Can't find MOZILLA HOME. Please install manually"; exit 1; fi
		echo "Installing into ${DEST}"
		if [ ! -d ${DEST} ]; then mkdir ${DEST}; fi
		for mod in     mplayerplug-in mplayerplug-in-wmp mplayerplug-in-qt mplayerplug-in-rm mplayerplug-in-dvx
		do
			$INSTALL -D -m0755 ${mod}.so ${DEST}/${mod}.so 
			$INSTALL -D -m0755 Source/nsIScriptableMplayerPlugin.xpt ${DEST}/${mod}.xpt 
		done
		$INSTALL -b -D -m0644 mplayerplug-in.conf /etc/mplayerplug-in.conf 
		$INSTALL -b -D -m0644 mplayerplug-in.types /etc/mplayerplug-in.types 
		make install -C po 
	fi
fi

