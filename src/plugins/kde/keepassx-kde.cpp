/***************************************************************************
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

#include <kapplication.h>
#include <kfiledialog.h>
#include <kcmdlineargs.h>
#include <kiconloader.h>
#include <kfilefiltercombo.h>
#include <QPixmap>
#include <QHash>
#include <QStringList>
#include <kurl.h>
#include <QByteArray>
#include "keepassx-kde.h"

#define CSTR(x)(x.toUtf8().data())

QHash<QString,QString>IconMap;
int LastFilter;

Q_EXPORT_PLUGIN2(keepassx_kde, KdePlugin)

void createIconMap(){
	IconMap["alarmclock"]="alarmclock";
	IconMap["appsettings"]="configure";
	IconMap["autotype"]="input-keyboard";
	IconMap["clock"]="chronometer";
	IconMap["clonenetry"]="edit-copy";
	IconMap["copypwd"]="kgpg-export-kgpg";
	IconMap["copyusername"]="user";
	IconMap["dbsearch"]="edit-find";
	IconMap["dbsettings"]="configure";
	IconMap["delete"]="edit-delete";
	IconMap["delete-entry"]="edit-delete";
	IconMap["delete-group"]="edit-delete";
	IconMap["editentry"]="edit";
	IconMap["editgroup"]="edit";
	IconMap["exit"]="application-exit";
	IconMap["expired"]="flag-red";
	IconMap["fileclose"]="dialog-close";
	IconMap["filedelete"]="edit-delete";
	IconMap["filenew"]="document-new";
	IconMap["fileopen"]="document-open";
	IconMap["filesave"]="document-save";
	IconMap["filesaveas"]="document-save-as";
	IconMap["filesaveasdisabled"]="document-save-as"; ///FIXME needs to be grayed to reflect it's status
	IconMap["generator"]="roll";
	IconMap["groupsearch"]="file-find";
	IconMap["help"]="help-contents";
	IconMap["key"]="password";
	IconMap["manual"]="help-contents";
	IconMap["newentry"]="kgpg-key3-kpgp";
	IconMap["newgroup"]="folder";
	IconMap["ok"]="ok";
	IconMap["openurl"]="network";
	IconMap["search"]="edit-find";
}

QString convertFilters(const QStringList& qtfilters){
	/*
	Qt Filter Syntax:
	StringList:       "DescrA (*.ext1 *.ext2 *.ext3)",
	                  "DescrB (*.ext4)"
	
    KDE Filter Syntax:
	Single String:   "*.ext1 *.ext2 *.ext3|DescrA\n*ext4|DescrB"
	*/
	
	QString kdefilters;
	for(int i=0;i<qtfilters.size();i++){
		int a=qtfilters[i].indexOf('(');
		int b=qtfilters[i].indexOf(')');
		QString exts=qtfilters[i].mid(a+1,b-a-1);
		kdefilters+=exts;
		kdefilters+='|';
		kdefilters+=qtfilters[i];
		if(i<qtfilters.size()-1)kdefilters+='\n';
	}
	return kdefilters;
}


int getFilterIndex(const QStringList& qtfilters,const QString& KdeExtString){
	int i;
	bool found=false;
	for(i=0;i<qtfilters.size();i++){
		int a=qtfilters[i].indexOf('(');
		int b=qtfilters[i].indexOf(')');
		QString QtExtString=qtfilters[i].mid(a+1,b-a-1); // "*.ext1 *.ext2"
		if(KdeExtString==QtExtString){
			found=true;
			break;
		}
	}
	if(found)
		return i;
	else
		return 0;
}


QString KdePlugin::openExistingFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,int SelectedFilter){	
	KFileDialog FileDlg(KUrl(dir),convertFilters(Filters),parent);
	//FileDlg.filterWidget()->setCurrentFilter(convertFilters(QStringList()<<Filters[SelectedFilter]));
	FileDlg.setMode(KFile::ExistingOnly | KFile::File);
	FileDlg.setOperationMode(KFileDialog::Opening);
	if(!FileDlg.exec())return QString();
	LastFilter=getFilterIndex(Filters,FileDlg.currentFilter());
	return FileDlg.selectedFiles()[0];
}

QStringList KdePlugin::openExistingFilesDialog(QWidget* parent,QString title,QString dir,QStringList Filters,int SelectedFilter){
	KFileDialog FileDlg(KUrl(dir),convertFilters(Filters),parent);
	//FileDlg.filterWidget()->setCurrentFilter(convertFilters(QStringList()<<Filters[SelectedFilter]));
	FileDlg.setMode(KFile::ExistingOnly | KFile::Files);
	FileDlg.setOperationMode(KFileDialog::Opening);
	if(!FileDlg.exec())return QStringList();
	LastFilter=getFilterIndex(Filters,FileDlg.currentFilter());
	return FileDlg.selectedFiles();
}

QString KdePlugin::saveFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,int SelectedFilter,bool OverWriteWarn){
	KFileDialog FileDlg(KUrl(dir),convertFilters(Filters),parent);
	//FileDlg.filterWidget()->setCurrentFilter(convertFilters(QStringList()<<Filters[SelectedFilter]));
	FileDlg.setMode(KFile::File);
	FileDlg.setOperationMode(KFileDialog::Saving);
	if(!FileDlg.exec())return QString();	
	LastFilter=getFilterIndex(Filters,FileDlg.currentFilter());
	return FileDlg.selectedFiles()[0];

}

QApplication* KdePlugin::getMainAppObject(int argc, char** argv){
	KCmdLineArgs::init(argc,argv,QByteArray("keepassx"),
					   			 QByteArray("none"),
								 ki18n("KeePassX"),
		 						 QByteArray("0.2.3"),
								 ki18n("Cross Platform Password Manager"));
	createIconMap();
#ifdef GLOBAL_AUTOTYPE
	return dynamic_cast<QApplication*>( new KpKApplication() );
#else
	return dynamic_cast<QApplication*>( new KApplication() );
#endif
}


QIcon KdePlugin::getIcon(const QString& name){
	KIconLoader loader;
	QPixmap pxm=loader.loadIcon(IconMap.value(name),KIconLoader::Desktop,0,KIconLoader::DefaultState,QStringList(),NULL,true);
	if(pxm.isNull())return QIcon();
	QIcon icon(pxm);
	return icon;
}


