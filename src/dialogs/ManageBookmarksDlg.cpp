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


#include "dialogs/ManageBookmarksDlg.h"
#include "dialogs/AddBookmarkDlg.h"

ManageBookmarksDlg::ManageBookmarksDlg(QWidget* parent):QDialog(parent)
{
	setupUi(this);

	for(int i=0;i<KpxBookmarks::count();i++){
		QListWidgetItem* item=new QListWidgetItem(ListWidget);
		item->setData(Qt::UserRole,i);
		item->setText(KpxBookmarks::title(i));
	}
	connect(Button_Add,SIGNAL(clicked()),this,SLOT(OnButtonAdd()));
	connect(Button_Edit,SIGNAL(clicked()),this,SLOT(OnButtonEdit()));
	connect(Button_Delete,SIGNAL(clicked()),this,SLOT(OnButtonDelete()));
	connect(Button_Up,SIGNAL(clicked()),this,SLOT(OnButtonUp()));
	connect(Button_Down,SIGNAL(clicked()),this,SLOT(OnButtonDown()));
	connect(ListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(edit(QListWidgetItem*)));
	connect(buttonBox->button(QDialogButtonBox::Close),SIGNAL(clicked()),this,SLOT(close()));

	Button_Add->setIcon(getIcon("bookmark_add"));
	Button_Edit->setIcon(getIcon("bookmark_edit"));
	Button_Delete->setIcon(getIcon("bookmark_del"));
	Button_Up->setIcon(getIcon("up"));
	Button_Down->setIcon(getIcon("down"));
}

void ManageBookmarksDlg::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void ManageBookmarksDlg::resizeEvent(QResizeEvent* event){
	createBanner(&BannerPixmap,getPixmap("bookmark"),tr("Manage Bookmarks"),width());
	QDialog::resizeEvent(event);
}

void ManageBookmarksDlg::OnButtonAdd(){
	AddBookmarkDlg dlg(this);
	if(dlg.exec()){
		int i=dlg.ItemID;
		QListWidgetItem* item=new QListWidgetItem(ListWidget);
		item->setData(Qt::UserRole,i);
		item->setText(KpxBookmarks::title(i));
	}
	return;
}

void ManageBookmarksDlg::OnButtonEdit(){
	QListWidgetItem* item=ListWidget->currentItem();
	if(!item)return;
	edit(item);
}

void ManageBookmarksDlg::OnButtonDelete(){
	QListWidgetItem* item=ListWidget->currentItem();
	if(!item)return;
	int index=item->data(Qt::UserRole).toInt();
	KpxBookmarks::remove(index);
	delete item;
	for(int i=0;i<ListWidget->count();i++){
		int itemindex=ListWidget->item(i)->data(Qt::UserRole).toInt();
		if(itemindex>index)
		ListWidget->item(i)->setData(Qt::UserRole,itemindex-1);
	}
}


void ManageBookmarksDlg::OnButtonUp(){
	int row=ListWidget->currentRow();
	QListWidgetItem* item=ListWidget->currentItem();
	if(row==-1 || !item || row==0)return;
	ListWidget->takeItem(row);
	row--;
	ListWidget->insertItem(row,item);
	ListWidget->setCurrentRow(row);
}

void ManageBookmarksDlg::OnButtonDown(){
	int row=ListWidget->currentRow();
	QListWidgetItem* item=ListWidget->currentItem();
	if(row==-1 || !item || row==ListWidget->count()-1)return;
	ListWidget->takeItem(row);
	row++;
	ListWidget->insertItem(row,item);
	ListWidget->setCurrentRow(row);
}



void ManageBookmarksDlg::edit(QListWidgetItem* item){
	int i=item->data(Qt::UserRole).toInt();
	AddBookmarkDlg dlg(this,QString(),i);
	dlg.exec();
	item->setText(KpxBookmarks::title(i));
}



void ManageBookmarksDlg::closeEvent(QCloseEvent * event){
	QList<int> Order;
	// Creating a list with the new indices
	// Order[OldIndex]==NewIndex
	for(int i=0;i<KpxBookmarks::count();i++){
		Order<<ListWidget->item(i)->data(Qt::UserRole).toInt();
	}
	KpxBookmarks::resort(Order);
	event->accept();
}
