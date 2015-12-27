/***************************************************************************
 *   Copyright (C) 2005-2008 by Tarek Saidi                                *
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

#include <QTimer>
#include "dialogs/PasswordDlg.h"

PasswordDialog::PasswordDialog(QWidget* parent,DlgMode mode,DlgFlags flags,const QString& filename)
: QDialog(parent)
{
	Q_UNUSED(flags);
	setupUi(this);
	Mode=mode;
	Filename=filename;
	QString BannerTitle;
	if(Mode==Mode_Ask){
		BannerTitle=tr("Enter Master Key");
	}
	else if(Mode==Mode_Set){
		BannerTitle=tr("Set Master Key");
	}
	else if(Mode==Mode_Change){
		BannerTitle=tr("Change Master Key");
	}
	
	if(filename==QString()){
		setWindowTitle(tr("Database Key"));
	}
	else {
		setWindowTitle(filename);
	}
	
	// Add list of subdirs in the mounting dir to the combobox.
	// For example making /media/cd1 and /media/myflashdrive to two entries cd1 and myflashdrive in the combobox
	QDir mountDir(config->mountDir());
	if(mountDir.exists()){
		QStringList Paths;
		Paths=mountDir.entryList(QStringList()<<"*",QDir::Dirs | QDir::NoDotAndDotDot);
		for(int i=0;i<Paths.count();i++)
			Combo_KeyFile->addItem(config->mountDir()+Paths[i]);
	}
	Combo_KeyFile->setEditText(QString());
	
	if(config->rememberLastKey() && Mode!=Mode_Change && Mode!=Mode_Set){
		switch(config->lastKeyType()){
		case PASSWORD:
			Check_Password->setChecked(true);
			Check_KeyFile->setChecked(false);
			Combo_KeyFile->setEditText("");
			Edit_Password->setFocus(Qt::OtherFocusReason);
			break;
			
		case KEYFILE:
			Check_Password->setChecked(false);
			Check_KeyFile->setChecked(true);
			Combo_KeyFile->setEditText(QDir::cleanPath(QDir::current().absoluteFilePath(config->lastKeyLocation())));
			Combo_KeyFile->setFocus(Qt::OtherFocusReason);
			break;

		case BOTH:
			Check_Password->setChecked(true);
			Check_KeyFile->setChecked(true);
			Combo_KeyFile->setEditText(QDir::cleanPath(QDir::current().absoluteFilePath(config->lastKeyLocation())));
			Edit_Password->setFocus(Qt::OtherFocusReason);
			break;
		}
	}
	else{
		Edit_Password->setFocus(Qt::OtherFocusReason);
	}

	if(Mode!=Mode_Set && Mode!=Mode_Change){
		Button_GenKeyFile->hide();
	}
	/*if(flags & Flag_Auto){
		/ *
		QPushButton* Button_Quit = buttonBox->addButton(tr("Quit"),QDialogButtonBox::DestructiveRole);
		connect(Button_Quit,SIGNAL(clicked()),this,SLOT(OnButtonQuit()));
		* /
		if(config->rememberLastKey()){
			switch(config->lastKeyType()){
			case PASSWORD:
				Check_Password->setChecked(true);
				Check_KeyFile->setChecked(false);
				break;
			case KEYFILE:
				Check_Password->setChecked(false);
				Check_KeyFile->setChecked(true);
				Combo_KeyFile->setEditText(config->lastKeyLocation());
				break;
			case BOTH:
				Check_Password->setChecked(true);
				Check_KeyFile->setChecked(true);
				Combo_KeyFile->setEditText(config->lastKeyLocation());
				break;
			}
		}
	}*/
	
	// Setting up the bookmark button
	if(Mode==Mode_Ask && config->featureBookmarks()){
		// Button Color
		QPalette palette=Button_Bookmarks->palette();
		palette.setColor(QPalette::Active,QPalette::Button,config->bannerColor1());
		palette.setColor(QPalette::Inactive,QPalette::Button,config->bannerColor1());
		palette.setColor(QPalette::Active,QPalette::Window,config->bannerColor2());
		palette.setColor(QPalette::Inactive,QPalette::Window,config->bannerColor2());
		Button_Bookmarks->setPalette(palette);
		palette=Label_Bookmark->palette();
		palette.setColor(QPalette::Active,QPalette::WindowText,config->bannerTextColor());
		palette.setColor(QPalette::Inactive,QPalette::WindowText,config->bannerTextColor());
		Label_Bookmark->setPalette(palette);		
		// Create menu and add "last file" menu entry
		QMenu* BookmarkMenu=new QMenu(this);
		QAction* action=new QAction(this);
		action->setData(QString());
		action->setText(tr("Last File"));
		action->setIcon(getIcon("document"));
		BookmarkMenu->addAction(action);
		BookmarkMenu->addSeparator();
		// Adding all existing bookmarks
		for(int i=0;i<KpxBookmarks::count();i++){
			QAction* action=new QAction(this);
			action->setData(KpxBookmarks::path(i));
			action->setText(KpxBookmarks::title(i));
			action->setIcon(getIcon("document"));
			BookmarkMenu->addAction(action);		
		}
		Button_Bookmarks->setMenu(BookmarkMenu);
		connect(BookmarkMenu,SIGNAL(triggered(QAction*)),this,SLOT(OnBookmarkTriggered(QAction*)));
	}
	else {
		Button_Bookmarks->hide();
		Label_Bookmark->hide();
	}
	
	connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL( clicked() ), this, SLOT( OnCancel() ) );
	connect(ButtonChangeEchoMode, SIGNAL( clicked() ), this, SLOT( ChangeEchoModeDatabaseKey() ) );
	connect(Edit_Password, SIGNAL( returnPressed() ), this, SLOT( OnOK() ) );
	connect(Edit_PwRepeat, SIGNAL( returnPressed() ), this, SLOT( OnOK() ) );
	connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL( clicked() ), this, SLOT( OnOK() ) );
	connect(Button_Browse, SIGNAL( clicked() ), this, SLOT( OnButtonBrowse() ) );
	connect(Button_GenKeyFile,SIGNAL(clicked()),this,SLOT(OnGenKeyFile()));
	connect(Check_Password,SIGNAL(stateChanged(int)),this,SLOT(OnCheckBoxesChanged()));
	connect(Check_KeyFile,SIGNAL(stateChanged(int)),this,SLOT(OnCheckBoxesChanged()));
	connect(Button_Back,SIGNAL(clicked()),this,SLOT(OnButtonBack()));
	if(!config->showPasswordsPasswordDlg())
		ChangeEchoModeDatabaseKey();
	else
		ButtonChangeEchoMode->setIcon(getIcon("pwd_show"));
	
	adjustSize();
	setMaximumSize(size());
	setMinimumSize(size());
	createBanner(&BannerPixmap,getPixmap("key"),BannerTitle,width());
	Button_Bookmarks->setIcon(getIcon("bookmark"));
	OnCheckBoxesChanged();
}


void PasswordDialog::OnButtonBrowse()
{
	QString filename=KpxFileDialogs::openExistingFile(this,"PasswordDlg",tr("Select a Key File"),
                                                      QStringList() << tr("All Files (*)")
		                                                            << tr("Key Files (*.key)"));
	if(filename!=QString())
		Combo_KeyFile->setEditText(filename);
}

void PasswordDialog::OnCancel()
{
	done(Exit_Cancel);
}

void PasswordDialog::OnOK(){	
	if(stackedWidget->currentIndex()==1){
		if(Password==Edit_PwRepeat->text()){
			done(Exit_Ok);
		}
		Edit_PwRepeat->clear();
		Edit_PwRepeat->setFocus(Qt::OtherFocusReason);
		Label_Unequal->show();
		QTimer::singleShot(2000,Label_Unequal,SLOT(hide()));
		return;
	}
	
	Password=Edit_Password->text();
	KeyFile=Combo_KeyFile->currentText();
	
	if(!Check_Password->isChecked() && !Check_KeyFile->isChecked()){
		showErrMsg(tr("Please enter a Password or select a key file."),this);
		return;
	}
	
	if(Check_Password->isChecked() && Password.isEmpty()){
		showErrMsg(tr("Please enter a Password."));
		return;
	}
	
	if(Check_KeyFile->isChecked() && KeyFile.isEmpty()){
		showErrMsg(tr("Please provide a key file."));
		return;
	}

	if(Check_KeyFile->isChecked()){
		/* Check wether key path exists and is readable */
		QFileInfo fileinfo(KeyFile);
		if(!fileinfo.exists()){
			showErrMsg(tr("%1:\nNo such file or directory.").arg(KeyFile),this);
			return;
		}
		if(!fileinfo.isReadable()){
			showErrMsg(tr("The selected key file or directory is not readable."),this);
			return;
		}
		
		/* If the given path is a directory, we need to find the key file in it */ 
		if(fileinfo.isDir()){
			if(KeyFile.right(1)!="/")KeyFile+="/";			
			// First, we try to find $path/pwsafe.key
			QFile file(KeyFile+"pwsafe.key");
			if(file.exists())
				KeyFile+="pwsafe.key";
			else{
				// If pwsafe.key does not exist, we try to see if there is exactly one file in the
				// given directory which has the extension *.key.
				QDir dir(KeyFile);
				QStringList files;
				files=dir.entryList(QStringList()<<"*.key",QDir::Files);
				// No Key Files
				if(!files.size()){
					showErrMsg(tr("The given directory does not contain any key files."),this);
					return;
				}
				// More than one key file
				if(files.size()>1){
					showErrMsg(tr("The given directory contains more then one key files.\n"
					              "Please specify the key file directly."),this);
					return;
				}
				KeyFile+=files[0];
			}
			
			// Check again whether the found file exists and is readable
			QFileInfo fileinfo(KeyFile);
			if(!fileinfo.exists()){
				showErrMsg(tr("%1:\nNo such file or directory.").arg(KeyFile),this);
				return;
			}
			if(!fileinfo.isReadable()){
				showErrMsg(tr("%1:\nFile is not readable."),this);
				return;
			}			
		}		

	}
	
	if(Check_Password->isChecked() && (Mode==Mode_Set || Mode==Mode_Change)){
		Edit_PwRepeat->clear();
		Label_Unequal->hide();
		stackedWidget->setCurrentIndex(1);
		Edit_PwRepeat->setFocus(Qt::OtherFocusReason);
		return;
	}
	
	if((Mode==Mode_Ask || Mode==Mode_Set) && config->rememberLastKey()){
		if(Check_Password->isChecked() && Check_KeyFile->isChecked()){
			config->setLastKeyType(BOTH);
			config->setLastKeyLocation(Combo_KeyFile->currentText());
		}
		else if(Check_Password->isChecked()){
			config->setLastKeyType(PASSWORD);
			config->setLastKeyLocation(QString());
		}
		else if(Check_KeyFile->isChecked()){
			config->setLastKeyType(KEYFILE);
			config->setLastKeyLocation(Combo_KeyFile->currentText());
		}		
	}
	
	done(Exit_Ok);
}

void PasswordDialog::OnCheckBoxesChanged(){
	Edit_Password->setEnabled(Check_Password->isChecked());
	Combo_KeyFile->setEnabled(Check_KeyFile->isChecked());
	Button_Browse->setEnabled(Check_KeyFile->isChecked());
	Button_GenKeyFile->setEnabled(Check_KeyFile->isChecked());	
}

void PasswordDialog::ChangeEchoModeDatabaseKey(){
	if(Edit_Password->echoMode()==QLineEdit::Normal){
		Edit_Password->setEchoMode(QLineEdit::Password);
		Edit_PwRepeat->setEchoMode(QLineEdit::Password);
		ButtonChangeEchoMode->setIcon(getIcon("pwd_hide"));
	}
	else{
		Edit_Password->setEchoMode(QLineEdit::Normal);
		Edit_PwRepeat->setEchoMode(QLineEdit::Normal);
		ButtonChangeEchoMode->setIcon(getIcon("pwd_show"));
	}
}

void PasswordDialog::OnButtonQuit(){
	done(Exit_Quit);
}

void PasswordDialog::paintEvent(QPaintEvent* event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);	
}

void PasswordDialog::OnBookmarkTriggered(QAction* action){
	if(action->data().toString()==QString())
		setWindowTitle(Filename);
	else
		setWindowTitle(action->data().toString());
	Label_Bookmark->setText(action->text());
	BookmarkFilename=action->data().toString();
}

void PasswordDialog::OnGenKeyFile(){
	QString filename=KpxFileDialogs::saveFile(this,"PasswordDlg",tr("Create Key File..."),
	                                          QStringList() << tr("All Files (*)")
	                                                        << tr("Key Files (*.key)"));
	if(!filename.isEmpty()){
		QString error;
		if(!createKeyFile(filename,&error,32,true)){
			showErrMsg(error,this);
			return;
		}
		else {
			if(Check_KeyFile->isChecked())
				Combo_KeyFile->setEditText(filename);
			return;
		}
	}	
}

QString PasswordDialog::password(){
	if(Check_Password->isChecked())
		return Password;
	else
		return QString();
}


QString PasswordDialog::keyFile(){
	if(Check_KeyFile->isChecked())
		return Combo_KeyFile->currentText();
	else
		return QString();
}

QString PasswordDialog::selectedBookmark(){
	return BookmarkFilename;
}

void PasswordDialog::OnButtonBack(){
	stackedWidget->setCurrentIndex(0);
	Edit_PwRepeat->clear();
}

void PasswordDialog::done(int r){
	// workaround for a Qt crash bug
	Password = Edit_Password->text();
	Edit_Password->clear();
	Edit_Password->setEchoMode(QLineEdit::Normal);
	Edit_PwRepeat->clear();
	Edit_PwRepeat->setEchoMode(QLineEdit::Normal);
	
	QDialog::done(r);
}
