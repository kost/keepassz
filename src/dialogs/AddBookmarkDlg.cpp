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


#include "AddBookmarkDlg.h"


AddBookmarkDlg::AddBookmarkDlg(QWidget* parent, QString DefaultFilename, int _ItemID):QDialog(parent)
{
	setupUi(this);
	ItemID=_ItemID;
	connect(Button_Browse,SIGNAL(clicked()),this,SLOT(OnButtonBrowse()));
	connect(buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked()),this,SLOT(OnButtonOk()));
	connect(buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),this,SLOT(reject()));
	if(ItemID==-1){
		createBanner(&BannerPixmap,getPixmap("bookmark_add"),tr("Add Bookmark"),width());

		if(DefaultFilename.isEmpty())
			QMetaObject::invokeMethod(this, "OnButtonBrowse", Qt::QueuedConnection);
		else
			Edit_Filename->setText(DefaultFilename);
	}
	else {
		createBanner(&BannerPixmap,getPixmap("bookmark_edit"),tr("Edit Bookmark"),width());

		Edit_Title->setText(KpxBookmarks::title(ItemID));
		Edit_Filename->setText(KpxBookmarks::path(ItemID));
		setWindowTitle(tr("Edit Bookmark"));
	}
}

void AddBookmarkDlg::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void AddBookmarkDlg::OnButtonBrowse(){
	QString path=KpxFileDialogs::openExistingFile(this,"AddBookmarkDlg", tr("Add Bookmark"),
									              QStringList() << tr("KeePass Databases (*.kdb)") << tr("All Files (*)"));
	if(path!=QString())
		Edit_Filename->setText(path);
}

void AddBookmarkDlg::OnButtonOk(){
	if(ItemID==-1)
		ItemID=KpxBookmarks::add(Edit_Title->text(),Edit_Filename->text());
	else
		KpxBookmarks::edit(Edit_Title->text(),Edit_Filename->text(),ItemID);
	accept();
}
