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

#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include "ui_SettingsDlg.h"
#include "lib/AutoType.h"

class CSettingsDlg : public QDialog, private Ui_SettingsDialog
{
	Q_OBJECT
	public:
		CSettingsDlg(QWidget* parent);
		~CSettingsDlg();
	
	private slots:
    	virtual void OnCancel();
    	virtual void OnOK();
    	virtual void OnTextColor();
    	virtual void OnColor2();
    	virtual void OnColor1();
		void OnOtherButton(QAbstractButton*);
		/*void OnIntPluginNone();
		void OnIntPluginGnome();
		void OnIntPluginKde();*/
		void OnMountDirBrowse();
		void OnBrowserCmdBrowse();
		void OnCustomizeEntryDetails();
		void OnInactivityLockChange(bool checked);
		void OnAutoSaveToggle(bool checked);
		void OnAutoSaveChangeToggle(bool checked);
		void OnBackupDeleteChange();
		void OnSelectLanguage(int index);
	
#ifdef GLOBAL_AUTOTYPE
	private slots:
		void resetGlobalShortcut();
#endif
	
	private:
 		virtual void paintEvent(QPaintEvent*);
		virtual void resizeEvent(QResizeEvent*);
		void initLanguageList();
		void apply();
 		QColor color1,color2,textcolor;
 		QPixmap BannerPixmap;
		QList<Translation> translations;
#ifdef GLOBAL_AUTOTYPE
		Shortcut pShortcut;
#endif
		//static bool PluginsModified;

};

#endif

