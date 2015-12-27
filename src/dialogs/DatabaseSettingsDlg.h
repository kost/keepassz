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
 
#ifndef DBSETTINGSDLG_H
#define DBSETTINGSDLG_H

#include "ui_DatabaseSettingsDlg.h"

class CDbSettingsDlg : public QDialog, private Ui_DatabaseSettingsDlg
{
	Q_OBJECT
	public:
		CDbSettingsDlg(QWidget* parent,IDatabase* db, Qt::WFlags fl = 0 );
		~CDbSettingsDlg();
		virtual void paintEvent(QPaintEvent *);

	private slots:
    	void OnCancel();
    	void OnOK();
		void OnBenchmark();

	private:
		IKdbSettings* database;
		QPixmap BannerPixmap;
};

#endif

