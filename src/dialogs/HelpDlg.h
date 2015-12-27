
#ifndef HELPDLG_H
#define HELPDLG_H

#include "ui_HelpDlg.h"

class HelpDlg : public QDialog, private Ui_HelpDlg
{
	Q_OBJECT
	
	public:
		HelpDlg(QWidget* parent);
};

#endif // HELPDLG_H
