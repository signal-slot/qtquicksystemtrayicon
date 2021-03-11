QT = gui-private quick-private quicktemplates2-private
TARGETPATH = QtQuick/SystemTrayIcon.2
IMPORT_VERSION = 2.$$QT_MINOR_VERSION

SOURCES = main.cpp \
    qquicksystemtrayicon.cpp

load(qml_plugin)

OTHER_FILES += \
    plugin.json \
    qmldir

HEADERS += \
    qquicksystemtrayicon.h
