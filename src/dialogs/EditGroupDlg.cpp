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


#include "EditGroupDlg.h"
#include "SelectIconDlg.h"

CEditGroupDialog::CEditGroupDialog(IDatabase* database,IGroupHandle* Handle,QWidget* parent, Qt::WFlags fl)
: QDialog(parent,fl)
{
	db = database;
	handle = Handle;
	group = new CGroup();
	group->Title = handle->title();
	group->Image = handle->image();
	
	init();
}


CEditGroupDialog::CEditGroupDialog(IDatabase* database,CGroup* Group,QWidget* parent, Qt::WFlags fl)
	: QDialog(parent,fl)
{
	db = database;
	handle = NULL;
	group = Group;
	
	init();
}

CEditGroupDialog::~CEditGroupDialog(){
}

void CEditGroupDialog::init() {
	setupUi(this);
	
	connect( ButtonBox, SIGNAL( accepted() ), this, SLOT( OnOK() ) );
	connect( ButtonBox, SIGNAL( rejected() ), this, SLOT( OnCancel() ) );
	connect( Button_Icon, SIGNAL( clicked() ), this, SLOT( OnIconDlg() ));
	
	EditTitle->setText(group->Title);
	
	for(int i=0;i<db->numIcons();i++){
		ComboIconPicker->insertItem(i,db->icon(i),"");
	}
	ComboIconPicker->setCurrentIndex(group->Image);
	
	adjustSize();
	setMaximumSize(size());
	setMinimumSize(size());
}


void CEditGroupDialog::OnOK()
{
	int r=1;
	if(EditTitle->text()!=group->Title){
		group->Title=EditTitle->text();
		r=2;
	}
	if(ComboIconPicker->currentIndex()!=group->Image){
		group->Image=ComboIconPicker->currentIndex();
		r=2;
	}
	if(handle){
		handle->setTitle(group->Title);
		handle->setImage(group->Image);
	}
	done(r);
}

void CEditGroupDialog::OnCancel()
{
	done(0);
}


void CEditGroupDialog::OnIconDlg(){
	CSelectIconDlg dlg(db,group->Image,this);
	int r=dlg.exec();
	if(r!=-1){
		ComboIconPicker->clear();
		for(int i=0;i<db->numIcons();i++)
			ComboIconPicker->insertItem(i,db->icon(i),"");
		group->Image=r;
		ComboIconPicker->setCurrentIndex(r);
	}
}
