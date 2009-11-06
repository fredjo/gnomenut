#!/bin/bash
gtk1=no
gtk2=no
debian=no
fedora=no
geckosdk=yes
package=none
prefix=none

pkg-config --exists mozilla-plugin
if [ $? -eq 0 ]; then package=mozilla; geckosdk=no; fi
pkg-config --exists firefox-plugin
if [ $? -eq 0 ]; then package=firefox; geckosdk=no; fi
pkg-config --exists seamonkey-plugin
if [ $? -eq 0 ]; then package=seamonkey; geckosdk=no; fi
pkg-config --exists xulrunner-plugin
if [ $? -eq 0 ]; then package=xulrunner; geckosdk=no; fi
pkg-config --exists iceape-plugin
if [ $? -eq 0 ]; then package=iceape; geckosdk=no; fi

if [ ${package} != "none" ]; then
	# ok we have development packages, this is good
	#echo ${package}
	libdir=`pkg-config --variable=libdir ${package}-plugin`
	#echo ${libdir}
	prefix=`pkg-config --variable=prefix ${package}-plugin`


	if [ `ldd ${libdir}/${package}-bin | grep gtk-x11 | wc -l` != 0 ]; then gtk2=yes; fi
	if [ `ldd ${libdir}/${package}-bin | grep gtk-1 | wc -l` != 0 ]; then gtk1=yes; fi


else
	pkg-config --exists gtk+-2.0;
	if [ $? -eq 0 ]; then gtk2=yes; fi ;
	pkg-config --exists gtk+;
	if [ $? -eq 0 ]; then gtk1=yes; fi ;


	
fi

# Detecting GNU/Linux distribution
if [ -f /etc/debian_version ]; then
	debian=yes;
	if [ -d /usr/lib/iceweasel ]; then 
		MOZ_HOME=/usr/lib/iceweasel;
	elif [ -d /usr/lib/firefox ]; then 
		MOZ_HOME=/usr/lib/firefox;
	elif [ -d /usr/lib/mozilla ]; then
		MOZ_HOME=/usr/lib/mozilla;
	elif [ -d /usr/lib/iceape ]; then
		MOZ_HOME=/usr/lib/iceape;
	fi
fi 

#echo "GTK2 = ${gtk2}"
#echo "GTK1 = ${gtk1}"
#echo "geckosdk = ${geckosdk}"

echo "Run configure with the following options"
echo ""
echo -n "./configure"
if [ $prefix != none ]; then echo -n " --prefix=${prefix}"; fi
if [ $gtk1 == yes ]; then echo -n " --enable-gtk1"; fi
if [ $gtk2 == no -a $gtk1 == no ]; then echo -n " --enable-x"; fi
if [ $debian == yes ]; then 
	echo -n " --with-mozilla-home=${MOZ_HOME}"; 
else
	echo;
	echo;
	echo "Optional: --with-mozilla-home=${libdir}";
fi
if [ $geckosdk == yes ]; then echo " --with-gecko-sdk={path to gecko sdk}"; echo ; echo "You need to install the gecko-sdk"; fi
echo 
