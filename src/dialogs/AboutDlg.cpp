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


#include "AboutDlg.h"


AboutDialog::AboutDialog(QWidget* parent):QDialog(parent)
{
	setupUi(this);
	createBanner(&BannerPixmap,getPixmap("keepassx"),QString("%1 %2").arg(APP_DISPLAY_NAME, APP_VERSION),width());

    labelAppName->setText(APP_DISPLAY_NAME);
    labelAppFunc->setText(QString(" -  ").append(APP_LONG_FUNC));

	QString AboutTr=QString("<b>%1: %2</b><br><br>").arg(tr("Current Translation")).arg(tr("None","Please replace 'None' with the language of your translation"));
	if(isTranslationActive()){
		AboutTr+=QString("<b>%1:</b> %2<br>").arg(tr("Author")).arg(QApplication::translate("Translation", "$TRANSLATION_AUTHOR"));
		QString mail=QApplication::translate("Translation", "$TRANSLATION_AUTHOR_EMAIL","Here you can enter your email or homepage if you want.");
		if(!mail.isEmpty()){
			AboutTr+=mail+"<br>";
		}
		AboutTr+="<br>";
	}
	Edit_Translation->setHtml(AboutTr+tr("Information on how to translate KeePassX can be found under:")
		+"<br>http://www.keepassx.org/");
	QString str;
	str+="<b>"+tr("Team")+"</b><br>";
	str+="<div style='margin-left:10px;'>";
	str+="<u>Tarek Saidi</u><br>"+tr("Developer, Project Admin")+"<br>tarek_saidi@users.sf.net<br>";
	str+="<br>";
	str+="<u>Felix Geyer</u><br>"+tr("Developer, Project Admin")+"<br>sniperbeamer@users.sf.net<br>";
	str+="<br>";
	str+="<u>Brandon Mayes</u><br>"+tr("Developer")+"<br>bdmayes@users.sourceforge.net<br>";
	str+="<br>";
	str+="<u>Eugen Gorschenin</u><br>"+tr("Web Designer")+"<br>geugen@users.sf.de<br>";
	str+="<br>";
	str+="<u>Juan J Gonz&aacute;lez C&aacute;rdenas [Jota Jota]</u><br>"+tr("Developer")+"<br>myxelf@users.sf.net<br>";
	str+="</div><br><div style='margin-left:0px;'>";
    str+="<b>"+tr("Thanks To")+"</b><br>";
	str+="</div><div style='margin-left:10px;'>";
	str+="<u>Matthias Miller</u><br>"+tr("Patches for better MacOS X support")+"<br>www.outofhanwell.com<br></div>";
	str+="<br>";
	str+="<u>James Nicholls</u><br>"+tr("Main Application Icon")/*+"<br>"+tr("mailto:???")*/+"<br></div>";
	str+="<br>";
	str+="<u>Constantin Makshin</u><br>"+tr("Various fixes and improvements")+"<br>dinosaur-rus@users.sourceforge.net<br></div>";
	Edit_Thanks->setHtml(str);
	
	QFile gpl(DataDir+"/license.html");
	if (!gpl.open(QIODevice::ReadOnly)){
		QMessageBox::critical(this,tr("Error"),tr("File '%1' could not be found.")
				.arg("'license.html'")+"\n"+tr("Make sure that the program is installed correctly.")
				,tr("OK"),0,0,2,1);
	}
	
	Edit_License->setHtml(QString::fromUtf8(gpl.readAll()));
	
	connect(ButtonBox, SIGNAL(rejected()), SLOT(close()));
}

void AboutDialog::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}
