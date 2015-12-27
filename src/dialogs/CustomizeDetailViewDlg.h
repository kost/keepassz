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
#ifndef _CUSTOMIZE_DETAIL_VIEW_H_
#define _CUSTOMIZE_DETAIL_VIEW_H_

#include "ui_CustomizeDetailViewDlg.h"

class CustomizeDetailViewDialog : public QDialog, private Ui_CustomizeDetailViewDialog{
	Q_OBJECT
	public:
		CustomizeDetailViewDialog(QWidget* parent);
		
	private slots:
		void OnBtnBold(bool);
		void OnBtnItalic(bool);
		void OnBtnUnderline(bool);
		void OnBtnAlignLeft();
		void OnBtnAlignRight();
		void OnBtnAlignCenter();
		void OnBtnAlignBlock();
		void OnBtnColor();
		void OnSave();
		void OnCancel();
		void OnRestoreDefault(QAbstractButton* button);
		void OnInsertTemplate(QAction*);
		void OnCursorPositionChanged();
		void OnTabChanged(int index);
		void OnFontSizeChanged(const QString& text=QString());
	
	private:
		QColor CurrentColor;


};

#endif
