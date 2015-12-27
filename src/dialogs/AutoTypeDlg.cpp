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

#include <QDesktopWidget>
#include "AutoTypeDlg.h"

bool AutoTypeDlg::dialogVisible = false;

AutoTypeDlg::AutoTypeDlg(QList<IEntryHandle*> entries, QList<int> numbers, bool wasLocked) : pWasLocked(wasLocked){
	Q_ASSERT(!dialogVisible);
	dialogVisible = true;
	setupUi(this);
	
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
	setGeometry( QRect(QApplication::desktop()->screenGeometry(QCursor::pos()).center() - rect().center(), size()) );
	setWindowIcon(getIcon("keepassx_small"));
	entryList->setAlternatingRowColors(config->alternatingRowColors());
	
	bool hideUsernames = config->hideUsernames();
	if (hideUsernames)
		entryList->setHeaderLabels(QStringList() << tr("Group") << tr("Title"));
	else
		entryList->setHeaderLabels(QStringList() << tr("Group") << tr("Title") << tr("Username"));
	
	QList<QTreeWidgetItem*> itemList;
	AutoTypeEntry autoTypeEntry;
	for (int i=0; i<entries.size(); i++){
		QStringList cols;
		cols << entries[i]->group()->title() << entries[i]->title();
		if (!hideUsernames)
			cols << entries[i]->username();
		
		QTreeWidgetItem* widgetItem = new QTreeWidgetItem(cols);
		itemList.append(widgetItem);
		autoTypeEntry.dbHandle = entries[i];
		autoTypeEntry.nr = numbers[i];
		itemToEntry.insert(widgetItem, autoTypeEntry);
	}
	
	qSort(itemList.begin(), itemList.end(), itemLessThan);
	for (int i=0; i<itemList.size(); i++)
		entryList->addTopLevelItem(itemList[i]);
	entryList->setCurrentItem(itemList[0]);
	
	entryList->resizeColumnToContents(0);
	entryList->resizeColumnToContents(1);
	if (!hideUsernames)
		entryList->resizeColumnToContents(2);
	
	entryList->setColumnWidth(0, entryList->columnWidth(0)+10);
	if (!hideUsernames)
		entryList->setColumnWidth(1, entryList->columnWidth(1)+10);
	
	connect(ButtonBox, SIGNAL(rejected()), SLOT(close()));
	connect(entryList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(itemSelected(QTreeWidgetItem*)));
	connect(entryList, SIGNAL(returnPressed(QTreeWidgetItem*)), SLOT(itemSelected(QTreeWidgetItem*)));
}

bool AutoTypeDlg::itemLessThan(QTreeWidgetItem* i1, QTreeWidgetItem* i2){
	if (i1->text(0) != i2->text(0))
		return i1->text(0) < i2->text(0);
	else if (i1->text(1) != i2->text(1))
		return i1->text(1) < i2->text(1);
	else
		return i1->text(2) < i2->text(2);
}


void AutoTypeDlg::paintEvent(QPaintEvent* event){
	QWidget::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void AutoTypeDlg::resizeEvent(QResizeEvent* event){
	createBanner(&BannerPixmap,getPixmap("keepassx"),tr("Auto-Type"),width());
	QWidget::resizeEvent(event);
}

void AutoTypeDlg::closeEvent(QCloseEvent* event) {
	Q_ASSERT(dialogVisible);
	dialogVisible = false;
	QWidget::closeEvent(event);
}

bool AutoTypeDlg::event(QEvent* event){
	if (!EventOccurred){
		int t = event->type();
		if ( (t>=QEvent::MouseButtonPress && t<=QEvent::KeyRelease) || (t>=QEvent::HoverEnter && t<=QEvent::HoverMove) )
			EventOccurred = true;
	}
	return QWidget::event(event);
}

void AutoTypeDlg::itemSelected(QTreeWidgetItem* item){
	close();
	autoType->perform(itemToEntry[item].dbHandle, pWasLocked, itemToEntry[item].nr, pWasLocked);
}
