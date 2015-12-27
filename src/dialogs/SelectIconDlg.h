/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi                                *
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

#ifndef _SELECT_ICON_DLG_
#define _SELECT_ICON_DLG_

#include "ui_SelectIconDlg.h"

class CSelectIconDlg:public QDialog, private Ui_SelectIconDlg{
	Q_OBJECT
	
	public:
		CSelectIconDlg(IDatabase* db,int ImageID,QWidget* parent = 0, Qt::WFlags fl = 0);

	private slots:
		void OnAddIcon();
		void OnPickIcon();
		void OnCancel();
		void OnDelete();
		void OnReplace();
		void OnSelectionChanged(QListWidgetItem*);

	private:
		int Id;
		IDatabase* db;
		void updateView();
		QMenu* CtxMenu;
		QAction* DeleteAction;
		QAction* ReplaceAction;
		QPushButton* Button_PickIcon;

	protected:
		virtual void contextMenuEvent(QContextMenuEvent *event);
		virtual void showEvent(QShowEvent* event);

};


#endif
