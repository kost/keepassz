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
 
#ifndef _EXPORT_KPX_XML_H_
#define _EXPORT_KPX_XML_H_

#include "Export.h"

class Export_KeePassX_Xml:public ExporterBase, public IExport{
	Q_OBJECT
	
	public:
		virtual bool exportDatabase(QWidget* GuiParent, IDatabase* Database);	
		virtual QString identifier(){return "EXPORT_KEEPASSX_XML";}
		virtual QString title(){return tr("KeePassX XML File");}
	private:
		void addGroup(IGroupHandle* group,QDomElement& parent,QDomDocument& doc);
		void addEntry(IEntryHandle* group,QDomElement& parent,QDomDocument& doc);
		IDatabase* db;
};

#endif
 
