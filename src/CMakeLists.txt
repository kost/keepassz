# CMake is still experimental, use qmake instead

SET(keepassx_HEADERS
	main.h
	mainwindow.h
	Database.h
	Kdb3Database.h
	lib/EntryView.h
	lib/FileDialogs.h
	lib/GroupView.h
	lib/ShortcutWidget.h
	lib/UrlLabel.h
	lib/WaitAnimationWidget.h
	dialogs/AboutDlg.h
	dialogs/AddBookmarkDlg.h
	dialogs/CalendarDlg.h
	dialogs/CollectEntropyDlg.h
	dialogs/CustomizeDetailViewDlg.h
	dialogs/DatabaseSettingsDlg.h
	dialogs/EditEntryDlg.h
	dialogs/EditGroupDlg.h
	dialogs/ExpiredEntriesDlg.h
	dialogs/HelpDlg.h
	dialogs/ManageBookmarksDlg.h
	dialogs/PasswordDlg.h
	dialogs/PasswordGenDlg.h
	dialogs/SearchDlg.h
	dialogs/SelectIconDlg.h
	dialogs/SettingsDlg.h
	dialogs/SimplePasswordDlg.h
#	dialogs/TrashCanDlg.h
	import/Import.h
#	import/Import_GnuKeyRing.h
	import/Import_KeePassX_Xml.h
	import/Import_KWalletXml.h
	import/Import_PwManager.h
	export/Export.h
	export/Export_KeePassX_Xml.h
	export/Export_Txt.h
)

SET(keepassx_SOURCES
	main.cpp
	mainwindow.cpp
	KpxConfig.cpp
	Database.cpp
	Kdb3Database.cpp
	lib/bookmarks.cpp
	lib/EntryView.cpp
	lib/FileDialogs.cpp
	lib/GroupView.cpp
	lib/random.cpp
	lib/SecString.cpp
	lib/ShortcutWidget.cpp
	lib/tools.cpp
	lib/UrlLabel.cpp
	lib/WaitAnimationWidget.cpp
	crypto/aescrypt.c
	crypto/aeskey.c
	crypto/aes_modes.c
	crypto/aestab.c
	crypto/arcfour.cpp
	crypto/blowfish.cpp
	crypto/sha256.cpp
	crypto/twoclass.cpp
	crypto/twofish.cpp
	crypto/yarrow.cpp
	apg/convert.c
	apg/pronpass.c
	apg/randpass.c
	dialogs/AboutDlg.cpp
	dialogs/AddBookmarkDlg.cpp
	dialogs/CalendarDlg.cpp
	dialogs/CollectEntropyDlg.cpp
	dialogs/CustomizeDetailViewDlg.cpp
	dialogs/DatabaseSettingsDlg.cpp
	dialogs/EditEntryDlg.cpp
	dialogs/EditGroupDlg.cpp
	dialogs/ExpiredEntriesDlg.cpp
	dialogs/HelpDlg.cpp
	dialogs/ManageBookmarksDlg.cpp
	dialogs/PasswordDlg.cpp
	dialogs/PasswordGenDlg.cpp
	dialogs/SearchDlg.cpp
	dialogs/SelectIconDlg.cpp
	dialogs/SettingsDlg.cpp
	dialogs/SimplePasswordDlg.cpp
#	dialogs/TrashCanDlg.cpp
	import/Import.cpp
#	import/Import_GnuKeyRing.cpp
	import/Import_KeePassX_Xml.cpp
	import/Import_KWalletXml.cpp
	import/Import_PwManager.cpp
	export/Export.cpp
	export/Export_KeePassX_Xml.cpp
	export/Export_Txt.cpp
)

SET(keepassx_FORMS
	forms/AboutDlg.ui
	forms/AddBookmarkDlg.ui
	forms/CalendarDlg.ui
	forms/CollectEntropyDlg.ui
	forms/CustomizeDetailViewDlg.ui
	forms/DatabaseSettingsDlg.ui
	forms/EditEntryDlg.ui
	forms/EditGroupDlg.ui
	forms/ExpiredEntriesDlg.ui
	forms/HelpDlg.ui
	forms/MainWindow.ui
	forms/ManageBookmarksDlg.ui
	forms/PasswordDlg.ui
	forms/PasswordGenDlg.ui
	forms/SearchDlg.ui
	forms/SelectIconDlg.ui
	forms/SettingsDlg.ui
	forms/SimplePasswordDlg.ui
#	forms/TrashCanDlg.ui
	forms/WorkspaceLockedWidget.ui
)

SET(keepassx_RESOURCES res/resources.qrc)

set(keepassx_TRANSLATIONS
#	translations/keepassx-cs_CZ.ts
	translations/keepassx-de_DE.ts
	translations/keepassx-es_ES.ts
	translations/keepassx-fi_FI.ts
	translations/keepassx-fr_FR.ts
	translations/keepassx-gl_ES.ts
	translations/keepassx-hu_HU.ts
	translations/keepassx-it_IT.ts
	translations/keepassx-ja_JP.ts
	translations/keepassx-nb_NO.ts
	translations/keepassx-nl_NL.ts
	translations/keepassx-pl_PL.ts
	translations/keepassx-ru_RU.ts
	translations/keepassx-tr_TR.ts
	translations/keepassx-uk_UA.ts
	translations/keepassx-zh_CN.ts
	translations/qt_fi.ts
	translations/qt_hu.ts
	translations/qt_it.ts
	translations/qt_nl.ts
	translations/qt_tr.ts
)

#IF( MINGW )
#	SET(keepassx_SOURCES ${keepassx_SOURCES} main_win32.cpp)
#ELSEIF( APPLE )
#	SET(keepassx_SOURCES ${keepassx_SOURCES} main_macx.cpp)
#ELSE( MINGW )
#	SET(keepassx_SOURCES ${keepassx_SOURCES} main_unix.cpp)
#ENDIF( MINGW )

IF( AUTOTYPE )
	SET(keepassx_HEADERS ${keepassx_HEADERS}
		Application_X11.h dialogs/AutoTypeDlg.h lib/AutoTypeTreeWidget.h dialogs/TargetWindowDlg.h)
	SET(keepassx_SOURCES ${keepassx_SOURCES}
		lib/HelperX11.cpp lib/AutoTypeX11.cpp Application_X11.cpp lib/AutoTypeGlobalX11.cpp
		dialogs/AutoTypeDlg.cpp lib/AutoTypeTreeWidget.cpp dialogs/TargetWindowDlg.cpp)
	SET(keepassx_FORMS ${keepassx_FORMS}
		forms/AutoTypeDlg.ui forms/TargetWindowDlg.ui)
ENDIF( AUTOTYPE )

IF( NOT PRECOMPILED_HEADER )
	ADD_DEFINITIONS(-include keepassx.h)
ENDIF( NOT PRECOMPILED_HEADER )

INCLUDE_DIRECTORIES(. lib crypto plugins/interfaces export import dialogs ${CMAKE_CURRENT_BINARY_DIR})

QT4_ADD_RESOURCES( keepassx_RESOURCES_SOURCES ${keepassx_RESOURCES} )
QT4_WRAP_UI( keepassx_FORMS_HEADERS ${keepassx_FORMS} )
QT4_WRAP_CPP( keepassx_HEADERS_MOC ${keepassx_HEADERS} )

ADD_EXECUTABLE( ${PROGNAME} WIN32 MACOSX_BUNDLE ${keepassx_SOURCES} ${keepassx_HEADERS_MOC}
	${keepassx_RESOURCES_SOURCES} ${keepassx_FORMS_HEADERS} ${keepassx_RCS} )
IF( PRECOMPILED_HEADER )
	ADD_PRECOMPILED_HEADER(${PROGNAME} keepassx.h)
ENDIF( PRECOMPILED_HEADER )

TARGET_LINK_LIBRARIES(${PROGNAME} ${QT_LIBRARIES} ${AUTOTYPE_LIBRARIES})

QT4_ADD_TRANSLATION(QM_FILES ${keepassx_TRANSLATIONS})
ADD_CUSTOM_TARGET(translations ALL DEPENDS ${QM_FILES})

INSTALL(TARGETS ${PROGNAME} DESTINATION ${BINDIR})
INSTALL(FILES ${QM_FILES} DESTINATION ${SHAREDIR}/i18n)
