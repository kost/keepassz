/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   mail@tarek-saidi.de                                                   *
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


#include "Import_PwManager.h"

#include "crypto/blowfish.h"
#include <QCryptographicHash>

bool Import_PwManager::importDatabase(QWidget* GuiParent, IDatabase* db){	
	database=db;
	QFile* file=openFile(GuiParent,identifier(),QStringList()<<tr("PwManager Files (*.pwm)")<<tr("All Files (*)"));
	if(!file)return false;
	QString password=getPassword(GuiParent);
	if(password.isEmpty()){delete file; return false;}
	char* buffer=NULL;
	int offset=0;
	int len=0;
	if((len=file->size())){
		buffer=new char[len];
	}
	else {
		QMessageBox::critical(GuiParent,tr("Import Failed"),tr("File is empty."));
		delete file;
		return false;
	}
	file->read(buffer,len);
	file->close();
	delete file;
	if(QString::fromAscii(buffer,17)!="PWM_PASSWORD_FILE")
	{QMessageBox::critical(GuiParent,tr("Import Failed"),tr("File is no valid PwManager file.")); return false;}
	offset+=17;
	if(buffer[offset]!=0x05)
	{QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Unsupported file version.")); return false;}
	offset++;
	if(buffer[offset]!=0x01)
	{QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Unsupported hash algorithm.")); return false;}
	offset++;
	if(buffer[offset]!=0x01)
	{QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Unsupported hash algorithm.")); return false;}
	offset++;
	if(buffer[offset]!=0x01)
	{QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Unsupported encryption algorithm.")); return false;}
	offset++;
	if(buffer[offset]==0x00)Compression=0;
	if(buffer[offset]==0x01)Compression=1;
	if(buffer[offset]==0x02)Compression=2;
	///@TODO Compression
	if(buffer[offset])
	{QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Compressed files are not supported yet.")); return false;}
	offset++;
	if(buffer[offset]==0x00)KeyFlag=true;
	else KeyFlag=false;
	offset++;
	//Reserved Bytes (64)
	offset+=64;
	memcpy(KeyHash,buffer+offset,20);
	offset+=20;
	memcpy(DataHash,buffer+offset,20);
	offset+=20;
	
	Blowfish blowfish;
	int pwlen=password.length();
	byte* Key=new byte[pwlen];
	byte* xml=new byte[len-offset+1];
	xml[len-offset]=0;
	memcpy(Key,password.toAscii(),pwlen);
	QCryptographicHash sha(QCryptographicHash::Sha1);
	sha.addData((const char*)Key,pwlen);
	QByteArray key_hash = sha.result();
	if(memcmp(key_hash.constData(),KeyHash,20)){
		delete[] Key;
		delete [] buffer;
		QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Wrong password."));
		return false;
	}
	blowfish.bf_setkey(Key,password.length());
	blowfish.bf_decrypt(xml,(byte*)buffer+offset,len-offset);
	delete [] Key;
	delete [] buffer;
	sha.reset();
	sha.addData((const char*)xml,strlen((char*)xml)-1);
	QByteArray content_hash = sha.result();
	if(memcmp(content_hash.constData(),DataHash,20)){
		delete [] xml;
		QMessageBox::critical(GuiParent,tr("Import Failed"),tr("File is damaged (hash test failed)."));
		return false;
	}
	
	if(!parseXmlContent((char*)xml)){
		delete [] xml;
		QMessageBox::critical(GuiParent,tr("Import Failed"),tr("Invalid XML data (see stdout for details).")); return false;
	}
	return true;
}

bool Import_PwManager::parseXmlContent(char* content){
	QDomDocument db;
	QString err;
	int col,line;
	if(!db.setContent(QString::fromUtf8(content,strlen(content)-1),false,&err,&line,&col)){
		qWarning("Import_PwManager::parseXmlContent():\n");
		qWarning("%s (Line:%d Column:%d)\n", CSTR(err), line, col);
		return false;
	}
	QDomElement root=db.documentElement();
	if(root.tagName()!="P")return false;
	//Achtung! Kommentare und Kategorien haben das selbe Tag "c"
	if(!root.elementsByTagName("c").item(0).isElement())return false;
	QDomElement groups=root.elementsByTagName("c").item(0).toElement();
	
	int i=0;
	while(1){
	QDomElement CurrGroup;
	if(!groups.elementsByTagName("c"+QString::number(i)).length())break;
	if(groups.elementsByTagName("c"+QString::number(i)).length()>1)return false;
	if(!groups.elementsByTagName("c"+QString::number(i)).item(0).isElement())return false;
	CurrGroup=groups.elementsByTagName("c"+QString::number(i)).item(0).toElement();
	if(!CurrGroup.hasAttribute("n"))return false;
	CGroup tmpGroup;
	IGroupHandle* NewGroup=database->addGroup(&tmpGroup,NULL);
	NewGroup->setTitle(CurrGroup.attribute("n"));
	int j=0;
		while(1){
		QDomElement CurrEntry;
		if(!CurrGroup.elementsByTagName("e"+QString::number(j)).length())break;
		if(CurrGroup.elementsByTagName("e"+QString::number(j)).length()>1)return false;
		if(!CurrGroup.elementsByTagName("e"+QString::number(j)).item(0).isElement())return false;
		CurrEntry=CurrGroup.elementsByTagName("e"+QString::number(j)).item(0).toElement();
		if(!xml_parseEntryAttributes(&CurrEntry,NewGroup))return false;
		j++;
		}
	i++;
	}
	
	return true;
}

bool Import_PwManager::xml_parseEntryAttributes(QDomElement* EntryElement,IGroupHandle* NewGroup){
	IEntryHandle* e=database->newEntry(NewGroup);
	e->setTitle(EntryElement->elementsByTagName("d").item(0).toElement().text());
	e->setUsername(EntryElement->elementsByTagName("n").item(0).toElement().text());
	QString pw(EntryElement->elementsByTagName("p").item(0).toElement().text());
	SecString spw;
	spw.setString(pw,true);
	e->setPassword(spw);
	QString comment=EntryElement->elementsByTagName("c").item(0).toElement().text();
	comment.replace("$>--endl--<$","\n");
	e->setComment(comment);
	e->setUrl(EntryElement->elementsByTagName("u").item(0).toElement().text());
	return true;
}
