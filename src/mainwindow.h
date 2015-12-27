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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSessionManager>
#include <QSystemTrayIcon>
#include "lib/EntryView.h"
#include "lib/GroupView.h"
#include "export/Export.h"

#include "ui_MainWindow.h"
#include "ui_WorkspaceLockedWidget.h"

class KeepassMainWindow : public QMainWindow, private Ui_MainWindow{
	Q_OBJECT
	public:
		KeepassMainWindow (const QString& ArgFile,bool ArgMin,bool ArgLock,QWidget *parent=0, Qt::WFlags flags=0);
		IDatabase* db;
		inline bool isLocked() { return IsLocked; };
		inline bool isOpened() { return FileOpen; };
	
	public slots:
		void OnUnLockWorkspace();
		void openFile(const QString& filename);

	signals:
		void entryChanged();

	private slots:
		void OnFileNewKdb();
		//void OnFileNewKxdb(); // TODO Kxdb
		void OnFileOpen();
		void OnFileClose();
		bool OnFileSave();
		bool OnFileSaveAs();
		void OnFileSettings();
		void OnFileChangeKey();
		void OnFileExit();
		void OnBookmarkTriggered(QAction* action);
		void OnSearch();
		void OnGroupSearch();
		void OnViewShowToolbar(bool);
		void OnViewShowEntryDetails(bool);
		void OnViewToolbarIconSize16(bool);
		void OnViewToolbarIconSize22(bool);
		void OnViewToolbarIconSize28(bool);
		void OnGroupSelectionChanged(IGroupHandle*);
		void OnQuickSearch();
		void OnColumnVisibilityChanged();
		void OnUsernPasswVisibilityChanged();
		void OnFileModified();
		void OnExtrasSettings();
		void OnExtrasPasswordGen();
		void OnExtrasShowExpiredEntries();
		//void OnExtrasTrashCan(); //TODO TrashCan
		void OnHelpAbout();
		void OnHelpHandbook();
		//void OnItemExpanded(QTreeWidgetItem*);
		//void OnItemCollaped(QTreeWidgetItem*);
		void OnShowSearchResults();
		void OnEntryChanged(SelectionState);
		void OnSysTrayActivated(QSystemTrayIcon::ActivationReason);
		void restoreWindow();
		void OnImport(QAction*);
		void OnExport(QAction*);
		void OnDetailViewUrlClicked(const QUrl& url);
		void OnLockClose();
		void OnInactivityTimer();
		void OnShutdown(QSessionManager& manager);
		void loadColumnVisibility();

	private:
		void closeEvent(QCloseEvent* event);
		void hideEvent(QHideEvent* event);
		void showEvent(QShowEvent* event);
		void setLock();
		void resetLock();
		enum StatusBarMsg {
			StatusBarReady, StatusBarLoading, StatusBarLoadingFailed,
			StatusBarReadOnlyLock
		};
		void setStatusBarMsg(StatusBarMsg statusBarMsg);
		SelectionState GroupSelection, EntrySelection;
		bool FileOpen;
		bool ModFlag;
		bool IsLocked;
		QList<quint32> SearchResults;
		inline void setupToolbar();
		inline void setupIcons();
		inline void setupConnections();
		inline void setupMenus();
		void setStateFileOpen(bool);
		void setStateFileModified(bool);
		void setStateGroupSelected(SelectionState s);
		void setStateEntrySelected(SelectionState s);
		bool openDatabase(QString filename,bool IsAuto=false);
		void fakeOpenDatabase(const QString& filename);
		void setupDatabaseConnections(IDatabase* DB);
		bool closeDatabase(bool lock=false);
		void search(IGroupHandle* Group);
		//void removeFromSearchResults(int sID);
		void updateDetailView();
		void exportDatabase(IExport* exporter,QStringList filters);
		void saveLastFilename(const QString& filename);
		void createBookmarkActions();
		void updateWindowTitle();
		void updateTrayTooltip();
		void updateCurrentFile(const QString& filePath);
		QLineEdit* QuickSearchEdit;
		QLabel* StatusBarGeneral;
		//QLabel* StatusBarSelection;
		QToolBar* toolBar;
		QSystemTrayIcon* SysTray;
		QAction* ViewShowToolbarAction;
		QMenu* SysTrayMenu;
		//QAssistantClient* HelpBrowser; //TODO HelpBrowser
		QWidget* NormalCentralWidget;
		QWidget* LockedCentralWidget;
		Ui_WorkspaceLockedWidget WorkspaceLockedWidget;
		bool ShutingDown;
		bool InUnLock;
		QList<int> lockGroup;
		QDialog* unlockDlg;
		QString currentFilePath;
		QString currentFileName;
		int inactivityCounter;
		QTimer* inactivityTimer;
		StatusBarMsg statusbarState;
		bool dbReadOnly;
};

#endif
