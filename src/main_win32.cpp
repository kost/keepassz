/***************************************************************************
 *   Copyright (C) 1992-2007 Trolltech ASA								   *
 *                                                                         *
 *   Copyright (C) 2005-2007 by Tarek Saidi                                *
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

#include <windows.h>
#include "main.h"

void initAppPaths(int argc,char** argv){
	QFileInfo filePath;
	QT_WA({
		wchar_t module_name[256];
		GetModuleFileNameW(0, module_name, sizeof(module_name) / sizeof(wchar_t));
		filePath = QString::fromUtf16((ushort *)module_name);
	}, {
		char module_name[256];
		GetModuleFileNameA(0, module_name, sizeof(module_name));
		filePath = QString::fromLocal8Bit(module_name);
	});	
	AppDir = filePath.filePath();
	AppDir.truncate(AppDir.lastIndexOf("/"));
	
	HomeDir = QString::fromLocal8Bit(qgetenv("APPDATA").constData());
	if(!HomeDir.isEmpty() && QFile::exists(HomeDir))
		HomeDir = QDir::fromNativeSeparators(HomeDir)+"/KeePassX";
	else
		HomeDir = QDir::homePath()+"/KeePassX";
	
	DataDir=AppDir+"/share";
}
