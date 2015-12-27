/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   mail@tarek-saidi.de                                                   *
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


#include "dialogs/SearchDlg.h"


SearchDialog::SearchDialog(IDatabase* database,IGroupHandle* Group,QWidget* parent):QDialog(parent)
{
	setupUi(this);
	QPushButton* Button_Search = ButtonBox->addButton(tr("Search"),QDialogButtonBox::ActionRole);
	connect( Button_Search, SIGNAL( clicked() ), this, SLOT( OnSearch() ) );
	connect( ButtonBox, SIGNAL( rejected() ), this, SLOT( OnClose() ) );
	db=database;
	group=Group;
	QBitArray searchOptions=config->searchOptions();
	checkBox_Cs->setChecked(searchOptions.at(0));
	checkBox_regExp->setChecked(searchOptions.at(1));
	checkBox_Title->setChecked(searchOptions.at(2));
	checkBox_Username->setChecked(searchOptions.at(3));
	checkBox_Password->setChecked(searchOptions.at(4));
	checkBox_Comment->setChecked(searchOptions.at(5));
	checkBox_URL->setChecked(searchOptions.at(6));
	checkBox_Attachment->setChecked(searchOptions.at(7));
	if(group)
		checkBox_Recursive->setChecked(searchOptions.at(8));
	else{
		checkBox_Recursive->setChecked(false);
		checkBox_Recursive->setEnabled(false);
	}
	adjustSize();
	setMaximumSize(size());
	setMinimumSize(size());
	createBanner(&BannerPixmap,getPixmap("search"),tr("Search"),width());
}

SearchDialog::~SearchDialog()
{
	QBitArray searchOptions(9);
	searchOptions.setBit(0,checkBox_Cs->isChecked());
	searchOptions.setBit(1,checkBox_regExp->isChecked());
	searchOptions.setBit(2,checkBox_Title->isChecked());
	searchOptions.setBit(3,checkBox_Username->isChecked());
	searchOptions.setBit(4,checkBox_Password->isChecked());
	searchOptions.setBit(5,checkBox_Comment->isChecked());
	searchOptions.setBit(6,checkBox_URL->isChecked());
	searchOptions.setBit(7,checkBox_Attachment->isChecked());
	if(group) searchOptions.setBit(8,checkBox_Recursive->isChecked());
	config->setSearchOptions(searchOptions);
}

void SearchDialog::OnClose()
{
	done(0);
}

void SearchDialog::OnSearch()
{
	bool Fields[6];
	Fields[0]=checkBox_Title->isChecked();
	Fields[1]=checkBox_Username->isChecked();
	Fields[2]=checkBox_URL->isChecked();
	Fields[3]=checkBox_Password->isChecked();
	Fields[4]=checkBox_Comment->isChecked();
	Fields[5]=checkBox_Attachment->isChecked();
	Result=db->search(group,Edit_Search->text(),checkBox_Cs->isChecked(),checkBox_regExp->isChecked(),checkBox_Recursive->isChecked(),Fields);
	done(1);
}

void SearchDialog::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}
