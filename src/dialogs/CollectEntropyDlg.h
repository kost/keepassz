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

#ifndef _COLLECT_ENTROPY_DLG_H_
#define _COLLECT_ENTROPY_DLG_H_

#include "ui_CollectEntropyDlg.h"

class CollectEntropyDlg: public QDialog, private Ui_CollectEntropyDlg{
	Q_OBJECT
	public:
		CollectEntropyDlg(QWidget* parent);
		~CollectEntropyDlg();

	private:
		QPixmap BannerPixmap;
		virtual void paintEvent(QPaintEvent* event);
		virtual void keyReleaseEvent(QKeyEvent* event );
		virtual void showEvent(QShowEvent* event);
		void updateProgress();
		unsigned char* KeyEntropyBuffer;
		quint16* MouseEntropyBuffer;
		int KeyCounter,MouseCounter;
		QPoint LastPos;
		bool ReseedDone;
	private slots:
		void trackMousePos();

};




#endif
