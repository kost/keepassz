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

#include <QFileDialog>
#include "SelectIconDlg.h"
#include "PasswordGenDlg.h"
#include "EditEntryDlg.h"
#include "CalendarDlg.h"

#ifdef GLOBAL_AUTOTYPE
#include "TargetWindowDlg.h"
#endif

CEditEntryDlg::CEditEntryDlg(IDatabase* _db, IEntryHandle* _entry,QWidget* parent, bool newEntry)
: QDialog(parent)
{
	Q_ASSERT(_db);
	Q_ASSERT(_entry);
	entry=_entry;
	db=_db;
	pNewEntry=newEntry;
	setupUi(this);
	ModFlag=false;
	QMenu *ExpirePresetsMenu=new QMenu();
	
	setGeometry( config->dialogGeometry(this) );

	connect(Edit_Title, SIGNAL(textChanged(const QString&)), this, SLOT( OnTitleTextChanged(const QString&)));
	connect(Edit_Password_w, SIGNAL(editingFinished()), this, SLOT(OnPasswordwLostFocus()));
	connect(Edit_Password_w, SIGNAL(textChanged(const QString&)), this, SLOT( OnPasswordwTextChanged()));
	connect(Edit_Password, SIGNAL(textChanged(const QString&)), this, SLOT( OnPasswordTextChanged()));
	connect(ButtonEchoMode, SIGNAL(clicked()), this, SLOT( ChangeEchoMode()));
	connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT( OnButtonCancel()));
	connect(ButtonOpenAttachment, SIGNAL(clicked()), this, SLOT( OnNewAttachment()));
	connect(ButtonDeleteAttachment, SIGNAL(clicked()), this, SLOT( OnDeleteAttachment()));
	connect(ButtonSaveAttachment, SIGNAL(clicked()), this, SLOT( OnSaveAttachment()));
	connect(ButtonGenPw, SIGNAL(clicked()), this, SLOT( OnButtonGenPw()));
	connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),this,SLOT(OnButtonOK()));
	connect(CheckBox_ExpiresNever,SIGNAL(stateChanged(int)),this,SLOT(OnCheckBoxExpiresNeverChanged(int)));
	connect(Button_Icons,SIGNAL(clicked()),this,SLOT(OnButtonIcons()));
	connect(ExpirePresetsMenu,SIGNAL(triggered(QAction*)),this,SLOT(OnExpirePreset(QAction*)));
	connect(ButtonExpirePresets,SIGNAL(triggered(QAction*)),this,SLOT(OnCalendar(QAction*)));
	connect(this, SIGNAL(finished(int)), this, SLOT(OnClose()));

	// QAction::data() contains the time until expiration in days.
	ExpirePresetsMenu->addAction(tr("Today"))->setData(0);
	ExpirePresetsMenu->addSeparator();
	ExpirePresetsMenu->addAction(tr("1 Week"))->setData(7);
	ExpirePresetsMenu->addAction(tr("2 Weeks"))->setData(14);
	ExpirePresetsMenu->addAction(tr("3 Weeks"))->setData(21);
	ExpirePresetsMenu->addSeparator();
	ExpirePresetsMenu->addAction(tr("1 Month"))->setData(30);
	ExpirePresetsMenu->addAction(tr("3 Months"))->setData(90);
	ExpirePresetsMenu->addAction(tr("6 Months"))->setData(180);
	ExpirePresetsMenu->addSeparator();
	ExpirePresetsMenu->addAction(tr("1 Year"))->setData(365);
	ButtonExpirePresets->setMenu(ExpirePresetsMenu);
	QAction* actionCalendar = new QAction(tr("Calendar..."),ButtonExpirePresets);
	actionCalendar->setData(-1);
	ButtonExpirePresets->setDefaultAction(actionCalendar);

	IconIndex = entry->image();
	Button_Icons->setIcon(db->icon(IconIndex));

	ButtonOpenAttachment->setIcon(getIcon("fileopen"));
	ButtonDeleteAttachment->setIcon(getIcon("filedelete"));
	ButtonSaveAttachment->setIcon(getIcon("filesave"));
	ButtonExpirePresets->setIcon(getIcon("clock"));

    OnTitleTextChanged(entry->title());
	setWindowIcon(db->icon(IconIndex));
	Edit_Title->setText(entry->title());
	Edit_UserName->setText(entry->username());
	Edit_URL->setText(entry->url());
	SecString Password=entry->password();
	Password.unlock();
	Edit_Password->setText(Password.string());
	Edit_Password_w->setText(Password.string());
	Password.lock();
	if(!config->showPasswords())
		ChangeEchoMode();
    else
        ButtonEchoMode->setIcon(getIcon("pwd_show"));

    // MX-COMMENT: This call is not needed. Both Passwords fields will always have the same value
    OnPasswordwLostFocus();
	int bits=(Password.length()*8);
	Label_Bits->setText(tr("%1 Bit").arg(QString::number(bits)));
	if(bits>128)
		bits=128;
	Progress_Quali->setValue(100*bits/128);
	Edit_Attachment->setText(entry->binaryDesc());
	Edit_Comment->setPlainText(entry->comment());
	InitGroupComboBox();

	if(!entry->binarySize()){
		ButtonSaveAttachment->setDisabled(true);
		ButtonDeleteAttachment->setDisabled(true);
		Label_AttachmentSize->setText("");
	}
	else
        Label_AttachmentSize->setText(entry->friendlySize());

	if(entry->expire()==Date_Never){
		DateTime_Expire->setDisabled(true);
		CheckBox_ExpiresNever->setChecked(true);
	}
	else{
		DateTime_Expire->setDateTime(entry->expire());
	}
	
#ifdef AUTOTYPE
	QToolButton* buttonTools = new QToolButton(buttonBox);
	buttonTools->setText("Tools");
	buttonTools->setPopupMode(QToolButton::InstantPopup);
	QMenu* menuTools = new QMenu(buttonTools);
	connect(menuTools->addAction("Auto-Type: Customize Sequence"), SIGNAL(triggered(bool)), SLOT(OnCustomizeSequence()));
#ifdef GLOBAL_AUTOTYPE
	connect(menuTools->addAction("Auto-Type: Select target window"), SIGNAL(triggered(bool)), SLOT(OnSelectTarget()));
#endif
	buttonTools->setMenu(menuTools);
	buttonBox->addButton(buttonTools, QDialogButtonBox::ResetRole); // ResetRole: workaround to display button on the left
#endif
}

CEditEntryDlg::~CEditEntryDlg(){
}

void CEditEntryDlg::resizeEvent(QResizeEvent *event){
	createBanner(&BannerPixmap,getPixmap("keepassx"),pNewEntry?tr("New Entry"):tr("Edit Entry"),width());
	QDialog::resizeEvent(event);
}



void CEditEntryDlg::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void CEditEntryDlg::InitGroupComboBox(){
	QString Space;
	groups=db->sortedGroups();
	for(int i=0;i<groups.size();i++){
		Space.fill(' ', 2 * (groups[i]->level()));
		Combo_Group->insertItem(i,db->icon(groups[i]->image()),Space+groups[i]->title());
		if(groups[i]==entry->group()){
			Combo_Group->setCurrentIndex(i);
			GroupIndex=i;
		}
	}
}

void CEditEntryDlg::OnButtonOK()
{
	bool EntryMoved=false;
	if(QString::compare(Edit_Password->text(),Edit_Password_w->text())!=0){
		QMessageBox::warning(NULL,tr("Warning"),tr("Password and password repetition are not equal.\nPlease check your input."),tr("OK"));
		return;
	}

	if(CheckBox_ExpiresNever->checkState()==Qt::Checked){
		DateTime_Expire->setDateTime(Date_Never);
	}

	if(DateTime_Expire->dateTime()!=entry->expire())
		ModFlag=true;
	if(entry->title()!=Edit_Title->text())
		ModFlag=true;
	if(entry->username()!=Edit_UserName->text())
		ModFlag=true;
	if(entry->url()!=Edit_URL->text())
		ModFlag=true;
	if(entry->comment()!=Edit_Comment->toPlainText())
		ModFlag=true;
	SecString pw=entry->password();
	pw.unlock();
	QString password=pw.string();
	if(password!=Edit_Password->text())
		ModFlag=true;
	pw.lock();
	password.fill('X');
	if(entry->image()!=IconIndex)
		ModFlag=true;

	if(ModFlag){
		QDateTime now = QDateTime::currentDateTime();
		entry->setExpire(DateTime_Expire->dateTime());
		entry->setLastAccess(now);
		entry->setLastMod(now);
		entry->setTitle(Edit_Title->text());
		entry->setUsername(Edit_UserName->text());
		entry->setUrl(Edit_URL->text());
		SecString pw;
		QString password=Edit_Password->text();
		pw.setString(password,true);
		entry->setPassword(pw);
		entry->setComment(Edit_Comment->toPlainText());
		entry->setImage(IconIndex);
	}
	if(Combo_Group->currentIndex()!=GroupIndex){
		db->moveEntry(entry,groups[Combo_Group->currentIndex()]);
		EntryMoved=true;
	}

	if(EntryMoved)
		done(ModFlag?2:3);
	else if (ModFlag)
		done(1);
	else
		done(0);
}

void CEditEntryDlg::OnButtonCancel()
{
	entry->setLastAccess(QDateTime::currentDateTime());
	done(0);
}

void CEditEntryDlg::ChangeEchoMode()
{
	if(Edit_Password->echoMode()==QLineEdit::Normal){
		Edit_Password->setEchoMode(QLineEdit::Password);
		Edit_Password_w->setEchoMode(QLineEdit::Password);
		ButtonEchoMode->setIcon(getIcon("pwd_hide"));
	}
	else
	{
		Edit_Password->setEchoMode(QLineEdit::Normal);
		Edit_Password_w->setEchoMode(QLineEdit::Normal);
		ButtonEchoMode->setIcon(getIcon("pwd_show"));
	}
}

void CEditEntryDlg::OnTitleTextChanged(const QString& txt)
{
	setWindowTitle((txt=="") ? tr("[Untitled Entry]") : txt);
}

void CEditEntryDlg::OnPasswordTextChanged()
{
	Edit_Password_w->setText(QString());
	int bits=(Edit_Password->text().length()*8);
	Label_Bits->setText(QString::number(bits)+" Bit");
	if(bits>128)bits=128;
	Progress_Quali->setValue(100*bits/128);
}

void CEditEntryDlg::OnPasswordwTextChanged()
{
	if(QString::compare(Edit_Password_w->text(),Edit_Password->text().mid(0,(Edit_Password_w->text().length())))!=0){
		QPalette palette;
		palette.setColor(Edit_Password_w->backgroundRole(),QColor(255,125,125));
		Edit_Password_w->setPalette(palette);
	}
	else {
		Edit_Password_w->setPalette(QApplication::palette());
	}
}

void CEditEntryDlg::OnPasswordwLostFocus()
{
	if (QString::compare(Edit_Password_w->text(),Edit_Password->text())!=0){
		QPalette palette;
		palette.setColor(Edit_Password_w->backgroundRole(),QColor(255,125,125));
		Edit_Password_w->setPalette(palette);
	}
	else {
		Edit_Password_w->setPalette(QApplication::palette ());
	}
}

void CEditEntryDlg::OnNewAttachment()
{
	QString filename=QFileDialog::getOpenFileName(this,tr("Add Attachment..."),QDir::homePath());
	if(filename=="")return;
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly)){
		file.close();
		QMessageBox::warning(NULL,tr("Error"),tr("Could not open file."),tr("OK"));
		return;
	}
	ModFlag=true;
	entry->setBinary(file.readAll());
	file.close();
	QFileInfo info(filename);
	entry->setBinaryDesc(info.fileName());
	Edit_Attachment->setText(entry->binaryDesc());
	Label_AttachmentSize->setText(entry->friendlySize());
	ButtonOpenAttachment->setEnabled(true);
	ButtonSaveAttachment->setEnabled(true);
	ButtonDeleteAttachment->setEnabled(true);
}

void CEditEntryDlg::OnSaveAttachment(){
	saveAttachment(entry,this);
}

void CEditEntryDlg::saveAttachment(IEntryHandle* pEntry, QWidget* ParentWidget)
{
	if(!pEntry->binarySize()){
		QMessageBox::information(NULL,tr("Error"),tr("The chosen entry has no attachment or it is empty."),tr("OK"));
		return;
	}
	QFileDialog FileDlg(ParentWidget,tr("Save Attachment..."),QDir::homePath());
	FileDlg.selectFile(pEntry->binaryDesc());
	FileDlg.setAcceptMode(QFileDialog::AcceptSave);
	if(!FileDlg.exec())return;
	QString filename=FileDlg.selectedFiles()[0];
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
		QMessageBox::critical(NULL,tr("Error"),tr("Could not open file."),tr("OK"));
		return;
	}

	int r=file.write(pEntry->binary());
	if(r==-1){
		file.close();
		QMessageBox::critical(NULL,tr("Error"),tr("Error while writing the file."),tr("OK"));
		return;
	}
	if(r!=pEntry->binarySize()){
		file.close();
		QMessageBox::critical(NULL,tr("Error"),tr("Error while writing the file."),tr("OK"));
		return;
	}
	file.close();
}

void CEditEntryDlg::OnDeleteAttachment()
{
	int r=QMessageBox::warning(this,tr("Delete Attachment?"),
			tr("You are about to delete the attachment of this entry.\nAre you sure?"),
			tr("Yes"),tr("No, Cancel"),NULL,1,1);
	if(r==0){
		ModFlag=true;
		entry->setBinary(QByteArray());
		entry->setBinaryDesc("");
		Edit_Attachment->setText("");
		Label_AttachmentSize->setText("");
		ButtonOpenAttachment->setEnabled(true);
		ButtonSaveAttachment->setDisabled(true);
		ButtonDeleteAttachment->setDisabled(true);
	}
}

void CEditEntryDlg::OnButtonGenPw()
{
	CGenPwDialog dlg(this,false);
	if(dlg.exec()){
		Edit_Password->setText(dlg.Edit_dest->text());
		Edit_Password_w->setText(dlg.Edit_dest->text());
		ModFlag=true;
	}
}


void CEditEntryDlg::OnCheckBoxExpiresNeverChanged(int state){
	if(state==Qt::Unchecked)
		DateTime_Expire->setDisabled(false);
	else
		DateTime_Expire->setDisabled(true);
}

void CEditEntryDlg::OnButtonIcons(){
	// CSelectIconDlg dlg(db,Combo_IconPicker->currentIndex(),this);
	CSelectIconDlg dlg(db, IconIndex, this);
	int r=dlg.exec();
	if (r!=-1)
	{
		IconIndex=r;
		Button_Icons->setIcon(db->icon(IconIndex));
		setWindowIcon(db->icon(IconIndex));
	}
}

void CEditEntryDlg::OnExpirePreset(QAction* action){
	CheckBox_ExpiresNever->setChecked(false);
	int days = action->data().toInt();
	switch (days){
		case 30:
		case 90:
		case 180:
			DateTime_Expire->setDate(QDate::currentDate().addMonths(days/30));
			break;
		case 365:
			DateTime_Expire->setDate(QDate::currentDate().addYears(1));
			break;
		default:
			DateTime_Expire->setDate(QDate::currentDate().addDays(days));
	}
	DateTime_Expire->setTime(QTime(0,0,0));
}

void CEditEntryDlg::OnCalendar(QAction* action){
	if (action->data().toInt()!=-1)
		return;
	
	CalendarDialog dlg(this);
	if(dlg.exec()==QDialog::Accepted){
		CheckBox_ExpiresNever->setChecked(false);
		DateTime_Expire->setDate(dlg.calendarWidget->selectedDate());
		DateTime_Expire->setTime(QTime(0,0,0));
	}
}

void CEditEntryDlg::OnClose(){
	config->setDialogGeometry(this);
}


void CEditEntryDlg::OnCustomizeSequence(){
#ifdef AUTOTYPE
	QString text = Edit_Comment->toPlainText();
	if (!text.isEmpty())
		text.append("\n");
	Edit_Comment->setPlainText(text.append("Auto-Type: {USERNAME}{TAB}{PASSWORD}{ENTER}"));
#endif
}

void CEditEntryDlg::OnSelectTarget(){
#ifdef GLOBAL_AUTOTYPE
	TargetWindowDlg dlg(this);
	if (dlg.exec() && !dlg.windowTitle().isEmpty()){
		QString text = Edit_Comment->toPlainText();
		if (!text.isEmpty())
			text.append("\n");
		Edit_Comment->setPlainText(text.append("Auto-Type-Window: "+dlg.windowTitle()));
	}
#endif
}
