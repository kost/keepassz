/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
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
#include <QColorDialog>
#include "dialogs/SettingsDlg.h"
#include "dialogs/CustomizeDetailViewDlg.h"

//bool CSettingsDlg::PluginsModified=false;


CSettingsDlg::CSettingsDlg(QWidget* parent):QDialog(parent,Qt::Dialog)
{
	setupUi(this);
	connect(listWidget, SIGNAL( currentRowChanged(int) ), stackedWidget, SLOT( setCurrentIndex(int) ) );
	
	connect(DialogButtons, SIGNAL( accepted() ), this, SLOT( OnOK() ) );
	connect(DialogButtons, SIGNAL( rejected() ), this, SLOT( OnCancel() ) );
	connect(DialogButtons, SIGNAL( clicked(QAbstractButton*)), this, SLOT(OnOtherButton(QAbstractButton*)));
	
	connect(CheckBox_ShowSysTrayIcon, SIGNAL( toggled(bool) ), CheckBox_CloseToTray, SLOT( setEnabled(bool) ) );
	connect(CheckBox_ShowSysTrayIcon, SIGNAL( toggled(bool) ), CheckBox_MinimizeTray, SLOT( setEnabled(bool) ) );
	connect(CheckBox_OpenLast, SIGNAL( toggled(bool) ), CheckBox_RememberLastKey, SLOT( setEnabled(bool) ) );
	connect(CheckBox_OpenLast, SIGNAL( toggled(bool) ), CheckBox_StartMinimized, SLOT( setEnabled(bool) ) );
	connect(CheckBox_OpenLast, SIGNAL( toggled(bool) ), CheckBox_StartLocked, SLOT( setEnabled(bool) ) );
	
	connect(listSelectLanguage, SIGNAL( currentRowChanged(int) ), SLOT( OnSelectLanguage(int) ) );
	
	connect(Button_ClearFileDlgHistory, SIGNAL(clicked()), &fileDlgHistory, SLOT(clear()));
	connect(ButtonColor1, SIGNAL( clicked() ), this, SLOT( OnColor1() ) );
	connect(ButtonColor2, SIGNAL( clicked() ), this, SLOT( OnColor2() ) );
	connect(ButtonTextColor, SIGNAL( clicked() ), this, SLOT( OnTextColor() ) );
	connect(Button_MountDirBrowse,SIGNAL(clicked()),this,SLOT(OnMountDirBrowse()));
	connect(Button_BrowserCmdBrowse,SIGNAL(clicked()),this,SLOT(OnBrowserCmdBrowse()));

	//connect(Radio_IntPlugin_None,SIGNAL(toggled(bool)),this,SLOT(OnIntPluginNone()));
	//connect(Radio_IntPlugin_Gnome,SIGNAL(toggled(bool)),this,SLOT(OnIntPluginGnome()));
	//connect(Radio_IntPlugin_Kde,SIGNAL(toggled(bool)),this,SLOT(OnIntPluginKde()));

	connect(Button_CustomizeEntryDetails,SIGNAL(clicked()),this,SLOT(OnCustomizeEntryDetails()));
	connect(CheckBox_InactivityLock, SIGNAL(toggled(bool)), SLOT(OnInactivityLockChange(bool)));
	
	connect(CheckBox_Backup, SIGNAL( toggled(bool) ), CheckBox_BackupDelete, SLOT( setEnabled(bool) ) );
	connect(CheckBox_Backup, SIGNAL( toggled(bool) ), SLOT( OnBackupDeleteChange() ) );
	connect(CheckBox_BackupDelete, SIGNAL( toggled(bool) ), SLOT( OnBackupDeleteChange() ) );
	connect(CheckBox_AutoSave, SIGNAL(toggled(bool)), SLOT(OnAutoSaveToggle(bool)));
	connect(CheckBox_AutoSaveChange, SIGNAL(toggled(bool)), SLOT(OnAutoSaveChangeToggle(bool)));
	
#if !defined(AUTOTYPE)
	Box_AutoType->setVisible(false);
#endif
#if !defined(GLOBAL_AUTOTYPE)
	Label_GlobalShortcut->setVisible(false);
	Edit_GlobalShortcut->setVisible(false);
	CheckBox_EntryTitlesMatch->setVisible(false);
#endif
#ifdef Q_WS_MAC
	CheckBox_AlwaysOnTop->setVisible(false);
#endif
	
#ifdef GLOBAL_AUTOTYPE
	pShortcut = autoType->getShortcut();
	connect(this,SIGNAL(rejected()),SLOT(resetGlobalShortcut()));
#endif
	
	listWidget->setCurrentRow(0);

	//General (1)
	CheckBox_OpenLast->setChecked(config->openLastFile());
	CheckBox_RememberLastKey->setChecked(config->rememberLastKey());
	CheckBox_ShowSysTrayIcon->setChecked(config->showSysTrayIcon());
	CheckBox_CloseToTray->setChecked(config->minimizeToTray());
	CheckBox_MinimizeTray->setChecked(config->minimizeTray());
	CheckBox_StartMinimized->setChecked(config->startMinimized());
	CheckBox_StartLocked->setChecked(config->startLocked());
	checkBox_SaveFileDlgHistory->setChecked(config->saveFileDlgHistory());
	checkBox_AskBeforeDelete->setChecked(config->askBeforeDelete());
	
	//General (2)
	CheckBox_Backup->setChecked(config->backup());
	CheckBox_BackupDelete->setChecked(config->backupDelete());
	SpinBox_BackupDeleteAfter->setValue(config->backupDeleteAfter());
	CheckBox_AutoSave->setChecked(config->autoSave());
	CheckBox_AutoSaveChange->setChecked(config->autoSaveChange());

	switch(config->groupTreeState()){
		case KpxConfig::RestoreLast:
			Radio_GroupTreeRestore->setChecked(true);
			break;
		case KpxConfig::ExpandAll:
			Radio_GroupTreeExpand->setChecked(true);
			break;
		default:
			Radio_GroupTreeDoNothing->setChecked(true);
	}

	//Appearance
	CheckBox_AlwaysOnTop->setChecked(config->alwaysOnTop());
	QPixmap *pxt=new QPixmap(pixmTextColor->width(),pixmTextColor->height());
	pxt->fill(config->bannerTextColor());
	pixmTextColor->clear();
	pixmTextColor->setPixmap(*pxt);

	QPixmap *px1=new QPixmap(pixmColor1->width(),pixmColor1->height());
	px1->fill(config->bannerColor1());
	pixmColor1->clear();
	pixmColor1->setPixmap(*px1);

	QPixmap *px2=new QPixmap(pixmColor2->width(),pixmColor2->height());
	px2->fill(config->bannerColor2());
	pixmColor2->clear();
	pixmColor2->setPixmap(*px2);

	color1=config->bannerColor1();
	color2=config->bannerColor2();
	textcolor=config->bannerTextColor();
	CheckBox_AlternatingRowColors->setChecked(config->alternatingRowColors());
	
	//Language
	translations = getAllTranslations();
	initLanguageList();
	QString currentLang = config->language();
	bool foundCurrent = false;
	for (int i=0; i<translations.size(); i++){
		if (translations[i].nameCode==currentLang){
			listSelectLanguage->setCurrentRow(i+2);
			foundCurrent = true;
		}
	}
	if (!foundCurrent){
		if (currentLang=="en_US")
			listSelectLanguage->setCurrentRow(1);
		else
			listSelectLanguage->setCurrentRow(0);
	}
	
	//Security
	SpinBox_ClipboardTime->setValue(config->clipboardTimeOut());
	CheckBox_ShowPasswords->setChecked(config->showPasswords());
	CheckBox_ShowPasswords_PasswordDlg->setChecked(config->showPasswordsPasswordDlg());
	CheckBox_LockMinimize->setChecked(config->lockOnMinimize());
	CheckBox_InactivityLock->setChecked(config->lockOnInactivity());
	SpinBox_InacitivtyTime->setValue(config->lockAfterSec());
	
	//Features
	stackedWidget->removeWidget(pageFeatures);
	//CheckBox_FeatureBookmarks->setChecked(config->featureBookmarks());


	// TODO Desktop Integration
	stackedWidget->removeWidget(pageDesktop);
	/*if(PluginLoadError==QString())
		Label_IntPlugin_Error->hide();
	else
		Label_IntPlugin_Error->setText(QString("<html><p style='font-weight:600; color:#8b0000;'>%1</p></body></html>")
				.arg(tr("Error: %1")).arg(PluginLoadError));

	switch(config->integrPlugin()){
		case KpxConfig::KDE:
			Radio_IntPlugin_Kde->setChecked(true);
			break;
		case KpxConfig::Gnome:
			Radio_IntPlugin_Gnome->setChecked(true);
			break;
		default:
			Radio_IntPlugin_None->setChecked(true);
	}
	if(!PluginsModified)
		Label_IntPlugin_Info->hide();*/


	//Advanced
	Box_BrowserCmd->setChecked(!config->urlCmdDef());
	Edit_BrowserCmd->setText(config->urlCmd());

	Edit_MountDir->setText(config->mountDir());
	CheckBox_SaveRelativePaths->setChecked(config->saveRelativePaths());
#ifdef AUTOTYPE
	SpinBox_AutoTypePreGap->setValue(config->autoTypePreGap());
	SpinBox_AutoTypeKeyStrokeDelay->setValue(config->autoTypeKeyStrokeDelay());
#endif
#ifdef GLOBAL_AUTOTYPE
	Edit_GlobalShortcut->setShortcut(config->globalShortcut());
	CheckBox_EntryTitlesMatch->setChecked(config->entryTitlesMatch());
#endif
	
	adjustSize();
	resize( size() + QSize(20,20) );
}

CSettingsDlg::~CSettingsDlg(){
}

void CSettingsDlg::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void CSettingsDlg::resizeEvent(QResizeEvent* event){
	createBanner(&BannerPixmap,getPixmap("appsettings"),tr("Settings"),width());
	QDialog::resizeEvent(event);
}

void CSettingsDlg::OnOK()
{
	apply();
	accept();
}

void CSettingsDlg::OnCancel()
{
	reject();
}

void CSettingsDlg::OnOtherButton(QAbstractButton* button){
	if(DialogButtons->buttonRole(button)==QDialogButtonBox::ApplyRole)
		apply();
}

void CSettingsDlg::apply(){
	
	//General (1)
	config->setShowSysTrayIcon(CheckBox_ShowSysTrayIcon->isChecked());
	config->setMinimizeToTray(CheckBox_CloseToTray->isChecked());
	config->setMinimizeTray(CheckBox_MinimizeTray->isChecked());
	config->setStartMinimized(CheckBox_StartMinimized->isChecked());
	config->setStartLocked(CheckBox_StartLocked->isChecked());
	config->setSaveFileDlgHistory(checkBox_SaveFileDlgHistory->isChecked());
	if(Radio_GroupTreeRestore->isChecked())config->setGroupTreeState(KpxConfig::RestoreLast);
	else if(Radio_GroupTreeExpand->isChecked())config->setGroupTreeState(KpxConfig::ExpandAll);
	else config->setGroupTreeState(KpxConfig::DoNothing);
	config->setOpenLastFile(CheckBox_OpenLast->isChecked());
	config->setRememberLastKey(CheckBox_RememberLastKey->isChecked());
	config->setAskBeforeDelete(checkBox_AskBeforeDelete->isChecked());
	
	//General (2)
	config->setBackup(CheckBox_Backup->isChecked());
	config->setBackupDelete(CheckBox_BackupDelete->isChecked());
	config->setBackupDeleteAfter(SpinBox_BackupDeleteAfter->value());
	config->setAutoSave(CheckBox_AutoSave->isChecked());
	config->setAutoSaveChange(CheckBox_AutoSaveChange->isChecked());

	//Appearence
	config->setAlwaysOnTop(CheckBox_AlwaysOnTop->isChecked());
	config->setBannerColor1(color1);
	config->setBannerColor2(color2);
	config->setBannerTextColor(textcolor);
	config->setAlternatingRowColors(CheckBox_AlternatingRowColors->isChecked());

	//Language
	int langIndex = listSelectLanguage->currentRow();
	QString oldLang = config->language();
	if (langIndex==0)
		config->setLanguage("auto");
	else if (langIndex==1)
		config->setLanguage("en_US");
	else
		config->setLanguage(translations[langIndex-2].nameCode);
	if (config->language() != oldLang){
		installTranslator();
		retranslateUi(this);
		initLanguageList();
		listSelectLanguage->setCurrentRow(langIndex);
	}

	//Security
	config->setClipboardTimeOut(SpinBox_ClipboardTime->value());
	config->setShowPasswords(CheckBox_ShowPasswords->isChecked());
	config->setShowPasswordsPasswordDlg(CheckBox_ShowPasswords_PasswordDlg->isChecked());
	config->setLockOnMinimize(CheckBox_LockMinimize->isChecked());
	config->setLockOnInactivity(CheckBox_InactivityLock->isChecked());
	config->setLockAfterSec(SpinBox_InacitivtyTime->value());

	//Features
	//config->setFeatureBookmarks(CheckBox_FeatureBookmarks->isChecked());

	//TODO Desktop Integration
	/*PluginsModified=Label_IntPlugin_Info->isVisible();
	if(Radio_IntPlugin_Kde->isChecked())config->setIntegrPlugin(KpxConfig::KDE);
	else if(Radio_IntPlugin_Gnome->isChecked())config->setIntegrPlugin(KpxConfig::Gnome);
	else config->setIntegrPlugin(KpxConfig::NoIntegr);*/

	//Advanced
	config->setUrlCmdDef(!Box_BrowserCmd->isChecked());
	config->setUrlCmd(Edit_BrowserCmd->text());
	config->setMountDir(Edit_MountDir->text());
	if(!config->mountDir().isEmpty() && config->mountDir().right(1)!="/")
		config->setMountDir(config->mountDir()+"/");
	config->setSaveRelativePaths(CheckBox_SaveRelativePaths->isChecked());
#ifdef AUTOTYPE
	config->setAutoTypePreGap(SpinBox_AutoTypePreGap->value());
	config->setAutoTypeKeyStrokeDelay(SpinBox_AutoTypeKeyStrokeDelay->value());
#endif
#ifdef GLOBAL_AUTOTYPE
	config->setGlobalShortcut(Edit_GlobalShortcut->shortcut());
	config->setEntryTitlesMatch(CheckBox_EntryTitlesMatch->isChecked());
#endif
}

void CSettingsDlg::OnTextColor()
{
	QColor c=QColorDialog::getColor(textcolor,this);
	if(c.isValid()){
	textcolor=c;
	QPixmap *px=new QPixmap(pixmTextColor->width(),pixmTextColor->height());
	px->fill(c);
	pixmTextColor->clear();
	pixmTextColor->setPixmap(*px);
	createBanner(&BannerPixmap,getPixmap("appsettings"),tr("Settings"),width(),color1,color2,textcolor);
	}
}


void CSettingsDlg::OnColor2()
{
	QColor c=QColorDialog::getColor(color2,this);
	if(c.isValid()){
		color2=c;
		QPixmap *px=new QPixmap(pixmColor2->width(),pixmColor2->height());
		px->fill(c);
		pixmColor2->clear();
		pixmColor2->setPixmap(*px);
		createBanner(&BannerPixmap,getPixmap("appsettings"),tr("Settings"),width(),color1,color2,textcolor);
	}
}


void CSettingsDlg::OnColor1()
{
	QColor c=QColorDialog::getColor(color1,this);
	if(c.isValid()){
		color1=c;
		QPixmap *px=new QPixmap(pixmColor1->width(),pixmColor1->height());
		px->fill(c);
		pixmColor1->clear();
		pixmColor1->setPixmap(*px);
		createBanner(&BannerPixmap,getPixmap("appsettings"),tr("Settings"),width(),color1,color2,textcolor);
	}
}

void CSettingsDlg::OnMountDirBrowse(){
	QString dir=QFileDialog::getExistingDirectory(this,tr("Select a directory..."));
	if(!dir.isEmpty()){
		Edit_MountDir->setText(dir);
	}
}

void CSettingsDlg::OnBrowserCmdBrowse(){
	QString filename=QFileDialog::getOpenFileName(this,tr("Select an executable..."));
	if(!filename.isEmpty()){
		Edit_BrowserCmd->setText(filename);
	}
}

/*void CSettingsDlg::OnIntPluginNone(){
	Label_IntPlugin_Info->show();
}

void CSettingsDlg::OnIntPluginGnome(){
	Label_IntPlugin_Info->show();
}

void CSettingsDlg::OnIntPluginKde(){
	Label_IntPlugin_Info->show();
}*/

void CSettingsDlg::OnCustomizeEntryDetails(){
	CustomizeDetailViewDialog dlg(this);
	dlg.exec();
}

void CSettingsDlg::OnInactivityLockChange(bool checked){
	SpinBox_InacitivtyTime->setEnabled(checked);
}

void CSettingsDlg::OnAutoSaveToggle(bool checked){
	CheckBox_AutoSaveChange->setEnabled(!checked);
}

void CSettingsDlg::OnAutoSaveChangeToggle(bool checked){
	CheckBox_AutoSave->setEnabled(!checked);
}

void CSettingsDlg::OnBackupDeleteChange(){
	SpinBox_BackupDeleteAfter->setEnabled(CheckBox_Backup->isChecked() && CheckBox_BackupDelete->isChecked());
}

void CSettingsDlg::OnSelectLanguage(int index){
	if (index == -1)
		return;
	
	if (index==0){
		labelLang->clear();
		labelAuthor->clear();
	}
	else if (index==1){
		labelLang->setText("English (United States)");
		labelAuthor->setText("KeePassX Development Team");
	}
	else{
		if (translations[index-2].nameLong != translations[index-2].nameEnglish)
			labelLang->setText(QString("%1 / %2").arg(translations[index-2].nameLong).arg(translations[index-2].nameEnglish));
		else
			labelLang->setText(translations[index-2].nameEnglish);
		labelAuthor->setText(translations[index-2].author);
	}
}

void CSettingsDlg::initLanguageList() {
	listSelectLanguage->clear(); // completely rebuild the list because of a bug in Qt 4.3
	listSelectLanguage->addItem(tr("System Language"));
	listSelectLanguage->addItem("English (United States)"); // Don't translate this string
	for (int i=0; i<translations.size(); i++)
		listSelectLanguage->addItem(translations[i].nameLong);
}

#ifdef GLOBAL_AUTOTYPE
void CSettingsDlg::resetGlobalShortcut(){
	autoType->unregisterGlobalShortcut();
	autoType->registerGlobalShortcut(pShortcut);
}
#endif
