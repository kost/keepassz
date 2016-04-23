#!/bin/sh

LRELEASE=""

if [ `which lrelease-qt4` ] ; then
	LRELEASE="`which lrelease-qt4`"
elif [ `which lrelease` ] ; then
	LRELEASE="`which lrelease`"
fi

if [ "$LRELEASE" = "" ] ; then
	echo "Error: lrelease / lrelease-qt4 not found."
	exit 1
fi

cd `dirname $0`
rm share/keepassz/i18n/*.qm
cd src
$LRELEASE translations/*.ts
mv translations/*.qm ../share/keepassz/i18n
rm ../share/keepassz/i18n/keepassz-xx_XX.qm
rm ../share/keepassz/i18n/keepassz-cs_CZ.qm
