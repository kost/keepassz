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


#include "SimplePasswordDlg.h"

SimplePasswordDialog::SimplePasswordDialog(QWidget* parent, Qt::WFlags fl)
: QDialog(parent,fl)
{
	setupUi(this);
	connect(buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked()),this,SLOT(OnOK()));
	connect(buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),this,SLOT(OnCancel()));
	connect(Button_HidePassword,SIGNAL(toggled(bool)),this,SLOT(OnHidePasswordToggled(bool)));
	connect(EditPassword,SIGNAL(textChanged(const QString&)),this,SLOT(OnTextChanged(const QString&)));
	if(!config->showPasswordsPasswordDlg())Button_HidePassword->toggle();
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

SimplePasswordDialog::~SimplePasswordDialog()
{
}

void SimplePasswordDialog::OnTextChanged(const QString& txt){
	if(txt.isEmpty())
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void SimplePasswordDialog::OnCancel()
{
	done(0);
}


void SimplePasswordDialog::OnOK()
{
	password=EditPassword->text();
	done(1);
}


void SimplePasswordDialog::OnHidePasswordToggled(bool state)
{
	if(state){
		EditPassword->setEchoMode(QLineEdit::Password);
		Button_HidePassword->setIcon(getIcon("pwd_hide"));
	}
	else {
		EditPassword->setEchoMode(QLineEdit::Normal);
		Button_HidePassword->setIcon(getIcon("pwd_show"));
	}
}
