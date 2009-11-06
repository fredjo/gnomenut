#!/bin/bash
MOZHOME=

echo "Cleaning up user plugins"
if [ -f ${HOME}/.mozilla/plugins/mplayerplug-in.so ]; then rm ${HOME}/.mozilla/plugins/mplayerplug-in*; fi
if [ `whoami` == 'root' ]; then
	echo "Cleaning up system plugins"
	if [ -z ${MOZHOME} ]; then
	    MOZ=`which mozilla`
	    MOZHOME=`grep MOZILLA_FIVE_HOME= ${MOZ} | awk  -F \" '{ print $2 }'`
	    if [ -z ${MOZHOME} ]; then MOZHOME=`grep moz_libdir= ${MOZ} | awk  -F \" '{ print $2 }'`; fi
	fi
	if [ -z ${MOZHOME} ]; then echo "Can't find MOZILLA HOME. Please uninstall manually"; exit 1; fi
	if [ -f ${MOZHOME}/plugins/mplayerplug-in.so ]; then rm ${MOZHOME}/plugins/mplayerplug-in*; fi
	if [ -f ${MOZHOME}/components/mplayerplug-in.xpt ]; then rm ${MOZHOME}/components/mplayerplug-in*; fi
	if [ "x${MOZ}" != "x" ]; then
	    MOZPLUGINS=`grep MOZ_PLUGIN_PATH ${MOZ} | awk -F = '{ print $2 }' | awk -F : '{ if ( $1 != "") if ( index($1,"_PLUGIN_") == 0) print $1 }'`
	    if [ -f ${MOZPLUGINS}/mplayerplug-in.so ]; then rm ${MOZPLUGINS}/mplayerplug-in*; fi
	fi
	make uninstall -C po
fi
