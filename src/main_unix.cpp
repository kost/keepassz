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


#include "main.h"

void initAppPaths(int argc,char** argv) {
	// Try looking for a /proc/<pid>/exe symlink first which points to
	// the absolute path of the executable
	QFileInfo pfi(QString::fromLatin1("/proc/%1/exe").arg(getpid()));
	if (pfi.exists() && pfi.isSymLink()) {
		AppDir = pfi.canonicalFilePath();
	}
	else {	
		QString argv0 = QFile::decodeName(QByteArray(argv[0]));
		QString absPath;
		
		if (!argv0.isEmpty() && argv0.at(0) == QLatin1Char('/')) {
				/*
				If argv0 starts with a slash, it is already an absolute
				file path.
				*/
			absPath = argv0;
		} else if (argv0.contains(QLatin1Char('/'))) {
				/*
				If argv0 contains one or more slashes, it is a file path
				relative to the current directory.
				*/
			absPath = QDir::current().absoluteFilePath(argv0);
		} else {
				/*
				Otherwise, the file path has to be determined using the
				PATH environment variable.
				*/
			QByteArray pEnv = qgetenv("PATH");
			QDir currentDir = QDir::current();
			QStringList paths = QString::fromLocal8Bit(pEnv.constData()).split(QLatin1String(":"));
			for (QStringList::const_iterator p = paths.constBegin(); p != paths.constEnd(); ++p) {
				if ((*p).isEmpty())
					continue;
				QString candidate = currentDir.absoluteFilePath(*p + QLatin1Char('/') + argv0);
				QFileInfo candidate_fi(candidate);
				if (candidate_fi.exists() && !candidate_fi.isDir()) {
					absPath = candidate;
					break;
				}
			}
		}		
		absPath = QDir::cleanPath(absPath);		
		QFileInfo fi(absPath);
		AppDir = fi.exists() ? fi.canonicalFilePath() : QString();
	}
	AppDir.truncate(AppDir.lastIndexOf("/"));
	DataDir=AppDir+"/../share/keepassx";
	if (!QFile::exists(DataDir) && QFile::exists(AppDir+"/share"))
		DataDir=AppDir+"/share";
	HomeDir = QDir::homePath()+"/.keepassx";
}
