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
 
#ifndef GENPWDIALOG_H
#define GENPWDIALOG_H

#include "ui_PasswordGenDlg.h"
#include "EditEntryDlg.h"

class CGenPwDialog : public QDialog, public Ui_GenPwDlg
{
	Q_OBJECT
	public:
		CGenPwDialog(QWidget* parent, bool StandAloneMode, Qt::WFlags fl = 0);
		~CGenPwDialog();
	
	private:
		struct PwGroup {
			int start;
			int end;
		};
		void AddToAssoctable(QList<QChar>& table,int start,int end,int& pos);
		PwGroup AddToAssoctableGroup(QList<QChar>& table,int start,int end,int& pos);
		void paintEvent(QPaintEvent* event);
		QString generatePasswordInternal(int length);
		QPixmap BannerPixmap;
		static bool EntropyCollected;
		QPushButton* AcceptButton;
	
	private slots:
		void OnGeneratePw();
		void OnCancel();
		void OnAccept();
		void estimateQuality();
		void OnCollectEntropyChanged(int);
		void SwapEchoMode();
		void setGenerateEnabled();
		void setAcceptEnabled(const QString& str);
};

class PassCharValidator : public QValidator
{
	Q_OBJECT
	
	public:
		PassCharValidator(QObject* parent);
		State validate(QString& input, int& pos) const;
};

#endif

