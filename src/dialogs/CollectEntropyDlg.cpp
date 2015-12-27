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


#include "CollectEntropyDlg.h"


CollectEntropyDlg::CollectEntropyDlg(QWidget* parent):QDialog(parent){
	setupUi(this);
	resize(layout()->closestAcceptableSize(this,QSize(0,0)));
	setMinimumSize(size());
	setMaximumSize(size());
	createBanner(&BannerPixmap,getPixmap("dice"),tr("Entropy Collection"),width());
	KeyEntropyBuffer=new unsigned char[105];
	MouseEntropyBuffer=new quint16[210];
	KeyCounter=0;
	MouseCounter=0;
	QTimer* timer=new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(trackMousePos()));
	timer->setInterval(50);
	timer->start();
	ReseedDone=false;
}

CollectEntropyDlg::~CollectEntropyDlg(){
	delete [] KeyEntropyBuffer;
	delete [] MouseEntropyBuffer;
}

void CollectEntropyDlg::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void CollectEntropyDlg::trackMousePos(){
	QPoint p=QCursor::pos();
	if(LastPos-p==QPoint(0,0))return;
	LastPos=p;
	if(MouseCounter==105 || ReseedDone)return;
	MouseEntropyBuffer[2*MouseCounter]=p.x();
	MouseEntropyBuffer[2*MouseCounter+1]=p.y();
	MouseCounter++;
	updateProgress();
}

void CollectEntropyDlg::keyReleaseEvent(QKeyEvent* event ){
	QDialog::keyReleaseEvent(event);
	if(KeyCounter==105 || ReseedDone)return;
	KeyEntropyBuffer[KeyCounter]=event->key();
	KeyCounter++;
	updateProgress();
}

void CollectEntropyDlg::updateProgress(){
	if(4*KeyCounter+4*MouseCounter>=420){
		progressBar->setValue(420);
		ReseedDone=true;
		reseedStrongPool((quint8*)MouseEntropyBuffer,4*MouseCounter,KeyEntropyBuffer,KeyCounter);
		Animation->stop();
		stackedWidget->setCurrentIndex(1);
	}
	else
		progressBar->setValue(4*KeyCounter+4*MouseCounter);

}

void CollectEntropyDlg::showEvent(QShowEvent* event){
	if(!event->spontaneous()){
		Animation->start();
	}
}
