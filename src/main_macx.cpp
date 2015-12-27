/***************************************************************************
 *   Copyright (C) 1992-2008 Trolltech ASA								   *
 *                                                                         *
 *   Copyright (C) 2005-2008 by Tarek Saidi                                *
 *   tarek.saidi@arcor.de                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QVarLengthArray>
#include <Carbon/Carbon.h>
#include "main.h"

void initAppPaths(int argc,char** argv) {
	CFURLRef bundleURL(CFBundleCopyExecutableURL(CFBundleGetMainBundle()));
	//assert(bundleURL);
	CFStringRef cfPath(CFURLCopyFileSystemPath(bundleURL, kCFURLPOSIXPathStyle));
	//assert(cfPath);
	CFIndex length = CFStringGetLength(cfPath);
	const UniChar *chars = CFStringGetCharactersPtr(cfPath);
	if (chars) {
		AppDir = QString(reinterpret_cast<const QChar *>(chars), length);
	}
	else {	
		QVarLengthArray<UniChar> buffer(length);
		CFStringGetCharacters(cfPath, CFRangeMake(0, length), buffer.data());
		AppDir = QString(reinterpret_cast<const QChar *>(buffer.constData()), length);
	}
	AppDir.truncate(AppDir.lastIndexOf("/"));
	HomeDir = QDir::homePath()+"/.keepassx";
	DataDir=AppDir+"/../Resources/keepassx";	
}
