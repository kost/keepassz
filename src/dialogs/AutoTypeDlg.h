/***************************************************************************
 *   Copyright (C) 2005-2008 by Felix Geyer                                *
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

#include "ui_AutoTypeDlg.h"

class AutoTypeDlg : public QWidget, private Ui::AutoTypeDlg
{
	Q_OBJECT
	
	public:
		AutoTypeDlg(QList<IEntryHandle*> entries, QList<int> numbers, bool wasLocked);
		inline static bool isDialogVisible() { return dialogVisible; };
	
	protected:
		void paintEvent(QPaintEvent* event);
		void resizeEvent(QResizeEvent* event);
		void closeEvent(QCloseEvent* event);
		bool event(QEvent* event);
	
	private slots:
		void itemSelected(QTreeWidgetItem* item);
	
	private:
		struct AutoTypeEntry {
			IEntryHandle* dbHandle;
			int nr;
		};
		static bool itemLessThan(QTreeWidgetItem* i1, QTreeWidgetItem* i2);
		QHash<QTreeWidgetItem*,AutoTypeEntry> itemToEntry;
		QPixmap BannerPixmap;
		bool pWasLocked;
		static bool dialogVisible;
};
