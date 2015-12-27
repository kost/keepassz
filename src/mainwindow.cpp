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

#include <QPointer>
#include <QToolBar>
#include <QStatusBar>
#include "mainwindow.h"
#include "lib/AutoType.h"
#include "import/Import_PwManager.h"
#include "import/Import_KWalletXml.h"
#include "import/Import_KeePassX_Xml.h"
#include "export/Export_Txt.h"
#include "export/Export_KeePassX_Xml.h"

#include "dialogs/AboutDlg.h"
#include "dialogs/SearchDlg.h"
#include "dialogs/SettingsDlg.h"
#include "dialogs/DatabaseSettingsDlg.h"
#include "dialogs/PasswordDlg.h"
#include "dialogs/SimplePasswordDlg.h"
#include "dialogs/PasswordGenDlg.h"
#include "dialogs/CollectEntropyDlg.h"
#include "dialogs/CustomizeDetailViewDlg.h"
#include "dialogs/ExpiredEntriesDlg.h"
//#include "dialogs/TrashCanDlg.h" //TODO TrashCan
#include "dialogs/AddBookmarkDlg.h"
#include "dialogs/ManageBookmarksDlg.h"
#include "dialogs/HelpDlg.h"

Import_KeePassX_Xml import_KeePassX_Xml;
Import_PwManager import_PwManager;
Import_KWalletXml import_KWalletXml;
Export_Txt export_Txt;
Export_KeePassX_Xml export_KeePassX_Xml;

KeepassMainWindow::KeepassMainWindow(const QString& ArgFile,bool ArgMin,bool ArgLock,QWidget *parent, Qt::WFlags flags) :QMainWindow(parent,flags){
	ShutingDown=false;
	IsLocked=false;
	EventOccurred=true;
	inactivityCounter=0;
	InUnLock=false;
	unlockDlg=NULL;
	db=NULL;
	setupUi(this);
#ifdef Q_WS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
#endif
#ifdef AUTOTYPE
	initAutoType(this);
#endif
#ifdef GLOBAL_AUTOTYPE
	autoType->registerGlobalShortcut(config->globalShortcut());
#endif
	setWindowModified(false);
	QByteArray windowGeo = config->mainWindowGeometry();
	if (!windowGeo.isEmpty())
		restoreGeometry(windowGeo);
	VSplitter->restoreState(config->vSplitterPos());
	HSplitter->restoreState(config->hSplitterPos());
	SysTray=new QSystemTrayIcon(this);
	setupToolbar();
	setupIcons();
	setStateFileOpen(false);
	setupMenus();
	DetailView->setVisible(config->showEntryDetails());
	StatusBarGeneral=new QLabel(statusBar());
	//StatusBarSelection=new QLabel(statusBar());
	statusBar()->addWidget(StatusBarGeneral,15);
	//statusBar()->addWidget(StatusBarSelection,85);
	statusBar()->setVisible(config->showStatusbar());
	setStatusBarMsg(StatusBarReady);
#ifndef Q_WS_MAC
	if (config->alwaysOnTop())
		setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
#endif

	NormalCentralWidget=QMainWindow::centralWidget();
	LockedCentralWidget=new QWidget(this);
	WorkspaceLockedWidget.setupUi(LockedCentralWidget);
	LockedCentralWidget->setVisible(false);

	setupConnections();
	connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)), SLOT(OnShutdown(QSessionManager&)));
	
	inactivityTimer = new QTimer(this);
	inactivityTimer->setInterval(500);
	connect(inactivityTimer, SIGNAL(timeout()), SLOT(OnInactivityTimer()));
	if (config->lockOnInactivity() && config->lockAfterSec()!=0)
		inactivityTimer->start();

	bool showWindow = !ArgMin;
	FileOpen=false;
	if(!ArgFile.isEmpty()){
		QString f = QDir::cleanPath(QDir::current().absoluteFilePath(ArgFile));
		if (ArgLock)
			fakeOpenDatabase(f);
		else
			openDatabase(f,false);
	}
	else if(config->openLastFile() && !config->lastFile().isEmpty()){
		QFileInfo file(config->lastFile());
		if(file.exists()){
			QString f = QDir::cleanPath(QDir::current().absoluteFilePath(config->lastFile()));
			if (!ArgMin)
				showWindow = !config->startMinimized();
			if (ArgLock || config->startLocked())
				fakeOpenDatabase(f);
			else
				openDatabase(f,true);
		}
		else
			config->setLastFile(QString());
	}
	
	// TODO HelpBrowser
	/*HelpBrowser = new QAssistantClient(QString(),this);
	HelpBrowser->setArguments(QStringList()<< "-profile" << "share/keepass/doc/keepassx.adp");*/

	createBookmarkActions();
	
	if (showWindow)
		show();
	else if (!config->showSysTrayIcon())
		showMinimized();
}

void KeepassMainWindow::setupConnections(){
	connect(FileNewAction, SIGNAL(triggered()), this, SLOT(OnFileNewKdb()));
	connect(FileOpenAction, SIGNAL(triggered()), this, SLOT(OnFileOpen()));
	connect(FileCloseAction, SIGNAL(triggered()), this, SLOT(OnFileClose()));
	connect(FileSaveAction, SIGNAL(triggered()), this, SLOT(OnFileSave()));
	connect(FileSaveAsAction, SIGNAL(triggered()), this, SLOT(OnFileSaveAs()));
	connect(FileSettingsAction, SIGNAL(triggered()), this, SLOT(OnFileSettings()));
	connect(FileChangeKeyAction, SIGNAL(triggered()), this, SLOT(OnFileChangeKey()));
	connect(FileExitAction, SIGNAL(triggered()), this, SLOT(OnFileExit()));
	connect(FileUnLockWorkspaceAction,SIGNAL(triggered()), this, SLOT(OnUnLockWorkspace()));
	connect(menuImport,SIGNAL(triggered(QAction*)),this,SLOT(OnImport(QAction*)));
	connect(menuExport,SIGNAL(triggered(QAction*)),this,SLOT(OnExport(QAction*)));
	connect(menuBookmarks,SIGNAL(triggered(QAction*)),this,SLOT(OnBookmarkTriggered(QAction*)));

	connect(EditNewGroupAction, SIGNAL(triggered()), GroupView, SLOT(OnNewGroup()));
	connect(EditNewSubgroupAction, SIGNAL(triggered()), GroupView, SLOT(OnNewSubgroup()));
	connect(EditEditGroupAction, SIGNAL(triggered()), GroupView, SLOT(OnEditGroup()));
	connect(EditDeleteGroupAction, SIGNAL(triggered()), GroupView, SLOT(OnDeleteGroup()));
	connect(EditGroupSortAction, SIGNAL(triggered()), GroupView, SLOT(OnSort()));
	connect(EditNewEntryAction, SIGNAL(triggered()), EntryView, SLOT(OnNewEntry()));
	connect(EditEditEntryAction, SIGNAL(triggered()), EntryView, SLOT(OnEditEntry()));
	connect(EntryView, SIGNAL(requestCreateGroup(QString,quint32,GroupViewItem*)),
			GroupView, SLOT(createGroup(QString,quint32,GroupViewItem*)));
	connect(EditCloneEntryAction, SIGNAL(triggered()), EntryView, SLOT(OnCloneEntry()));
	connect(EditDeleteEntryAction, SIGNAL(triggered()), EntryView, SLOT(OnDeleteEntry()));
	connect(EditUsernameToClipboardAction, SIGNAL(triggered()), EntryView, SLOT(OnUsernameToClipboard()));
	connect(EditPasswordToClipboardAction, SIGNAL(triggered()), EntryView, SLOT(OnPasswordToClipboard()));
	connect(EditOpenUrlAction, SIGNAL(triggered()), EntryView, SLOT(OnEditOpenUrl()));
	connect(EditCopyUrlAction, SIGNAL(triggered()), EntryView, SLOT(OnEditCopyUrl()));
	connect(EditSaveAttachmentAction, SIGNAL(triggered()),EntryView, SLOT(OnSaveAttachment()));
	connect(EditSearchAction, SIGNAL(triggered()), this, SLOT(OnSearch()));
	connect(EditGroupSearchAction, SIGNAL(triggered()), this, SLOT(OnGroupSearch()));
#ifdef AUTOTYPE
	connect(EditAutoTypeAction,SIGNAL(triggered()),EntryView,SLOT(OnAutoType()));
#endif

	connect(ViewShowToolbarAction,SIGNAL(toggled(bool)),this,SLOT(OnViewShowToolbar(bool)));
	connect(ViewShowEntryDetailsAction,SIGNAL(toggled(bool)),this,SLOT(OnViewShowEntryDetails(bool)));
	connect(ViewHidePasswordsAction,SIGNAL(toggled(bool)), this, SLOT(OnUsernPasswVisibilityChanged()));
	connect(ViewHideUsernamesAction,SIGNAL(toggled(bool)), this, SLOT(OnUsernPasswVisibilityChanged()));

	connect(menuColumns,SIGNAL(triggered(QAction*)),this,SLOT(OnColumnVisibilityChanged()));
	connect(ViewToolButtonSize16Action,SIGNAL(toggled(bool)), this, SLOT(OnViewToolbarIconSize16(bool)));
	connect(ViewToolButtonSize22Action,SIGNAL(toggled(bool)), this, SLOT(OnViewToolbarIconSize22(bool)));
	connect(ViewToolButtonSize28Action,SIGNAL(toggled(bool)), this, SLOT(OnViewToolbarIconSize28(bool)));
	connect(ViewShowStatusbarAction,SIGNAL(toggled(bool)),statusBar(),SLOT(setVisible(bool)));
#ifdef Q_WS_MAC
	connect(ViewMinimizeAction, SIGNAL(triggered()), SLOT(showMinimized()));
#endif

	connect(ExtrasSettingsAction,SIGNAL(triggered(bool)),this,SLOT(OnExtrasSettings()));
	connect(ExtrasPasswordGenAction,SIGNAL(triggered(bool)),this,SLOT(OnExtrasPasswordGen()));
	connect(ExtrasShowExpiredEntriesAction,SIGNAL(triggered(bool)),this,SLOT(OnExtrasShowExpiredEntries()));
	//connect(ExtrasTrashCanAction,SIGNAL(triggered(bool)),this,SLOT(OnExtrasTrashCan())); //TODO ExtrasTrashCan

	connect(HelpHandbookAction,SIGNAL(triggered()),this,SLOT(OnHelpHandbook()));
	connect(HelpAboutAction,SIGNAL(triggered()),this,SLOT(OnHelpAbout()));

	connect(QuickSearchEdit,SIGNAL(returnPressed()), this, SLOT(OnQuickSearch()));
	connect(GroupView,SIGNAL(groupChanged(IGroupHandle*)),EntryView,SLOT(OnGroupChanged(IGroupHandle*)));
	connect(GroupView,SIGNAL(groupChanged(IGroupHandle*)),this,SLOT(OnGroupSelectionChanged(IGroupHandle*)));
	connect(GroupView,SIGNAL(fileModified()),this,SLOT(OnFileModified()));
	connect(EntryView,SIGNAL(fileModified()),this,SLOT(OnFileModified()));
	connect(EntryView,SIGNAL(selectionChanged(SelectionState)),this,SLOT(OnEntryChanged(SelectionState)));
	connect(GroupView,SIGNAL(searchResultsSelected()),EntryView,SLOT(OnShowSearchResults()));
	connect(GroupView,SIGNAL(searchResultsSelected()),this,SLOT(OnShowSearchResults()));
	connect(GroupView,SIGNAL(entriesDropped()),EntryView,SLOT(removeDragItems()));
	connect(HideSearchResultsAction,SIGNAL(triggered()),GroupView,SLOT(OnHideSearchResults()));
	connect(EntryView, SIGNAL(viewModeChanged(bool)), SLOT(loadColumnVisibility()));
	connect(EntryView, SIGNAL(viewModeChanged(bool)), ViewColumnsGroupAction, SLOT(setVisible(bool)));

	connect(SysTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(OnSysTrayActivated(QSystemTrayIcon::ActivationReason)));
	connect(DetailView,SIGNAL(anchorClicked(const QUrl&)),this,SLOT(OnDetailViewUrlClicked(const QUrl&)));
	connect(WorkspaceLockedWidget.Button_Unlock,SIGNAL(clicked()),this,SLOT(OnUnLockWorkspace()));
	connect(WorkspaceLockedWidget.Button_CloseDatabase,SIGNAL(clicked()),this,SLOT(OnLockClose()));
}

void KeepassMainWindow::setupToolbar(){
	toolBar=new QToolBar(this);
	toolBar->setMovable(false);
	addToolBar(toolBar);
	toolBar->setIconSize(QSize(config->toolbarIconSize(),config->toolbarIconSize()));
	ViewShowToolbarAction=toolBar->toggleViewAction();
	toolBar->addAction(FileNewAction);
	toolBar->addAction(FileOpenAction);
	toolBar->addAction(FileSaveAction);
	toolBar->addSeparator();
	toolBar->addAction(EditNewEntryAction);
	toolBar->addAction(EditEditEntryAction);
	toolBar->addAction(EditDeleteEntryAction);
	toolBar->addSeparator();
	toolBar->addAction(EditUsernameToClipboardAction);
	toolBar->addAction(EditPasswordToClipboardAction);
	toolBar->addSeparator();
	toolBar->addAction(FileUnLockWorkspaceAction);
	toolBar->addSeparator();
	QuickSearchEdit=new QLineEdit(toolBar);
	QuickSearchEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	toolBar->addWidget(QuickSearchEdit);
	toolBar->setVisible(config->showToolbar());
}

void KeepassMainWindow::setupIcons(){
	setWindowIcon(getIcon("keepassx_small"));
	FileNewAction->setIcon(getIcon("filenew"));
	FileOpenAction->setIcon(getIcon("fileopen"));
	FileSaveAction->setIcon(getIcon("filesave"));
	FileSaveAsAction->setIcon(getIcon("filesaveas"));
	FileCloseAction->setIcon(getIcon("fileclose"));
	FileSettingsAction->setIcon(getIcon("dbsettings"));
	FileUnLockWorkspaceAction->setIcon(getIcon("lock"));
	FileExitAction->setIcon(getIcon("exit"));
	EditNewEntryAction->setIcon(getIcon("newentry"));
	EditEditEntryAction->setIcon(getIcon("editentry"));
	EditDeleteEntryAction->setIcon(getIcon("deleteentry"));
	EditGroupSortAction->setIcon(getIcon("swap"));
	EditUsernameToClipboardAction->setIcon(getIcon("copyusername"));
	EditPasswordToClipboardAction->setIcon(getIcon("copypwd"));
	EditCloneEntryAction->setIcon(getIcon("cloneentry"));
	EditOpenUrlAction->setIcon(getIcon("openurl"));
	EditSaveAttachmentAction->setIcon(getIcon("filesave"));
	EditNewGroupAction->setIcon(getIcon("newgroup"));
	EditNewSubgroupAction->setIcon(getIcon("newgroup"));
	EditEditGroupAction->setIcon(getIcon("editgroup"));
	EditDeleteGroupAction->setIcon(getIcon("deletegroup"));
	EditSearchAction->setIcon(getIcon("dbsearch"));
	EditGroupSearchAction->setIcon(getIcon("groupsearch"));
	ExtrasShowExpiredEntriesAction->setIcon(getIcon("expired"));
	ExtrasPasswordGenAction->setIcon(getIcon("generator"));
	//ExtrasTrashCanAction->setIcon(getIcon("trashcan")); //TODO ExtrasTrashCan
	ExtrasSettingsAction->setIcon(getIcon("appsettings"));
#ifdef AUTOTYPE
	EditAutoTypeAction->setIcon(getIcon("autotype"));
#else
	EditAutoTypeAction->setVisible(false);
#endif
	HelpHandbookAction->setIcon(getIcon("manual"));
	HelpAboutAction->setIcon(getIcon("help_about"));
	menuBookmarks->menuAction()->setIcon(getIcon("bookmark_folder"));
	AddThisAsBookmarkAction->setIcon(getIcon("bookmark_this"));
	AddBookmarkAction->setIcon(getIcon("bookmark_add"));
	ManageBookmarksAction->setIcon(getIcon("bookmark"));
	SysTray->setIcon(getIcon("keepassx"));
	if(config->showSysTrayIcon())
		SysTray->show();
}

void KeepassMainWindow::setupMenus(){
	GroupView->ContextMenu->addAction(EditNewSubgroupAction);
	GroupView->ContextMenu->addAction(EditEditGroupAction);
	GroupView->ContextMenu->addAction(EditDeleteGroupAction);
	GroupView->ContextMenu->addAction(EditGroupSortAction);
	GroupView->ContextMenu->addSeparator();
	GroupView->ContextMenu->addAction(EditNewEntryAction);
	GroupView->ContextMenu->addSeparator();
	GroupView->ContextMenu->addAction(EditGroupSearchAction);
	GroupView->ContextMenuSearchGroup->addAction(HideSearchResultsAction);

	EntryView->ContextMenu->addAction(EditUsernameToClipboardAction);
	EntryView->ContextMenu->addAction(EditPasswordToClipboardAction);
	EntryView->ContextMenu->addAction(EditOpenUrlAction);
	EntryView->ContextMenu->addAction(EditCopyUrlAction);
	EntryView->ContextMenu->addAction(EditSaveAttachmentAction);
#ifdef AUTOTYPE
	EntryView->ContextMenu->addAction(EditAutoTypeAction);
#endif
	EntryView->ContextMenu->addSeparator();
	EntryView->ContextMenu->addAction(EditNewEntryAction);
	EntryView->ContextMenu->addAction(EditEditEntryAction);
	EntryView->ContextMenu->addAction(EditCloneEntryAction);
	EntryView->ContextMenu->addAction(EditDeleteEntryAction);

	ViewShowToolbarAction->setText(tr("Show &Toolbar"));
	ViewMenu->insertAction(ViewShowEntryDetailsAction,ViewShowToolbarAction);
	ViewShowToolbarAction->setChecked(config->showToolbar());
	ViewShowEntryDetailsAction->setChecked(config->showEntryDetails());
	ViewHidePasswordsAction->setChecked(config->hidePasswords());
	ViewHideUsernamesAction->setChecked(config->hideUsernames());
	loadColumnVisibility();
	ViewShowStatusbarAction->setChecked(config->showStatusbar());

	switch(config->toolbarIconSize()){
		case 16: ViewToolButtonSize16Action->setChecked(true); break;
		case 22: ViewToolButtonSize22Action->setChecked(true); break;
		case 28: ViewToolButtonSize28Action->setChecked(true); break;
	}
	
#ifdef Q_WS_MAC
	ViewMenu->addSeparator();
	ViewMenu->addAction(ViewMinimizeAction);
#endif

	SysTrayMenu = new QMenu(APP_DISPLAY_NAME,this);
	SysTrayMenu->addAction(FileUnLockWorkspaceAction);
	SysTrayMenu->addSeparator();
	SysTrayMenu->addAction(FileExitAction);
	SysTray->setContextMenu(SysTrayMenu);
	updateTrayTooltip();

	#define _add_import(name){\
	QAction* import=new QAction(this);\
	import->setData(qVariantFromValue(dynamic_cast<QObject*>(&name)));\
	import->setText(name.title());\
	menuImport->addAction(import);}

	#define _add_export(name){\
	QAction* Export=new QAction(this);\
	Export->setData(qVariantFromValue(dynamic_cast<QObject*>(&name)));\
	Export->setText(name.title());\
	menuExport->addAction(Export);}

	_add_import(import_KeePassX_Xml)
	_add_import(import_PwManager)
	_add_import(import_KWalletXml)
	_add_export(export_Txt);
	_add_export(export_KeePassX_Xml);

	FileNewAction->setShortcut(tr("Ctrl+N"));
	FileOpenAction->setShortcut(tr("Ctrl+O"));
	FileCloseAction->setShortcut(tr("Ctrl+W"));
	FileSaveAction->setShortcut(tr("Ctrl+S"));
	FileUnLockWorkspaceAction->setShortcut(tr("Ctrl+L"));
	FileExitAction->setShortcut(tr("Ctrl+Q"));
	EditNewGroupAction->setShortcut(tr("Ctrl+G"));
	EditPasswordToClipboardAction->setShortcut(tr("Ctrl+C"));
	EditUsernameToClipboardAction->setShortcut(tr("Ctrl+B"));
	EditOpenUrlAction->setShortcut(tr("Ctrl+U"));
	EditCopyUrlAction->setShortcut(tr("Ctrl+I"));
	EditNewEntryAction->setShortcut(tr("Ctrl+Y"));
	EditEditEntryAction->setShortcut(tr("Ctrl+E"));
	EditDeleteEntryAction->setShortcut(tr("Ctrl+D"));
	EditCloneEntryAction->setShortcut(tr("Ctrl+K"));
	EditSearchAction->setShortcut(tr("Ctrl+F"));
	ExtrasPasswordGenAction->setShortcut(tr("Ctrl+P"));
	ExtrasShowExpiredEntriesAction->setShortcut(tr("Ctrl+X"));
#ifdef AUTOTYPE
	EditAutoTypeAction->setShortcut(tr("Ctrl+V"));
#endif
#ifdef Q_WS_MAC
	FileSaveAsAction->setShortcut(tr("Shift+Ctrl+S"));
	EditGroupSearchAction->setShortcut(tr("Shift+Ctrl+F"));
	ViewMinimizeAction->setShortcut(tr("Ctrl+M"));
#endif

	//ExtrasTrashCanAction->setVisible(false); //TODO For KP 2.x only
	menuBookmarks->menuAction()->setVisible(config->featureBookmarks());
}

void KeepassMainWindow::loadColumnVisibility() {
	ViewColumnsTitleAction->setChecked(EntryView->columnVisible(0));
	ViewColumnsUsernameAction->setChecked(EntryView->columnVisible(1));
	ViewColumnsUrlAction->setChecked(EntryView->columnVisible(2));
	ViewColumnsPasswordAction->setChecked(EntryView->columnVisible(3));
	ViewColumnsCommentAction->setChecked(EntryView->columnVisible(4));
	ViewColumnsExpireAction->setChecked(EntryView->columnVisible(5));
	ViewColumnsCreationAction->setChecked(EntryView->columnVisible(6));
	ViewColumnsLastChangeAction->setChecked(EntryView->columnVisible(7));
	ViewColumnsLastAccessAction->setChecked(EntryView->columnVisible(8));
	ViewColumnsAttachmentAction->setChecked(EntryView->columnVisible(9));
	ViewColumnsGroupAction->setChecked(EntryView->columnVisible(10));
}

void KeepassMainWindow::setupDatabaseConnections(IDatabase* DB){
	ICustomIcons* CustomIconsDb=dynamic_cast<ICustomIcons*>(DB);
	if(CustomIconsDb){
		connect(CustomIconsDb,SIGNAL(iconsModified()),this,SLOT(OnFileModified()));
		connect(CustomIconsDb,SIGNAL(iconsModified()),EntryView,SLOT(updateIcons()));
		connect(CustomIconsDb,SIGNAL(iconsModified()),GroupView,SLOT(updateIcons()));
	}
}


bool KeepassMainWindow::openDatabase(QString filename,bool IsAuto){
	if (!QFile::exists(filename)){
		QMessageBox::critical(this, tr("Error"), tr("The database file does not exist."));
		return false;
	}
	
	dbReadOnly = false;
	
	if (QFile::exists(filename+".lock")){
		QMessageBox msgBox(this);
		msgBox.setIcon(QMessageBox::Question);
		msgBox.setWindowTitle(tr("Database locked"));
		msgBox.setText(tr("The database you are trying to open is locked.\n"
				"This means that either someone else has opened the file or KeePassX crashed last time it opened the database.\n\n"
				"Do you want to open it anyway?"
		));
		msgBox.addButton(QMessageBox::Yes);
		msgBox.addButton(QMessageBox::No);
		QPushButton* readOnlyButton = new QPushButton(tr("Open read-only"), &msgBox);
		msgBox.addButton(readOnlyButton, QMessageBox::AcceptRole);
		msgBox.setDefaultButton(readOnlyButton);
		msgBox.exec();
		
		if (!msgBox.clickedButton() || msgBox.clickedButton() == msgBox.button(QMessageBox::No))
			return false;
		else if (msgBox.clickedButton() == readOnlyButton)
			dbReadOnly = true;
	}
	
	if(!IsAuto){
		config->setLastKeyLocation(QString());
		config->setLastKeyType(PASSWORD);
	}
	db = new Kdb3Database();
	PasswordDialog::DlgFlags flags=PasswordDialog::Flag_None;
	if(IsAuto)
		flags = PasswordDialog::Flag_Auto;
	PasswordDialog dlg(this,PasswordDialog::Mode_Ask,flags,filename);
	if (InUnLock){
		dlg.setWindowModality(Qt::WindowModal);
		unlockDlg = &dlg;
	}
	bool rejected = (dlg.exec()==PasswordDialog::Exit_Cancel);
	if (InUnLock)
		unlockDlg = NULL;
	if (rejected)
		return false;
	
	if(dlg.selectedBookmark()!=QString())
		filename=dlg.selectedBookmark();

	GroupView->db=db;
	EntryView->db=db;
	setupDatabaseConnections(db);
	QString err;
	setStatusBarMsg(StatusBarLoading);
	db->setKey(dlg.password(),dlg.keyFile());
	
	if (!dbReadOnly && !QFile::exists(filename+".lock")){
		QFile lock(filename+".lock");
		if (!lock.open(QIODevice::WriteOnly)){
			setStatusBarMsg(StatusBarReadOnlyLock);
			dbReadOnly = true;
		}
	}
	
	if(db->load(filename, dbReadOnly)){
		if (IsLocked)
			resetLock();
		updateCurrentFile(filename);
		saveLastFilename(filename);
		GroupView->createItems();
		EntryView->showGroup(NULL);
		setStateFileOpen(true);
		setStateFileModified(static_cast<Kdb3Database*>(db)->hasPasswordEncodingChanged());
	}
	else{
		if (!dbReadOnly && QFile::exists(filename+".lock"))
			QFile::remove(filename+".lock");
		setStatusBarMsg(StatusBarLoadingFailed);
		QString error=db->getError();
		if(error.isEmpty())error=tr("Unknown error while loading database.");
		QMessageBox::critical(this,tr("Error"),
		                      QString("%1\n%2").arg(tr("The following error occured while opening the database:"))
		                      .arg(error));
		if(db->isKeyError()){
			delete db;
			return openDatabase(filename,IsAuto);
		}
		else{
			delete db;
			return false;
		}
	}
	if (statusbarState != StatusBarReadOnlyLock)
		setStatusBarMsg(StatusBarReady);
	inactivityCounter = 0;
	
	GroupView->selectFirstGroup();
	
	return true;
}

void KeepassMainWindow::fakeOpenDatabase(const QString& filename){
	if (!QFile::exists(filename)){
		QMessageBox::critical(this, tr("Error"), tr("The database file does not exist."));
		return;
	}
	
	config->setLastFile(filename);
	updateCurrentFile(filename);
	setLock();
}

bool KeepassMainWindow::closeDatabase(bool lock){
	Q_ASSERT(FileOpen);
	Q_ASSERT(db!=NULL);
	if(ModFlag){
		if(config->autoSave() && db->file()){
			if(!OnFileSave()) return false;
		}
		else{
			QMessageBox::StandardButton r=QMessageBox::question(this,tr("Save modified file?"),
							tr("The current file was modified.\nDo you want to save the changes?"),
							QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);
			if(r==QMessageBox::Cancel) return false; //Cancel
			if(r==QMessageBox::Yes){ //Yes (Save file)
				if (dbReadOnly) {
					if(!OnFileSaveAs()) return false;
				}
				else {
					if(!OnFileSave()) return false;
				}
			}
		}
	}
	db->close();
	delete db;
	db=NULL;
	if (!dbReadOnly && QFile::exists(currentFilePath+".lock")){
		if (!QFile::remove(currentFilePath+".lock"))
			QMessageBox::critical(this, tr("Error"), tr("Couldn't remove database lock file."));
	}
	EntryView->db=NULL;
	EntryView->clear();
	EntryView->Items.clear();
	GroupView->db=NULL;
	GroupView->clear();
	GroupView->Items.clear();
	SearchResults.clear();
	if (lock)
		IsLocked = true;
	setStateFileOpen(false);
	if (!lock){
		updateCurrentFile(QString());
		QuickSearchEdit->setText("");
		updateTrayTooltip();
	}
	return true;
}


void KeepassMainWindow::OnFileNewKdb(){
	IDatabase* db_new=dynamic_cast<IDatabase*>(new Kdb3Database());
	db_new->create();
	PasswordDialog dlg(this,PasswordDialog::Mode_Set,PasswordDialog::Flag_None,"New Database");
	if(dlg.exec()==PasswordDialog::Exit_Ok){
		if(FileOpen)
			if(!closeDatabase())return;
		if (IsLocked)
			resetLock();
		db=db_new;
		db->setKey(dlg.password(),dlg.keyFile());
		db->generateMasterKey();
		updateCurrentFile(QString());
		GroupView->db=db;
		EntryView->db=db;
		GroupView->createItems();
		EntryView->showGroup(NULL);
		setStateFileOpen(true);
		setStateFileModified(true);
		setupDatabaseConnections(db);
		setStateGroupSelected(NONE);
		setStateEntrySelected(NONE);
		GroupView->createGroup("Internet", 1);
		GroupView->createGroup("eMail", 19);
	}
	else{
		delete db_new;
	}
}

void KeepassMainWindow::openFile(const QString& filename) {
	if(FileOpen) {
		if(!closeDatabase())
			return;
	}
	openDatabase(filename);
}

void KeepassMainWindow::OnFileOpen(){
	/*QFileDialog FileDlg(this,tr("Open Database..."),QDir::homePath());
	FileDlg.setFilters(QStringList()<< tr("KeePass Databases (*.kdb)")<< tr("All Files (*)"));
	FileDlg.setFileMode(QFileDialog::ExistingFile);
	FileDlg.setAcceptMode(QFileDialog::AcceptOpen);
	if(!FileDlg.exec())return;
	if(!FileDlg.selectedFiles().size())return;*/
	QString filename=KpxFileDialogs::openExistingFile(this,"MainWindow_FileOpen",
			tr("Open Database..."),QStringList()<<tr("KeePass Databases (*.kdb)")<< tr("All Files (*)"));
	if (!filename.isEmpty())
		openFile(filename);
}

void KeepassMainWindow::OnFileClose(){
	if (IsLocked)
		OnLockClose();
	else
		closeDatabase();
}

void KeepassMainWindow::setStateFileOpen(bool IsOpen){
	FileOpen=IsOpen;
	FileSaveAction->setEnabled(IsOpen);
	FileSaveAsAction->setEnabled(IsOpen);
	FileCloseAction->setEnabled(IsOpen||IsLocked);
	FileSettingsAction->setEnabled(IsOpen);
	FileChangeKeyAction->setEnabled(IsOpen);
	menuExport->setEnabled(IsOpen);
	EditNewGroupAction->setEnabled(IsOpen);
	EditSearchAction->setEnabled(IsOpen);
	GroupView->setEnabled(IsOpen);
	EntryView->setEnabled(IsOpen);
	DetailView->setEnabled(IsOpen);
	QuickSearchEdit->setEnabled(IsOpen);
	ExtrasShowExpiredEntriesAction->setEnabled(IsOpen);
	AddThisAsBookmarkAction->setEnabled(IsOpen && db->file());
	FileUnLockWorkspaceAction->setEnabled(IsOpen||IsLocked);
	
	if(!IsOpen){
		EditNewSubgroupAction->setEnabled(false);
		EditEditGroupAction->setEnabled(false);
		EditDeleteGroupAction->setEnabled(false);
		EditPasswordToClipboardAction->setEnabled(false);
		EditUsernameToClipboardAction->setEnabled(false);
		EditOpenUrlAction->setEnabled(false);
		EditCopyUrlAction->setEnabled(false);
		EditSaveAttachmentAction->setEnabled(false);
		EditNewEntryAction->setEnabled(false);
		EditEditEntryAction->setEnabled(false);
		EditCloneEntryAction->setEnabled(false);
		EditDeleteEntryAction->setEnabled(false);
		EditGroupSearchAction->setEnabled(false);
#ifdef AUTOTYPE
		EditAutoTypeAction->setEnabled(false);
#endif
	}
	
	updateWindowTitle();
	updateTrayTooltip();
}


void KeepassMainWindow::setStateFileModified(bool mod){
	if (config->autoSaveChange() && mod && db->file()){
		if (OnFileSave())
			return; // return on success, so we don't set the state to modified
	}
	
	ModFlag=mod;
	if(mod)
		FileSaveAction->setIcon(getIcon("filesave"));
	else
		FileSaveAction->setIcon(getIcon("filesavedisabled"));
	updateWindowTitle();
	setWindowModified(mod);
}

void KeepassMainWindow::setStateGroupSelected(SelectionState s){
	GroupSelection=s;
	switch(GroupSelection){
		case NONE:
			EditNewSubgroupAction->setEnabled(false);
			EditEditGroupAction->setEnabled(false);
			EditDeleteGroupAction->setEnabled(false);
			EditGroupSearchAction->setEnabled(false);
			EditNewEntryAction->setEnabled(false);
			break;
		case SINGLE:
			EditNewSubgroupAction->setEnabled(true);
			EditEditGroupAction->setEnabled(true);
			EditDeleteGroupAction->setEnabled(true);
			EditGroupSearchAction->setEnabled(true);
			EditNewEntryAction->setEnabled(true);
			break;
		case SEARCHGROUP:
			EditNewSubgroupAction->setEnabled(false);
			EditEditGroupAction->setEnabled(false);
			EditDeleteGroupAction->setEnabled(false);
			EditGroupSearchAction->setEnabled(false);
			EditNewEntryAction->setEnabled(false);
			break;
		default: Q_ASSERT(false);
	}
}

void KeepassMainWindow::updateDetailView(){
	if(EntryView->selectedItems().size()!=1){
		DetailView->setPlainText("");
		return;
	}

	QString templ=DetailViewTemplate;
	IEntryHandle* entry=((EntryViewItem*)(EntryView->selectedItems()[0]))->EntryHandle;

	templ.replace("%group%", Qt::escape(entry->group()->title()));
	templ.replace("%title%", Qt::escape(entry->title()));
	if (config->hideUsernames())
		templ.replace("%username%","****");
	else
		templ.replace("%username%", Qt::escape(entry->username()));
	if (!config->hidePasswords()) {
		SecString password=entry->password();
		password.unlock();
		templ.replace("%password%", Qt::escape(password.string()));
	}
	else {
		templ.replace("%password%","****");
	}
	templ.replace("%url%", Qt::escape(entry->url()));
	templ.replace("%creation%", Qt::escape(entry->creation().toString(Qt::SystemLocaleDate)));
	templ.replace("%lastmod%", Qt::escape(entry->lastMod().toString(Qt::SystemLocaleDate)));
	templ.replace("%lastaccess%", Qt::escape(entry->lastAccess().toString(Qt::SystemLocaleDate)));
	templ.replace("%expire%", Qt::escape(entry->expire().toString(Qt::SystemLocaleDate)));
	templ.replace("%comment%", Qt::escape(entry->comment()).replace("\n","<br/>"));
	templ.replace("%attachment%", Qt::escape(entry->binaryDesc()));

	if(entry->expire()!=Date_Never){
		int secs=QDateTime::currentDateTime().secsTo(entry->expire());
		if(secs < 0)
			templ.replace("%expire-timeleft%",tr("Expired"));
		else{
			int years=0;
			int months=0;
			int days=0;
			years=secs/(86400*365);
			secs-=years*(86400*365);
			months=secs/(86400*30);
			secs-=months*(86400*30);
			days=secs/86400;

			QString out;

			if(months==1)
				out=tr("1 Month");
			if(months>1)
				out=tr("%1 Months").arg(months);

			if(years){
				if(out!=QString())
					out.prepend(", ");
				if(years==1)
					out.prepend(tr("1 Year"));
				if(years>1)
					out.prepend(tr("%1 Years").arg(years));
			}
			else if(days){
				if(out!=QString())
					out.append(", ");
				if(days==1)
					out.append(tr("1 Day"));
				if(days>1)
					out.append(tr("%1 Days").arg(days));
			}

			if(!days && !years && !months)
				out=tr("less than 1 day");

			templ.replace("%expire-timeleft%","in " + out);
		}
	}
	else
		templ.replace("%expire-timeleft%","-");

	DetailView->setHtml(templ);
}


void KeepassMainWindow::setStateEntrySelected(SelectionState s){
	EntrySelection = s;
	if (GroupSelection == NONE || GroupSelection == SINGLE){
		switch (EntrySelection){
			case NONE:
				EditPasswordToClipboardAction->setEnabled(false);
				EditUsernameToClipboardAction->setEnabled(false);
				EditOpenUrlAction->setEnabled(false);
				EditCopyUrlAction->setEnabled(false);
				EditSaveAttachmentAction->setEnabled(false);
				EditEditEntryAction->setEnabled(false);
				EditCloneEntryAction->setEnabled(false);
				EditCloneEntryAction->setText(tr("Clone Entry"));
				EditDeleteEntryAction->setEnabled(false);
				EditDeleteEntryAction->setText(tr("Delete Entry"));
#ifdef AUTOTYPE
				EditAutoTypeAction->setEnabled(false);
#endif
				break;
			case SINGLE:
				EditPasswordToClipboardAction->setEnabled(true);
				EditUsernameToClipboardAction->setEnabled(true);
				EditOpenUrlAction->setEnabled(true);
				EditCopyUrlAction->setEnabled(true);
				EditSaveAttachmentAction->setEnabled(((EntryViewItem*)(EntryView->selectedItems()[0]))->EntryHandle->binarySize() > 0);
				EditEditEntryAction->setEnabled(true);
				EditCloneEntryAction->setEnabled(true);
				EditCloneEntryAction->setText(tr("Clone Entry"));
				EditDeleteEntryAction->setEnabled(true);
				EditDeleteEntryAction->setText(tr("Delete Entry"));
#ifdef AUTOTYPE
				EditAutoTypeAction->setEnabled(true);
#endif
				break;
			case MULTIPLE:
				EditPasswordToClipboardAction->setEnabled(false);
				EditUsernameToClipboardAction->setEnabled(false);
				EditOpenUrlAction->setEnabled(false);
				EditCopyUrlAction->setEnabled(false);
				EditSaveAttachmentAction->setEnabled(false);
				EditEditEntryAction->setEnabled(false);
				EditCloneEntryAction->setEnabled(true);
				EditCloneEntryAction->setText(tr("Clone Entries"));
				EditDeleteEntryAction->setEnabled(true);
				EditDeleteEntryAction->setText(tr("Delete Entries"));
#ifdef AUTOTYPE
				EditAutoTypeAction->setEnabled(false);
#endif
				break;
			default:
				Q_ASSERT(false);
		}
	}
	else if (GroupSelection == SEARCHGROUP){
		switch(EntrySelection){
			case NONE:
				EditUsernameToClipboardAction->setEnabled(false);
				EditPasswordToClipboardAction->setEnabled(false);
				EditOpenUrlAction->setEnabled(false);
				EditCopyUrlAction->setEnabled(false);
				EditSaveAttachmentAction->setEnabled(false);
				EditEditEntryAction->setEnabled(false);
				EditCloneEntryAction->setEnabled(false);
				EditCloneEntryAction->setText(tr("Clone Entry"));
				EditDeleteEntryAction->setEnabled(false);
				EditDeleteEntryAction->setText(tr("Delete Entry"));
#ifdef AUTOTYPE
				EditAutoTypeAction->setEnabled(false);
#endif
				break;
			case SINGLE:
				EditUsernameToClipboardAction->setEnabled(true);
				EditPasswordToClipboardAction->setEnabled(true);
				EditOpenUrlAction->setEnabled(true);
				EditCopyUrlAction->setEnabled(true);
				EditSaveAttachmentAction->setEnabled(((EntryViewItem*)(EntryView->selectedItems()[0]))->EntryHandle->binarySize() > 0);
				EditEditEntryAction->setEnabled(true);
				EditCloneEntryAction->setEnabled(false);
				EditCloneEntryAction->setText(tr("Clone Entry"));
				EditDeleteEntryAction->setEnabled(true);
				EditDeleteEntryAction->setText(tr("Delete Entry"));
#ifdef AUTOTYPE
				EditAutoTypeAction->setEnabled(true);
#endif
				break;
			case MULTIPLE:
				EditUsernameToClipboardAction->setEnabled(false);
				EditPasswordToClipboardAction->setEnabled(false);
				EditOpenUrlAction->setEnabled(false);
				EditCopyUrlAction->setEnabled(false);
				EditSaveAttachmentAction->setEnabled(false);
				EditEditEntryAction->setEnabled(false);
				EditCloneEntryAction->setEnabled(false);
				EditCloneEntryAction->setText(tr("Clone Entries"));
				EditDeleteEntryAction->setEnabled(true);
				EditDeleteEntryAction->setText(tr("Delete Entries"));
#ifdef AUTOTYPE
				EditAutoTypeAction->setEnabled(false);
#endif
				break;
			default:
				Q_ASSERT(false);
		}
	}
	else
		Q_ASSERT(false);
}


bool KeepassMainWindow::OnFileSave(){
	if(!db->file())
		return OnFileSaveAs();
	saveLastFilename(db->file()->fileName());
	if(db->save()){
		setStateFileOpen(true); // necessary for AddThisAsBookmarkAction
		setStateFileModified(false);
		if (config->backup() && config->backupDelete() && config->backupDeleteAfter()>0){
			IGroupHandle* backupGroup = db->backupGroup();
			if (backupGroup && backupGroup==EntryView->getCurrentGroup())
				EntryView->showGroup(backupGroup);
		}
		return true;
	}
	else{
		showErrMsg(QString("%1\n%2").arg(tr("File could not be saved.")).arg(db->getError()));
		return false;
	}
}

bool KeepassMainWindow::OnFileSaveAs(){
	QString filename=KpxFileDialogs::saveFile(this,"MainWindow_FileSave",
			tr("Save Database..."),QStringList()<<tr("KeePass Databases (*.kdb)")<< tr("All Files (*)"));
	if (filename.isEmpty() || filename.compare(".kdb", Qt::CaseInsensitive)==0)
		return false;
	
	QFile lock(filename+".lock");
	if (!lock.open(QIODevice::WriteOnly)){
		QMessageBox::critical(this, tr("Error"), tr("Couldn't create database lock file."));
		return false;
	}
	
	if(!db->changeFile(filename)){
		showErrMsg(QString("%1\n%2").arg(tr("File could not be saved.")).arg(db->getError()));
		QFile::remove( filename+".lock" );
		return false;
	}
	
	if (!dbReadOnly && !currentFilePath.isEmpty() && QFile::exists(currentFilePath+".lock")){
		if (!QFile::remove(currentFilePath+".lock"))
			QMessageBox::critical(this, tr("Error"), tr("Couldn't remove database lock file."));
	}
	
	dbReadOnly = false;
	updateCurrentFile(filename);
	updateWindowTitle();
	updateTrayTooltip();
	
	return OnFileSave();
}

void KeepassMainWindow::OnFileSettings(){
	CDbSettingsDlg dlg(this,db);
	if(dlg.exec()){
		db->generateMasterKey();
		setStateFileModified(true);
	}
}

void KeepassMainWindow::OnFileChangeKey(){
	QFile* file=db->file();
	QString filename = file ? file->fileName() : QString();
	PasswordDialog dlg(this,PasswordDialog::Mode_Change,PasswordDialog::Flag_None,filename);
	if(dlg.exec()==PasswordDialog::Exit_Ok){
		db->setKey(dlg.password(),dlg.keyFile());
		db->generateMasterKey();
		setStateFileModified(true);
	}
}

void KeepassMainWindow::OnFileExit(){
	ShutingDown = true;
	close();
}


void KeepassMainWindow::OnExport(QAction* action){
	dynamic_cast<IExport*>(action->data().value<QObject*>())->exportDatabase(this,db);
}

void KeepassMainWindow::OnImport(QAction* action){
	if(FileOpen)
		if(!closeDatabase())return;
	IDatabase* tmpdb=dynamic_cast<IDatabase*>(new Kdb3Database());
	tmpdb->create();
	if(dynamic_cast<IImport*>(action->data().value<QObject*>())->importDatabase(this,tmpdb)){
		PasswordDialog dlg(this,PasswordDialog::Mode_Set,PasswordDialog::Flag_None,QString());
		if(dlg.exec()!=PasswordDialog::Exit_Ok){
			delete tmpdb;
			return;
		}
		db=tmpdb;
		db->setKey(dlg.password(),dlg.keyFile());
		db->generateMasterKey();
		GroupView->db=db;
		EntryView->db=db;
		setupDatabaseConnections(db);
		GroupView->createItems();
		EntryView->showGroup(NULL);
		setStateFileOpen(true);
		setStateFileModified(true);
	}
	else
		delete tmpdb;
}

/*
void KeepassMainWindow::removeFromSearchResults(int id){
for(int i=0; i<SearchResults.size();i++){
 if(SearchResults[i]==id){
	SearchResults.removeAt(i);
	return;
 }
}
}
*/

void KeepassMainWindow::search(IGroupHandle* group){
	SearchDialog dlg(db,group,this);
	if(dlg.exec()){
		EntryView->SearchResults=dlg.Result;
		GroupView->showSearchResults();
	}
}

void KeepassMainWindow::OnSearch(){
	search(NULL);
}

void KeepassMainWindow::OnGroupSearch(){
	Q_ASSERT(GroupView->currentItem());
	search(((GroupViewItem*)GroupView->currentItem())->GroupHandle);
}

void KeepassMainWindow::OnQuickSearch(){
	EntryView->SearchResults=db->search(NULL,QuickSearchEdit->text(),false,false,false,NULL);
	GroupView->showSearchResults();
}

void KeepassMainWindow::OnColumnVisibilityChanged(){
	EntryView->setColumnVisible(0, ViewColumnsTitleAction->isChecked());
	EntryView->setColumnVisible(1, ViewColumnsUsernameAction->isChecked());
	EntryView->setColumnVisible(2, ViewColumnsUrlAction->isChecked());
	EntryView->setColumnVisible(3, ViewColumnsPasswordAction->isChecked());
	EntryView->setColumnVisible(4, ViewColumnsCommentAction->isChecked());
	EntryView->setColumnVisible(5, ViewColumnsExpireAction->isChecked());
	EntryView->setColumnVisible(6, ViewColumnsCreationAction->isChecked());
	EntryView->setColumnVisible(7, ViewColumnsLastChangeAction->isChecked());
	EntryView->setColumnVisible(8, ViewColumnsLastAccessAction->isChecked());
	EntryView->setColumnVisible(9, ViewColumnsAttachmentAction->isChecked());
	EntryView->setColumnVisible(10, ViewColumnsGroupAction->isVisible() && ViewColumnsGroupAction->isChecked());
}

void KeepassMainWindow::OnUsernPasswVisibilityChanged(){
 	config->setHidePasswords(ViewHidePasswordsAction->isChecked());
 	config->setHideUsernames(ViewHideUsernamesAction->isChecked());
	EntryView->refreshItems();
}

void KeepassMainWindow::OnFileModified(){
	setStateFileModified(true);
}

void KeepassMainWindow::closeEvent(QCloseEvent* e){
	if (!ShutingDown && config->showSysTrayIcon() && config->minimizeToTray()){
		e->ignore();
		if (config->lockOnMinimize() && !IsLocked && FileOpen)
			OnUnLockWorkspace();
		hide();
		return;
	}
	
	if(FileOpen && !closeDatabase()){
		ShutingDown = false;
		e->ignore();
		if (!isVisible())
			show();
		return;
	}
	
	e->accept();
	
#ifdef GLOBAL_AUTOTYPE
	autoType->unregisterGlobalShortcut();
#endif

	config->setMainWindowGeometry(saveGeometry());
	// workaround if window has never been visible
	if (isVisible() || VSplitter->sizes()[0]!=VSplitter->sizes()[1])
		config->setVSplitterPos(VSplitter->saveState());
	if (config->showEntryDetails())
		config->setHSplitterPos(HSplitter->saveState());
	config->setShowStatusbar(statusBar()->isVisible());
	
	delete SysTray;
	QMainWindow::closeEvent(e);
	QApplication::quit();
}

void KeepassMainWindow::hideEvent(QHideEvent* event){
	if (event->spontaneous() && QApplication::activeModalWidget()==NULL){
		if (config->lockOnMinimize() && !IsLocked && FileOpen)
			OnUnLockWorkspace();
		if (config->showSysTrayIcon() && config->minimizeTray()){
#ifdef Q_WS_WIN
			QTimer::singleShot(100, this, SLOT(hide()));
#else
			hide();
#endif
			event->accept();
			return;
		}
	}
	
	QMainWindow::hideEvent(event);
}

void KeepassMainWindow::showEvent(QShowEvent* event){
	if (IsLocked && !InUnLock && event->spontaneous()){
#ifndef Q_WS_MAC
		showNormal(); // workaround for some graphic glitches
#endif
		OnUnLockWorkspace();
	}
	
	QMainWindow::showEvent(event);
}

void KeepassMainWindow::OnExtrasSettings(){
	QString oldLang = config->language();
	bool oldAlwaysOnTop = config->alwaysOnTop();
	CSettingsDlg dlg(this);
	dlg.exec();
	if (config->language() != oldLang){
		retranslateUi(this);
		WorkspaceLockedWidget.retranslateUi(LockedCentralWidget);
		ViewShowToolbarAction->setText(tr("Show &Toolbar"));
		EntryView->retranslateColumns();
		GroupView->retranslateUi();
		updateWindowTitle();
		updateTrayTooltip();
		setStatusBarMsg(statusbarState);
	}
	
	EntryView->setAlternatingRowColors(config->alternatingRowColors());
	SysTray->setVisible(config->showSysTrayIcon());
	menuBookmarks->menuAction()->setVisible(config->featureBookmarks());
#ifndef Q_WS_MAC
	if (config->alwaysOnTop() != oldAlwaysOnTop) {
		if (config->alwaysOnTop())
			setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
		else
			setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
		show();
	}
#endif
	
	EventOccurred = true;
	if (config->lockOnInactivity() && config->lockAfterSec()!=0 && !inactivityTimer->isActive()){
		inactivityCounter = 0;
		inactivityTimer->start();
	}
	else if ((!config->lockOnInactivity() || config->lockAfterSec()==0) && inactivityTimer->isActive()){
		inactivityTimer->stop();
	}
}

void KeepassMainWindow::OnHelpAbout(){
	AboutDialog dlg(this);
	dlg.exec();
}

void KeepassMainWindow::OnHelpHandbook(){
	QPointer<HelpDlg> dlg = new HelpDlg(this);
	dlg->exec();
	delete dlg;
}

void KeepassMainWindow::OnViewShowToolbar(bool show){
	config->setShowToolbar(show);
	toolBar->setVisible(show);
}

void KeepassMainWindow::OnViewShowEntryDetails(bool show){
	config->setShowEntryDetails(show);
	DetailView->setVisible(show);
}

/*void KeepassMainWindow::OnItemExpanded(QTreeWidgetItem* item){
//((GroupViewItem*)item)->pGroup->UI_ItemIsExpanded=true;
}

void KeepassMainWindow::OnItemCollaped(QTreeWidgetItem* item){
//((GroupViewItem*)item)->pGroup->UI_ItemIsExpanded=false;
}*/

void KeepassMainWindow::OnGroupSelectionChanged(IGroupHandle* group){
	if(group)
		setStateGroupSelected(SINGLE);
	else
		setStateGroupSelected(NONE);
}

void KeepassMainWindow::OnEntryChanged(SelectionState Selection){
	updateDetailView();
	setStateEntrySelected(Selection);
}

void KeepassMainWindow::OnShowSearchResults(){
	setStateGroupSelected(SEARCHGROUP);
}


void KeepassMainWindow::OnViewToolbarIconSize16(bool state){
	if(!state)return;
	ViewToolButtonSize22Action->setChecked(false);
	ViewToolButtonSize28Action->setChecked(false);
	config->setToolbarIconSize(16);
	toolBar->setIconSize(QSize(16,16));
}

void KeepassMainWindow::OnViewToolbarIconSize22(bool state){
	if(!state)return;
	ViewToolButtonSize16Action->setChecked(false);
	ViewToolButtonSize28Action->setChecked(false);
	config->setToolbarIconSize(22);
	toolBar->setIconSize(QSize(22,22));
}

void KeepassMainWindow::OnViewToolbarIconSize28(bool state){
	if(!state)return;
	ViewToolButtonSize16Action->setChecked(false);
	ViewToolButtonSize22Action->setChecked(false);
	config->setToolbarIconSize(28);
	toolBar->setIconSize(QSize(28,28));
}

void KeepassMainWindow::OnSysTrayActivated(QSystemTrayIcon::ActivationReason reason){
	if(reason!=QSystemTrayIcon::Context){
		if (isVisible()){
			if (unlockDlg!=NULL)
				unlockDlg->reject();
			else if (config->lockOnMinimize() && !IsLocked && FileOpen)
				OnUnLockWorkspace();
			hide();
		}
		else{
#ifdef Q_WS_WIN
			QTimer::singleShot(100, this, SLOT(restoreWindow()));
#else
			restoreWindow();
#endif
		}
	}
}

void KeepassMainWindow::restoreWindow(){
#ifdef Q_WS_WIN
	if (windowState() & Qt::WindowMaximized)
		showMaximized();
	else
		showNormal();
#else
	showNormal();
#endif
	activateWindow();
	if (IsLocked)
		OnUnLockWorkspace();
}

void KeepassMainWindow::OnExtrasPasswordGen(){
	CGenPwDialog dlg(this,true);
	dlg.exec();
}


void KeepassMainWindow::saveLastFilename(const QString& filename){

	if(config->openLastFile()){
		if(config->saveRelativePaths()){
			QString Path=filename.left(filename.lastIndexOf("/"));
			Path=makePathRelative(Path,QDir::currentPath());
			config->setLastFile(Path+filename.right(filename.length()-filename.lastIndexOf("/")-1));
		}
		else
			config->setLastFile(filename);
	}
}

void KeepassMainWindow::OnExtrasShowExpiredEntries(){
	ExpiredEntriesDialog dlg(this,db,db->expiredEntries());
	if(dlg.exec()==QDialog::Accepted){
		GroupView->setCurrentGroup(dlg.SelectedEntry->group());
		EntryView->setCurrentEntry(dlg.SelectedEntry);
	}

}

//TODO TrashCan
/*void KeepassMainWindow::OnExtrasTrashCan(){
	TrashCanDialog dlg(this,db,db->expiredEntries());
	if(dlg.exec()==QDialog::Accepted){

	}

}*/

void KeepassMainWindow::OnDetailViewUrlClicked(const QUrl& url){
	openBrowser(url.toString());
}

void KeepassMainWindow::OnUnLockWorkspace(){
	if(IsLocked){
		if (InUnLock) return;
		InUnLock = true;
		if ( openDatabase(currentFilePath,true) ){
			QTreeWidgetItem* item = GroupView->invisibleRootItem();
			if (lockGroup.size()>0){
				for (int i=0; i<lockGroup.size(); i++){
					item = item->child(lockGroup[i]);
					if (item==NULL) break;
				}
				if (item!=NULL)
					GroupView->setCurrentItem(item);
				lockGroup.clear();
			}
		}
		InUnLock = false;
	}
	else {
		QTreeWidgetItem* item = GroupView->currentItem();
		bool root = false;
		while (item!=NULL){
			QTreeWidgetItem* parent = item->parent();
			if (parent==NULL && !root) {
				parent = GroupView->invisibleRootItem();
				root = true;
			}
			if (parent!=NULL)
				lockGroup.prepend(parent->indexOfChild(item));
			item = parent;
		}
		
		if (closeDatabase(true)) {
			setStateFileModified(false);
			setLock();
		}
		else
			lockGroup.clear();
	}
}

void KeepassMainWindow::OnLockClose(){
	resetLock();
	setStateFileOpen(false);
}

void KeepassMainWindow::setLock(){
	QuickSearchEdit->setEchoMode(QLineEdit::NoEcho);
	NormalCentralWidget->setVisible(false);
	NormalCentralWidget->setParent(NULL);
	setCentralWidget(LockedCentralWidget);
	LockedCentralWidget->setVisible(true);
	SysTray->setIcon(getIcon("keepassx_locked"));
	FileUnLockWorkspaceAction->setText(tr("Un&lock Workspace"));
	IsLocked=true;
	updateTrayTooltip();
	setStateFileOpen(false);
}

void KeepassMainWindow::resetLock(){
	if (!InUnLock)
		QuickSearchEdit->setText("");
	QuickSearchEdit->setEchoMode(QLineEdit::Normal);
	LockedCentralWidget->setVisible(false);
	LockedCentralWidget->setParent(NULL);
	setCentralWidget(NormalCentralWidget);
	NormalCentralWidget->setVisible(true);
	SysTray->setIcon(getIcon("keepassx"));
	FileUnLockWorkspaceAction->setText(tr("&Lock Workspace"));
	IsLocked=false;
	updateTrayTooltip();
}

void KeepassMainWindow::OnInactivityTimer(){
	if (IsLocked || !FileOpen)
		return;
	
	if (QApplication::activeModalWidget()!=NULL || EventOccurredBlock){
		inactivityCounter = 0;
		return;
	}
	
	if (EventOccurred){
		inactivityCounter = 0;
		EventOccurred = false;
	}
	else{
		inactivityCounter++;
		if (inactivityCounter*(inactivityTimer->interval()) >= config->lockAfterSec()*1000){
			QWidget* popUpWidget = QApplication::activePopupWidget();
			if (popUpWidget!=NULL)
				popUpWidget->hide();
			OnUnLockWorkspace();
		}
	}
}

void KeepassMainWindow::OnShutdown(QSessionManager& manager) {
	ShutingDown = true;
	
	/* QApplication::commitData() only closes visible windows,
	   so we need to manually close mainwindow if it's hidden */
	if (manager.allowsInteraction() && !isVisible()) {
		close();
	}
}

void KeepassMainWindow::OnBookmarkTriggered(QAction* action){
	if(action==AddBookmarkAction){
		AddBookmarkDlg dlg(this);
		if(dlg.exec()){
			int id=dlg.ItemID;
			QAction* action=new QAction(this);
			action->setData(id);
			action->setText(KpxBookmarks::title(id));
			action->setIcon(getIcon("document"));
			menuBookmarks->addAction(action);
		}
	}
	else if(action==ManageBookmarksAction){
		ManageBookmarksDlg dlg(this);
		dlg.exec();
		menuBookmarks->clear();
		createBookmarkActions();
	}
	else if(action==AddThisAsBookmarkAction){
		AddBookmarkDlg dlg(this,db->file()->fileName());
		if(dlg.exec()){
			int id=dlg.ItemID;
			QAction* action=new QAction(this);
			action->setData(id);
			action->setText(KpxBookmarks::title(id));
			action->setIcon(getIcon("document"));
			menuBookmarks->addAction(action);
		}
	}
	else {
		openDatabase(KpxBookmarks::path(action->data().toInt()));
	}
}

void KeepassMainWindow::createBookmarkActions(){
	menuBookmarks->addAction(AddBookmarkAction);
	menuBookmarks->addAction(AddThisAsBookmarkAction);
	menuBookmarks->addAction(ManageBookmarksAction);
	menuBookmarks->addSeparator();
	for(int i=0;i<KpxBookmarks::count();i++){
		QAction* action=new QAction(this);
		action->setData(i);
		action->setText(KpxBookmarks::title(i));
		action->setIcon(getIcon("document"));
		menuBookmarks->addAction(action);
	}
}

void KeepassMainWindow::setStatusBarMsg(StatusBarMsg statusBarMsg) {
	QString text;
	
	switch (statusBarMsg) {
		case StatusBarReady:
			text = tr("Ready");
			break;
		case StatusBarLoading:
			text = tr("Loading Database...");
			break;
		case StatusBarLoadingFailed:
			text = tr("Loading Failed");
			break;
		case StatusBarReadOnlyLock:
			text = tr("Couldn't create lock file. Opening the database read-only.");
			break;
	}
	
	statusbarState = statusBarMsg;
	StatusBarGeneral->setText(text);
}

void KeepassMainWindow::updateWindowTitle() {
	if (!IsLocked && !FileOpen)
		setWindowTitle( QString("%1 - %2").arg(APP_DISPLAY_NAME, APP_SHORT_FUNC) );
	else if (currentFilePath.isEmpty())
		setWindowTitle( QString("[%1][*] - %2").arg(tr("new"), APP_DISPLAY_NAME) );
	else if (IsLocked)
		setWindowTitle( QString("%1 (%2) - %3").arg(currentFilePath, tr("locked"), APP_DISPLAY_NAME) );
	else if (ModFlag)
		setWindowTitle( QString("%1[*] - %2").arg(currentFilePath, APP_DISPLAY_NAME) );
	else
		setWindowTitle( QString("%1 - %2").arg(currentFilePath, APP_DISPLAY_NAME) );
}

void KeepassMainWindow::updateTrayTooltip() {
	if (!IsLocked && !FileOpen)
		SysTray->setToolTip(QString("%1 - %2").arg(APP_DISPLAY_NAME, APP_SHORT_FUNC));
	else {
		QString tooltip = QString("%1 - %2").arg(APP_DISPLAY_NAME, currentFilePath.isEmpty()
				? QString("[%1]").arg(tr("new")) : currentFileName);
		if (IsLocked)
			tooltip.append( QString(" (%1)").arg(tr("locked")) );
		SysTray->setToolTip(tooltip);
	}
}

void KeepassMainWindow::updateCurrentFile(const QString& filePath) {
	currentFilePath = filePath;
	currentFileName = QFileInfo(filePath).fileName();
}
