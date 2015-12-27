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

#include "Export_KeePassX_Xml.h"

bool Export_KeePassX_Xml::exportDatabase(QWidget* GuiParent,IDatabase* database){
	db=database;	
	QFile *file=openFile(GuiParent,identifier(),QStringList()<<tr("XML Files (*.xml)") << tr("All Files (*)"));
	if(!file)return false;
	QDomDocument doc("KEEPASSX_DATABASE");
	QDomElement root=doc.createElement("database");
	doc.appendChild(root);
	QList<IGroupHandle*> Groups=db->sortedGroups();
	for(int i=0;i<Groups.size();i++){
		if(Groups[i]->parent()==NULL){
			addGroup(Groups[i],root,doc);
		}
	}
	file->write(doc.toByteArray());
	file->close();
	delete file;
	return true;
}

void Export_KeePassX_Xml::addGroup(IGroupHandle* group,QDomElement& parent,QDomDocument& doc){
	QDomElement GroupElement=doc.createElement("group");
	parent.appendChild(GroupElement);
	QDomElement Title=doc.createElement("title");
	QDomElement Icon=doc.createElement("icon");
	Title.appendChild(doc.createTextNode(group->title()));
	Icon.appendChild(doc.createTextNode(QString::number(group->image())));
	GroupElement.appendChild(Title);
	GroupElement.appendChild(Icon);
	QList<IGroupHandle*> children=group->children();
	for(int i=0;i<children.size();i++){
		addGroup(children[i],GroupElement,doc);
	}
	QList<IEntryHandle*> entries=db->entriesSortedStd(group);
	for(int i=0;i<entries.size();i++){
		addEntry(entries[i],GroupElement,doc);
	}
	
}

void Export_KeePassX_Xml::addEntry(IEntryHandle* entry,QDomElement& parent,QDomDocument& doc){
	QDomElement GroupElement=doc.createElement("entry");
	parent.appendChild(GroupElement);
	QDomElement Title=doc.createElement("title");
	QDomElement Username=doc.createElement("username");
	QDomElement Password=doc.createElement("password");
	QDomElement Url=doc.createElement("url");
	QDomElement Comment=doc.createElement("comment");
	QDomElement BinaryDesc=doc.createElement("bindesc");
	QDomElement Binary=doc.createElement("bin");	
	QDomElement Icon=doc.createElement("icon");
	QDomElement Creation=doc.createElement("creation");
	QDomElement LastAccess=doc.createElement("lastaccess");	
	QDomElement LastMod=doc.createElement("lastmod");
	QDomElement Expire=doc.createElement("expire");	
	
	Title.appendChild(doc.createTextNode(entry->title()));
	Username.appendChild(doc.createTextNode(entry->username()));
	SecString password=entry->password();
	password.unlock();
	Password.appendChild(doc.createTextNode(password.string()));
	password.lock();
	Url.appendChild(doc.createTextNode(entry->url()));
	QStringList CommentLines=entry->comment().split('\n');
	for(int i=0;i<CommentLines.size();i++){
		Comment.appendChild(doc.createTextNode(CommentLines[i]));
		if(i==CommentLines.size()-1)break;
		Comment.appendChild(doc.createElement("br"));
	}
	bool HasAttachment=!entry->binary().isNull();
	if(HasAttachment){
		BinaryDesc.appendChild(doc.createTextNode(entry->binaryDesc()));
		Binary.appendChild(doc.createTextNode(entry->binary().toBase64()));
	}
	Icon.appendChild(doc.createTextNode(QString::number(entry->image())));
	Creation.appendChild(doc.createTextNode(entry->creation().toString(Qt::ISODate)));
	LastAccess.appendChild(doc.createTextNode(entry->lastAccess().toString(Qt::ISODate)));
	LastMod.appendChild(doc.createTextNode(entry->lastMod().toString(Qt::ISODate)));
	Expire.appendChild(doc.createTextNode(entry->expire().toString(Qt::ISODate)));
	GroupElement.appendChild(Title);
	GroupElement.appendChild(Username);
	GroupElement.appendChild(Password);
	GroupElement.appendChild(Url);
	GroupElement.appendChild(Comment);
	if(HasAttachment){
		GroupElement.appendChild(BinaryDesc);
		GroupElement.appendChild(Binary);
	}
	GroupElement.appendChild(Icon);
	GroupElement.appendChild(Creation);
	GroupElement.appendChild(LastAccess);
	GroupElement.appendChild(LastMod);
	GroupElement.appendChild(Expire);
}
