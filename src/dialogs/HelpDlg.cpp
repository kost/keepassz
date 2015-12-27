
#include "HelpDlg.h"

HelpDlg::HelpDlg(QWidget* parent) : QDialog(parent) {
	setupUi(this);
	setWindowTitle( windowTitle().append(" - ").append(APP_DISPLAY_NAME) );
	textBrowser->setSource(QUrl("qrc:/docs/index.html"));
	buttonPrevious->setIcon(getIcon("go-previous"));
	buttonNext->setIcon(getIcon("go-next"));
	buttonFirst->setIcon(getIcon("go-home"));
	showMaximized();
}
