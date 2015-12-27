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


#include "dialogs/PasswordGenDlg.h"
#include "dialogs/CollectEntropyDlg.h"

#include "random.h"
#include "apg/randpass.h"
#include "apg/pronpass.h"

bool CGenPwDialog::EntropyCollected=false;

CGenPwDialog::CGenPwDialog(QWidget* parent, bool StandAloneMode,Qt::WFlags fl)
: QDialog(parent,fl)
{
	setupUi(this);
	connect(ButtonGenerate, SIGNAL(clicked()), SLOT(OnGeneratePw()));
	connect(DialogButtons, SIGNAL(rejected()), SLOT(OnCancel()));
	connect(DialogButtons, SIGNAL(accepted()), SLOT(OnAccept()));
	connect(tabCategory, SIGNAL(currentChanged(int)), SLOT(estimateQuality()));
	connect(checkBox1, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox2, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox3, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox4, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox5, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox6, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox7, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(Edit_chars, SIGNAL(textChanged(const QString&)), SLOT(estimateQuality()));
	connect(checkBoxPU, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBoxPL, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBoxPN, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBoxPS, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(Spin_Num, SIGNAL(valueChanged(int)), SLOT(estimateQuality()));
	connect(Check_ExcludeLookAlike, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(Check_CollectEntropy, SIGNAL(stateChanged(int)), SLOT(OnCollectEntropyChanged(int)));
	connect(ButtonChangeEchoMode, SIGNAL(clicked()), SLOT(SwapEchoMode()));
	connect(tabCategory, SIGNAL(currentChanged(int)), SLOT(setGenerateEnabled()));
	connect(checkBox1, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox2, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox3, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox4, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox5, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox6, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox7, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(Edit_chars, SIGNAL(textChanged(const QString&)), SLOT(setGenerateEnabled()));
	connect(checkBoxPU, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBoxPL, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBoxPN, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBoxPS, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));

	if(!StandAloneMode){
		AcceptButton=DialogButtons->addButton(QDialogButtonBox::Ok);
		AcceptButton->setEnabled(false);
		DialogButtons->addButton(QDialogButtonBox::Cancel);
		connect(Edit_dest, SIGNAL(textChanged(const QString&)), SLOT(setAcceptEnabled(const QString&)));
	}
	else{
		DialogButtons->addButton(QDialogButtonBox::Close);
		AcceptButton=NULL;
	}
	
	Edit_chars->setValidator(new PassCharValidator(this));
	
	tabCategory->setCurrentIndex(config->pwGenCategory());
	QBitArray pwGenOptions=config->pwGenOptions();
	//Radio_1->setChecked(pwGenOptions.at(0));
	//Radio_2->setChecked(!pwGenOptions.at(0));
	checkBox1->setChecked(pwGenOptions.at(1));
	checkBox2->setChecked(pwGenOptions.at(2));
	checkBox3->setChecked(pwGenOptions.at(3));
	checkBox4->setChecked(pwGenOptions.at(4));
	checkBox5->setChecked(pwGenOptions.at(5));
	checkBox6->setChecked(pwGenOptions.at(6));
	checkBox7->setChecked(pwGenOptions.at(7));
	Check_CollectEntropy->setChecked(pwGenOptions.at(8));
	Check_CollectOncePerSession->setChecked(pwGenOptions.at(9));
	//OnRadio1StateChanged(pwGenOptions.at(0));
	//OnRadio2StateChanged(!pwGenOptions.at(0));
	if (pwGenOptions.size()>=14){
		checkBoxPU->setChecked(pwGenOptions.at(10));
		checkBoxPL->setChecked(pwGenOptions.at(11));
		checkBoxPN->setChecked(pwGenOptions.at(12));
		checkBoxPS->setChecked(pwGenOptions.at(13));
	}
	else{
		checkBoxPU->setChecked(true);
		checkBoxPL->setChecked(true);
		checkBoxPN->setChecked(true);
		checkBoxPS->setChecked(false);
	}
	Edit_chars->setText(config->pwGenCharList());
	Check_ExcludeLookAlike->setChecked(config->pwGenExcludeLookAlike());
	Check_EveryGroup->setChecked(config->pwGenEveryGroup());
	Spin_Num->setValue(config->pwGenLength());
	adjustSize();
	resize(size() + QSize(5, 10));
	createBanner(&BannerPixmap,getPixmap("dice"),tr("Password Generator"),width());
	
	if(!config->showPasswords())
		SwapEchoMode();
	else
		ButtonChangeEchoMode->setIcon(getIcon("pwd_show"));
}

CGenPwDialog::~CGenPwDialog(){
	config->setPwGenCategory(tabCategory->currentIndex());
	QBitArray pwGenOptions(14);
	//pwGenOptions.setBit(0,Radio_1->isChecked());
	pwGenOptions.setBit(1,checkBox1->isChecked());
	pwGenOptions.setBit(2,checkBox2->isChecked());
	pwGenOptions.setBit(3,checkBox3->isChecked());
	pwGenOptions.setBit(4,checkBox4->isChecked());
	pwGenOptions.setBit(5,checkBox5->isChecked());
	pwGenOptions.setBit(6,checkBox6->isChecked());
	pwGenOptions.setBit(7,checkBox7->isChecked());
	pwGenOptions.setBit(8,Check_CollectEntropy->isChecked());
	pwGenOptions.setBit(9,Check_CollectOncePerSession->isChecked());
	pwGenOptions.setBit(10,checkBoxPU->isChecked());
	pwGenOptions.setBit(11,checkBoxPL->isChecked());
	pwGenOptions.setBit(12,checkBoxPN->isChecked());
	pwGenOptions.setBit(13,checkBoxPS->isChecked());
	config->setPwGenOptions(pwGenOptions);
	config->setPwGenCharList(Edit_chars->text());
	config->setPwGenExcludeLookAlike(Check_ExcludeLookAlike->isChecked());
	config->setPwGenEveryGroup(Check_EveryGroup->isChecked());
	config->setPwGenLength(Spin_Num->value());
}

void CGenPwDialog::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void CGenPwDialog::OnGeneratePw()
{
	if(Check_CollectEntropy->isChecked()){
		if((Check_CollectOncePerSession->isChecked() && !EntropyCollected) || !Check_CollectOncePerSession->isChecked()){
			CollectEntropyDlg dlg(this);
			dlg.exec();
			EntropyCollected=true;
		}
	}
	
	int length = Spin_Num->value();
	QString password;
	
	if (tabCategory->currentIndex() == 1)
	{
		unsigned int mode = 0;
		if (checkBoxPU->isChecked())
			mode |= S_CL;
		if (checkBoxPL->isChecked())
			mode |= S_SL;
		if (checkBoxPN->isChecked())
			mode |= S_NB;
		if (checkBoxPS->isChecked())
			mode |= S_SS;
		
		char* buffer = new char[length+1];
		char* hyphenated_word = new char[length*18+1];
		gen_pron_pass(buffer, hyphenated_word, length, length, mode);
		password = buffer;
		delete[] hyphenated_word;
		delete[] buffer;
	}
	else{
		password = generatePasswordInternal(length);
	}

	Edit_dest->setText(password);
}

void CGenPwDialog::estimateQuality(){
	int num = 0;
	int index = tabCategory->currentIndex();
	if (index == 0) {
		if (checkBox1->isChecked()) {
			num+=26;
			if (Check_ExcludeLookAlike->isChecked())
				num -= 2;
		}
		if (checkBox2->isChecked()) {
			num+=26;
			if (Check_ExcludeLookAlike->isChecked())
				num -= 1;
		}
		if (checkBox3->isChecked()) {
			num+=10;
			if (Check_ExcludeLookAlike->isChecked())
				num -= 2;
		}
		if (checkBox4->isChecked()) {
			num+=32;
			if (Check_ExcludeLookAlike->isChecked())
				num -= 1;
		}
		if (checkBox5->isChecked())
			num++;
		if (checkBox6->isChecked())
			num++;
		if (checkBox7->isChecked())
			num++;
	}
	else if (index == 1) {
		if (checkBoxPU->isChecked())
			num+=26;
		if (checkBoxPL->isChecked())
			num+=26;
		if (checkBoxPN->isChecked())
			num+=10;
		if (checkBoxPS->isChecked())
			num+=32;
	}
	else {
		num=Edit_chars->text().length();
	}

	float bits = 0;
	if (num)
		bits = log((float)num) / log(2.0f);
	bits = bits * ((float)Spin_Num->value());
	int bitsNum = (int) (bits+0.5);
	
	Progress_Quali->setFormat(tr("%1 Bits").arg(bitsNum));
	Progress_Quali->update();
	Progress_Quali->setValue((bitsNum > 128) ? 128 : bitsNum);
}

void CGenPwDialog::OnAccept()
{
	done(1);
}

void CGenPwDialog::OnCancel()
{
	done(0);
}

void CGenPwDialog::OnCollectEntropyChanged(int state){
	if(state==Qt::Checked)
		Check_CollectOncePerSession->setDisabled(false);
	else
		Check_CollectOncePerSession->setDisabled(true);

}

void CGenPwDialog::SwapEchoMode(){
	if(Edit_dest->echoMode()==QLineEdit::Normal){
		Edit_dest->setEchoMode(QLineEdit::Password);
		ButtonChangeEchoMode->setIcon(getIcon("pwd_hide"));
	}
	else{
		Edit_dest->setEchoMode(QLineEdit::Normal);
		ButtonChangeEchoMode->setIcon(getIcon("pwd_show"));
	}
}

void CGenPwDialog::AddToAssoctable(QList<QChar>& table,int start,int end,int& pos){
	for (int i=start;i<=end;i++){
		if (Check_ExcludeLookAlike->isChecked()){
			switch (i){
				case 48:  // 0
				case 79:  // O
				case 49:  // 1
				case 73:  // I
				case 108: // l
				case 124: // |
					continue;
			}
		}
		table.append(QChar(i));
		pos++;
	}
}

CGenPwDialog::PwGroup CGenPwDialog::AddToAssoctableGroup(QList<QChar>& table,int start,int end,int& pos){
	PwGroup group;
	group.start = pos;
	AddToAssoctable(table,start,end,pos);
	group.end = pos-1;
	return group;
}

QString CGenPwDialog::generatePasswordInternal(int length){
	/*-------------------------------------------------------
	     ASCII
	  -------------------------------------------------------
	  "A...Z" 65...90
	  "a...z" 97...122
	  "0...9" 48...57
	  Special Characters 33...47; 58...64; 91...96; 123...126
	  "-" 45
	  "_" 95
	  -------------------------------------------------------
	*/

	int num=0;
	QList<QChar> assoctable;
	int groups=0;
	bool ensureEveryGroup = false;
	QList<PwGroup> groupTable;
	
	if (tabCategory->currentIndex() == 0) {
		if (Check_EveryGroup->isChecked()){
			if (checkBox1->isChecked()) groups++;
			if (checkBox2->isChecked()) groups++;
			if (checkBox3->isChecked()) groups++;
			if (checkBox4->isChecked()) groups++;
			if (checkBox5->isChecked()) groups++;
			if (checkBox6->isChecked()) groups++;
			if (checkBox7->isChecked()) groups++;
			if (groups<=length)
				ensureEveryGroup = true;
		}
		
		if(checkBox1->isChecked()){
			if (ensureEveryGroup) groupTable.append(AddToAssoctableGroup(assoctable,65,90,num));
			else AddToAssoctable(assoctable,65,90,num);
		}
		if(checkBox2->isChecked()){
			if (ensureEveryGroup) groupTable.append(AddToAssoctableGroup(assoctable,97,122,num));
			else AddToAssoctable(assoctable,97,122,num);
		}
		if(checkBox3->isChecked()){
			if (ensureEveryGroup) groupTable.append(AddToAssoctableGroup(assoctable,48,57,num));
			else AddToAssoctable(assoctable,48,57,num);
		}
		if(checkBox4->isChecked()){
			PwGroup group;
			group.start = num;
			AddToAssoctable(assoctable,33,44,num);
			AddToAssoctable(assoctable,46,47,num);
			AddToAssoctable(assoctable,58,64,num);
			AddToAssoctable(assoctable,91,94,num);
			AddToAssoctable(assoctable,96,96,num);
			AddToAssoctable(assoctable,123,126,num);
			if (ensureEveryGroup){
				group.end = num-1;
				groupTable.append(group);
			}
			
		}
		if(checkBox5->isChecked()){
			if (ensureEveryGroup) groupTable.append(AddToAssoctableGroup(assoctable,32,32,num));
			else AddToAssoctable(assoctable,32,32,num);
		}
		if(checkBox6->isChecked()){
			if (ensureEveryGroup) groupTable.append(AddToAssoctableGroup(assoctable,45,45,num));
			else AddToAssoctable(assoctable,45,45,num);
		}
		if(checkBox7->isChecked()){
			if (ensureEveryGroup) groupTable.append(AddToAssoctableGroup(assoctable,95,95,num));
			else AddToAssoctable(assoctable,95,95,num);
		}
	}
	else {
		QString str=Edit_chars->text();
		for(int i=0;i<str.length();i++){
			assoctable.append(str[i]);
			num++;
		}
	}
	
	QString password(length, '\0');
	
	if (ensureEveryGroup){
		QList<int> charPos;
		for (int i=0; i<length; i++)
			charPos.append(i);
		
		for (int i=0; i<groups; i++){
			int posIndex = randintRange(0, charPos.count()-1);
			int pos = charPos[posIndex];
			charPos.removeAt(posIndex);
			password[pos] = assoctable[randintRange(groupTable[i].start, groupTable[i].end)];
		}
		
		for (int i=groups; i<length; i++){
			int posIndex = randintRange(0, charPos.count()-1);
			int pos = charPos[posIndex];
			charPos.removeAt(posIndex);
			password[pos] = assoctable[randint(num)];
		}
	}
	else{
		for (int i=0; i<length; i++)
			password[i] = assoctable[randint(num)];
	}
	
	return password;
}

void CGenPwDialog::setGenerateEnabled(){
	bool enable;
	int index = tabCategory->currentIndex();
	
	if (index == 0) {
		enable = checkBox1->isChecked() || checkBox2->isChecked() || checkBox3->isChecked() ||
				 checkBox4->isChecked() || checkBox5->isChecked() || checkBox6->isChecked() ||
				 checkBox7->isChecked();
	}
	else if (index == 1) {
		enable = checkBoxPU->isChecked() || checkBoxPL->isChecked() ||
				 checkBoxPN->isChecked() || checkBoxPS->isChecked();
	}
	else {
		enable = !Edit_chars->text().isEmpty();
	}
	
	ButtonGenerate->setEnabled(enable);
}

void CGenPwDialog::setAcceptEnabled(const QString& str){
	AcceptButton->setEnabled(!str.isEmpty());
}

PassCharValidator::PassCharValidator(QObject* parent) : QValidator(parent) {
}

QValidator::State PassCharValidator::validate(QString& input, int& pos) const {
	Q_UNUSED(pos);
	QSet<QChar> chars;
	
	for (int i=0; i<input.size(); i++) {
		if (chars.contains(input[i]))
			return QValidator::Invalid;
		else
			chars.insert(input[i]);
	}
	
	return QValidator::Acceptable;
}
