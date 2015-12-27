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

#ifndef _IMPORT_KPX_XML_H_
#define _IMPORT_KPX_XML_H_

#include "Import.h"


class Import_KeePassX_Xml:public ImporterBase, public IImport{
	Q_OBJECT
	
	public:
		virtual bool importDatabase(QWidget* GuiParent, IDatabase* Database);
		virtual QString identifier(){return "KeePassX_Xml";}
		virtual QString title(){return "KeePassX XML (*.xml)";}
	private:
		bool parseGroup(const QDomElement& GroupElement,IGroupHandle* ParentGroup);
		bool parseEntry(const QDomElement& EntryElement,IGroupHandle* Group);
		IDatabase* db;
		QWidget* GuiParent;	
};


#endif
