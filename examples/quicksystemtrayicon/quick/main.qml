import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.SystemTrayIcon 2.15

Window {
    id: window
    width: 500
    height: 500
    title: 'QtQuick.SystemTrayIcon'
    visible: !systemTrayIcon.systemTrayAvailable

    GridLayout {
        anchors.fill: parent
        anchors.margins: 10
        columns: 2
        enabled: systemTrayIcon.systemTrayAvailable
        Label {
            text: 'Icon'
        }
        Row {
            Repeater {
                model: ['heart.png', 'trash.png', 'bad.png']
                Image {
                    source: model.modelData
                    sourceSize: Qt.size(64, 64)
                    MouseArea {
                        anchors.fill: parent
                        onClicked: image.source = model.modelData
                    }
                }
            }
        }
        Label {
            text: 'ToolTip'
        }
        TextField {
            id: toolTip
            Layout.fillWidth: true
        }

        Item {
            width: 1
            height: 1
        }

        Label {
            text: 'Message'
        }

        Label {
            text: 'Type'
        }

        RowLayout {
            Layout.fillWidth: true
            ComboBox {
                id: type
                textRole: 'text'
                valueRole: 'value'
                Layout.fillWidth: true
                model: ListModel {
                    ListElement { text: 'None'; value: SystemTrayIcon.NoIcon}
                    ListElement { text: 'Information'; value: SystemTrayIcon.Information }
                    ListElement { text: 'Warning'; value: SystemTrayIcon.Warning }
                    ListElement { text: 'Critical'; value: SystemTrayIcon.Critical }
                }
            }
            SpinBox {
                id: duration
                from: 1
                to: 20
                value: 5
            }
            Label {
                text: 's'
            }
        }

        Label {
            text: 'Title'
        }

        TextField {
            id: title
            text: 'Cannot connect to network'
            Layout.fillWidth: true
        }

        Label {
            text: 'Body'
        }

        TextArea {
            id: body
            text: "Don't believe me. Honestly, I don't have a clue.
Click this balloon for details."
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Item {
            width: 1
            height: 1
        }

        Button {
            text: 'Show'
            Layout.fillWidth: true
            enabled: systemTrayIcon.supportsMessages
            onClicked: {
                systemTrayIcon.showMessage(title.text, body.text, type.currentValue, duration.value * 1000)
            }
        }
    }

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
                enabled: !window.visible
                onTriggered: window.show()
            }
            Action {
                icon.name: 'document-close'
                text: qsTr("Close")
                enabled: window.visible
                onTriggered: window.hide()
            }
            MenuSeparator {}
            MenuItem {
                icon.name: 'application-exit'
                text: qsTr("Quit")
                onTriggered: Qt.quit()
            }
        }
        toolTip: toolTip.text

        onActivated: window.visible = !window.visible
    }
}
