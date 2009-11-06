#!/bin/bash

rm -rf mplayerplug-in-3.55
mkdir mplayerplug-in-3.55
mkdir mplayerplug-in-3.55/Source
cp -pr Source/*.cpp Source/*.h Source/*.idl Source/*.xpt mplayerplug-in-3.55/Source
cp -pr *.in mplayerplug-in-3.55
cp *.spec mplayerplug-in-3.55
cp *.sh mplayerplug-in-3.55
cp -pr common mplayerplug-in-3.55
cp -pr extras mplayerplug-in-3.55
cp -pr include mplayerplug-in-3.55
cp -pr pixmaps mplayerplug-in-3.55
cp -pr plugingate mplayerplug-in-3.55
cp -pr po mplayerplug-in-3.55
cp ChangeLog configure INSTALL mplayerplug-in.conf mplayerplug-in.types README TODO mplayerplug-in-3.55
echo "GZIP=--best gzip -c >mplayerplug-in-3.55.tar.gz"
tar -czvf mplayerplug-in-3.55.tar.gz mplayerplug-in-3.55
