/***************************************************************************
 *   Copyright (C) 2006 by Brian Johnson                                   *
 *   dev-keepassx@sherbang.com                                             *
 *																		   *
 *   Copyright (C) 2007 by Tarek Saidi		                               *
 *   tarek.saidi@arcor.de                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#include <iostream>
#include <QtXml>
#include <QtCore>

#include "IImport.h"
#include "Import_GnuKeyRing.h"
#include "lib/FileDialogs.h"
#include "main.h"

using namespace std;

QString Import_GnuKeyRing::importDatabase(QWidget* GuiParent, IDatabase* Database){
	QString FileName=KpxFileDialogs::openExistingFile(GuiParent,
													 "Import_Keyring",
													 tr("Import Database..."),
													 QStringList()<<tr("All Files (*)"));
	
	if(FileName==QString())
		return QString();
	
	QFile file(FileName);
	if(!file.exists()){
		return tr("File not found.");
	}
	if(!file.open(QIODevice::ReadOnly)){
		return decodeFileError(file.error());
	}
	int len=file.size();
	if(len==0){
		return tr("File is empty.");
	}
	QTextStream ts(&file);
	
	uint entryNum = 0;
	QRegExp newEntry("^#\\d*$");
	QMap<QString, CGroup*> categories;
	/*
	CGroup* DefaultGroup=pwm->addGroup(NULL);
	DefaultGroup->Name="def-group";
	QString* field = NULL;
	QString category;
	QString name;
	QString account;
	QString password;
	QString note;
	while (!ts.atEnd()){
		QString line = ts.readLine(400);
	if (newEntry.exactMatch(line)){
		//Save entry
		CGroup* newGroup;
		if (entryNum != 0){
		if (category != ""){
			if (categories.contains(category)){
			newGroup=categories[category];
			}else{
			newGroup=pwm->addGroup(NULL);
			newGroup->Name=category;
			categories.insert(category, newGroup);
			}
		}else{
			newGroup = DefaultGroup;
		}
	
		CEntry* NewEntry=pwm->addEntry();
		NewEntry->GroupID=newGroup->ID;
		NewEntry->Title=name;
		NewEntry->UserName=account;
		NewEntry->Password.setString(password,true);
		NewEntry->Additional=note;
		}
		//New Entry
		category = "";
		name = "";
		account = "";
		password = "";
		note = "";
		entryNum = line.remove(0,1).toUInt();
	}else if(entryNum == 0){
		//Do nothing (waiting for start of first entry)
	}else if(line.startsWith("Category:")){
		category = line.remove(0,10);
		field = &category;
	}else if(line.startsWith("Name:")){
		name = line.remove(0,6);
		field = &name;
	}else if(line.startsWith("Account:")){
		account = line.remove(0,9);
		field = &account;
	}else if(line.startsWith("Password:")){
		password = line.remove(0,10);
		field = &password;
	}else if(line.startsWith("Notes:")){
		note = line.remove(0,7);
		field = &note;
	}else if(field != NULL){
		field->append("\n");
		field->append(line);
	}
	}
	file.close();
	pwm->SearchGroupID=-1;
	pwm->CryptoAlgorithmus=ALGO_AES;
	pwm->KeyEncRounds=6000;
	return true;
	*/
	return QString();
} 
