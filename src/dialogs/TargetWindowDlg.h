/***************************************************************************
 *   Copyright (C) 2008 by Felix Geyer                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
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

#ifndef TARGETWINDOWDLG_H
#define TARGETWINDOWDLG_H

#include "ui_TargetWindowDlg.h"

class TargetWindowDlg : public QDialog, private Ui_TargetWindowDlg
{
	Q_OBJECT
	
	public:
		TargetWindowDlg(QWidget* parent);
		inline QString windowTitle() { return pWindowTitle; };
	
	private slots:
		void OnAccept();
	
	private:
		QString pWindowTitle;
};

#endif // TARGETWINDOWDLG_H
