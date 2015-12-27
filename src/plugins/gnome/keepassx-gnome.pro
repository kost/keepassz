
INCLUDEPATH +=	/opt/gnome/include/gtk-2.0 \
		/opt/gnome/include/glib-2.0 \
		/opt/gnome/include/pango-1.0 \
		/opt/gnome/include/atk-1.0 \
		/opt/gnome/include/orbit-2.0 \
		/usr/include/cairo \
		/opt/gnome/lib/glib-2.0/include \
		/opt/gnome/lib/gtk-2.0/include
TEMPLATE = lib
CONFIG += plugin release
HEADERS += keepassx-gnome.h
SOURCES += keepassx-gnome.cpp
MOC_DIR = build/moc
OBJECTS_DIR = build/
LIBS+=-L /opt/gnome/lib -lgtk-x11-2.0