#!/bin/sh
# you need to have create-dmg available at
# https://github.com/andreyvit/create-dmg.git
test -f KeePassZ-Installer.dmg && rm KeePassZ-Installer.dmg
create-dmg \
--volname "KeePassZ Installer" \
--volicon "bin/keepassz.app/Contents/Resources/icon.icns" \
--window-pos 200 120 \
--window-size 800 400 \
--icon-size 100 \
--icon Application.app 200 190 \
--hide-extension Application.app \
--app-drop-link 600 185 \
KeePassZ-Installer.dmg \
bin/

