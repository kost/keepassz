isEmpty(KDEDIR){
	KDEDIR=/usr
}
INCLUDEPATH +=	$$(KDEDIR)/include
TEMPLATE = lib
CONFIG += plugin release
HEADERS += keepassx-kde.h
SOURCES += keepassx-kde.cpp
contains(DEFINES,GLOBAL_AUTOTYPE) {
HEADERS += KpKApplication.h
SOURCES += KpKApplication.cpp
}
MOC_DIR = ../../../build/moc
OBJECTS_DIR = ../../../build
TARGET = ../../../lib/keepassx-kde
LIBS+=-L$$KDEDIR/lib -lkio -lkdecore