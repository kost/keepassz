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

#ifndef _SIMPLEPASSWORDDIALOG_H
#define _SIMPLEPASSWORDDIALOG_H

#include "ui_SimplePasswordDlg.h"

class SimplePasswordDialog : public QDialog, private Ui_SimplePasswordDialog
{
	Q_OBJECT

	public:
		SimplePasswordDialog(QWidget* parent = 0, Qt::WFlags fl = 0 );
		~SimplePasswordDialog();
		QString password;

	private slots:
		virtual void OnCancel();
		virtual void OnOK();
		virtual void OnHidePasswordToggled(bool state);
		virtual void OnTextChanged(const QString&);
};

#endif

