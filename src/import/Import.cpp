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


#include "Import.h"
#include "dialogs/SimplePasswordDlg.h"

QFile* ImporterBase::openFile(QWidget* parent, QString id, QStringList Filters){
	QString filename=KpxFileDialogs::openExistingFile(parent,id,tr("Import File..."),Filters);
	if(filename==QString())return NULL;
	QFile* file=new QFile(filename);
	if(!file->open(QIODevice::ReadOnly)){
		QMessageBox::critical(parent,tr("Import Failed"),decodeFileError(file->error()));
		delete file;
		return NULL;
	}
	return file;	
}

QString ImporterBase::getPassword(QWidget* parent){
	SimplePasswordDialog dlg(parent);
	dlg.exec();
	return dlg.password;	
}
