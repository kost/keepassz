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
#include <QString>
#include <QStringList>

#ifndef _I_FILE_DIALOG_H_
#define _I_FILE_DIALOG_H_

class IFileDialog{
	public:
		virtual ~IFileDialog(){}
		virtual QString openExistingFileDialog(QWidget* parent,QString title,QString dir,
							QStringList Filters,int SelectedFilter)=0;
		virtual QStringList openExistingFilesDialog(QWidget* parent,QString title,QString dir,
							QStringList Filters,int SelectedFilter)=0;
		
		virtual QString saveFileDialog(QWidget* parent,QString title,QString dir,
							QStringList Filters,int SelectedFilter, bool ShowOverwriteWarning=true)=0;
		
		virtual int getLastFilter()=0;
};
Q_DECLARE_INTERFACE(IFileDialog,"org.KeePassX.FileDialogInterface/1.0")
		
#endif
