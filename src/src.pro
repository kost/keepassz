
CONFIG = qt uic resources thread stl warn_on
QT += xml

*-g++ : QMAKE_CXXFLAGS_WARN_ON += -Wno-sign-compare

DEPENDPATH += crypto dialogs export forms import lib translations res
INCLUDEPATH += . lib crypto plugins/interfaces export import dialogs

MOC_DIR = ../build/moc
UI_DIR = ../build/ui
OBJECTS_DIR = ../build
RCC_DIR = ../build/rcc

isEqual(DEBUG,1){
	CONFIG += debug
}
else {
	CONFIG += release
}

isEqual(NOSTRIP,1) {
	CONFIG += nostrip
}

# lipo and freebsd cannot handle precompiled headers (yet)
!isEqual(PRECOMPILED,1){
	macx : isEqual(ARCH,UNIVERSAL) : PRECOMPILED = 0
	freebsd-* : PRECOMPILED = 0
}

win32 : QMAKE_WIN32 = 1

#-------------------------------------------------------------------------------
#   Platform Specific: Unix (except MacOS X)
#-------------------------------------------------------------------------------
unix : !macx : !isEqual(QMAKE_WIN32,1){
	isEmpty(PREFIX): PREFIX = /usr
	!isEqual(AUTOTYPE,0){
		DEFINES += AUTOTYPE
		!isEqual(GLOBAL_AUTOTYPE,0){
			DEFINES += GLOBAL_AUTOTYPE
		}
	}
	TARGET = ../bin/keepassx
	target.path = $${PREFIX}/bin
	datai18n.files = ../share/keepassx/i18n/*.qm
	datai18n.path = $${PREFIX}/share/keepassx/i18n
	dataicons.files = ../share/keepassx/icons/*.png
	dataicons.path = $${PREFIX}/share/keepassx/icons
	datalicense.files = ../share/keepassx/license.html
	datalicense.path = $${PREFIX}/share/keepassx
	
	shareapp.files = ../share/applications/*.desktop
	shareapp.path = $${PREFIX}/share/applications
	shararemime.files = ../share/mime/packages/*.xml
	shararemime.path = $${PREFIX}/share/mime/packages
	sharemimelnk.files = ../share/mimelnk/application/*.desktop
	sharemimelnk.path = $${PREFIX}/share/mimelnk/application
	sharepixmaps.files = ../share/pixmaps/*.xpm
	sharepixmaps.path = $${PREFIX}/share/pixmaps
	INSTALLS += datai18n dataicons datalicense
	INSTALLS += shareapp shararemime sharemimelnk sharepixmaps
	
	contains(DEFINES,AUTOTYPE){
		LIBS += -lX11 -lXtst
		SOURCES += lib/HelperX11.cpp lib/AutoTypeX11.cpp
		HEADERS += lib/HelperX11.h lib/AutoTypeX11.h
	}
	contains(DEFINES,GLOBAL_AUTOTYPE){
		SOURCES += Application_X11.cpp lib/AutoTypeGlobalX11.cpp
		HEADERS += Application_X11.h lib/AutoTypeGlobalX11.h
	}
#	SOURCES += main_unix.cpp
}


#-------------------------------------------------------------------------------
#   Platform Specific: MacOS X
#-------------------------------------------------------------------------------
macx {
	isEmpty(PREFIX): PREFIX = /Applications
	TARGET = ../bin/KeePassX
	target.path = $${PREFIX}
	data.files += ../share/keepassx
	data.path = Contents/Resources
	INSTALLS += data
	LIBS += -framework CoreFoundation
	isEqual(LINK,DYNAMIC){
		isEmpty(QT_FRAMEWORK_DIR): QT_FRAMEWORK_DIR = /Library/Frameworks
		private_frameworks.files += $${QT_FRAMEWORK_DIR}/QtCore.framework
		private_frameworks.files += $${QT_FRAMEWORK_DIR}/QtGui.framework
		private_frameworks.files += $${QT_FRAMEWORK_DIR}/QtXml.framework
		private_frameworks.path = Contents/Frameworks
		QMAKE_BUNDLE_DATA +=  private_frameworks
	}
	isEqual(LINK,STATIC){
		LIBS += -framework Carbon -framework AppKit -lz
	}
	QMAKE_BUNDLE_DATA += data
	QMAKE_INFO_PLIST= ../share/macx_bundle/Info.plist
	ICON = ../share/macx_bundle/icon.icns
	CONFIG += app_bundle
	isEqual(ARCH,UNIVERSAL){
		CONFIG += x86 ppc
	}
	isEqual(ARCH,INTEL): CONFIG += x86
	isEqual(ARCH,PPC): CONFIG += ppc
#	SOURCES += main_macx.cpp
}

#-------------------------------------------------------------------------------
#   Platform Specific: Windows
#-------------------------------------------------------------------------------
isEqual(QMAKE_WIN32,1){
	CONFIG += windows
	isEmpty(PREFIX): PREFIX = "C:/Program\ files/KeePassX"
	TARGET = ../bin/KeePassX
	target.path = $${PREFIX}
	data.files += ../share/keepassx/*
	data.path = $${PREFIX}/share
	INSTALLS += data
	win32-msvc*: LIBS += advapi32.lib
	!isEqual(INSTALL_QTLIB,0){
		qt_libs.files = $${QMAKE_LIBDIR_QT}/QtCore4.dll $${QMAKE_LIBDIR_QT}/QtGui4.dll $${QMAKE_LIBDIR_QT}/QtXml4.dll
		qt_libs.path = $${PREFIX}
		INSTALLS += qt_libs
	}
	RC_FILE = ../share/win_ico/keepassx.rc
	QMAKE_LINK_OBJECT_SCRIPT = $${OBJECTS_DIR}/$${QMAKE_LINK_OBJECT_SCRIPT}
#	SOURCES += main_win32.cpp
}

INSTALLS += target

contains(DEFINES,GLOBAL_AUTOTYPE){
	FORMS += forms/AutoTypeDlg.ui forms/TargetWindowDlg.ui
	HEADERS += dialogs/AutoTypeDlg.h lib/AutoTypeTreeWidget.h dialogs/TargetWindowDlg.h
	SOURCES += dialogs/AutoTypeDlg.cpp lib/AutoTypeTreeWidget.cpp dialogs/TargetWindowDlg.cpp
}

FORMS += forms/AboutDlg.ui \
         forms/AddBookmarkDlg.ui \
         forms/CalendarDlg.ui \
         forms/CollectEntropyDlg.ui \
         forms/CustomizeDetailViewDlg.ui \
         forms/DatabaseSettingsDlg.ui \
         forms/EditEntryDlg.ui \
         forms/EditGroupDlg.ui \
         forms/ExpiredEntriesDlg.ui \
         forms/HelpDlg.ui \
         forms/MainWindow.ui \
         forms/ManageBookmarksDlg.ui \
         forms/PasswordDlg.ui \
         forms/PasswordGenDlg.ui \
         forms/SearchDlg.ui \
         forms/SelectIconDlg.ui \
         forms/SettingsDlg.ui \
         forms/SimplePasswordDlg.ui \
#         forms/TrashCanDlg.ui \
         forms/WorkspaceLockedWidget.ui

TRANSLATIONS_KX = translations/keepassx-de_DE.ts \
                  translations/keepassx-es_ES.ts \
                  translations/keepassx-fi_FI.ts \
                  translations/keepassx-fr_FR.ts \
                  translations/keepassx-gl_ES.ts \
                  translations/keepassx-hu_HU.ts \
                  translations/keepassx-it_IT.ts \
                  translations/keepassx-ja_JP.ts \
                  translations/keepassx-nb_NO.ts \
                  translations/keepassx-nl_NL.ts \
                  translations/keepassx-pl_PL.ts \
                  translations/keepassx-pt_PT.ts \
                  translations/keepassx-ru_RU.ts \
                  translations/keepassx-sk_SK.ts \
                  translations/keepassx-sr_RS.ts \
                  translations/keepassx-tr_TR.ts \
                  translations/keepassx-uk_UA.ts \
                  translations/keepassx-zh_CN.ts

# also update in translations_release.sh
TRANSLATIONS_DISABLED = translations/keepassx-cs_CZ.ts

TRANSLATIONS_QT = translations/qt_fi.ts \
                  translations/qt_gl_ES.ts \
                  translations/qt_hu.ts \
                  translations/qt_it.ts \
                  translations/qt_nl.ts \
                  translations/qt_sr.ts \
                  translations/qt_tr.ts

# missing Qt translation: nb_NO

TRANSLATIONS = $$TRANSLATIONS_KX $$TRANSLATIONS_DISABLED translations/keepassx-xx_XX.ts
#TRANSLATIONS_UPDATE = $$TRANSLATIONS_KX $$TRANSLATIONS_DISABLED translations/keepassx-xx_XX.ts
#TRANSLATIONS_COMPILE = $$TRANSLATIONS_KX $$TRANSLATIONS_QT

HEADERS += main.h \
           mainwindow.h \
           KpxConfig.h \
           Database.h \
           Kdb3Database.h \
           lib/AutoType.h \
           lib/bookmarks.h \
           lib/EntryView.h \
           lib/FileDialogs.h \
           lib/GroupView.h \
           lib/random.h \
           lib/SecString.h \
           lib/ShortcutWidget.h \
           lib/tools.h \
           lib/UrlLabel.h \
           lib/WaitAnimationWidget.h \
           crypto/aes.h \
           crypto/aescpp.h \
           crypto/aes_endian.h \
           crypto/aes_types.h \
           crypto/aesopt.h \
           crypto/aestab.h \
           crypto/arcfour.h \
           crypto/blowfish.h \
           crypto/sha256.h \
           crypto/twoclass.h \
           crypto/twofish.h \
           crypto/yarrow.h \
           apg/convert.h \
           apg/owntypes.h \
           apg/pronpass.h \
           apg/randpass.h \
           apg/smbl.h \
           dialogs/AboutDlg.h \
           dialogs/AddBookmarkDlg.h \
           dialogs/CalendarDlg.h \
           dialogs/CollectEntropyDlg.h \
           dialogs/CustomizeDetailViewDlg.h \
           dialogs/DatabaseSettingsDlg.h \
           dialogs/EditEntryDlg.h \
           dialogs/EditGroupDlg.h \
           dialogs/ExpiredEntriesDlg.h \
           dialogs/HelpDlg.h \
           dialogs/ManageBookmarksDlg.h \
           dialogs/PasswordDlg.h \
           dialogs/PasswordGenDlg.h \
           dialogs/SearchDlg.h \
           dialogs/SelectIconDlg.h \
           dialogs/SettingsDlg.h \
           dialogs/SimplePasswordDlg.h \
#           dialogs/TrashCanDlg.h \
           import/Import.h \
#           import/Import_GnuKeyRing.h \
           import/Import_KeePassX_Xml.h \
           import/Import_KWalletXml.h \
           import/Import_PwManager.h \
           export/Export.h \
           export/Export_KeePassX_Xml.h \
           export/Export_Txt.h \
           plugins/interfaces/IFileDialog.h \
           plugins/interfaces/IIconTheme.h \
           plugins/interfaces/IGnomeInit.h \
           plugins/interfaces/IKdeInit.h

SOURCES += main.cpp \
           mainwindow.cpp \
           KpxConfig.cpp \
           Database.cpp \
           Kdb3Database.cpp \
           lib/bookmarks.cpp \
           lib/EntryView.cpp \
           lib/FileDialogs.cpp \
           lib/GroupView.cpp \
           lib/random.cpp \
           lib/SecString.cpp \
           lib/ShortcutWidget.cpp \
           lib/tools.cpp \
           lib/UrlLabel.cpp \
           lib/WaitAnimationWidget.cpp \
           crypto/aescrypt.c \
           crypto/aeskey.c \
           crypto/aes_modes.c \
           crypto/aestab.c \
           crypto/arcfour.cpp \
           crypto/blowfish.cpp \
           crypto/sha256.cpp \
           crypto/twoclass.cpp \
           crypto/twofish.cpp \
           crypto/yarrow.cpp \
           apg/convert.c \
           apg/pronpass.c \
           apg/randpass.c \
           dialogs/AboutDlg.cpp \
           dialogs/AddBookmarkDlg.cpp \
           dialogs/CalendarDlg.cpp \
           dialogs/CollectEntropyDlg.cpp \
           dialogs/CustomizeDetailViewDlg.cpp \
           dialogs/DatabaseSettingsDlg.cpp \
           dialogs/EditEntryDlg.cpp \
           dialogs/EditGroupDlg.cpp \
           dialogs/ExpiredEntriesDlg.cpp \
           dialogs/HelpDlg.cpp \
           dialogs/ManageBookmarksDlg.cpp \
           dialogs/PasswordDlg.cpp \
           dialogs/PasswordGenDlg.cpp \
           dialogs/SearchDlg.cpp \
           dialogs/SelectIconDlg.cpp \
           dialogs/SettingsDlg.cpp \
           dialogs/SimplePasswordDlg.cpp \
#           dialogs/TrashCanDlg.cpp \
           import/Import.cpp \
#           import/Import_GnuKeyRing.cpp \
           import/Import_KeePassX_Xml.cpp \
           import/Import_KWalletXml.cpp \
           import/Import_PwManager.cpp \
           export/Export.cpp \
           export/Export_KeePassX_Xml.cpp \
           export/Export_Txt.cpp

isEqual(PRECOMPILED,0) {
	QMAKE_CXXFLAGS += -include keepassx.h
}
else {
	CONFIG += precompile_header
	PRECOMPILED_HEADER = keepassx.h
}

RESOURCES += res/resources.qrc

#isEmpty(QMAKE_LRELEASE) {
#	win32 {
#		QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
#	}
#	else {
#		QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease-qt4
#		!exists($$QMAKE_LRELEASE) : QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
#	}
#}

#exists($$QMAKE_LRELEASE) {
#	updateqm.input = TRANSLATIONS_COMPILE
#	updateqm.output = ../share/keepassx/i18n/${QMAKE_FILE_BASE}.qm
#	updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ../share/keepassx/i18n/${QMAKE_FILE_BASE}.qm
#	updateqm.CONFIG += no_link
#	
#	QMAKE_EXTRA_COMPILERS += updateqm
#	PRE_TARGETDEPS += compiler_updateqm_make_all
#}
#else {
#	message("*** lrelease not found - can't compile translation files")
#}
