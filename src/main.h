/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
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

#ifndef _MAIN_H_
#define _MAIN_H_

#include "mainwindow.h"

QString findPlugin(const QString& filename);
void loadImages();
void initAppPaths(int argc, char **argv);

class CmdLineArgs {
public:
	CmdLineArgs();
	bool parse(const QStringList& argv);
	bool preparse(int argc,char** argv);
	static void printHelp();
	QString error() {return Error;}
	QString file() {return File;}
	QString configLocation() {return ConfigLocation;}
	QString language() {return Language;}
	bool startMinimized() {return StartMinimized;}
	bool startLocked() {return StartLocked;}
	bool help() {return Help;}
	void setFile(const QString& filename) {File = filename;};
private:
	QString Error;
	QString File;
	QString ConfigLocation;
	QString Language;
	bool StartMinimized;
	bool StartLocked;
	bool Help;
};

class EventListener : public QObject {
	Q_OBJECT
	
#ifdef Q_WS_MAC
	public:
		EventListener() { pMainWindow = NULL; };
		inline QString file() { return pFile; };
		inline void setMainWin(KeepassMainWindow* mainWin) {
			pMainWindow = mainWin;
		};
	
	private:
		QString pFile;
		KeepassMainWindow* pMainWindow;
#endif
	
	protected:
		bool eventFilter(QObject*, QEvent* event);
};

#endif
