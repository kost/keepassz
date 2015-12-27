/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek@linux                                                           *
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

#ifndef EDITGROUPDLG_H
#define EDITGROUPDLG_H

#include "ui_EditGroupDlg.h"

class CEditGroupDialog : public QDialog, private Ui_EditGroupDialog
{
	Q_OBJECT
	public:
		CEditGroupDialog(IDatabase*,IGroupHandle*,QWidget* parent = 0,  Qt::WFlags fl = 0 );
		CEditGroupDialog(IDatabase*,CGroup*,QWidget* parent = 0, Qt::WFlags fl = 0 );
		~CEditGroupDialog();
	
	private:
		void init();
		IDatabase* db;
		IGroupHandle* handle; 
		CGroup* group;

	private slots:
		virtual void OnOK();
		virtual void OnCancel();
		virtual void OnIconDlg();
};

#endif

