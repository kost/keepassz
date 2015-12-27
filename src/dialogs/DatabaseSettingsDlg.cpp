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
 

#include "DatabaseSettingsDlg.h"
#include "Kdb3Database.h"


CDbSettingsDlg::CDbSettingsDlg(QWidget* parent,IDatabase* db, Qt::WFlags fl)
: QDialog(parent,fl)
{
	setupUi(this);
	database=dynamic_cast<IKdbSettings*>(db);
	adjustSize();
	setMaximumSize(size());
	setMinimumSize(size());
	createBanner(&BannerPixmap,getPixmap("appsettings"),tr("Settings"),width());
	ComboAlgo->insertItem(0,tr("AES(Rijndael):  256 Bit   (default)"));
	ComboAlgo->insertItem(1,tr("Twofish:  256 Bit"));
	ComboAlgo->setCurrentIndex(database->cryptAlgorithm()); //Achtung: AlgoID muss gleich dem ComboBox Index sein!
	EditRounds->setText(QString::number( database->keyTransfRounds() ));
	ButtonBench->setIcon(getIcon("alarmclock"));
	connect( ButtonBox, SIGNAL( accepted() ), this, SLOT( OnOK() ) );
	connect( ButtonBox, SIGNAL( rejected() ), this, SLOT( OnCancel() ) );
	connect( ButtonBench, SIGNAL( clicked() ), this, SLOT( OnBenchmark() ) );
}

CDbSettingsDlg::~CDbSettingsDlg(){
}

void CDbSettingsDlg::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void CDbSettingsDlg::OnCancel()
{
	done(0);
}


void CDbSettingsDlg::OnOK()
{
	if(EditRounds->text()==""){
		QMessageBox::warning(NULL,tr("Warning"),tr("Please determine the number of encryption rounds."),tr("OK"));
		return;
	}
	bool valid;
	int rounds=EditRounds->text().toUInt(&valid,10);
	if(valid==false){
		QMessageBox::warning(NULL,tr("Error"),tr("'%1' is not valid integer value.").arg(EditRounds->text()),tr("OK"));
		return;
	}
	if(rounds==0){
		QMessageBox::warning(NULL,tr("Error"),tr("The number of encryption rounds have to be greater than 0."),tr("OK"));
		return;
	}
	database->setKeyTransfRounds(rounds);
	database->setCryptAlgorithm((CryptAlgorithm)ComboAlgo->currentIndex());
	done(1);
}

void CDbSettingsDlg::OnBenchmark(){
	EditRounds->setText(QString::number( KeyTransformBenchmark::benchmark(1000) ));
}
