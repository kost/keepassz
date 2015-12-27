/***************************************************************************
 *   Copyright (C) 2005-2007 by Tarek Saidi                                *
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
  
#include <QColorDialog>
#include "CustomizeDetailViewDlg.h"

bool DisableButtonSlots=false;

CustomizeDetailViewDialog::CustomizeDetailViewDialog(QWidget* parent):QDialog(parent){
	setupUi(this);
	BtnBold->setIcon(getIcon("text_bold"));
	BtnItalic->setIcon(getIcon("text_italic"));
	BtnUnderline->setIcon(getIcon("text_under"));
	BtnAlignLeft->setIcon(getIcon("text_left"));
	BtnAlignRight->setIcon(getIcon("text_right"));
	BtnAlignCenter->setIcon(getIcon("text_center"));
	BtnAlignBlock->setIcon(getIcon("text_block"));
	BtnTemplates->setIcon(getIcon("templates"));	
	
	QMenu* tmplmenu=new QMenu();
	tmplmenu->addAction(tr("Group"))->setData("%group%");
	tmplmenu->addAction(tr("Title"))->setData("%title%");
	tmplmenu->addAction(tr("Username"))->setData("%username%");
	tmplmenu->addAction(tr("Password"))->setData("%password%");
	tmplmenu->addAction(tr("Url"))->setData("%url%");
	tmplmenu->addAction(tr("Comment"))->setData("%comment%");
	tmplmenu->addAction(tr("Attachment Name"))->setData("%attachment%");
	tmplmenu->addAction(tr("Creation Date"))->setData("%creation%");
	tmplmenu->addAction(tr("Last Access Date"))->setData("%lastaccess%");
	tmplmenu->addAction(tr("Last Modification Date"))->setData("%lastmod%");
	tmplmenu->addAction(tr("Expiration Date"))->setData("%expire%");
	tmplmenu->addAction(tr("Time till Expiration"))->setData("%expire-timeleft%");
	BtnTemplates->setMenu(tmplmenu);	
	
	connect(BtnBold,SIGNAL(toggled(bool)),this,SLOT(OnBtnBold(bool)));
	connect(BtnItalic,SIGNAL(toggled(bool)),this,SLOT(OnBtnItalic(bool)));
	connect(BtnUnderline,SIGNAL(toggled(bool)),this,SLOT(OnBtnUnderline(bool)));
	connect(BtnAlignLeft,SIGNAL(toggled(bool)),this,SLOT(OnBtnAlignLeft()));
	connect(BtnAlignRight,SIGNAL(toggled(bool)),this,SLOT(OnBtnAlignRight()));
	connect(BtnAlignCenter,SIGNAL(toggled(bool)),this,SLOT(OnBtnAlignCenter()));
	connect(BtnAlignBlock,SIGNAL(toggled(bool)),this,SLOT(OnBtnAlignBlock()));
	connect(BtnColor,SIGNAL(clicked()),this,SLOT(OnBtnColor()));
	connect(ButtonBox,SIGNAL(accepted()),this,SLOT(OnSave()));
	connect(ButtonBox,SIGNAL(rejected()),this,SLOT(OnCancel()));
	connect(ButtonBox,SIGNAL(clicked(QAbstractButton*)),this,SLOT(OnRestoreDefault(QAbstractButton*)));
	connect(tmplmenu,SIGNAL(triggered(QAction*)),this,SLOT(OnInsertTemplate(QAction*)));
	connect(RichEdit,SIGNAL(cursorPositionChanged()),this,SLOT(OnCursorPositionChanged()));
	connect(TabWidget,SIGNAL(currentChanged(int)),this,SLOT(OnTabChanged(int)));
	connect(FontSize,SIGNAL(activated(const QString&)),this,SLOT(OnFontSizeChanged(const QString&)));
	connect(FontSize->lineEdit(),SIGNAL(returnPressed()),this,SLOT(OnFontSizeChanged()));
	
	RichEdit->setHtml(DetailViewTemplate);
	
	OnCursorPositionChanged();
}

void CustomizeDetailViewDialog::OnTabChanged(int index){
	
	if(index==0){
		RichEdit->setHtml(HtmlEdit->toPlainText());
	}	
	if(index==1){
		HtmlEdit->setPlainText(RichEdit->toHtml());
	}	
}

void CustomizeDetailViewDialog::OnFontSizeChanged(const QString& text){
	bool ok=false;
	int size=text.toInt(&ok);
	if(ok && size > 0){
		RichEdit->setFontPointSize(size);
	}
}


void CustomizeDetailViewDialog::OnCursorPositionChanged(){
	DisableButtonSlots=true;
	if(RichEdit->fontWeight()==QFont::Bold)BtnBold->setChecked(true);
	else BtnBold->setChecked(false);
	BtnItalic->setChecked(RichEdit->fontItalic());
	BtnUnderline->setChecked(RichEdit->fontUnderline());
	switch(RichEdit->alignment()){
		case Qt::AlignLeft:
			BtnAlignLeft->setChecked(true);
			BtnAlignCenter->setChecked(false);
			BtnAlignRight->setChecked(false);
			BtnAlignBlock->setChecked(false);
			break;
		case Qt::AlignHCenter:
			BtnAlignLeft->setChecked(false);
			BtnAlignCenter->setChecked(true);
			BtnAlignRight->setChecked(false);
			BtnAlignBlock->setChecked(false);
			break;
		case Qt::AlignRight:
			BtnAlignLeft->setChecked(false);
			BtnAlignCenter->setChecked(false);
			BtnAlignRight->setChecked(true);
			BtnAlignBlock->setChecked(false);
			break;
		case Qt::AlignJustify:
			BtnAlignLeft->setChecked(false);
			BtnAlignCenter->setChecked(false);
			BtnAlignRight->setChecked(false);
			BtnAlignBlock->setChecked(true);
			break;			
	}
	CurrentColor=RichEdit->textColor();
	QPixmap pixmap=QPixmap(16,16);
	pixmap.fill(CurrentColor);
	BtnColor->setIcon(QIcon(pixmap));
	if(RichEdit->fontPointSize()>0)
		FontSize->lineEdit()->setText(QString::number((int)RichEdit->fontPointSize()));
	else
		FontSize->lineEdit()->setText("9");
								 
	DisableButtonSlots=false;	
}

void CustomizeDetailViewDialog::OnBtnBold(bool toggled){
	if(DisableButtonSlots)return;
	if(toggled)
			RichEdit->setFontWeight(QFont::Bold);		
	else
			RichEdit->setFontWeight(QFont::Normal);
}

void CustomizeDetailViewDialog::OnBtnItalic(bool toggled){
	if(DisableButtonSlots)return;
	RichEdit->setFontItalic(toggled);		
}

void CustomizeDetailViewDialog::OnBtnUnderline(bool toggled){
	if(DisableButtonSlots)return;	
	RichEdit->setFontUnderline(toggled);		
}


void CustomizeDetailViewDialog::OnBtnAlignLeft(){
	if(DisableButtonSlots)return;
	RichEdit->setAlignment(Qt::AlignLeft);
	OnCursorPositionChanged();
}

void CustomizeDetailViewDialog::OnBtnAlignRight(){
	if(DisableButtonSlots)return;
	RichEdit->setAlignment(Qt::AlignRight);	
	OnCursorPositionChanged();
}

void CustomizeDetailViewDialog::OnBtnAlignCenter(){
	if(DisableButtonSlots)return;	
	RichEdit->setAlignment(Qt::AlignHCenter);	
	OnCursorPositionChanged();
}

void CustomizeDetailViewDialog::OnBtnAlignBlock(){
	if(DisableButtonSlots)return;	
	RichEdit->setAlignment(Qt::AlignJustify);	
	OnCursorPositionChanged();
}

void CustomizeDetailViewDialog::OnBtnColor(){
	CurrentColor=QColorDialog::getColor(CurrentColor,this);
	QPixmap pixmap=QPixmap(16,16);
	pixmap.fill(CurrentColor);
	BtnColor->setIcon(QIcon(pixmap));
	RichEdit->setTextColor(CurrentColor);
}

void CustomizeDetailViewDialog::OnInsertTemplate(QAction* action){
	RichEdit->insertPlainText(action->data().toString());	
}


void CustomizeDetailViewDialog::OnSave(){
	if(TabWidget->currentIndex()==0)
		DetailViewTemplate=RichEdit->toHtml();
	else if(TabWidget->currentIndex()==1)
		DetailViewTemplate=HtmlEdit->toPlainText();
	
	config->setDetailViewTemplate(DetailViewTemplate);
	
	done(1);
}

void CustomizeDetailViewDialog::OnCancel(){
	done(0);
}

void CustomizeDetailViewDialog::OnRestoreDefault(QAbstractButton* button){
	if (button==ButtonBox->button(QDialogButtonBox::RestoreDefaults)){
		DetailViewTemplate = config->defaultDetailViewTemplate();
		HtmlEdit->setPlainText(DetailViewTemplate);
		RichEdit->setHtml(DetailViewTemplate);
	}
}
