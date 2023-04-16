TEMPLATE = subdirs
 
SUBDIRS = cameraplugin customwidgetsplugin

install_cameraplugin.path = ../../bin/plugins/camera/plugins
install_cameraplugin.files = cameraplugin/cameraplugin.dylib

install_customwidgetsplugin.path = ../../bin
install_customwidgetsplugin.files = customwidgetsplugin/customwidgets.dylib

INSTALLS += install_cameraplugin install_customwidgetsplugin
