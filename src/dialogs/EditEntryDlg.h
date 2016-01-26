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

#ifndef EDITENTRYDLG_H
#define EDITENTRYDLG_H

#include "ui_EditEntryDlg.h"
#include "Kdb3Database.h"

class CEditEntryDlg : public QDialog, private Ui_EditEntryDialog
{
	Q_OBJECT
	public:
		CEditEntryDlg(IDatabase* _db, IEntryHandle* _entry,QWidget* parent = 0, bool newEntry = FALSE);
		~CEditEntryDlg();
		void InitGroupComboBox();
		static void saveAttachment(IEntryHandle* pEntry, QWidget* ParentWidget=NULL);
		
		IEntryHandle* entry;
		IDatabase* db;
		bool ModFlag;
		int GroupIndex;
		QList<IGroupHandle*> groups;
		QPixmap BannerPixmap;	
	
	private slots:
		void OnTitleTextChanged(const QString&);
		void OnPasswordwLostFocus();
		void OnPasswordwTextChanged();
		void OnPasswordTextChanged();
		void ChangeEchoMode();
		void OnButtonCancel();
		void OnNewAttachment();
		void OnDeleteAttachment();
		void OnSaveAttachment();
		void OnButtonGenPw();
		void OnCheckBoxExpiresNeverChanged(int state);
		void OnButtonIcons();
		void OnButtonOK();
		void OnExpirePreset(QAction* action);
		void OnCalendar(QAction* action);
		void OnClose();
		void OnCustomizeSequence();
		void OnSelectTarget();
		
	private:
		virtual void paintEvent(QPaintEvent*);
		virtual void resizeEvent(QResizeEvent *);
		
		int IconIndex;
		bool pNewEntry;
};

#endif
