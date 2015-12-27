/***************************************************************************
 *   Copyright (C) 2007 by Tarek Saidi                                     *
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
 

#include "Import_KeePassX_Xml.h"

bool Import_KeePassX_Xml::importDatabase(QWidget* Parent, IDatabase* database){
	db=database;
	GuiParent=Parent;
	QFile* file=openFile(GuiParent,identifier(),QStringList()<<tr("KeePass XML Files (*.xml)")<<tr("All Files (*)"));
	if(!file)return false;
	QDomDocument doc;
	QString ErrMsg;
	int ErrLine;
	int ErrCol;
	if(!doc.setContent(file,false,&ErrMsg,&ErrLine,&ErrCol)){
		QMessageBox::critical(GuiParent,tr("Import Failed"),tr("XML parsing error on line %1 column %2:\n%3").arg(ErrLine).arg(ErrCol).arg(ErrMsg));		
		delete file;
		return false;
	}	
	delete file;
	QDomElement root=doc.documentElement();
	if(root.tagName()!="database"){
		QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Parsing error: File is no valid KeePassX XML file."));		
		return false;		
	}
	QDomNodeList TopLevelGroupNodes=root.childNodes();
	QStringList GroupNames;
	for(int i=0;i<TopLevelGroupNodes.count();i++){
		if(TopLevelGroupNodes.at(i).toElement().tagName()!="group"){
			qWarning("Import_KeePassX_Xml: Error: Unknow tag '%s'",CSTR(TopLevelGroupNodes.at(i).toElement().tagName()));
			QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Parsing error: File is no valid KeePassX XML file."));		
			return false;
		}
		if(!parseGroup(TopLevelGroupNodes.at(i).toElement(),NULL)){
			QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Parsing error: File is no valid KeePassX XML file."));
			return false;}		
	}
	
	return true;
	
}

bool Import_KeePassX_Xml::parseGroup(const QDomElement& GroupElement,IGroupHandle* ParentGroup){
	CGroup Group;
	QDomNodeList ChildNodes=GroupElement.childNodes();
	for(int i=0;i<ChildNodes.size();i++){
		if(!ChildNodes.item(i).isElement()){
			qWarning("Import_KeePassX_Xml: Error: Invalid node.");
			return false;
		}
		if(ChildNodes.item(i).toElement().tagName()=="icon")
			Group.Image=ChildNodes.item(i).toElement().text().toInt();
		else if(ChildNodes.item(i).toElement().tagName()=="title")
			Group.Title=ChildNodes.item(i).toElement().text();
	}
	
	IGroupHandle* GroupHandle=db->addGroup(&Group,ParentGroup);
	
	for(int i=0;i<ChildNodes.size();i++){
		if(ChildNodes.item(i).toElement().tagName()=="entry"){
			if(!parseEntry(ChildNodes.item(i).toElement(), GroupHandle))return false;
		}else if(ChildNodes.item(i).toElement().tagName()=="group"){
			if(!parseGroup(ChildNodes.item(i).toElement(),GroupHandle))return false;
		}		
	}
	
	return true;

}


bool Import_KeePassX_Xml::parseEntry(const QDomElement& EntryElement,IGroupHandle* Group){
	if(EntryElement.isNull()){
		qWarning("Import_KeePassX_Xml: Error: Entry element is null.");
		return false;
	}
	IEntryHandle* entry=db->newEntry(Group);
	QDomNodeList ChildNodes=EntryElement.childNodes();
	for(int i=0;i<ChildNodes.size();i++){
		if(!ChildNodes.item(i).isElement()){
			qWarning("Import_KeePassX_Xml: Error: Invalid node.");
			return false;
		}
		if(ChildNodes.item(i).toElement().tagName()=="title")
			entry->setTitle(ChildNodes.item(i).toElement().text());
		else if(ChildNodes.item(i).toElement().tagName()=="username")
			entry->setUsername(ChildNodes.item(i).toElement().text());
		else if(ChildNodes.item(i).toElement().tagName()=="password"){
			SecString pw;
			QString cpw=ChildNodes.item(i).toElement().text();
			pw.setString(cpw,true);
			entry->setPassword(pw);			
		}
		else if(ChildNodes.item(i).toElement().tagName()=="url")
			entry->setUrl(ChildNodes.item(i).toElement().text());
		else if(ChildNodes.item(i).toElement().tagName()=="icon")
			entry->setImage(ChildNodes.item(i).toElement().text().toInt());
		else if(ChildNodes.item(i).toElement().tagName()=="creation")
			entry->setCreation(KpxDateTime::fromString(ChildNodes.item(i).toElement().text(),Qt::ISODate));			
		else if(ChildNodes.item(i).toElement().tagName()=="lastaccess")
			entry->setLastAccess(KpxDateTime::fromString(ChildNodes.item(i).toElement().text(),Qt::ISODate));	
		else if(ChildNodes.item(i).toElement().tagName()=="lastmod")
			entry->setLastMod(KpxDateTime::fromString(ChildNodes.item(i).toElement().text(),Qt::ISODate));	
		else if(ChildNodes.item(i).toElement().tagName()=="expire")
			entry->setExpire(KpxDateTime::fromString(ChildNodes.item(i).toElement().text(),Qt::ISODate));
		else if(ChildNodes.item(i).toElement().tagName()=="bindesc")
			entry->setBinaryDesc(ChildNodes.item(i).toElement().text());
		else if(ChildNodes.item(i).toElement().tagName()=="bin")
			entry->setBinary(QByteArray::fromBase64(ChildNodes.item(i).toElement().text().toAscii()));
		else if(ChildNodes.item(i).toElement().tagName()=="comment"){
			QDomNodeList Lines=ChildNodes.item(i).childNodes();
			QString comment;
			for(int i=0;i<Lines.size();i++){
				if(Lines.item(i).isText())
					comment+=Lines.item(i).toText().data();
				else if(Lines.item(i).toElement().tagName()=="br")
					comment+="\n";
				else{
					qWarning("Import_KeePassX_Xml: Error: Comment element contains invalid nodes.");
					return false;
				}				
			}
			entry->setComment(comment);
		}
	}
	return true;	
}
