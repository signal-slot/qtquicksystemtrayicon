TARGET = QtQuickSystemTrayIcon
QT = gui-private quick-private quicktemplates2-private
CONFIG += git_build

HEADERS += \
    qtquicksystemtrayiconglobal.h \
    qquicksystemtrayicon.h

SOURCES += \
    qquicksystemtrayicon.cpp

load(qt_module)
