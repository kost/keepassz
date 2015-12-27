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
#ifndef SEARCHDLG_H
#define SEARCHDLG_H

#include "ui_SearchDlg.h"

class SearchDialog : public QDialog, private Ui_Search_Dlg
{
	Q_OBJECT
	public:
		SearchDialog(IDatabase* database, IGroupHandle* group,QWidget* parent);
		~SearchDialog();
		QList<IEntryHandle*> Result;
	
	private slots:
		virtual void OnClose();
		virtual void OnSearch();
	
	private:
		QPixmap BannerPixmap;
		IGroupHandle* group;
		IDatabase* db;
		virtual void paintEvent(QPaintEvent*);
};

#endif

