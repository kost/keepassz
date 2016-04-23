message("See 'INSTALL' for configuration options.")
#message("*** Build Configuration:")
TEMPLATE = subdirs
isEmpty(PREFIX){
	PREFIX = /usr
}
#isEmpty(KDEDIR){
#	KDEDIR = /usr
#}
SUBDIRS += src
#contains(INT_PLUGINS,kde) {
#	SUBDIRS +=src/plugins/kde
#	message("Build KDE Plugin: yes")
#}
#!contains(INT_PLUGINS,kde) {
#	message("Build KDE Plugin: no")
#}
#contains(INT_PLUGINS,gtk) {
#	SUBDIRS +=src/plugins/gnome
#	message("Build Gnome Plugin: yes")
#}
#!contains(INT_PLUGINS,gtk) {
#	message("Build Gnome Plugin: no")
#}
message("Install Prefix:" $$PREFIX)
#message("KDE Prefix:" $$KDEDIR)
message("*** Makefile successfully generated.")
message("*** Start make now.")
