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


#include "ExpiredEntriesDlg.h"

ExpiredEntriesDialog::ExpiredEntriesDialog(QWidget* parent,IDatabase* database,const QList<IEntryHandle*>& ExpiredEntries):QDialog(parent){
	setupUi(this);
	Entries=ExpiredEntries;
	for(int i=0;i<Entries.size();i++){
		QTreeWidgetItem* item=new QTreeWidgetItem(treeWidget);
		item->setData(0,Qt::UserRole,i);
		item->setText(0,Entries[i]->group()->title());
		item->setText(1,Entries[i]->title());
		item->setText(2,Entries[i]->username());
		item->setText(3,Entries[i]->expire().dateToString(Qt::SystemLocaleDate));
		item->setIcon(0,database->icon(Entries[i]->group()->image()));
		item->setIcon(1,database->icon(Entries[i]->image()));

	}
	connect(treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(OnItemDoubleClicked(QTreeWidgetItem*)));
}


void ExpiredEntriesDialog::paintEvent(QPaintEvent* event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void ExpiredEntriesDialog::resizeEvent(QResizeEvent* event){
	createBanner(&BannerPixmap,getPixmap("alarmclock"),tr("Expired Entries in the Database"),width());
	QDialog::resizeEvent(event);
}

void ExpiredEntriesDialog::OnItemDoubleClicked(QTreeWidgetItem* item){
	SelectedEntry=Entries[item->data(0,Qt::UserRole).toInt()];
	accept();
}

///TODO 0.2.3 locale aware string/date compare for correct sorting
