# QtQuick.SystemTrayIcon

This QML module provides light weight SystemTrayIcon element.

```qml:main.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.SystemTrayIcon 2.15

SystemTrayIcon {
    id: systemTrayIcon
    quitOnLastWindowClosed: false
    visible: true
    icon: Image {
        id: image
        source: 'heart.png'
    }
    contextMenu: Menu {
        Action {
            icon.name: 'document-open'
            text: qsTr("Open")
        }
        Action {
            icon.name: 'document-close'
            text: qsTr("Close")
        }
    }
}
```

## Why we need this?

Qt Labs Platform plugin provides [SystemTrayIcon](https://doc.qt.io/qt-6/qml-qt-labs-platform-systemtrayicon.html).
However, as the documentation says, it needs the QtWidgets library just for a system tray icon because it uses [QSystemTrayIcon](https://doc.qt.io/qt-6/qsystemtrayicon.html) internally.

This QtQuick.SystemTrayIcon doesn't depend on QtWidgets.

## Requirements

- Perl
- Qt 5.15 or later
  - QtGui
  - QtQuick 
  - QtQuick Controls 2

## Build instruction

$ git clone https://github.com/signal-slot/qtquicksystemtrayicon.git

$ mkdir build

$ cd $_

$ /path/to/qmake ../qtquicksystemtrayicon

$ make

$ make sub-examples

$ export QML2_IMPORT_PATH=$PWD/qml

$ ./qtquicksystemtrayicon/examples/quicksystemtrayicon/quick/quick
