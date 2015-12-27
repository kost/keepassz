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

#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QPaintEvent>
#include "ui_PasswordDlg.h"


class PasswordDialog : public QDialog, private Ui_PasswordDlg {
	Q_OBJECT
	public:
		enum DlgMode {
			Mode_Ask,     // Normal password entry when opening a database
			Mode_Set,     // Setting password for the first time after creating a new database
			Mode_Change   // Changing the password of a database
		};
		
		enum DlgFlags {
			Flag_None = 0x00, 
			Flag_Auto = 0x01  // Dialog was automatically opened on start-up	
		};
		
		enum DlgExit {
			Exit_Ok=QDialog::Accepted,
			Exit_Cancel=QDialog::Rejected,
			Exit_Quit=3
		};
		
		typedef bool (KeyFileGenProc)(const QString& filename,QString* error);
			
		PasswordDialog(QWidget* parent,DlgMode mode,DlgFlags flags,const QString& filename=QString());
		
		// result functions
		QString selectedBookmark();
		QString keyFile();
		QString password();
	
	private slots:
		void OnOK();
		void OnCancel();
		void OnButtonBrowse();
		void OnButtonQuit();
		void OnGenKeyFile();
		void OnButtonBack();
		void ChangeEchoModeDatabaseKey();
		void OnBookmarkTriggered(QAction* action);
		void OnCheckBoxesChanged();
		void done(int r);

	private:
		DlgMode Mode;
		QPixmap BannerPixmap;
		QString BookmarkFilename;
		QString Filename;
		QString Password;
		QString KeyFile;
		void setStatePasswordOnly();
		void setStateKeyFileOnly();
		void setStateBoth();
		virtual void paintEvent(QPaintEvent*);
};

#endif

