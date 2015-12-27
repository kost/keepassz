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
 

#include "Export_Txt.h"


QString EntryTemplate=QString("\n\
  Title:    %1\n\
  Username: %2\n\
  Url:      %3\n\
  Password: %4\n\
  Comment:  %5\n\
");

QString GroupTemplate=QString("\n\
*** Group: %1 ***\n\
");

bool Export_Txt::exportDatabase(QWidget* GuiParent, IDatabase* db){
	QFile *file=openFile(GuiParent,identifier(),QStringList()<<tr("All Files (*)") << tr("Text Files (*.txt)"));
	if(!file)return false;
	QList<IGroupHandle*> groups=db->sortedGroups();
	for(int g=0;g<groups.size();g++){
		file->write(GroupTemplate.arg(groups[g]->title()).toUtf8());
		QList<IEntryHandle*> entries=db->entriesSortedStd(groups[g]);
		for(int e=0;e<entries.size();e++){
			SecString password=entries[e]->password();
			password.unlock();
			QString entryText = EntryTemplate.arg(
						entries[e]->title(),
						entries[e]->username(),
						entries[e]->url(),
						password.string(),
						entries[e]->comment().replace('\n',"\n            "));
									
			file->write( entryText.toUtf8() );
			password.lock();
		}
	}
	delete file;
	return true;
}
