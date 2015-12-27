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

#include "main.h"
#include "mainwindow.h"
#if defined(Q_WS_X11) && defined(GLOBAL_AUTOTYPE)
	#include "Application_X11.h"
#endif
#ifdef Q_WS_WIN
	#include <Windows.h>
#endif

#include "plugins/interfaces/IFileDialog.h"
#include "plugins/interfaces/IKdeInit.h"
#include "plugins/interfaces/IGnomeInit.h"

//#include <QPluginLoader>
#include <iostream>

using namespace std;

KpxConfig *config;
QString  AppDir;
QString HomeDir;
QString DataDir;
QString PluginLoadError;
QString DetailViewTemplate;
bool EventOccurred;
bool EventOccurredBlock = false;

QPixmap* EntryIcons;
IIconTheme* IconLoader=NULL;


int main(int argc, char **argv)
{
#ifdef Q_WS_WIN
	// Make sure Windows doesn't load DLLs from the current working directory
	SetDllDirectoryA("");
	SetSearchPathMode(BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE);
#endif

	setlocale(LC_CTYPE, "");
	
#if defined(Q_WS_X11) && defined(AUTOTYPE)
	QApplication* app = new KeepassApplication(argc,argv);
#else
	QApplication* app = new QApplication(argc,argv);
#endif
	EventListener* eventListener = new EventListener();
	app->installEventFilter(eventListener);
	
	QApplication::setQuitOnLastWindowClosed(false);
	
	AppDir = QApplication::applicationFilePath();
	AppDir.truncate(AppDir.lastIndexOf("/"));
#if defined(Q_WS_X11)
	DataDir = AppDir+"/../share/keepassx";
	if (!QFile::exists(DataDir) && QFile::exists(AppDir+"/share"))
		DataDir = AppDir+"/share";
	const char* env = getenv("XDG_CONFIG_HOME");
	if (!env) {
		HomeDir = QDir::homePath() + "/.config";
	}
	else {
		QString qenv = QTextCodec::codecForLocale()->toUnicode(env);
		if (qenv[0] == '/')
			HomeDir = qenv;
		else
			HomeDir = QDir::homePath() + '/' + qenv;
	}
	HomeDir += "/keepassx";
#elif defined(Q_WS_MAC)
	HomeDir = QDir::homePath()+"/.keepassx";
	DataDir = AppDir+"/../Resources/keepassx";
#else //Q_WS_WIN
	HomeDir = qtWindowsConfigPath(CSIDL_APPDATA);
	if(!HomeDir.isEmpty() && QFile::exists(HomeDir))
		HomeDir = QDir::fromNativeSeparators(HomeDir)+"/KeePassX";
	else
		HomeDir = QDir::homePath()+"/KeePassX";
	
	DataDir = AppDir+"/share";
#endif
	DataDir = QDir::cleanPath(DataDir);
	
	CmdLineArgs args;
	if ( !args.parse(QApplication::arguments()) ){
		qCritical("%s\n", CSTR( args.error() ));
		args.printHelp();
		return 1;
	}
	if (args.help()){
		args.printHelp();
		return 1;
	}

	//Load Config
	QString IniFilename;
	if(args.configLocation().isEmpty()){
		if(!QDir(HomeDir).exists()){
			QDir conf(QDir::homePath());
			if(!QDir().mkpath(HomeDir))
				qWarning("Warning: Could not create directory '%s'", CSTR(HomeDir));
		}
		IniFilename=HomeDir+"/config.ini";
	}
	else
		IniFilename=args.configLocation();

#ifdef Q_WS_X11
	{
		QString OldHomeDir = QDir::homePath()+"/.keepassx";
		if (args.configLocation().isEmpty() && QFile::exists(OldHomeDir+"/config") && !QFile::exists(HomeDir+"/config")) {
			QFile::rename(OldHomeDir+"/config", HomeDir+"/config.ini");
			if (QDir(OldHomeDir).entryList(QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden|QDir::System).count()==0)
				QDir().rmdir(OldHomeDir);
		}
	}
#else
	if (args.configLocation().isEmpty() && QFile::exists(HomeDir+"/config") && !QFile::exists(HomeDir+"/config.ini"))
		QFile::rename(HomeDir+"/config", HomeDir+"/config.ini");
#endif
	config = new KpxConfig(IniFilename);
	fileDlgHistory.load();
	
	// PlugIns
	/*
#ifdef Q_WS_X11
	if(config->integrPlugin()!=KpxConfig::NoIntegr){
		QString LibName="libkeepassx-";
		if(config->integrPlugin()==KpxConfig::KDE)
			LibName+="kde.so";
		else if(config->integrPlugin()==KpxConfig::Gnome)
			LibName+="gnome.so";
		QString filename=findPlugin(LibName);
		if(filename!=QString()){
			QPluginLoader plugin(filename);
			if(!plugin.load()){
				PluginLoadError=plugin.errorString();
				qWarning("Could not load desktop integration plugin:");
				qWarning("%s", CSTR(PluginLoadError));
			}
			else{
				QObject *plugininstance=plugin.instance();
				IFileDialog* fdlg=qobject_cast<IFileDialog*>(plugininstance);
				IconLoader=qobject_cast<IIconTheme*>(plugininstance);
				if(IconLoader==NULL){
					qWarning("Error: Integration Plugin: Could not initialize IconTheme interface.");
				}
				KpxFileDialogs::setPlugin(fdlg);
				if(config->integrPlugin()==KpxConfig::KDE){
					IKdeInit* kdeinit=qobject_cast<IKdeInit*>(plugin.instance());
					app=kdeinit->getMainAppObject(argc,argv);
					if(!app) PluginLoadError = "Initialization failed.";
				}
				if(config->integrPlugin()==KpxConfig::Gnome){
					IGnomeInit* ginit=qobject_cast<IGnomeInit*>(plugin.instance());
					if(!ginit->init(argc,argv)){
						KpxFileDialogs::setPlugin(NULL);
						qWarning("GtkIntegrPlugin: Gtk init failed.");
						PluginLoadError = "Initialization failed.";
					}
				}
			}
		}
		else{
			qWarning(CSTR(QString("Could not load desktop integration plugin: File '%1' not found.").arg(LibName)));
			PluginLoadError=QApplication::translate("Main", "Could not locate library file.");
		}
	}
#endif
	*/
	
	DetailViewTemplate=config->detailViewTemplate();

	loadImages();
	KpxBookmarks::load();
	initYarrow(); //init random number generator
	SecString::generateSessionKey();
	
	installTranslator();

#ifdef Q_WS_MAC
	QApplication::processEvents();
	if (args.file().isEmpty() && !eventListener->file().isEmpty()) {
		args.setFile(eventListener->file());
	}
#endif

	KeepassMainWindow *mainWin = new KeepassMainWindow(args.file(), args.startMinimized(), args.startLocked());
#ifdef Q_WS_MAC
	eventListener->setMainWin(mainWin);
#endif

	int r=app->exec();
	delete mainWin;
	delete eventListener;

	fileDlgHistory.save();
	SecString::deleteSessionKey();
	delete app;
	delete config;
	return r;
}


///TODO 0.2.3 remove
void loadImages(){
	QPixmap tmpImg(getImageFile("clientic.png"));
	EntryIcons=new QPixmap[BUILTIN_ICONS];
	for(int i=0;i<BUILTIN_ICONS;i++){
		EntryIcons[i]=tmpImg.copy(i*16,0,16,16);
	}
}





CmdLineArgs::CmdLineArgs(){
	StartMinimized=false;
	StartLocked=false;
	Help=false;
}

bool CmdLineArgs::parse(const QStringList& argv){
	for(int i=1;i<argv.size();i++){
		if(argv[i]=="-help" || argv[i]=="--help" || argv[i]=="-h"){
			Help=true;
			break; // break, because other arguments will be ignored anyway
		}
		if(argv[i]=="-cfg"){
			if(argv.size() == i+1){
				Error="Missing argument for '-cfg'.";
				return false;
			}
			if(argv[i+1].left(1)=="-"){
				Error=QString("Expected a path as argument for '-cfg' but got '%1.'").arg(argv[i+1]);
				return false;
			}
			QFileInfo file(argv[i+1]);
			ConfigLocation=file.absoluteFilePath();
			i++;
			continue;
		}
		if(argv[i]=="-min"){
			StartMinimized=true;
			continue;
		}
		if(argv[i]=="-lock"){
			StartLocked=true;
			continue;
		}
		if(i==1 && argv[i].left(1)!="-"){
			File=argv[1];
			continue;
		}
		Error=QString("** Unrecognized argument: '%1'").arg(argv[i]);
		return false;
	}
	return true;
}

void CmdLineArgs::printHelp(){
	cerr << "KeePassX " << APP_VERSION << endl;
	cerr << "Usage: keepassx [filename] [options]" << endl;
	cerr << "  -help             This Help" << endl;
	cerr << "  -cfg <CONFIG>     Use specified file for loading/saving the configuration." << endl;
	cerr << "  -min              Start minimized." << endl;
	cerr << "  -lock             Start locked." << endl;
}


/*QString findPlugin(const QString& filename){
	QFileInfo info;	
	info.setFile(AppDir+"/../lib/"+filename);
	if(info.exists() && info.isFile())
		return AppDir+"/../lib/"+filename;	
	return QString();
}*/


bool EventListener::eventFilter(QObject*, QEvent* event){
	if (!EventOccurred){
		int t = event->type();
		if ( (t>=QEvent::MouseButtonPress && t<=QEvent::KeyRelease) || (t>=QEvent::HoverEnter && t<=QEvent::HoverMove) )
			EventOccurred = true;
	}
	
#ifdef Q_WS_MAC
	if (event->type() == QEvent::FileOpen) {
		QString filename = static_cast<QFileOpenEvent*>(event)->file();
		if (pMainWindow) {
			if (QApplication::activeModalWidget() == NULL)
				pMainWindow->openFile(filename);
			else
				return true; // ignore file open events while a modal dialog is displayed
		}
		else {
			pFile = filename;
		}
	}
#endif
	
	return false;
}
