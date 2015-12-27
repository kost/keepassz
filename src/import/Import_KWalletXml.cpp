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


#include "Import_KWalletXml.h"

bool Import_KWalletXml::importDatabase(QWidget* GuiParent, IDatabase* db){
	QFile* file=openFile(GuiParent,identifier(),QStringList()<<tr("XML Files (*.xml)")<<tr("All Files (*)"));
	if(!file)return false;
	int len=file->size();
	quint8* buffer=new quint8[len];
	file->read((char*)buffer,len);
	file->close();
	delete file;
	QDomDocument doc;
	QString xmlerr;
	int col,line;
	if(!doc.setContent(QString::fromUtf8((char*)buffer,len),false,&xmlerr,&line,&col)){
		qWarning("Import_PwManager::parseXmlContent():\n");
		qWarning("%s (Line:%d Column:%d)\n",CSTR(xmlerr), line, col);
		QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Invalid XML data (see stdout for details)."));
		delete [] buffer;
		return false;}
	delete [] buffer;
	QDomElement root=doc.documentElement();
	if(root.tagName()!="wallet"){QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Invalid XML file.")); return false;}
	QDomNodeList groups=root.elementsByTagName("folder");
	if(!groups.length()){QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Document does not contain data.")); return false;}
	for(int i=0;i<groups.length();i++){
		if(!groups.item(i).isElement()){QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Invalid XML file.")); return false;}
		QDomElement CurrGroup=groups.item(i).toElement();
		if(!CurrGroup.hasAttribute("name")){QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Invalid XML file.")); return false;}
		CGroup NewGroup;
		NewGroup.Title=CurrGroup.attribute("name");
		IGroupHandle* NewGroupHandle=db->addGroup(&NewGroup,NULL);
		QDomNodeList entries=CurrGroup.elementsByTagName("password");
		for(int j=0;j<entries.length();j++){
			if(!entries.item(j).isElement()){QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Invalid XML file.")); return false;}
			QDomElement CurrEntry=entries.item(j).toElement();
			if(!CurrEntry.hasAttribute("name")){QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Invalid XML file.")); return false;}
			IEntryHandle* NewEntry=db->newEntry(NewGroupHandle);
			NewEntry->setTitle(CurrEntry.attribute("name"));
			QString pw=CurrEntry.text();
			SecString pws;
			pws.setString(pw,true);
			NewEntry->setPassword(pws);
			}
	}
	return true;
}
